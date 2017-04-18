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
    if ( pWindow ) Array_append(pArray, WINDOW_TYPE , pWindow);
    // void SDL_SetWindowIcon(SDL_Window*  window , SDL_Surface* icon); //ajoute une icône à la fenêtre

    pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if ( pRenderer ) Array_append(pArray, RENDERER_TYPE, pRenderer);

    // init des ressources
    error += Font_new(&font_default, "data/fonts/arial.ttf", pArray, 15);
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
    pGrid->direction =DOWN;
    pGrid->score = 0;
    pGrid->is_randomizeInsert = randomizeInsert;
    pGrid->nbHelp = nbHelp;
    pGrid->nbSuperHelp = nbSuperHelp;
    pGrid->nbRevertOnce = nbRevertOnce;

    MakeRect(&pGrid->rect, rect.x, rect.y, rect.w, rect.h);

    pGrid->is_cursorOnGrid = false;
    pGrid->cursorTokenPosition.x = 0;
    pGrid->cursorTokenPosition.y = 0;
    pGrid->isHelpActive = false;

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

    while( IsLineOnGrid(pGrid) == true || IsTokenOfType(pGrid, NONE ) == true ){

        if( IsLineOnGrid(pGrid) == true ){

            // détruit les lignes et remplie les cases manquantes du tableau
            DestroyAlignedTokens(pGrid);
        }
        else {

            while( IsTokenOfType(pGrid, NONE ) == true ){

                // regroupe tout les jetons
                RegroupTokens(pGrid);

                // remplie les espaces vides
               InjectLigne(pGrid);
            }
        }
    }

    // recalcul les mouvements possibles
    MoveAvailable(pGrid, false);
}

// =========================================================

