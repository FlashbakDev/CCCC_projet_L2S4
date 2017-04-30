#include "game.h"
#include "core.h"

// =========================================================

SDL_Renderer *InitGame(char * pChar_name, Array *pArray, int w, int h){

    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    SDL_Rect rect_bounds;
    int error = 0;

    SDL_GetDisplayBounds(0, &rect_bounds);

    if ( !pArray || w > rect_bounds.w || h > rect_bounds.h ){

        SDL_Quit();
        return NULL;
    }

    screen_width = w;
    screen_height = h;

    Array_new(pArray);

    pWindow = SDL_CreateWindow(pChar_name, rect_bounds.w / 2 - w / 2, rect_bounds.h / 2 - h / 2, w, h, SDL_WINDOW_SHOWN);
    Array_append(pArray, ObjectTypes_WINDOW , pWindow);
    // void SDL_SetWindowIcon(SDL_Window*  window , SDL_Surface* icon); //ajoute une icône à la fenêtre

    pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    Array_append(pArray, ObjectTypes_RENDERER, pRenderer);

    // init des ressources
    error += Font_new(&font_default, "data/fonts/arial.ttf", pArray, 15);
    error += Font_new(&font_hight, "data/fonts/arial.ttf", pArray, 40);
    error += Image_new(&image_active, "data/UI/image_active.png", pArray, pRenderer);
    error += Image_new(&image_prelight, "data/UI/image_prelight.png", pArray, pRenderer);
    error += Image_new(&image_normal, "data/UI/image_normal.png", pArray, pRenderer);
    error += Image_new(&image_selected, "data/UI/image_selected.png", pArray, pRenderer);
    error += Image_new(&image_unselected, "data/UI/image_unselected.png", pArray, pRenderer);
    error += Image_new(&image_cursorBlue, "data/image_cursorBlue.png", pArray, pRenderer);
    error += Image_new(&image_cursorGreen, "data/image_cursorGreen.png", pArray, pRenderer);
    error += Image_new(&image_cursorRed, "data/image_cursorRed.png", pArray, pRenderer);

    error += LoadTokenImagesFromPath("data/Tokens/Jewellery/", pArray, pRenderer);

    if ( error > 0 ) {

		Clean(pArray);
		return NULL;
	}

	return pRenderer;
}

// =========================================================

Grid *NewGrid(SDL_Rect rect, int nbMove, int nbColor, bool randomizeInsert, int nbHelp, int nbSuperHelp, int nbRevertOnce){

    /* aléatoire */
    srand(time(NULL));

    Grid *pGrid = malloc(sizeof(Grid));

    pGrid->width = rect.w / TOKEN_WIDTH;
    pGrid->height = rect.h / TOKEN_HEIGHT;
    pGrid->nbMove = nbMove;
    pGrid->nbColor = nbColor;
    pGrid->direction =Directions_DOWN;
    pGrid->score = 0;
    pGrid->is_randomizeInsert = randomizeInsert;
    pGrid->nbHelp = nbHelp;
    pGrid->nbSuperHelp = nbSuperHelp;
    pGrid->nbRevertOnce = nbRevertOnce;
    pGrid->outline = true;

    MakeRect(&pGrid->rect, rect.x, rect.y, rect.w, rect.h);

    pGrid->is_cursorOnGrid = false;
    pGrid->cursorTokenPosition.x = 0;
    pGrid->cursorTokenPosition.y = 0;
    pGrid->isHelpActive = false;
    pGrid->isCalc = false;

    /* allocation de la grille et remplissage */
    pGrid->tokens = (Token*)malloc( pGrid->height * sizeof(Token*));
    pGrid->pastTokens = (Token*)malloc( pGrid->height * sizeof(Token*));

    if ( pGrid->tokens == NULL || pGrid->pastTokens == NULL){ return NULL; }

    for(int i = 0; i < pGrid->height; i++){

        pGrid->tokens[i] = (Token*)malloc( pGrid->width * sizeof(Token));
        pGrid->pastTokens[i] = (Token*)malloc( pGrid->width * sizeof(Token));

        /* désallocation de la mémoire en cas d'erreur */
        if ( pGrid->tokens[i] == NULL || pGrid->pastTokens[i] == NULL){

            for(i=i-1 ; i >= 0 ; i--){
                free(pGrid->tokens[i]);
                free(pGrid->pastTokens[i]);
            }
            free(pGrid->tokens);
            free(pGrid->pastTokens);

            return NULL;
        }
    }

    RandomizeGrid(pGrid);

    SaveTokensInPastTokens(pGrid);

    return pGrid;
}

// =========================================================

void RandomizeGrid(Grid *pGrid){

    fprintf(stdout,"game.c -> RandomizeGrid(Grid *pGrid)\n");

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            InitRandomToken(pGrid, &pGrid->tokens[i][j], j, i);
        }
    }

    while( IsLineOnGrid(pGrid) == true || IsTokenOfType(pGrid, TokenTypes_NONE ) == true ){

        if( IsLineOnGrid(pGrid) == true ){

            // détruit les lignes et remplie les cases manquantes du tableau
            DestroyAlignedTokens(pGrid);
        }
        else {

            while( IsTokenOfType(pGrid, TokenTypes_NONE ) == true ){

                // regroupe tout les jetons
                RegroupTokens(pGrid);

                // remplie les espaces vides
                InjectLigne(pGrid);
            }
        }
    }

    // recalcul les mouvements possibles
    MoveAvailable(pGrid);
}

// =========================================================

void InitRandomToken(Grid *pGrid, Token *token, int x, int y){

    ResetToken(token);

    token->type = TokenTypes_NORMAL;
    token->color = (Colors)(rand() % pGrid->nbColor);

    CalculTokenImages(pGrid, token, x, y);
}

// =========================================================

void CheckGrid(Grid *pGrid){

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            pGrid->tokens[i][j].aligned = false;
        }
    }

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].type != TokenTypes_NONE ){

                // vérification verticale
                if ( i < pGrid->height - 2){

                    if ( Compare_TokenColor(pGrid->tokens[i][j], pGrid->tokens[i+1][j]) && Compare_TokenColor(pGrid->tokens[i][j],pGrid->tokens[i+2][j]) ){

                        pGrid->tokens[i][j].aligned = true;
                        pGrid->tokens[i+1][j].aligned = true;
                        pGrid->tokens[i+2][j].aligned = true;
                    }
                }

                // vérification horizontale
                if ( j < pGrid->width - 2){

                    if ( Compare_TokenColor(pGrid->tokens[i][j],pGrid->tokens[i][j+1])&& Compare_TokenColor(pGrid->tokens[i][j], pGrid->tokens[i][j+2]) ){

                        pGrid->tokens[i][j].aligned = true;
                        pGrid->tokens[i][j+1].aligned = true;
                        pGrid->tokens[i][j+2].aligned = true;
                    }
                }
            }
        }
    }
}

// =========================================================

void AnimMovingTokens(Grid *pGrid){

    //fprintf(stdout, "AnimMovingTokens()\n");

    // anime les mouvement
    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            //fprintf(stdout, "Token %d %d : x = %d, y = %d\n", j, i, pGrid->tokens[i][j].rect_image.x, pGrid->tokens[i][j].rect_image.y );

            if ( pGrid->tokens[i][j].rect_image.x != ( pGrid->rect.x + ((j * TOKEN_WIDTH) + (TOKEN_WIDTH/2)) - (pGrid->tokens[i][j].rect_image.w / 2)) ){

                //fprintf(stdout, "Mouvement horizontal d'un jeton...");

                pGrid->tokens[i][j].isMoving = true;

                if ( pGrid->tokens[i][j].rect_image.x > ( pGrid->rect.x + ((j * TOKEN_WIDTH) + (TOKEN_WIDTH/2)) - (pGrid->tokens[i][j].rect_image.w / 2)) )
                    pGrid->tokens[i][j].rect_image.x -= FALL_SPEED ;
                else
                    pGrid->tokens[i][j].rect_image.x += FALL_SPEED ;

                if ( sqrt( pow( pGrid->tokens[i][j].rect_image.x - ( pGrid->rect.x + ((j * TOKEN_WIDTH) + (TOKEN_WIDTH/2)) - (pGrid->tokens[i][j].rect_image.w / 2)) ,2) ) <= FALL_SPEED ){

                    pGrid->tokens[i][j].rect_image.x = ((j * TOKEN_WIDTH) + (TOKEN_WIDTH/2)) - (pGrid->tokens[i][j].rect_image.w / 2);
                    pGrid->tokens[i][j].isMoving = false;
                }
            }

            if ( pGrid->tokens[i][j].rect_image.y != ( pGrid->rect.y + ((i * TOKEN_HEIGHT) + (TOKEN_HEIGHT/2)) - (pGrid->tokens[i][j].rect_image.h / 2)) ){

                //fprintf(stdout, "Mouvement vertical d'un jeton...");

                pGrid->tokens[i][j].isMoving = true;

                if ( pGrid->tokens[i][j].rect_image.y > ( pGrid->rect.y + ((i * TOKEN_HEIGHT) + (TOKEN_HEIGHT/2)) - (pGrid->tokens[i][j].rect_image.h / 2)) )
                    pGrid->tokens[i][j].rect_image.y -= FALL_SPEED ;
                else
                    pGrid->tokens[i][j].rect_image.y += FALL_SPEED ;

                if ( sqrt( pow( pGrid->tokens[i][j].rect_image.y - ( pGrid->rect.y + ((i * TOKEN_HEIGHT) + (TOKEN_HEIGHT/2)) - (pGrid->tokens[i][j].rect_image.h / 2)) ,2) ) <= FALL_SPEED ){

                    pGrid->tokens[i][j].rect_image.y = (((i * TOKEN_HEIGHT) + (TOKEN_HEIGHT/2)) - (pGrid->tokens[i][j].rect_image.h / 2));
                    pGrid->tokens[i][j].isMoving = false;
                }
            }
        }
    }
}

// =========================================================

void AnimDestructingTokens(Grid *pGrid){

    // anime les destructions
    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].isMoving == false && pGrid->tokens[i][j].isDestruct == true ){

                if ( pGrid->tokens[i][j].startDestructAnim == -1 ){

                    pGrid->tokens[i][j].startDestructAnim = SDL_GetTicks();

                    //fprintf(stdout, "animation de destruction commence.\n");
                }

                // animationd de destruction
                if (  SDL_GetTicks() - pGrid->tokens[i][j].startDestructAnim < DESTRUCT_SPEED ){

                    //fprintf(stdout, "nombre de tick ecoule : %d / %d, textureSize = %d.\n", SDL_GetTicks() - pGrid->tokens[i][j].startDestructAnim, DESTRUCT_SPEED, pGrid->tokens[i][j].textureSize);

                    // perte en une frame
                    pGrid->tokens[i][j].textureSize = 100 - ( 100.0 / DESTRUCT_SPEED * ( SDL_GetTicks() - pGrid->tokens[i][j].startDestructAnim ) ) ;

                    CalculTokenImages(pGrid, &pGrid->tokens[i][j], j, i);
                }
                else {

                    pGrid->tokens[i][j].isDestruct = false;
                    pGrid->tokens[i][j].startDestructAnim = -1;

                    //fprintf(stdout, "animation de destruction termine a %d/100 de la texture.\n", pGrid->tokens[i][j].textureSize );
                }
            }
        }
    }
}

// =========================================================

void Grid_anim(Grid *pGrid){

    if ( IsTokenDestructing(pGrid) == false ){

        AnimMovingTokens(pGrid);
    }
    else{

        AnimDestructingTokens(pGrid);
    }
}
//==========================================================
void Token_speciaux(Grid *pGrid){

    int vecteur_point[4][2][2] = {{{0,1},{1,0}} ,{{0,-1},{1,0}} ,{{0,1},{-1,0}},{{0,-1},{-1,0}}};

    Colors save_color;

    int cpt = 0;
    bool coude = false;

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            coude = false;

            if ( pGrid->tokens[i][j].aligned == true && pGrid->tokens[i][j].type == TokenTypes_NORMAL){

                save_color = pGrid->tokens[i][j].color;

                for(int n = 0; n < 3; n++){

                    if(i+vecteur_point[n][0][0] >=0 &&
                       i+vecteur_point[n][0][0] <pGrid->height &&
                       j+vecteur_point[n][0][1] <pGrid->width &&
                       j+vecteur_point[n][0][1] >=0  &&
                       i+vecteur_point[n][1][0] >=0 &&
                       i+vecteur_point[n][1][0] <pGrid->height &&
                       j+vecteur_point[n][1][1] <pGrid->width &&
                       j+vecteur_point[n][1][1] >=0  &&
                       Compare_TokenColor_color(pGrid->tokens[i+vecteur_point[n][0][0]][j+vecteur_point[n][0][1]], save_color) &&
                       Compare_TokenColor_color(pGrid->tokens[i+vecteur_point[n][1][0]][j+vecteur_point[n][1][1]], save_color) &&
                       pGrid->tokens[i+vecteur_point[n][0][0]][j+vecteur_point[n][0][1]].aligned == true &&
                       pGrid->tokens[i+vecteur_point[n][1][0]][j+vecteur_point[n][1][1]].aligned == true
                       )
                       coude = true;
                }

                 if(coude == true){

                    pGrid->tokens[i][j].aligned = false;
                    pGrid->tokens[i][j].type = TokenTypes_BOMB;
                    CalculTokenImages(pGrid,&pGrid->tokens[i][j],j,i);
                }
            }
        }
    }

    //Verification des allignements horizontaux
    int value_random;
    Token Token_save;
    int nb_align = 1;

    int j;
    int i;

    for(i = 0; i < pGrid->height; i++){


        Token_save = pGrid->tokens[i][0];
        nb_align = 1;

        for( j = 1; j < pGrid->width; j++){

            if(Compare_TokenColor(pGrid->tokens[i][j], Token_save)){

                nb_align++;
               // printf("align = %d\n", nb_align);
            }
            else {

                if(nb_align==4){

                    //Si ligne de 4 fait apparaitre un jeton horizontal qqpart dans la ligne
                    printf("ligne de 4\n");
                    cpt=0; //Anti bouclage infini
                    do{
                    value_random = 1+ rand()%4;
                    cpt++;
                    }
                    while(pGrid->tokens[i][j-value_random].type != TokenTypes_NORMAL && cpt<200);
                    pGrid->tokens[i][j-value_random].type = TokenTypes_HORIZONTAL;
                    pGrid->tokens[i][j-value_random].aligned = false;
                    pGrid->tokens[i][j-value_random].isDestruct = false;
                    CalculTokenImages(pGrid,&pGrid->tokens[i][j-value_random],j-value_random,i);
                }

                if(nb_align == 5){

                    pGrid->tokens[i][j-3].aligned = false;
                    pGrid->tokens[i][j-3].color = Colors_NONE;
                    pGrid->tokens[i][j-3].type = TokenTypes_MULTI;
                    CalculTokenImages(pGrid,&pGrid->tokens[i][j-3],j-3,i);

                }

                nb_align = 1;
                Token_save = pGrid->tokens[i][j];
            }
        }

         if(nb_align==4){

                        //Ajout jeton special
                cpt=0; //Anti bouclage infini
                do{
                value_random = 1+ rand()%4;
                cpt++;
                }
                while(pGrid->tokens[i][j-value_random].type != TokenTypes_NORMAL && cpt<200);
                pGrid->tokens[i][j-value_random].type = TokenTypes_HORIZONTAL;

                pGrid->tokens[i][j-value_random].aligned = false;
                CalculTokenImages(pGrid,&pGrid->tokens[i][j-value_random],j-value_random,i);

         }

         if(nb_align == 5){
                        pGrid->tokens[i][j-3].aligned = false;
                        pGrid->tokens[i][j-3].color = Colors_NONE;
                        pGrid->tokens[i][j-3].type = TokenTypes_MULTI;
                        CalculTokenImages(pGrid,&pGrid->tokens[i-3][j],j,i-3);

         }


    }


    //Verification des alignements verticaux
    for(j = 0; j < pGrid->width; j++){
        Token_save = pGrid->tokens[0][j];
        nb_align = 1;

        for( i = 1; i < pGrid->height; i++){

            if(Compare_TokenColor(pGrid->tokens[i][j], Token_save)){

                nb_align++;
            }
            else {

             if(nb_align==4){
                //Si ligne de 4 fait apparaitre un jeton horizontal qqpart dans la ligne
                cpt=0; //Anti boucle infini
                do{
                value_random = 1+ rand()%4;
                cpt++;
                }
                while(pGrid->tokens[i-value_random][j].type != TokenTypes_NORMAL && cpt<200);
                pGrid->tokens[i-value_random][j].type =TokenTypes_VERTICAL;
                pGrid->tokens[i-value_random][j].aligned = false;
                pGrid->tokens[i-value_random][j].isDestruct = false;
                CalculTokenImages(pGrid,&pGrid->tokens[i-value_random][j],j,i-value_random);
             }

             if(nb_align == 5){
                        pGrid->tokens[i-3][j].aligned = false;
                        pGrid->tokens[i-3][j].color = Colors_NONE;
                        pGrid->tokens[i-3][j].type = TokenTypes_MULTI;
                        CalculTokenImages(pGrid,&pGrid->tokens[i-3][j],j,i-3);

             }
                nb_align = 1;
                Token_save = pGrid->tokens[i][j];

            }


        }

         if(nb_align==4){

                        //Ajout jeton special
                cpt=0; //Anti bouclage infini
                do{
                value_random = 1+ rand()%4;
                cpt++;
                }
                while(pGrid->tokens[i-value_random][j].type != TokenTypes_NORMAL && cpt<200);
                pGrid->tokens[i-value_random][j].type = TokenTypes_VERTICAL;

                pGrid->tokens[i-value_random][j].aligned = false;
                CalculTokenImages(pGrid,&pGrid->tokens[i-value_random][j],j,i-value_random);

         }

         if(nb_align == 5){
                        pGrid->tokens[i-3][j].aligned = false;
                        pGrid->tokens[i-3][j].color = Colors_NONE;
                        pGrid->tokens[i-3][j].type = TokenTypes_MULTI;
                        CalculTokenImages(pGrid,&pGrid->tokens[i-3][j],j,i-3);

         }
    }
}