void InitRandomToken(Grid *pGrid, Token *token, int x, int y){

    token->type = TOKEN;
    token->color = (Colors)(rand() % pGrid->nbColor);
    token->isMoving = false;
    token->isDestruct = false;
    token->startDestructAnim = -1;

    token->textureSize = 100;
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

            if ( pGrid->tokens[i][j].type != NONE ){

                // vérification verticale
                if ( i > 0 && i < pGrid->height - 1){

                    if ( Compare_TokenColor(pGrid->tokens[i][j], pGrid->tokens[i-1][j]) && Compare_TokenColor(pGrid->tokens[i][j],pGrid->tokens[i+1][j]) ){

                        pGrid->tokens[i][j].aligned = true;
                        pGrid->tokens[i-1][j].aligned = true;
                        pGrid->tokens[i+1][j].aligned = true;
                    }
                }

                // vérification horizontale
                if ( j > 0 && j < pGrid->width - 1){

                    if ( Compare_TokenColor(pGrid->tokens[i][j],pGrid->tokens[i][j-1])&& Compare_TokenColor(pGrid->tokens[i][j], pGrid->tokens[i][j+1]) ){

                        pGrid->tokens[i][j].aligned = true;
                        pGrid->tokens[i][j-1].aligned = true;
                        pGrid->tokens[i][j+1].aligned = true;
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
void Token_speciaux(Grid *pGrid)
{
    int vecteur_point[4][2][2] = {{{0,1},{1,0}} ,{{0,-1},{1,0}} ,{{0,1},{-1,0}},{{0,-1},{-1,0}}};
    Colors save_color;
    int cpt = 0;
    bool coude = false;
    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){
            coude = false;
            if ( pGrid->tokens[i][j].aligned == true && pGrid->tokens[i][j].type == TOKEN){
                    save_color = pGrid->tokens[i][j].color;
                for(int n =0; n<3; n++)
                {
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

                 if(coude == true)
                    {
                        pGrid->tokens[i][j].aligned = false;
                        pGrid->tokens[i][j].type = PACKED;
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
                while(pGrid->tokens[i][j-value_random].type != TOKEN && cpt<200);
                pGrid->tokens[i][j-value_random].type = HORIZONTAL;
                pGrid->tokens[i][j-value_random].aligned = false;
                pGrid->tokens[i][j-value_random].isDestruct = false;
                CalculTokenImages(pGrid,&pGrid->tokens[i][j-value_random],j-value_random,i);
             }

             if(nb_align == 5){
                        pGrid->tokens[i][j-3].aligned = false;
                        pGrid->tokens[i][j-3].color = NONE_COLOR;
                        pGrid->tokens[i][j-3].type = MULTI;
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
                while(pGrid->tokens[i][j-value_random].type != TOKEN && cpt<200);
                pGrid->tokens[i][j-value_random].type = HORIZONTAL;

                pGrid->tokens[i][j-value_random].aligned = false;
                CalculTokenImages(pGrid,&pGrid->tokens[i][j-value_random],j-value_random,i);

         }

         if(nb_align == 5){
                        pGrid->tokens[i][j-3].aligned = false;
                        pGrid->tokens[i][j-3].color = NONE_COLOR;
                        pGrid->tokens[i][j-3].type = MULTI;
                        CalculTokenImages(pGrid,&pGrid->tokens[i][j-3],j-3,i);

         }


    }


    //Verification des alignements verticaux
    for(j = 0; j < pGrid->width; j++){
        Token_save = pGrid->tokens[0][j];
        nb_align = 1;

        for( i = 1; i < pGrid->height; i++){

            if(Compare_TokenColor(pGrid->tokens[i][j], Token_save)){

                nb_align++;
               // printf("align = %d\n", nb_align);
            }
            else {

             if(nb_align==4){
                //Si ligne de 4 fait apparaitre un jeton horizontal qqpart dans la ligne
                //printf("ligne de 4\n");
                cpt=0; //Anti bouclage infini
                do{
                value_random = 1+ rand()%4;
                cpt++;
                }
                while(pGrid->tokens[i-value_random][j].type != TOKEN && cpt<200);
                pGrid->tokens[i-value_random][j].type =VERTICAL;
                pGrid->tokens[i-value_random][j].aligned = false;
                pGrid->tokens[i-value_random][j].isDestruct = false;
                CalculTokenImages(pGrid,&pGrid->tokens[i-value_random][j],j,i-value_random);
             }

             if(nb_align == 5){
                        pGrid->tokens[i][j-3].aligned = false;
                        pGrid->tokens[i][j-3].color = NONE_COLOR;
                        pGrid->tokens[i][j-3].type = MULTI;
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
                while(pGrid->tokens[i-value_random][j].type != TOKEN && cpt<200);
                pGrid->tokens[i-value_random][j].type = VERTICAL;

                pGrid->tokens[i-value_random][j].aligned = false;
                CalculTokenImages(pGrid,&pGrid->tokens[i-value_random][j],j,i-value_random);

         }

         if(nb_align == 5){
                        pGrid->tokens[i-3][j].aligned = false;
                        pGrid->tokens[i-3][j].color = NONE_COLOR;
                        pGrid->tokens[i-3][j].type = MULTI;
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

                    //printf("Score de la ligne : %d \n", score);
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

            if ( pGrid->tokens[i][j].aligned == true ){ return true; }
        }
    }

    return false;
}

// =========================================================

bool IsTokenMoving(Grid *pGrid){

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].isMoving == true ){ return true; }
        }
    }

    return false;
}

// =========================================================

bool IsTokenDestructing(Grid *pGrid){

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].isDestruct == true ){ return true; }
        }
    }

    return false;
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

Token *GetFirstDirToken(Grid *pGrid, int x, Directions dir)
{
    int DirectionsVectors[4][2] = { {0,-1},{0,1},{-1,0},{1,0} };
    Token T;
    switch(dir){

        case UP:{

            for(int i = 0; i < pGrid->height; i ++ ){

                if ( pGrid->tokens[i][x].type != NONE ){

                    return &pGrid->tokens[i][x];
                }
            }
            //Renvoie un token ayant comme position vertical "juste en dehors de la grille
            T.rect_image.y = pGrid->height;

        }
        break;

        case DOWN:{

            for(int i = pGrid->height-1; i >=0; i -- ){

                if ( pGrid->tokens[i][x].type != NONE ){

                    return &pGrid->tokens[i][x];
                }
            }
            T.rect_image.y = -1;

        }
        break;

        case LEFT:{

            for(int i = 0; i < pGrid->width; i ++ ){

                if ( pGrid->tokens[x][i].type != NONE ){

                    return &pGrid->tokens[x][i];
                }
            }
            T.rect_image.x = pGrid->width;
        }
        break;

        case RIGHT:{

            for(int i = pGrid->width-1; i >=0 ; i -- ){

                if ( pGrid->tokens[x][i].type != NONE ){

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

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].aligned == true && pGrid->tokens[i][j].isDestruct == false){
                if(pGrid->tokens [i][j].type != TOKEN && pGrid->tokens [i][j].type != NULL )
                {
                    Token_special_action(pGrid->tokens[i][j].type,pGrid,i,j);
                    i=0;j=0;
                }
                else{
                      //  printf("salut \n");
                pGrid->tokens[i][j].type = NONE;
                pGrid->tokens[i][j].isDestruct = true;
                pGrid->tokens[i][j].startDestructAnim = -1;
                pGrid->tokens[i][j].aligned = false;
                }

               // pGrid->tokens[i][j].color = NONE_COLOR;

                cpt++;
            }
        }
    }

    return cpt;
}
//===========================================================