// =========================================================

int Calc_Score(Grid *pGrid ){

    //int[] scores = { 1, 1, 1, 5, 10 };

    /* actualisation de la grille */
    CheckGrid(pGrid);

    int score = 0;
    int nb_align;
    int multi;
    int val = 0;
    Token Token_save;

    /* traitement au cas par cas */

    //Verification des allignements horizontaux
    for(int i = 0; i < pGrid->height; i++){

        nb_align = 1;
        Token_save = pGrid->tokens[i][0];
        val = pGrid->tokens[i][0].score;

        for(int j = 1; j < pGrid->width; j++){

            if(Compare_TokenColor(pGrid->tokens[i][j], Token_save)){

                nb_align++;
                val += TOKEN_SCORE;

            }
            else {

                if(nb_align >= 3){

                    multi = 1;
                    if(nb_align>=4){

                        multi = 3;

                        if(nb_align>=5){

                            multi = 10;
                        }
                    }
                    //Faire boucle en ajoutant les scores de jetons individuels
                    //Puis multiplier par Multi

                    score += multi * val;
                }

                nb_align = 1;
                Token_save = pGrid->tokens[i][j];
                val = TOKEN_SCORE;
            }
        }

        if(nb_align >= 3){

            multi = 1;
            if(nb_align>=4)
            {
                multi = 3;

                if(nb_align>=5)
                {
                    multi = 10;

                }
            }
            //Faire boucle en ajoutant les scores de jetons individuels
            //Puis multiplier par Multi

            score += multi * val;

            //printf("Score de la ligne : %d \n", score);
        }
    }

    //Verification des alignements verticaux
    for(int j = 0; j < pGrid->width; j++){

        nb_align = 1;
        Token_save = pGrid->tokens[0][j];
         val = TOKEN_SCORE;

        for(int i = 1; i < pGrid->height; i++){

            if(Compare_TokenColor(pGrid->tokens[i][j],Token_save)){

                nb_align++;
                val += TOKEN_SCORE;
            }
            else {

                if(nb_align >= 3){

                    multi = 1;
                    if(nb_align>=4){

                        multi = 3;

                        if(nb_align>=5){

                            multi = 10;
                        }
                    }

                    score += multi * val;
                    //printf("Score de la colonne : %d \n", score);
                }
                nb_align = 1;
                Token_save = pGrid->tokens[i][j];
                val = TOKEN_SCORE;
            }
        }
        if(nb_align >= 3){

            multi = 1;
            if(nb_align>=4)
            {
                multi = 3;

                if(nb_align>=5)
                {
                    multi = 10;

                }
            }
            //Faire boucle en ajoutant les scores de jetons individuels
            //Puis multiplier par Multi

            score += multi * val;

            //printf("Score de la ligne : %d \n", score);
        }
    }

    return score;
}

//====================================================

void ChangeAlignedTokenBackgroundImage(Grid *pGrid, Image image){

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].aligned == true ){

                pGrid->tokens[i][j].image_background = image;
                pGrid->tokens[i][j].drawBackground = true;
            }
        }
    }
}

//====================================================

void ChangeColorTokenBackgroundImage(Grid *pGrid, Image image, Colors c){

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].color == c ){

                pGrid->tokens[i][j].image_background = image;
                pGrid->tokens[i][j].drawBackground = true;
            }
        }
    }
}

//====================================================

bool IsLineOnGrid(Grid *pGrid){

    //fprintf(stdout,"game.c : IsLigneOnGrid(Grid * pGrid)\n");

    CheckGrid(pGrid);

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].aligned == true )
                return true;
        }
    }

    return false;
}

// =========================================================

bool IsTokenMoving(Grid *pGrid){

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].isMoving == true )
                return true;
        }
    }

    return false;
}

// =========================================================

bool IsTokenDestructing(Grid *pGrid){

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].isDestruct == true )
                return true;
        }
    }

    return false;
}

// =========================================================

bool IsRandomGridStabilized(Grid *pGrid){

    if( IsTokenOfType(pGrid, TokenTypes_NONE) || IsTokenMoving(pGrid) || IsTokenDestructing(pGrid) || IsLineOnGrid(pGrid) ) return false;

    return true;
}

// =========================================================

int NbTokenOnGrid(Grid *pGrid){

    int cpt = 0;

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].type != TokenTypes_NONE && pGrid->tokens[i][j].type != TokenTypes_BLOCK )
                cpt++;
        }
    }

    return cpt;
}

// =========================================================

void HighlightBestMove(Grid *pGrid){

    int nbPointsBest = 0;
    int nbPointTemp = 0;
    int i1 = 0, i2 = 0, j1 = 0, j2 = 0;

    for(int i = 0; i < pGrid->height-1; i++){
        for(int j = 0; j < pGrid->width-1; j++){

            // vers le haut
            PermuteToken(pGrid,j,i,j+1,i);

            nbPointTemp = Calc_Score(pGrid);

            PermuteToken(pGrid,j,i,j+1,i);

            if(  nbPointTemp > nbPointsBest ){

                i1 = i;
                i2 = i;
                j1 = j;
                j2 = j+1;

                nbPointsBest = nbPointTemp;
            }

            // vers le bas
            PermuteToken(pGrid,j,i,j,i+1);

            nbPointTemp = Calc_Score(pGrid);

            PermuteToken(pGrid,j,i,j,i+1);

            if(  nbPointTemp > nbPointsBest ){

                i1 = i;
                i2 = i+1;
                j1 = j;
                j2 = j;

                nbPointsBest = nbPointTemp;
            }
        }
    }

    pGrid->tokens[i1][j1].image_background = image_cursorGreen;
    pGrid->tokens[i1][j1].drawBackground = true;
    pGrid->tokens[i2][j2].image_background = image_cursorGreen;
    pGrid->tokens[i2][j2].drawBackground = true;
}

// =========================================================

bool IsTokenOfType(Grid *pGrid, TokenTypes type){

   for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].type == type ){ return true; }
        }
    }

    return false;
}

// =========================================================

Token *GetFirstDirToken(Grid *pGrid, int x, Directions dir){

    int DirectionsVectors[4][2] = { {0,-1},{0,1},{-1,0},{1,0} };
    Token T;
    switch(dir){

        case Directions_UP:{

            for(int i = 0; i < pGrid->height; i ++ ){

                if ( pGrid->tokens[i][x].type != TokenTypes_NONE ){

                    return &pGrid->tokens[i][x];
                }
            }
            //Renvoie un token ayant comme position vertical "juste en dehors de la grille
            T.rect_image.y = pGrid->height;

        }
        break;

        case Directions_DOWN:{

            for(int i = pGrid->height-1; i >=0; i -- ){

                if ( pGrid->tokens[i][x].type != TokenTypes_NONE ){

                    return &pGrid->tokens[i][x];
                }
            }
            T.rect_image.y = -1;

        }
        break;

        case Directions_LEFT:{

            for(int i = 0; i < pGrid->width; i ++ ){

                if ( pGrid->tokens[x][i].type != TokenTypes_NONE ){

                    return &pGrid->tokens[x][i];
                }
            }
            T.rect_image.x = pGrid->width;
        }
        break;

        case Directions_RIGHT:{

            for(int i = pGrid->width-1; i >=0 ; i -- ){

                if ( pGrid->tokens[x][i].type != TokenTypes_NONE ){

                    return &pGrid->tokens[x][i];
                }
            }
            T.rect_image.y = -1;
        }
        break;
    }
    return &T;
}

// =========================================================

int DestroyAlignedTokens(Grid *pGrid){

    //fprintf(stdout,"game.c : DestroyAlignedTokens(Grid *pGrid)\n");

    int cpt = 0;
    int score = 0;

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].aligned == true && pGrid->tokens[i][j].isDestruct == false){

                if(pGrid->tokens [i][j].type != TokenTypes_NORMAL && pGrid->tokens [i][j].type != NULL ){

                    score += Token_special_action(pGrid->tokens[i][j].type,pGrid,i,j);
                    i=0;
                    j=0;
                }
                else{

                    pGrid->tokens[i][j].type = TokenTypes_NONE;
                    pGrid->tokens[i][j].color = Colors_NONE;
                    pGrid->tokens[i][j].isDestruct = true;
                    pGrid->tokens[i][j].startDestructAnim = -1;
                    pGrid->tokens[i][j].aligned = false;

                    cpt++;
                }
            }
        }
    }

    cpt += score;

    if(pGrid->isCalc == false)pGrid->score += score * TOKEN_SCORE;

    return cpt;
}

//===========================================================

int Destruct_square(int y,int x, int l, Grid *pGrid){

    int debut_x = x - (l/2);
    int debut_y = y - (l/2);
    //printf("debut x : %d, debut y : %d\n",debut_x,debut_y);
    int cpt = 0;
    //printf("fin x : %d, fin y : %d",debut_x+l-1,debut_y +l-1 );

    for(int i = debut_y; i<debut_y +l && i < pGrid->height;i++)
    {
        for(int j = debut_x; j< debut_x+l && j< pGrid->width;j++)
        {
            if(j >= 0 && i >=0)
            {
                 if(pGrid->tokens[i][j].type != TokenTypes_NORMAL && pGrid->tokens[i][j].type != TokenTypes_NONE && (i != y || j != x))
                {
                 cpt += Token_special_action(pGrid->tokens[i][j].type,pGrid,i,j);
                }
                pGrid->tokens[i][j].type = TokenTypes_NONE;
                pGrid->tokens[i][j].isDestruct = true;
                pGrid->tokens[i][j].startDestructAnim = -1;
                cpt ++;
            }
        }
    }
    CheckGrid(pGrid);
    //printf("nb detruit carre : %d \n",cpt);

    return cpt;
}

//===========================================================

int Destruct_column(int x,Grid * pGrid){

    int cpt =0;
    for(int i =0; i< pGrid->height; i++)
    {
        if(pGrid->tokens[i][x].type != TokenTypes_MULTI)
        {
            if(pGrid->tokens[i][x].type != TokenTypes_NORMAL &&pGrid->tokens[i][x].type !=TokenTypes_NONE &&pGrid->tokens[i][x].type !=TokenTypes_VERTICAL)
            {
                cpt += Token_special_action(pGrid->tokens[i][x].type,pGrid,i,x);
            }
         pGrid->tokens[i][x].type = TokenTypes_NONE;
         pGrid->tokens[i][x].isDestruct = true;
         pGrid->tokens[i][x].startDestructAnim = -1;
         cpt ++;
        }

    }
   // CheckGrid(pGrid);
    printf("nb detruit colonne : %d \n",cpt);

    return cpt;
}

//===========================================================

int Destruct_line(int y,Grid * pGrid){

    int cpt = 0;
    for(int j =0; j< pGrid->width; j++)
    {
        if(pGrid->tokens[y][j].type != TokenTypes_MULTI)
        {
            if(pGrid->tokens[y][j].type != TokenTypes_NORMAL &&pGrid->tokens[y][j].type != TokenTypes_NONE && pGrid->tokens[y][j].type != TokenTypes_HORIZONTAL)
            {
              cpt +=  Token_special_action(pGrid->tokens[y][j].type,pGrid,y,j);
            }
         pGrid->tokens[y][j].type = TokenTypes_NONE;
         pGrid->tokens[y][j].isDestruct = true;
         pGrid->tokens[y][j].startDestructAnim = -1;
         cpt ++;
        }

    }
   // CheckGrid(pGrid);
    printf("nb detruit ligne : %d \n",cpt);

    return cpt;
}
//==================================================================

int Token_special_action(TokenTypes t, Grid *pGrid, int y, int x){

    switch(t){

        case TokenTypes_VERTICAL: return Destruct_column(x, pGrid); break;
        case TokenTypes_HORIZONTAL: return Destruct_line(y, pGrid); break;
        case TokenTypes_BOMB: return Destruct_square(y,x,3,pGrid); break;
    }
}

//==========================================================

int Destruct_color(Colors c, Grid *pGrid){

    int cpt = 0;

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].color == c ){

                if(pGrid->tokens[i][j].type != TokenTypes_NORMAL)
                    cpt += Token_special_action(pGrid->tokens[i][j].type, pGrid, i,j);

                /*pGrid->tokens[i][j].type = TokenTypes_NONE;
                pGrid->tokens[i][j].color = Colors_NONE;
                pGrid->tokens[i][j].isDestruct = true;
                pGrid->tokens[i][j].startDestructAnim = -1;
                pGrid->tokens[i][j].drawBackground = false;*/
                ResetToken(&pGrid->tokens[i][j]);

                cpt++;
            }
        }
    }

    if(pGrid->isCalc == false)
        pGrid->score += cpt * TOKEN_SCORE;

    return cpt;
}


// =========================================================

bool Compare_TokenColor(Token t1, Token t2){

    return ( t1.color == Colors_NONE || t2.color == Colors_NONE )? false : t1.color == t2.color;
}

// =========================================================

bool Compare_TokenColor_color(Token t1, Colors c){

     return ( t1.color == Colors_NONE )? false : t1.color == c;
}

// =========================================================

void ChangeDirection(Grid *pGrid, Directions dir){

    //fprintf(stdout,"game.c : ChangeDirection(Grid *pGrid, Directions dir = %d)\n", dir);
    pGrid->direction = dir;
}

//==========================================================

void RegroupTokens(Grid *pGrid){

    //fprintf(stdout,"game.c : RegroupTokens(Grid *pGrid)\n");

    int DirectionsVectors[4][2] = { {0,-1},{0,1},{-1,0},{1,0} };

    Directions dir = pGrid->direction;

    bool flag = false;
    while ( flag == false ){

        flag = true;

        for(int i = 0; i < pGrid->height; i++){
            for(int j = 0; j < pGrid->width; j++){

                if ( pGrid->tokens[i][j].type != TokenTypes_NONE && pGrid->tokens[i][j].type != TokenTypes_BLOCK ){

                    if( i + DirectionsVectors[dir][1] >= 0 && i + DirectionsVectors[dir][1] < pGrid->height &&
                        j + DirectionsVectors[dir][0] >= 0 && j + DirectionsVectors[dir][0] < pGrid->width ){

                        if ( pGrid->tokens[(i + DirectionsVectors[dir][1])][(j + DirectionsVectors[dir][0])].type == TokenTypes_NONE ){

                            PermuteToken(pGrid, j + DirectionsVectors[dir][0], i + DirectionsVectors[dir][1], j, i );

                            flag = false;
                        }
                    }
                }
            }
        }
    }
}