int destruct_colon(int y,Grid * pGrid)
{
    for(int i =0; i< pGrid->height; i++)
    {
        if(pGrid->tokens[i][y].type != MULTI)
        {
            if(pGrid->tokens[i][y].type != TOKEN &&pGrid->tokens[i][y].type !=NONE &&pGrid->tokens[i][y].type !=VERTICAL)
            {
                Token_special_action(pGrid->tokens[i][y].type,pGrid,i,y);
            }
         pGrid->tokens[i][y].type = NONE;
         pGrid->tokens[i][y].isDestruct = true;
         pGrid->tokens[i][y].startDestructAnim = -1;
        }

    }
    CheckGrid(pGrid);
}

int destruct_lign(int x,Grid * pGrid)
{
    for(int i =0; i< pGrid->width; i++)
    {
        if(pGrid->tokens[x][i].type != MULTI)
        {
            if(pGrid->tokens[x][i].type != TOKEN &&pGrid->tokens[x][i].type !=NONE &&pGrid->tokens[x][i].type != HORIZONTAL)
            {
                Token_special_action(pGrid->tokens[x][i].type,pGrid,x,i);
            }
         pGrid->tokens[x][i].type = NONE;
         pGrid->tokens[x][i].isDestruct = true;
         pGrid->tokens[x][i].startDestructAnim = -1;
        }

    }
    CheckGrid(pGrid);
}
//==================================================================
void Token_special_action(TokenTypes t, Grid *pGrid, int x, int y){

    switch(t)
    {
    case VERTICAL:
        destruct_colon(y, pGrid);
        break;

    case HORIZONTAL:
        //pGrid->tokens[y][x].isDestruct = true;
        destruct_lign(x, pGrid);
        break;

    case MULTI:

        break;

    case PACKED:
         pGrid->tokens[y][x].isDestruct = true;
        break;

    }

}

//==========================================================
int destruct_color(Colors c, Grid *pGrid)
{
     int cpt = 0;

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].color == c ){
                if(pGrid->tokens[i][j].type != TOKEN)Token_special_action(pGrid->tokens[i][j].type, pGrid, i,j);
                pGrid->tokens[i][j].type = NONE;
                pGrid->tokens[i][j].isDestruct = true;
                pGrid->tokens[i][j].startDestructAnim = -1;
                pGrid->tokens[i][j].drawBackground = false;

                cpt++;
            }
        }
    }

    return cpt;


}


// =========================================================

bool Compare_TokenColor(Token t1, Token t2)
{
    //Si le type est un des types speciaux sans couleur, nb : cerise, noisette, multi (situé apres Vertical dans l'enumertation)
    return (t1.type >PACKED || t2.type >PACKED || t1.type == NONE || t2.type == NONE)?false: t1.color==t2.color;



}

bool Compare_TokenColor_color(Token t1, Colors c)
{
     return (t1.type >PACKED || t1.type == NONE )?false: t1.color==c;



}


// =========================================================