//==========================================================

Directions ReverseDirection(Directions dir){

    switch(dir){

        case Directions_UP : return Directions_DOWN;
        case Directions_DOWN : return Directions_UP;
        case Directions_RIGHT : return Directions_LEFT;
        case Directions_LEFT : return Directions_RIGHT;
    }

    return dir;
}

// =========================================================

void InjectLigne(Grid *pGrid){

    //fprintf(stdout,"game.c : InjectLigne(Grid *pGrid)\n");

    Directions dir = ReverseDirection(pGrid->direction);

    switch(dir){

        case Directions_UP :{

            for(int j = 0; j < pGrid->width; j++){

                if ( pGrid->tokens[0][j].type == TokenTypes_NONE ){
                    //printf("token == Null\n");
                    if (  ( GetFirstDirToken(pGrid,j,dir)->rect_image.y / TOKEN_HEIGHT ) - 1 < 0 )
                        InitRandomToken(pGrid, &pGrid->tokens[0][j], j, ( GetFirstDirToken(pGrid,j,dir)->rect_image.y / TOKEN_HEIGHT ) - 1 );
                    else
                        InitRandomToken(pGrid, &pGrid->tokens[0][j], j, - 1 );
                }
            }
        }
        break;

        case Directions_DOWN :{
           // printf("inject bas \n");
             for(int j = 0; j < pGrid->width; j++){

                if ( pGrid->tokens[pGrid->height-1][j].type == TokenTypes_NONE ){

                    if (  ( GetFirstDirToken(pGrid,j,dir)->rect_image.y / TOKEN_HEIGHT ) + 1 > pGrid->height-1 )
                        InitRandomToken(pGrid, &pGrid->tokens[pGrid->height-1][j], j, ( GetFirstDirToken(pGrid,j,dir)->rect_image.y / TOKEN_HEIGHT ) +1 );
                    else
                        InitRandomToken(pGrid, &pGrid->tokens[pGrid->height-1][j], j, pGrid->height );
                }
            }
        }
        break;

        case Directions_LEFT :{
            for(int i =0; i<pGrid->height; i++){

                if(pGrid->tokens[i][0].type==TokenTypes_NONE)
                {
                    if( (GetFirstDirToken(pGrid,i,dir)->rect_image.x /TOKEN_WIDTH) -1 < 0)
                        InitRandomToken(pGrid, &pGrid->tokens[i][0], ( GetFirstDirToken(pGrid,i,dir)->rect_image.x / TOKEN_WIDTH ) -1, i );
                    else
                        InitRandomToken(pGrid, &pGrid->tokens[i][0], -1, i );
                }
            }
        }
        break;

        case Directions_RIGHT :{
            for(int i =0; i<pGrid->height; i++){

                if(pGrid->tokens[i][pGrid->width-1].type==TokenTypes_NONE)
                {
                    if( (GetFirstDirToken(pGrid,i,dir)->rect_image.x /TOKEN_WIDTH) +1 > pGrid->width-1)
                        InitRandomToken(pGrid, &pGrid->tokens[i][pGrid->width-1], ( GetFirstDirToken(pGrid,i,dir)->rect_image.x / TOKEN_WIDTH ) +1, i );
                    else
                        InitRandomToken(pGrid, &pGrid->tokens[i][pGrid->width-1], pGrid->width-1, i );
                }
            }
        }
        break;
    }
}

// =========================================================

void PermuteToken(Grid *pGrid,int x1,int y1,int x2,int y2){

    Token tmp = pGrid->tokens[y1][x1];
    pGrid->tokens[y1][x1] = pGrid->tokens[y2][x2];
    pGrid->tokens[y2][x2] = tmp;
}

// =========================================================

void PermuteTokenImage(Grid *pGrid,int x1,int y1,int x2,int y2){

    Token tmp = pGrid->tokens[y1][x1];

    pGrid->tokens[y1][x1].image = pGrid->tokens[y2][x2].image;
    pGrid->tokens[y1][x1].image_background = pGrid->tokens[y2][x2].image_background;
    pGrid->tokens[y1][x1].drawBackground = pGrid->tokens[y2][x2].drawBackground;

    pGrid->tokens[y2][x2].image = tmp.image;
    pGrid->tokens[y2][x2].image_background = tmp.image_background;
    pGrid->tokens[y2][x2].drawBackground = tmp.drawBackground;
}

// =========================================================
// Evenements
// =========================================================

void GameButton_help_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid ){

    if ( UI_button_event(pButton, pEvent, pDraw) && pGrid->nbHelp > 0 ){

        pGrid->isHelpActive = true;
        pGrid->nbHelp --;

        pButton->draw = pGrid->nbHelp > 0;
    }
}

// =========================================================

void GameButton_superHelp_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid ){

    if ( UI_button_event(pButton, pEvent, pDraw) && pGrid->nbSuperHelp > 0 ){

        pGrid->isSuperHelpActive = true;
        pGrid->nbSuperHelp --;

        pButton->draw = pGrid->nbSuperHelp > 0;
    }
}

// =========================================================

void GameButton_revertOnce_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        LoadTokensInPastTokens(pGrid);
        pGrid->nbRevertOnce --;

        pButton->draw = pGrid->nbRevertOnce > 0;
    }
}

// =========================================================

void GameButton_restart_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        *pQuit = true;
    }
}

// =========================================================

void GameButton_return_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        gameState = gameState_prec;
        *pQuit = true;
    }
}

// =========================================================

void GameButton_quit_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        gameState = States_QUIT;
        *pQuit = true;
    }
}

// =========================================================

void GameButton_replay_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        *pQuit = true;
    }
}

// =========================================================

void Game_event(Grid *pGrid, SDL_Event *pEvent, bool *pQuit, bool *pDragAndDrop, SDL_Point *pDragStart){

    switch(pEvent->type){

        case SDL_KEYDOWN :{

            switch( pEvent->key.keysym.sym ){

                case SDLK_d :{

                    if( CHEAT_ENABLE != 0 ) pGrid->nbMove ++;
                }
                break;

                case SDLK_q :{

                    if( CHEAT_ENABLE != 0 ) pGrid->nbMove --;
                }
                break;

                case SDLK_z :{

                    if( CHEAT_ENABLE != 0 ){

                        if(pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].color < pGrid->nbColor-1){

                            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].color ++;
                        }
                        else {

                            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].color = 0;
                        }
                        CalculTokenImages(pGrid,&pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x],pGrid->cursorTokenPosition.x,pGrid->cursorTokenPosition.y);

                        CalculTokenImages(  pGrid,
                                            &pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x],
                                            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].rect_image.x/TOKEN_WIDTH,
                                            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].rect_image.y/TOKEN_HEIGHT);
                    }
                }
                break;

                case SDLK_s :{

                    if( CHEAT_ENABLE != 0 ){

                        if(pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].color >0){

                            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].color --;
                        }
                        else{
                            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].color = pGrid->nbColor-1;
                        }
                        CalculTokenImages(pGrid,&pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x],pGrid->cursorTokenPosition.x,pGrid->cursorTokenPosition.y);


                        CalculTokenImages(  pGrid,
                                            &pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x],
                                            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].rect_image.x/TOKEN_WIDTH,
                                            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].rect_image.y/TOKEN_HEIGHT);
                    }
                }
                break;

                case SDLK_e : {

                    if( CHEAT_ENABLE != 0 )
                        if ( !pGrid->is_puzzle )
                            RandomizeGrid(pGrid);
                }
                break;


                case SDLK_a : {

                    if( CHEAT_ENABLE != 0 ){

                        if(pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].type >1){

                            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].type --;
                        }
                        else{
                            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].type = 5;
                        }
                        CalculTokenImages(pGrid,&pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x],pGrid->cursorTokenPosition.x,pGrid->cursorTokenPosition.y);


                        CalculTokenImages(  pGrid,
                                            &pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x],
                                            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].rect_image.x/TOKEN_WIDTH,
                                            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].rect_image.y/TOKEN_HEIGHT);
                    }
                }
                break;


                case SDLK_j :{

                    if( CHEAT_ENABLE != 0 )
                        if ( !pGrid->is_puzzle )
                            ChangeDirection(pGrid,Directions_LEFT);

                }
                break;

                case SDLK_i :{

                    if( CHEAT_ENABLE != 0 )
                        if ( !pGrid->is_puzzle )
                            ChangeDirection(pGrid,Directions_UP);

                }
                break;


                case SDLK_k :{

                    if( CHEAT_ENABLE != 0 )
                        if ( !pGrid->is_puzzle )
                            ChangeDirection(pGrid,Directions_DOWN);

                }
                break;


                case SDLK_l :{

                    if( CHEAT_ENABLE != 0 )
                        if ( !pGrid->is_puzzle )
                            ChangeDirection(pGrid,Directions_RIGHT);

                }
                break;
            }
        }

        // bouttons de souris appuié
        case SDL_MOUSEBUTTONDOWN:{

            switch(pEvent->button.button){

                // bouton gauche
                case SDL_BUTTON_LEFT:{

                    if ( IsTokenMoving(pGrid) == false && IsTokenDestructing(pGrid) == false && pGrid->is_cursorOnGrid == true && pGrid->nbMove > 0 ){

                        pDragStart->x = pGrid->cursorTokenPosition.x;
                        pDragStart->y = pGrid->cursorTokenPosition.y;

                        *pDragAndDrop = pGrid->tokens[pDragStart->y][pDragStart->x].type != TokenTypes_NONE && pGrid->tokens[pDragStart->y][pDragStart->x].type != TokenTypes_BLOCK;
                    }
                }
                break;

                // click droit pour afficher les stats du jeton
                case SDL_BUTTON_RIGHT:{

                    if ( pGrid->is_cursorOnGrid == true ){

                        fprintf(stdout,"Jeton en posisition (%d,%d) : ",pGrid->cursorTokenPosition.x, pGrid->cursorTokenPosition.y);
                        DebugToken(pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x]);
                        fprintf(stdout,"\n");
                    }
                }
                break;
            }
        }
        break;

        case SDL_MOUSEBUTTONUP:{

            switch(pEvent->button.button){

                case SDL_BUTTON_LEFT:{

                    if ( *pDragAndDrop == true ){

                        SDL_Point dragEnd;

                        dragEnd.x = pGrid->cursorTokenPosition.x;
                        dragEnd.y = pGrid->cursorTokenPosition.y;

                        fprintf(stdout,"Distance du drag : %d, %d.\n",dragEnd.x - pDragStart->x, dragEnd.y - pDragStart->y);

                        int distX = sqrt( pow( dragEnd.x - pDragStart->x, 2) );
                        int distY = sqrt( pow( dragEnd.y - pDragStart->y, 2) );

                        if ( ( distX == 1 && distY == 0 ) || ( distX == 0 && distY == 1 ) ){

                            //if ( pGrid->tokens[dragEnd.y][dragEnd.x].type != TokenTypes_NONE && pGrid->tokens[dragEnd.y][dragEnd.x].type != TokenTypes_BLOCK ){

                                HardPermuteToken(pGrid, pDragStart->x, pDragStart->y, dragEnd.x, dragEnd.y);

                                if ( IsLineOnGrid(pGrid) == false && pGrid->tokens[dragEnd.y][dragEnd.x].type != TokenTypes_MULTI && pGrid->tokens[pDragStart->y][pDragStart->x].type != TokenTypes_MULTI){

                                    HardPermuteToken(pGrid, dragEnd.x, dragEnd.y, pDragStart->x, pDragStart->y);
                                }
                                else {

                                    ResetTokenImages(pGrid);

                                    HardPermuteToken(pGrid, dragEnd.x, dragEnd.y, pDragStart->x, pDragStart->y);
                                    SaveTokensInPastTokens(pGrid);
                                    HardPermuteToken(pGrid, dragEnd.x, dragEnd.y, pDragStart->x, pDragStart->y);

                                    /* coups réussi */
                                    pGrid->nbMove --;

                                    if ( pGrid->nbMove <= 0 ){

                                        pGrid->nbHelp = 0;
                                        pGrid->nbSuperHelp = 0;
                                        pGrid->nbRevertOnce = 0;
                                    }

                                    if(pGrid->tokens[dragEnd.y][dragEnd.x].type == TokenTypes_MULTI )
                                    {
                                        Destruct_color(pGrid->tokens[pDragStart->y][pDragStart->x].color, pGrid);
                                        pGrid->tokens[dragEnd.y][dragEnd.x].isDestruct = true;
                                        pGrid->tokens[dragEnd.y][dragEnd.x].type = TokenTypes_NONE;
                                        pGrid->tokens[dragEnd.y][dragEnd.x].startDestructAnim = -1;
                                    }else if(pGrid->tokens[pDragStart->y][pDragStart->x].type == TokenTypes_MULTI)
                                    {
                                        Destruct_color(pGrid->tokens[dragEnd.y][dragEnd.x].color, pGrid);
                                        pGrid->tokens[pDragStart->y][pDragStart->x].isDestruct = true;
                                        pGrid->tokens[pDragStart->y][pDragStart->x].type = TokenTypes_NONE;
                                        pGrid->tokens[pDragStart->y][pDragStart->x].startDestructAnim = -1;
                                    }

                                    pGrid->isHelpActive = false;
                                    pGrid->isSuperHelpActive = false;

                                    if(pGrid->is_randomizeInsert == true){

                                        ChangeDirection(pGrid, rand()%4);
                                    }
                                }
                            //}
                        }

                        *pDragAndDrop = false;
                    }
                }
                break;
            }
        }
        break;

        // mouvement de souris
        case SDL_MOUSEMOTION:{

            pGrid->is_cursorOnGrid = PointInRect(pEvent->motion.x, pEvent->motion.y, &pGrid->rect);

            if ( pGrid->is_cursorOnGrid == true ){

                SDL_Point cursorTokenPositionTemp;
                cursorTokenPositionTemp.x = pGrid->cursorTokenPosition.x;
                cursorTokenPositionTemp.y = pGrid->cursorTokenPosition.y;

                pGrid->cursorTokenPosition.x = (pEvent->motion.x / TOKEN_WIDTH);
                pGrid->cursorTokenPosition.y = (pEvent->motion.y / TOKEN_HEIGHT);

                // si drag and drop et changement de case
                if ( *pDragAndDrop && ( cursorTokenPositionTemp.x != pGrid->cursorTokenPosition.x || cursorTokenPositionTemp.y != pGrid->cursorTokenPosition.y ) ){

                    int distX = sqrt( pow( pGrid->cursorTokenPosition.x - pDragStart->x, 2) );
                    int distY = sqrt( pow( pGrid->cursorTokenPosition.y - pDragStart->y, 2) );

                    if ( ( distX == 1 && distY == 0 ) || ( distX == 0 && distY == 1 ) ){

                        if ( pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].type != TokenTypes_NONE && pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].type != TokenTypes_BLOCK ){

                            PermuteToken(pGrid, pDragStart->x, pDragStart->y, pGrid->cursorTokenPosition.x, pGrid->cursorTokenPosition.y);

                            if ( IsLineOnGrid(pGrid) == false && pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].type != TokenTypes_MULTI && pGrid->tokens[pDragStart->y][pDragStart->x].type != TokenTypes_MULTI ){

                                //fprintf(stdout,"game.c -> Game_event(Grid *pGrid, SDL_Event *pEvent, bool *pQuit) -> switch(pEvent->type) -> case SDL_MOUSEMOTION -> !IsLineOnGrid(pGrid) ");
                                pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].image_background = image_cursorRed;
                                pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].drawBackground = true;
                            }

                            if( pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].type == TokenTypes_MULTI){

                                ChangeColorTokenBackgroundImage(pGrid,image_cursorGreen,pGrid->tokens[pDragStart->y][pDragStart->x].color);

                            }else if(pGrid->tokens[pDragStart->y][pDragStart->x].type == TokenTypes_MULTI){

                                ChangeColorTokenBackgroundImage(pGrid,image_cursorGreen,pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].color);
                            }

                            ChangeAlignedTokenBackgroundImage(pGrid, image_cursorGreen);

                            PermuteToken(pGrid, pDragStart->x, pDragStart->y, pGrid->cursorTokenPosition.x, pGrid->cursorTokenPosition.y);

                            PermuteTokenImage(pGrid, pDragStart->x, pDragStart->y, pGrid->cursorTokenPosition.x, pGrid->cursorTokenPosition.y);
                        }
                    }
                    else{

                        ResetTokenImages(pGrid);
                        pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].drawBackground = true;

                        if  ( !( distX == 1 && distY == 0 ) && !( distX == 0 && distY == 1 ) && !( distX == 0 && distY == 0 ) )
                            *pDragAndDrop = false;
                    }
                }
                else if ( *pDragAndDrop == false ){

                    pGrid->tokens[cursorTokenPositionTemp.y][cursorTokenPositionTemp.x].drawBackground = false;
                    pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].drawBackground = true;
                }
            }
            else{

                pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].drawBackground = false;

                if ( *pDragAndDrop == true ){

                    *pDragAndDrop = false;
                }
            }
        }
        break;
    }
}