void ChangeDirectionRandom(Grid *pGrid){

    //fprintf(stdout,"game.c : ChangeDirectionRandom(Grid *pGrid)\n");
    pGrid->direction = rand()%4;

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
    //Directions dir = DOWN;
    bool flag = false;
    while ( flag == false ){

        flag = true;

        for(int i = 0; i < pGrid->height; i++){
            for(int j = 0; j < pGrid->width; j++){

                if ( pGrid->tokens[i][j].type != NONE ){

                    if( i + DirectionsVectors[dir][1] >= 0 && i + DirectionsVectors[dir][1] < pGrid->height &&
                        j + DirectionsVectors[dir][0] >= 0 && j + DirectionsVectors[dir][0] < pGrid->width ){

                        if ( pGrid->tokens[(i + DirectionsVectors[dir][1])][(j + DirectionsVectors[dir][0])].type == NONE ){

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

        case UP : return DOWN;
        case DOWN : return UP;
        case RIGHT : return LEFT;
        case LEFT : return RIGHT;
    }

    return dir;
}

// =========================================================

void InjectLigne(Grid *pGrid){

    //fprintf(stdout,"game.c : InjectLigne(Grid *pGrid)\n");

    Directions dir = ReverseDirection(pGrid->direction);

    switch(dir){

        case UP :{

            for(int j = 0; j < pGrid->width; j++){

                if ( pGrid->tokens[0][j].type == NONE ){
                    //printf("token == Null\n");
                    if (  ( GetFirstDirToken(pGrid,j,dir)->rect_image.y / TOKEN_HEIGHT ) - 1 < 0 )
                        InitRandomToken(pGrid, &pGrid->tokens[0][j], j, ( GetFirstDirToken(pGrid,j,dir)->rect_image.y / TOKEN_HEIGHT ) - 1 );
                    else
                        InitRandomToken(pGrid, &pGrid->tokens[0][j], j, - 1 );

                }
            }
        }
        break;

        case DOWN :{
           // printf("inject bas \n");
             for(int j = 0; j < pGrid->width; j++){

                if ( pGrid->tokens[pGrid->height-1][j].type == NONE ){
                    printf("token == Null\n");
                    if (  ( GetFirstDirToken(pGrid,j,dir)->rect_image.y / TOKEN_HEIGHT ) + 1 > pGrid->height-1 )
                        InitRandomToken(pGrid, &pGrid->tokens[pGrid->height-1][j], j, ( GetFirstDirToken(pGrid,j,dir)->rect_image.y / TOKEN_HEIGHT ) +1 );
                    else
                        InitRandomToken(pGrid, &pGrid->tokens[pGrid->height-1][j], j, pGrid->height );
                }
            }
        }
        break;

        case LEFT :{
            for(int i =0; i<pGrid->height; i++){

                if(pGrid->tokens[i][0].type==NONE)
                {
                    if( (GetFirstDirToken(pGrid,i,dir)->rect_image.x /TOKEN_WIDTH) -1 < 0)
                        InitRandomToken(pGrid, &pGrid->tokens[i][0], ( GetFirstDirToken(pGrid,i,dir)->rect_image.x / TOKEN_WIDTH ) -1, i );
                    else
                        InitRandomToken(pGrid, &pGrid->tokens[i][0], -1, i );

                }

            }

        }
        break;

        case RIGHT :{
            for(int i =0; i<pGrid->height; i++){

                if(pGrid->tokens[i][pGrid->width-1].type==NONE)
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

void Button_direction_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw,Grid *pGrid ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        if(pGrid->is_randomizeInsert==true){

            pGrid->is_randomizeInsert = false;
        }
        else {

            pGrid->is_randomizeInsert =true;
            ChangeDirectionRandom(pGrid);
        }

        //printf("direction  : %d\n",pGrid->isdir_random );
    }
}

// =========================================================

void Button_help_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid ){

    if ( UI_button_event(pButton, pEvent, pDraw) && pGrid->nbHelp > 0 ){

        pGrid->isHelpActive = true;

        pGrid->nbHelp --;
    }
}

// =========================================================

void Button_superHelp_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid ){

    if ( UI_button_event(pButton, pEvent, pDraw) && pGrid->nbSuperHelp > 0 ){

        pGrid->isSuperHelpActive = true;

        pGrid->nbSuperHelp --;
    }
}

// =========================================================

void Button_revertOnce_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        pGrid->nbRevertOnce --;

        LoadTokensInPastTokens(pGrid);
    }
}

// =========================================================

void Game_event(Grid *pGrid, SDL_Event *pEvent, bool *pQuit){

    switch(pEvent->type){

        case SDL_KEYDOWN :{

            switch( pEvent->key.keysym.sym ){

                case SDLK_d :{

                    pGrid->nbMove ++;
                }
                break;

                case SDLK_q :{

                    pGrid->nbMove --;

                    if ( pGrid->nbMove <= 0 ){

                        *pQuit = true;
                        gameState = MENU;
                    }
                }
                break;

                case SDLK_z :{

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
                break;

                case SDLK_s :{

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
                break;

                case SDLK_e : {

                    RandomizeGrid(pGrid);
                }
                break;


                case SDLK_j :{

                    ChangeDirection(pGrid,LEFT);

                }
                break;

                case SDLK_i :{

                    ChangeDirection(pGrid,UP);

                }
                break;


                case SDLK_k :{

                    ChangeDirection(pGrid,DOWN);

                }
                break;


                case SDLK_l :{

                    ChangeDirection(pGrid,RIGHT);

                }
                break;
            }
        }

        // bouttons de souris appuié
        case SDL_MOUSEBUTTONDOWN:{

            switch(pEvent->button.button){

                // bouton gauche
                case SDL_BUTTON_LEFT:{

                    if ( IsTokenMoving(pGrid) == false && IsTokenDestructing(pGrid) == false && pGrid->is_cursorOnGrid == true ){

                        dragStart.x = pGrid->cursorTokenPosition.x;
                        dragStart.y = pGrid->cursorTokenPosition.y;

                        dragAndDrop = pGrid->tokens[dragStart.y][dragStart.x].type != NONE;
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

                    if ( dragAndDrop == true ){

                        SDL_Point dragEnd;

                        dragEnd.x = pGrid->cursorTokenPosition.x;
                        dragEnd.y = pGrid->cursorTokenPosition.y;

                        fprintf(stdout,"Distance du drag : %d, %d.\n",dragEnd.x - dragStart.x, dragEnd.y - dragStart.y);

                        int distX = sqrt( pow( dragEnd.x - dragStart.x, 2) );
                        int distY = sqrt( pow( dragEnd.y - dragStart.y, 2) );

                        if ( ( distX == 1 && distY == 0 ) || ( distX == 0 && distY == 1 ) ){

                            HardPermuteToken(pGrid, dragStart.x, dragStart.y, dragEnd.x, dragEnd.y);


                            if ( IsLineOnGrid(pGrid) == false && pGrid->tokens[dragEnd.y][dragEnd.x].type != MULTI && pGrid->tokens[dragStart.y][dragStart.x].type != MULTI){

                                HardPermuteToken(pGrid, dragEnd.x, dragEnd.y, dragStart.x, dragStart.y);
                            }
                            else {

                                HardPermuteToken(pGrid, dragEnd.x, dragEnd.y, dragStart.x, dragStart.y);
                                SaveTokensInPastTokens(pGrid);
                                HardPermuteToken(pGrid, dragEnd.x, dragEnd.y, dragStart.x, dragStart.y);

                                /* coups réussi */
                                pGrid->nbMove --;

                                if(pGrid->tokens[dragEnd.y][dragEnd.x].type == MULTI )
                                {
                                    pGrid->score += destruct_color(pGrid->tokens[dragStart.y][dragStart.x].color, pGrid)* TOKEN_SCORE;
                                    pGrid->tokens[dragEnd.y][dragEnd.x].isDestruct = true;
                                    pGrid->tokens[dragEnd.y][dragEnd.x].type = NONE;
                                    pGrid->tokens[dragEnd.y][dragEnd.x].startDestructAnim = -1;
                                }else if(pGrid->tokens[dragStart.y][dragStart.x].type == MULTI)
                                {
                                    pGrid->score += destruct_color(pGrid->tokens[dragEnd.y][dragEnd.x].color, pGrid)* TOKEN_SCORE;
                                    pGrid->tokens[dragStart.y][dragStart.x].isDestruct = true;
                                    pGrid->tokens[dragStart.y][dragStart.x].type = NONE;
                                    pGrid->tokens[dragStart.y][dragStart.x].startDestructAnim = -1;

                                }

                                pGrid->isHelpActive = false;
                                pGrid->isSuperHelpActive = false;

                                if ( pGrid->nbMove <= 0 ){

                                    *pQuit = true;
                                    gameState = MENU;
                                }

                                if(pGrid->is_randomizeInsert == true){

                                    ChangeDirectionRandom(pGrid);
                                }
                            }
                        }

                        dragAndDrop = false;
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
                if ( dragAndDrop && ( cursorTokenPositionTemp.x != pGrid->cursorTokenPosition.x || cursorTokenPositionTemp.y != pGrid->cursorTokenPosition.y ) ){

                    int distX = sqrt( pow( pGrid->cursorTokenPosition.x - dragStart.x, 2) );
                    int distY = sqrt( pow( pGrid->cursorTokenPosition.y - dragStart.y, 2) );

                    if ( ( distX == 1 && distY == 0 ) || ( distX == 0 && distY == 1 ) ){

                        PermuteToken(pGrid, dragStart.x, dragStart.y, pGrid->cursorTokenPosition.x, pGrid->cursorTokenPosition.y);

                        if ( IsLineOnGrid(pGrid) == false && pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].type != MULTI && pGrid->tokens[dragStart.y][dragStart.x].type != MULTI ){

                            //fprintf(stdout,"game.c -> Game_event(Grid *pGrid, SDL_Event *pEvent, bool *pQuit) -> switch(pEvent->type) -> case SDL_MOUSEMOTION -> !IsLineOnGrid(pGrid) ");
                            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].image_background = image_cursorRed;
                            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].drawBackground = true;
                        }
                        if( pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].type == MULTI){
                            ChangeColorTokenBackgroundImage(pGrid,image_cursorGreen,pGrid->tokens[dragStart.y][dragStart.x].color);
                        }else if(pGrid->tokens[dragStart.y][dragStart.x].type == MULTI){
                            ChangeColorTokenBackgroundImage(pGrid,image_cursorGreen,pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].color);
                        }else
                        {

                        }
                        ChangeAlignedTokenBackgroundImage(pGrid, image_cursorGreen);

                        PermuteToken(pGrid, dragStart.x, dragStart.y, pGrid->cursorTokenPosition.x, pGrid->cursorTokenPosition.y);

                        PermuteTokenImage(pGrid, dragStart.x, dragStart.y, pGrid->cursorTokenPosition.x, pGrid->cursorTokenPosition.y);
                    }
                    else{

                        ResetTokenImages(pGrid);
                        pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].drawBackground = true;

                        if  ( !( distX == 1 && distY == 0 ) && !( distX == 0 && distY == 1 ) && !( distX == 0 && distY == 0 ) )
                            dragAndDrop = false;
                    }
                }
                else if ( dragAndDrop == false ){

                    for(int i = 0; i < pGrid->height; i++){
                        for(int j = 0; j < pGrid->width; j++){

                            pGrid->tokens[i][j].drawBackground = false;
                        }
                    }
                    pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].drawBackground = true;
                }
            }
            else{

                if ( dragAndDrop == true ){

                    dragAndDrop = false;
                }
            }
        }
        break;
    }
}

// =========================================================
// Logiques
// =========================================================

void Game_logic(Grid *pGrid){

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

            if(IsTokenOfType(pGrid, NONE ) == true ){

                while( IsTokenOfType(pGrid, NONE ) == true ){

                    // regroupe tout les jetons
                    RegroupTokens(pGrid);

                    // remplie les espaces vides
                   InjectLigne(pGrid);
                }

                // recalcul les mouvements possibles
                MoveAvailable(pGrid, false);

            }else {

                if(pGrid->moveAvailable == 0){

                    RandomizeGrid(pGrid);
                }
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

    UI_label label_score = {false};
    UI_button button_quit = {false};
    UI_button button_direction = {false};
    UI_button button_menu = {false};
    UI_button button_help = {false};
    UI_button button_superHelp = {false};
    UI_button button_revertOnce = {false};
    UI_label label_mouvements = {false};
    UI_label label_nbMove = {false};

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

    sprintf(label_score.text,"Score : %d ",grid1->score);
    sprintf(label_nbMove.text,"NbCoups : %d", grid1->nbMove);
    sprintf(label_mouvements.text,"Mouvement possible : %d",grid1->moveAvailable);

    fprintf(stdout,"game.c -> GameSessionRandom(...) -> UI_button_new return %d.\n", UI_button_new(&button_quit, &window, "Quitter", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 50 ));
    fprintf(stdout,"game.c -> GameSessionRandom(...) -> UI_button_new return %d.\n", UI_button_new(&button_menu, &window, "Retour menu", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 90 ));
    fprintf(stdout,"game.c -> GameSessionRandom(...) -> UI_button_new return %d.\n", UI_button_new(&button_direction, &window, "Direction", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 200 ));
    fprintf(stdout,"game.c -> GameSessionRandom(...) -> UI_button_new return %d.\n", UI_button_new(&button_help, &window, "Aide ( 0 )", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 240 ));
    fprintf(stdout,"game.c -> GameSessionRandom(...) -> UI_button_new return %d.\n", UI_button_new(&button_superHelp, &window, "Super aide ( 0 )", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 280 ));
    fprintf(stdout,"game.c -> GameSessionRandom(...) -> UI_button_new return %d.\n", UI_button_new(&button_revertOnce, &window, "Retour arriere ( 0 )", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 320 ));

    sprintf(button_help.text,"Aide ( %d )",grid1->nbHelp);
    sprintf(button_superHelp.text,"Super aide ( %d )",grid1->nbSuperHelp);
    sprintf(button_revertOnce.text,"Retour arriere ( %d )",grid1->nbRevertOnce);

    window.visible = true;

    bool draw = true; // non utilisé
    bool quit = false;

    while( !quit ){

        int frameStart = SDL_GetTicks();

        /* évènements */
        while( SDL_PollEvent( &event ) != 0 ){

            if( event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)){

                gameState = QUIT;
                quit = true;
            }

            // entré lié a la grille
            Game_event(grid1, &event, &quit);

            // event UI
            Window_event(&window, &event, &draw );
            Button_quit_event(&button_quit, &event, &draw, &quit);
            Button_menu_event(&button_menu, &event, &draw, &quit);
            Button_direction_event(&button_direction, &event, &draw, grid1);
            if( grid1->nbHelp > 0 ) Button_help_event(&button_help, &event, &draw, grid1);
            if( grid1->nbSuperHelp > 0 ) Button_superHelp_event(&button_superHelp, &event, &draw, grid1);
            if( grid1->nbRevertOnce > 0 ) Button_revertOnce_event(&button_revertOnce, &event, &draw, grid1);
        }

        /* logique */
        Game_logic(grid1);

        /* maj des labels */
        sprintf(label_nbMove.text," NbCoups : %d", grid1->nbMove);
        sprintf(label_score.text,"Score : %d ", grid1->score);
        sprintf(label_mouvements.text,"Nombre de mouvement : %d",grid1->moveAvailable);
        if( grid1->nbHelp > 0 ) sprintf(button_help.text,"Aide ( %d )",grid1->nbHelp);
        if( grid1->nbSuperHelp > 0 ) sprintf(button_superHelp.text,"Super aide ( %d )",grid1->nbSuperHelp);
        if( grid1->nbRevertOnce > 0 ) sprintf(button_revertOnce.text,"Retour arriere ( %d )",grid1->nbRevertOnce);

        /* animations */
        Grid_anim(grid1);

        /* affichage */
        SDL_RenderClear(pRenderer);                                                             // efface tout le contenu du renderer

        Window_draw(&window, pRenderer);

        Grid_draw(grid1,pRenderer);                                                              // désine la grille sur le renderer

        UI_label_draw(&label_score,pRenderer);
        UI_label_draw(&label_nbMove,pRenderer);
        UI_label_draw(&label_mouvements,pRenderer);
        UI_button_draw(&button_quit, pRenderer);
        UI_button_draw(&button_direction, pRenderer);
        UI_button_draw(&button_menu, pRenderer);
        if( grid1->nbHelp > 0 ) UI_button_draw(&button_help, pRenderer);
        if( grid1->nbSuperHelp > 0 ) UI_button_draw(&button_superHelp, pRenderer);
        if( grid1->nbRevertOnce > 0 ) UI_button_draw(&button_revertOnce, pRenderer);

        UI_outline(pRenderer, &rect_grid, black, -1 );
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

    UI_label label_score = {false};
    UI_button button_quit = {false};
    UI_button button_direction = {false};
    UI_button button_menu = {false};
    UI_button button_help = {false};
    UI_button button_superHelp = {false};
    UI_button button_revertOnce = {false};
    UI_label label_mouvements = {false};
    UI_label label_nbMove = {false};

    // création des zone de jeu et d'affichage
    SDL_Rect rect_grid = { 0,0,pGrid->width * TOKEN_WIDTH, pGrid->height * TOKEN_HEIGHT };
    SDL_Rect rect_UI = { rect_grid.x + rect_grid.w * TOKEN_WIDTH, 0, 250, rect_grid.h };

    SDL_Rect rect_screen = {0 ,
                            0 ,
                            (rect_grid.w + rect_grid.x > rect_UI.w + rect_UI.x ) ? rect_grid.w + rect_grid.x : rect_UI.w + rect_UI.x ,
                            (rect_grid.h + rect_grid.y > rect_UI.h + rect_UI.y ) ? rect_grid.h + rect_grid.y : rect_UI.h + rect_UI.y } ;

    Array_new(&objects);

    pRenderer = InitGame("Candy Crush Clone C", &objects, rect_screen.w, rect_screen.h );
    if ( !pRenderer )
        return;

    fprintf(stdout,"game.c -> GameSessionPuzzle(...) -> Window_new return %d.\n", Window_new(&window, NULL, false, 0, 0, screen_width, screen_height));
    fprintf(stdout,"game.c -> GameSessionPuzzle(...) -> UI_label_new return %d.\n", UI_label_new(&label_score, &window, "Test", rect_UI.x + 20 , rect_UI.y + 20 ));
    fprintf(stdout,"game.c -> GameSessionPuzzle(...) -> UI_label_new return %d.\n", UI_label_new(&label_nbMove, &window, "Test", rect_UI.x + 20 , rect_UI.y + 40 ));
    fprintf(stdout,"game.c -> GameSessionPuzzle(...) -> UI_label_new return %d.\n", UI_label_new(&label_mouvements, &window, "Test", rect_UI.x + 20 , rect_UI.y + 60 ));

    sprintf(label_score.text,"Score : %d ",pGrid->score);
    sprintf(label_nbMove.text,"NbCoups : %d", pGrid->nbMove);
    sprintf(label_mouvements.text,"Mouvement possible : %d",pGrid->moveAvailable);

    fprintf(stdout,"game.c -> GameSessionPuzzle(...) -> UI_button_new return %d.\n", UI_button_new(&button_quit, &window, "Quitter", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 50 ));
    fprintf(stdout,"game.c -> GameSessionPuzzle(...) -> UI_button_new return %d.\n", UI_button_new(&button_menu, &window, "Retour menu", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 90 ));
    fprintf(stdout,"game.c -> GameSessionPuzzle(...) -> UI_button_new return %d.\n", UI_button_new(&button_direction, &window, "Direction", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 200 ));
    fprintf(stdout,"game.c -> GameSessionPuzzle(...) -> UI_button_new return %d.\n", UI_button_new(&button_help, &window, "Aide ( 0 )", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 240 ));
    fprintf(stdout,"game.c -> GameSessionPuzzle(...) -> UI_button_new return %d.\n", UI_button_new(&button_superHelp, &window, "Super aide ( 0 )", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 280 ));
    fprintf(stdout,"game.c -> GameSessionPuzzle(...) -> UI_button_new return %d.\n", UI_button_new(&button_revertOnce, &window, "Retour arriere ( 0 )", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 320 ));

    sprintf(button_help.text,"Aide ( %d )",pGrid->nbHelp);
    sprintf(button_superHelp.text,"Super aide ( %d )",pGrid->nbSuperHelp);
    sprintf(button_revertOnce.text,"Retour arriere ( %d )",pGrid->nbRevertOnce);

    window.visible = true;

    bool draw = true; // non utilisé
    bool quit = false;

    while( !quit ){

        int frameStart = SDL_GetTicks();

        /* évènements */
        while( SDL_PollEvent( &event ) != 0 ){

            if( event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)){

                gameState = QUIT;
                quit = true;
            }

            // entré lié a la grille
            Game_event(pGrid, &event, &quit);

            // event UI
            Window_event(&window, &event, &draw );
            Button_quit_event(&button_quit, &event, &draw, &quit);
            Button_return_event(&button_menu, &event, &draw, &quit);
            Button_direction_event(&button_direction, &event, &draw, pGrid);
            if( pGrid->nbHelp > 0 ) Button_help_event(&button_help, &event, &draw, pGrid);
            if( pGrid->nbSuperHelp > 0 ) Button_superHelp_event(&button_superHelp, &event, &draw, pGrid);
            if( pGrid->nbRevertOnce > 0 ) Button_revertOnce_event(&button_revertOnce, &event, &draw, pGrid);
        }

        /* logique */
        Game_logic(pGrid);

        /* maj des labels */
        sprintf(label_nbMove.text," NbCoups : %d", pGrid->nbMove);
        sprintf(label_score.text,"Score : %d ", pGrid->score);
        sprintf(label_mouvements.text,"Nombre de mouvement : %d",pGrid->moveAvailable);
        if( pGrid->nbHelp > 0 ) sprintf(button_help.text,"Aide ( %d )",pGrid->nbHelp);
        if( pGrid->nbSuperHelp > 0 ) sprintf(button_superHelp.text,"Super aide ( %d )",pGrid->nbSuperHelp);
        if( pGrid->nbRevertOnce > 0 ) sprintf(button_revertOnce.text,"Retour arriere ( %d )",pGrid->nbRevertOnce);

        /* animations */
        Grid_anim(pGrid);

        /* affichage */
        SDL_RenderClear(pRenderer);                                                             // efface tout le contenu du renderer

        Window_draw(&window, pRenderer);

        Grid_draw(pGrid,pRenderer);                                                              // désine la grille sur le renderer

        UI_label_draw(&label_score,pRenderer);
        UI_label_draw(&label_nbMove,pRenderer);
        UI_label_draw(&label_mouvements,pRenderer);
        UI_button_draw(&button_quit, pRenderer);
        UI_button_draw(&button_direction, pRenderer);
        UI_button_draw(&button_menu, pRenderer);
        if( pGrid->nbHelp > 0 ) UI_button_draw(&button_help, pRenderer);
        if( pGrid->nbSuperHelp > 0 ) UI_button_draw(&button_superHelp, pRenderer);
        if( pGrid->nbRevertOnce > 0 ) UI_button_draw(&button_revertOnce, pRenderer);

        UI_outline(pRenderer, &rect_grid, black, -1 );
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