// =========================================================
// Logiques
// =========================================================

void Game_logic(Grid *pGrid, bool *pQuit){

    //fprintf(stdout,"game.c : Game_logic(Grid *pGrid)\n");

    if ( IsTokenMoving(pGrid) == false && IsTokenDestructing(pGrid) == false){

        if( IsLineOnGrid(pGrid) == true ){

            // score
            pGrid->score += Calc_Score(pGrid);

            //Token speciaux
            Token_speciaux(pGrid);

            // détruit les lignes et remplie les cases manquantes du tableau
            fprintf(stdout,"Nombre de jeton detruit(s) : %d\n", DestroyAlignedTokens(pGrid) );

        }
        else {

            if( !pGrid->is_puzzle ){

                while( IsTokenOfType(pGrid, TokenTypes_NONE ) == true ){

                    // regroupe tout les jetons
                    RegroupTokens(pGrid);

                    // remplie les espaces vides
                    InjectLigne(pGrid);
                }

            } else RegroupTokens(pGrid);

            // recalcul les mouvements possibles
            MoveAvailable(pGrid);

            if(pGrid->moveAvailable == 0 && !pGrid->is_puzzle){

                RandomizeGrid(pGrid);
            }
        }
    }
}

// =========================================================

void GameSessionRandom(int gridWidth, int gridHeight,int nbColor, int nbMove,bool randomizeInsert, int nbHelp, int nbSuperHelp, int nbRevertOnce){

    fprintf(stdout, "game.c -> GameSessionRandom(...) : start \n");

    SDL_Renderer *pRenderer;    // renderer = canvas ( endroit où l'on va déssiner )
    SDL_Event event;            // gestionnaire d'évènements
    Array objects;
    Window window;

    SDL_Color black = {0,0,0,255};
    SDL_Color white = {255,255,255,255};

    UI_label label_score = {false};
    UI_button button_return = {false};
    UI_button button_quit = {false};
    UI_button button_help = {false};
    UI_button button_superHelp = {false};
    UI_button button_revertOnce = {false};
    UI_button button_replay = {false};
    UI_label label_mouvements = {false};
    UI_label label_nbMove = {false};
    UI_label label_end = {false};

    // création des zone de jeu et d'affichage
    SDL_Rect rect_grid = { 0,0,gridWidth * TOKEN_WIDTH, gridHeight * TOKEN_HEIGHT };
    SDL_Rect rect_UI = { rect_grid.x + rect_grid.w, 0, 250, rect_grid.h };

    SDL_Rect rect_screen = {0 ,
                            0 ,
                            (rect_grid.w + rect_grid.x > rect_UI.w + rect_UI.x ) ? rect_grid.w + rect_grid.x : rect_UI.w + rect_UI.x ,
                            (rect_grid.h + rect_grid.y > rect_UI.h + rect_UI.y ) ? rect_grid.h + rect_grid.y : rect_UI.h + rect_UI.y } ;

    Array_new(&objects);

    pRenderer = InitGame("Candy Crush Clone C", &objects, rect_screen.w, rect_screen.h );
    if ( !pRenderer )
        return;

    Grid *grid1 = NewGrid(rect_grid,nbMove,nbColor,randomizeInsert,nbHelp,nbSuperHelp,nbRevertOnce);

    fprintf(stdout,"game.c -> GameSessionRandom(...) -> Window_new return %d.\n", Window_new(&window, NULL, false, 0, 0, screen_width, screen_height));
    fprintf(stdout,"game.c -> GameSessionRandom(...) -> UI_label_new return %d.\n", UI_label_new(&label_score, &window, "Test", rect_UI.x + 20 , rect_UI.y + 20 ));
    fprintf(stdout,"game.c -> GameSessionRandom(...) -> UI_label_new return %d.\n", UI_label_new(&label_nbMove, &window, "Test", rect_UI.x + 20 , rect_UI.y + 40 ));
    fprintf(stdout,"game.c -> GameSessionRandom(...) -> UI_label_new return %d.\n", UI_label_new(&label_mouvements, &window, "Test", rect_UI.x + 20 , rect_UI.y + 60 ));

    fprintf(stdout,"game.c -> GameSessionRandom(...) -> UI_label_new return %d.\n", UI_label_new(&label_end, &window, "Termine !", rect_grid.x + rect_grid.w/2 - TextWidth(font_hight, "Termine !", NULL)/2, rect_grid.y + rect_grid.h/2 - font_hight.fontHeight/2 ));
    label_end.font = font_hight;

    sprintf(label_score.text,"Score : %d ",grid1->score);
    sprintf(label_nbMove.text,"NbCoups : %d", grid1->nbMove);
    sprintf(label_mouvements.text,"Mouvement possible : %d",grid1->moveAvailable);

    fprintf(stdout,"game.c -> GameSessionRandom(...) -> UI_button_new return %d.\n", UI_button_new(&button_quit, &window, "Quitter", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 50 ));
    fprintf(stdout,"game.c -> GameSessionRandom(...) -> UI_button_new return %d.\n", UI_button_new(&button_return, &window, "Retour menu", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 90 ));
    fprintf(stdout,"game.c -> GameSessionRandom(...) -> UI_button_new return %d.\n", UI_button_new(&button_replay, &window, "Rejouer", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 130 ));

    fprintf(stdout,"game.c -> GameSessionRandom(...) -> UI_button_new return %d.\n", UI_button_new(&button_help, &window, "Aide ( 0 )", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 240 ));
    fprintf(stdout,"game.c -> GameSessionRandom(...) -> UI_button_new return %d.\n", UI_button_new(&button_superHelp, &window, "Super aide ( 0 )", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 280 ));
    fprintf(stdout,"game.c -> GameSessionRandom(...) -> UI_button_new return %d.\n", UI_button_new(&button_revertOnce, &window, "Retour arriere ( 0 )", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 320 ));

    sprintf(button_help.text,"Aide ( %d )",grid1->nbHelp);
    sprintf(button_superHelp.text,"Super aide ( %d )",grid1->nbSuperHelp);
    sprintf(button_revertOnce.text,"Retour arriere ( %d )",grid1->nbRevertOnce);

    window.visible = true;

    bool draw = true; // non utilisé
    bool quit = false;
    bool dragAndDrop = false;
    SDL_Point dragStart = {0, 0};

    while( !quit ){

        int frameStart = SDL_GetTicks();

        /* évènements */
        while( SDL_PollEvent( &event ) != 0 ){

            if( event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)){

                gameState = States_QUIT;
                quit = true;
            }

            // entré lié a la grille
            Game_event(grid1, &event, &quit, &dragAndDrop, &dragStart);

            // event UI
            Window_event(&window, &event, &draw );
            GameButton_return_event(&button_return, &event, &draw, &quit);
            GameButton_quit_event(&button_quit, &event, &draw, &quit);
            GameButton_replay_event(&button_replay, &event, &draw, &quit);
            GameButton_help_event(&button_help, &event, &draw, grid1);
            GameButton_superHelp_event(&button_superHelp, &event, &draw, grid1);
            GameButton_revertOnce_event(&button_revertOnce, &event, &draw, grid1);
        }

        /* logique */
        Game_logic(grid1, &quit);

        /* maj des labels */

        sprintf(label_nbMove.text," NbCoups : %d", grid1->nbMove);
        sprintf(label_score.text,"Score : %d ", grid1->score);
        sprintf(label_mouvements.text,"Nombre de mouvement : %d",grid1->moveAvailable);
        sprintf(button_help.text,"Aide ( %d )",grid1->nbHelp);
        sprintf(button_superHelp.text,"Super aide ( %d )",grid1->nbSuperHelp);
        sprintf(button_revertOnce.text,"Retour arriere ( %d )",grid1->nbRevertOnce);

        /* animations */
        Grid_anim(grid1);

        /* affichage */
        SDL_RenderClear(pRenderer);                                                             // efface tout le contenu du renderer

        Window_draw(&window, pRenderer);

        Grid_draw(grid1,pRenderer);                                                              // désine la grille sur le renderer

        UI_label_draw(&label_score,pRenderer);
        UI_label_draw(&label_nbMove,pRenderer);
        UI_label_draw(&label_mouvements,pRenderer);
        UI_button_draw(&button_return, pRenderer);
        UI_button_draw(&button_quit, pRenderer);
        UI_button_draw(&button_help, pRenderer);
        UI_button_draw(&button_superHelp, pRenderer);
        UI_button_draw(&button_revertOnce, pRenderer);
        UI_button_draw(&button_replay, pRenderer);

        if ( IsRandomGridStabilized(grid1) && grid1->nbMove <= 0 ){

            button_help.draw = false;
            button_revertOnce.draw = false;
            button_superHelp.draw = false;
            UI_fillRect(pRenderer, &rect_grid, white);
            UI_label_draw(&label_end,pRenderer);
        }

        UI_outline(pRenderer, &rect_UI, black, -1 );

        SDL_RenderPresent(pRenderer);                                                           // déssine le renderer à l'écran

        /* gestion de la fréquence d'affichage ( pour les animations )*/
        WaitForNextFrame(frameStart);
    }

    /* fin du programme */
    Clean(&objects);

    fprintf(stdout, "game.c -> GameSessionRandom(...) : end \n");
}

// =========================================================

void GameSessionPuzzle(Grid *pGrid){

    fprintf(stdout, "game.c -> GameSessionPuzzle(...) : start \n");

    SDL_Renderer *pRenderer;    // renderer = canvas ( endroit où l'on va déssiner )
    SDL_Event event;            // gestionnaire d'évènements
    Array objects;
    Window window;

    SDL_Color black = {0,0,0,255};
    SDL_Color white = {255,255,255,255};

    UI_button button_quit = {false};
    UI_button button_return = {false};
    UI_button button_restart = {false};
    UI_label label_nbMove = {false};
    UI_label label_end = {false};

    // création des zone de jeu et d'affichage
    SDL_Rect rect_grid = { 0,0,pGrid->width * TOKEN_WIDTH, pGrid->height * TOKEN_HEIGHT };
    SDL_Rect rect_UI = { rect_grid.x + rect_grid.w, 0, 250, rect_grid.h };

    SDL_Rect rect_screen = {0 ,
                            0 ,
                            (rect_grid.w + rect_grid.x > rect_UI.w + rect_UI.x ) ? rect_grid.w + rect_grid.x : rect_UI.w + rect_UI.x ,
                            (rect_grid.h + rect_grid.y > rect_UI.h + rect_UI.y ) ? rect_grid.h + rect_grid.y : rect_UI.h + rect_UI.y } ;

    Array_new(&objects);

    pRenderer = InitGame("Candy Crush Clone C", &objects, rect_screen.w, rect_screen.h );
    if ( !pRenderer )
        return;

    ResetTokenImages(pGrid);

    fprintf(stdout,"game.c -> GameSessionPuzzle(...) -> Window_new return %d.\n", Window_new(&window, NULL, false, 0, 0, screen_width, screen_height));
    fprintf(stdout,"game.c -> GameSessionPuzzle(...) -> UI_label_new return %d.\n", UI_label_new(&label_nbMove, &window, "Test", rect_UI.x + 20 , rect_UI.y + 40 ));

    fprintf(stdout,"game.c -> GameSessionPuzzle(...) -> UI_label_new return %d.\n", UI_label_new(&label_end, &window, "", rect_grid.x + rect_grid.w/2, rect_grid.y + rect_grid.h/2 - font_hight.fontHeight/2 ));
    label_end.font = font_hight;

    sprintf(label_nbMove.text,"NbCoups : %d", pGrid->nbMove);

    fprintf(stdout,"game.c -> GameSessionPuzzle(...) -> UI_button_new return %d.\n", UI_button_new(&button_quit, &window, "Quitter", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 50 ));
    fprintf(stdout,"game.c -> GameSessionPuzzle(...) -> UI_button_new return %d.\n", UI_button_new(&button_return, &window, "Retour", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 90 ));
    fprintf(stdout,"game.c -> GameSessionPuzzle(...) -> UI_button_new return %d.\n", UI_button_new(&button_restart, &window, "Recommencer", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 130 ));

    window.visible = true;

    bool draw = true; // non utilisé
    bool quit = false;
    bool dragAndDrop = false;
    SDL_Point dragStart = {0, 0};

    while( !quit ){

        int frameStart = SDL_GetTicks();

        /* évènements */
        while( SDL_PollEvent( &event ) != 0 ){

            if( event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)){

                gameState = States_QUIT;
                quit = true;
            }

            // entré lié a la grille
            Game_event(pGrid, &event, &quit, &dragAndDrop, &dragStart);

            // event UI
            Window_event(&window, &event, &draw );
            GameButton_quit_event(&button_quit, &event, &draw, &quit);
            GameButton_return_event(&button_return, &event, &draw, &quit);
            GameButton_restart_event(&button_restart, &event, &draw, &quit);
        }

        /* logique */
        Game_logic(pGrid, &quit);

        /* maj des labels */
        sprintf(label_nbMove.text," NbCoups : %d", pGrid->nbMove);

        /* animations */
        Grid_anim(pGrid);

        /* affichage */
        SDL_RenderClear(pRenderer);                                                             // efface tout le contenu du renderer

        Window_draw(&window, pRenderer);

        Grid_draw(pGrid,pRenderer);                                                              // désine la grille sur le renderer

        UI_label_draw(&label_nbMove,pRenderer);
        UI_button_draw(&button_quit, pRenderer);
        UI_button_draw(&button_return, pRenderer);
        UI_button_draw(&button_restart, pRenderer);

        if ( !IsTokenMoving(pGrid) && !IsTokenDestructing(pGrid) && !IsLineOnGrid(pGrid) && ( pGrid->nbMove <= 0 || NbTokenOnGrid(pGrid) == 0) ){

            if ( NbTokenOnGrid(pGrid) == 0 ){

                String_copy(&label_end.text, UI_MAX_LENGTH, "Gagné !", NULL);
            }
            else{

                String_copy(&label_end.text, UI_MAX_LENGTH, "Perdu !", NULL);
            }

            label_end.rect.x = rect_grid.x + rect_grid.w/2 - TextWidth(font_hight ,label_end.text, NULL);

            UI_fillRect(pRenderer, &rect_grid, white);
            UI_label_draw(&label_end,pRenderer);
        }

        UI_outline(pRenderer, &rect_UI, black, -1 );

        SDL_RenderPresent(pRenderer);                                                           // déssine le renderer à l'écran

        /* gestion de la fréquence d'affichage ( pour les animations )*/
        WaitForNextFrame(frameStart);
    }

    /* fin du programme */
    Clean(&objects);

    fprintf(stdout, "game.c -> GameSessionPuzzle(...) : end \n");
}

// =========================================================











