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
    error += Image_new(&image_cursorBlue, "data/image_cursorBlue.png", pArray, pRenderer);
    error += Image_new(&image_cursorGreen, "data/image_cursorGreen.png", pArray, pRenderer);
    error += Image_new(&image_cursorRed, "data/image_cursorRed.png", pArray, pRenderer);

    // image des jetons
    error += Image_new(&image_tokens[0], "data/Tokens/Token_red.png", pArray, pRenderer);
    error += Image_new(&image_tokens[1], "data/Tokens/Token_blue.png", pArray, pRenderer);
    error += Image_new(&image_tokens[2], "data/Tokens/Token_green.png", pArray, pRenderer);
    error += Image_new(&image_tokens[3], "data/Tokens/Token_yellow.png", pArray, pRenderer);
    error += Image_new(&image_tokens[4], "data/Tokens/Token_purple.png", pArray, pRenderer);
    error += Image_new(&image_tokens[5], "data/Tokens/Token_orange.png", pArray, pRenderer);
  
    error += Image_new(&image_tokens[6], "data/Tokens/Token_red_horizontal.png", pArray, pRenderer);
    error += Image_new(&image_tokens[7], "data/Tokens/Token_blue_horizontal.png", pArray, pRenderer);
    error += Image_new(&image_tokens[8], "data/Tokens/Token_green_horizontal.png", pArray, pRenderer);
    error += Image_new(&image_tokens[9], "data/Tokens/Token_yellow_horizontal.png", pArray, pRenderer);
    error += Image_new(&image_tokens[10], "data/Tokens/Token_purple_horizontal.png", pArray, pRenderer);
    error += Image_new(&image_tokens[11], "data/Tokens/Token_orange_horizontal.png", pArray, pRenderer);

    error += Image_new(&image_tokens[12], "data/Tokens/Token_red_vertical.png", pArray, pRenderer);
    error += Image_new(&image_tokens[13], "data/Tokens/Token_blue_vertical.png", pArray, pRenderer);
    error += Image_new(&image_tokens[14], "data/Tokens/Token_green_vertical.png", pArray, pRenderer);
    error += Image_new(&image_tokens[15], "data/Tokens/Token_yellow_vertical.png", pArray, pRenderer);
    error += Image_new(&image_tokens[16], "data/Tokens/Token_purple_vertical.png", pArray, pRenderer);
    error += Image_new(&image_tokens[17], "data/Tokens/Token_orange_vertical.png", pArray, pRenderer);

    error += Image_new(&image_tokens[18], "data/Tokens/Token_multi.png", pArray, pRenderer);
  
    if ( error > 0 ) {

		CleanGame(pArray);
		return NULL;
	}

	return pRenderer;
}

// =========================================================

int CleanGame(Array *pArray){

    if ( !pArray )
        return -1;

    if ( pArray->length ){

        for(int i = pArray->length - 1; i >= 0; i--){

            switch( Array_GET_id(pArray, i) ){

                case FONT_TYPE :{ TTF_CloseFont((TTF_Font*)Array_GET_data(pArray,i)); } break;
                case TEXTURE_TYPE : { SDL_DestroyTexture((SDL_Texture*)Array_GET_data(pArray,i)); } break;
                case RENDERER_TYPE : { SDL_DestroyRenderer((SDL_Renderer*)Array_GET_data(pArray,i)); } break;
                case WINDOW_TYPE : { SDL_DestroyWindow((SDL_Window*)Array_GET_data(pArray,i)); } break;
                case ARRAY_TYPE : { Array_free((Array*)Array_GET_data(pArray,i)); } break;
                default : { free( pArray->tab_data[i]); }
            }
        }
    }

    pArray->length = 0;
    Array_free(pArray);

    return 0;
}

// =========================================================

Grid *NewGrid(SDL_Rect rect, int nbMove, int nbColor, bool randomizeInsert, int nbHelp, int nbSuperHelp, int nbRevertOnce){

    /* aléatoire */
    srand(time(NULL));

    Grid *pGrid = malloc(sizeof(Grid));

    pGrid->width = rect.w;
    pGrid->height = rect.h;
    pGrid->nbMove = nbMove;
    pGrid->nbColor = nbColor;
    pGrid->pastTokens = NULL;
    pGrid->direction =DOWN;
    pGrid->is_randomizeInsert = true;
    pGrid->score = 0;
    pGrid->is_randomizeInsert = randomizeInsert;
    pGrid->nbHelp = nbHelp;
    pGrid->nbSuperHelp = nbSuperHelp;
    pGrid->nbRevertOnce = nbRevertOnce;

    MakeRect(&pGrid->rect, rect.x, rect.y, rect.w * TOKEN_WIDTH, rect.h * TOKEN_HEIGHT);

    pGrid->is_cursorOnGrid = false;
    pGrid->cursorTokenPosition.x = 0;
    pGrid->cursorTokenPosition.y = 0;

    /* allocation de la grille et remplissage */
    pGrid->tokens = (Token*)malloc( pGrid->height * sizeof(Token*));
    if ( pGrid->tokens == NULL ){ return NULL; }

    for(int i = 0; i < pGrid->height; i++){

        pGrid->tokens[i] = (Token*)malloc( pGrid->width * sizeof(Token));

        /* désallocation de la mémoire en cas d'erreur */
        if ( pGrid->tokens[i] == NULL ){

            for(i=i-1 ; i >= 0 ; i--){
                free(pGrid->tokens[i]);
            }
            free(pGrid->tokens);

            return NULL;
        }
    }

    RandomizeGrid(pGrid);

    return pGrid;
}

// =========================================================

void RandomizeGrid(Grid *pGrid){

    fprintf(stdout,"game.c -> RandomizeGrid(Grid *pGrid)\n");

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            InitRandomToken(pGrid, &pGrid->tokens[i][j], pGrid->nbColor, j, i);
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
    MoveAvailable(pGrid);
}

// =========================================================

void InitRandomToken(Grid *pGrid, Token *token, int nbColor, int x, int y){

    token->type = TOKEN;
    token->color = (Colors)(rand() % nbColor);
    token->isMoving = false;
    token->isDestruct = false;
    token->startDestructAnim = -1;

    token->textureSize = 100;
    CalculTokenImages(pGrid, token, x, y);

    token->score = TOKEN_SCORE;
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

                    if ( pGrid->tokens[i][j].color == pGrid->tokens[i-1][j].color && pGrid->tokens[i][j].color == pGrid->tokens[i+1][j].color ){

                        pGrid->tokens[i][j].aligned = true;
                        pGrid->tokens[i-1][j].aligned = true;
                        pGrid->tokens[i+1][j].aligned = true;
                    }
                }

                // vérification horizontale
                if ( j > 0 && j < pGrid->width - 1){

                    if ( pGrid->tokens[i][j].color == pGrid->tokens[i][j-1].color && pGrid->tokens[i][j].color == pGrid->tokens[i][j+1].color ){

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
                       pGrid->tokens[i+vecteur_point[n][0][0]][j+vecteur_point[n][0][1]].color == save_color &&
                       pGrid->tokens[i+vecteur_point[n][1][0]][j+vecteur_point[n][1][1]].color == save_color &&
                       pGrid->tokens[i+vecteur_point[n][0][0]][j+vecteur_point[n][0][1]].aligned == true &&
                       pGrid->tokens[i+vecteur_point[n][1][0]][j+vecteur_point[n][1][1]].aligned == true
                       )
                       coude = true;



                }

                 if(coude == true)
                    {
                        printf("coude detecter en %d,%d \n",i,j);
                        pGrid->tokens[i][j].aligned = false;
                        pGrid->tokens[i][j].color = NONE_COLOR;
                        pGrid->tokens[i][j].type = MULTI;
                        CalculTokenImages(pGrid,&pGrid->tokens[i][j],i,j);
                        printf("special apparut en %d,%d \n",i,j);

                    }

            }


        }
    }


}

//============================
TokenTypes TypeRandom(int borne)
{
    switch(rand()%borne)
    {
        case 0: return HORIZONTAL;
        case 1: return VERTICAL;

        default: printf("TypeRandom -> valeur non comprise"); return TOKEN;

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
    Colors savedColor;

    /* traitement au cas par cas */

    //Verification des allignements horizontaux
    for(int i = 0; i < pGrid->height; i++){

        nb_align = 1;
        savedColor = pGrid->tokens[i][0].color;
        val = pGrid->tokens[i][0].score;

        for(int j = 1; j < pGrid->width; j++){

            if(pGrid->tokens[i][j].color == savedColor){

                nb_align++;
                val += pGrid->tokens[i][j].score;

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

                    printf("Score de la ligne : %d \n", score);
                }
                nb_align = 1;
                savedColor = pGrid->tokens[i][j].color;
                val = pGrid->tokens[i][j].score;
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

            printf("Score de la ligne : %d \n", score);
        }
    }
    //Verification des allignements verticaux
    for(int j = 0; j < pGrid->width; j++){

        nb_align = 1;
        savedColor = pGrid->tokens[0][j].color;
         val = pGrid->tokens[0][j].score;

        for(int i = 1; i < pGrid->height; i++){

            if(pGrid->tokens[i][j].color == savedColor){

                nb_align++;
                val += pGrid->tokens[i][j].score;
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
                    printf("Score de la colonne : %d \n", score);
                }
                nb_align = 1;
                savedColor = pGrid->tokens[i][j].color;
                val = pGrid->tokens[i][j].score;
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

            printf("Score de la ligne : %d \n", score);
        }
    }

    pGrid->score += score;

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

    switch(dir){

        case UP:{

            for(int i = 0; i < pGrid->height; i ++ ){

                if ( pGrid->tokens[i][x].type != NONE ){

                    return &pGrid->tokens[i][x];
                }
            }
        }
        break;

        case DOWN:{

            for(int i = pGrid->height-1; i >=0; i -- ){

                if ( pGrid->tokens[i][x].type != NONE ){

                        return &pGrid->tokens[i][x];
                }
            }

        }
        break;

        case LEFT:{

            for(int i = 0; i < pGrid->width; i ++ ){

                if ( pGrid->tokens[x][i].type != NONE ){

                        return &pGrid->tokens[x][i];
                }
            }
        }
        break;

        case RIGHT:{

            for(int i = pGrid->width-1; i >=0 ; i -- ){

                if ( pGrid->tokens[x][i].type != NONE ){

                        return &pGrid->tokens[x][i];
                }
            }
        }
        break;
    }
}

// =========================================================

int DestroyAlignedTokens(Grid *pGrid){

    //fprintf(stdout,"game.c : DestroyAlignedTokens(Grid *pGrid)\n");

    int cpt = 0;

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].aligned == true ){

                pGrid->tokens[i][j].type = NONE;
                pGrid->tokens[i][j].isDestruct = true;
                pGrid->tokens[i][j].startDestructAnim = -1;

                cpt++;
            }
        }
    }

    return cpt;
}

// =========================================================

//bool Compare_Color()


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
                        InitRandomToken(pGrid, &pGrid->tokens[0][j], pGrid->nbColor, j, ( GetFirstDirToken(pGrid,j,dir)->rect_image.y / TOKEN_HEIGHT ) - 1 );
                    else
                        InitRandomToken(pGrid, &pGrid->tokens[0][j], pGrid->nbColor, j, - 1 );

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
                        InitRandomToken(pGrid, &pGrid->tokens[pGrid->height-1][j], pGrid->nbColor, j, ( GetFirstDirToken(pGrid,j,dir)->rect_image.y / TOKEN_HEIGHT ) +1 );
                    else
                        InitRandomToken(pGrid, &pGrid->tokens[pGrid->height-1][j], pGrid->nbColor, j, pGrid->height );
                }
            }
        }
        break;

        case LEFT :{
            for(int i =0; i<pGrid->height; i++){

                if(pGrid->tokens[i][0].type==NONE)
                {
                    if( (GetFirstDirToken(pGrid,i,dir)->rect_image.x /TOKEN_WIDTH) -1 < 0)
                        InitRandomToken(pGrid, &pGrid->tokens[i][0], pGrid->nbColor, ( GetFirstDirToken(pGrid,i,dir)->rect_image.x / TOKEN_WIDTH ) -1, i );
                    else
                        InitRandomToken(pGrid, &pGrid->tokens[i][0], pGrid->nbColor, -1, i );

                }

            }

        }
        break;

        case RIGHT :{
            for(int i =0; i<pGrid->height; i++){

                if(pGrid->tokens[i][pGrid->width-1].type==NONE)
                {
                    if( (GetFirstDirToken(pGrid,i,dir)->rect_image.x /TOKEN_WIDTH) +1 > pGrid->width-1)
                        InitRandomToken(pGrid, &pGrid->tokens[i][pGrid->width-1], pGrid->nbColor, ( GetFirstDirToken(pGrid,i,dir)->rect_image.x / TOKEN_WIDTH ) +1, i );
                    else
                        InitRandomToken(pGrid, &pGrid->tokens[i][pGrid->width-1], pGrid->nbColor, pGrid->width-1, i );

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

                }
                break;

                case SDLK_s :{

                    if(pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].color >0){

                        pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].color --;
                    }
                    else{
                        printf("Boutton s");
                        pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].color = pGrid->nbColor-1;
                    }

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

                            if ( IsLineOnGrid(pGrid) == false ){

                                HardPermuteToken(pGrid, dragEnd.x, dragEnd.y, dragStart.x, dragStart.y);
                            }
                            else {

                                pGrid->nbMove --;


                                if ( pGrid->nbMove <= 0 ){

                                    *pQuit = true;
                                    gameState = MENU;
                                }
                                //printf("direction  : %d\n",pGrid->isdir_random );
                                if(pGrid->is_randomizeInsert == true)
                                {
                                    ChangeDirectionRandom(pGrid);
                                    printf("changement direction: direction = %d ", pGrid->direction);
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

                        if ( IsLineOnGrid(pGrid) == false ){

                            //fprintf(stdout,"game.c -> Game_event(Grid *pGrid, SDL_Event *pEvent, bool *pQuit) -> switch(pEvent->type) -> case SDL_MOUSEMOTION -> !IsLineOnGrid(pGrid) ");
                            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].image_background = image_cursorRed;
                            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].drawBackground = true;
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
            Calc_Score(pGrid);

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
                MoveAvailable(pGrid);

            }else {

                if(pGrid->moveAvailable == 0){

                    RandomizeGrid(pGrid);
                }
            }
        }
    }
}

// =========================================================

void GameSession(int gridWidth, int gridHeight,int nbColor, int nbMove,bool randomizeInsert, int nbHelp, int nbSuperHelp, int nbRevertOnce){

    fprintf(stdout, "game.c -> GameSession(...) : start \n");

    SDL_Renderer *pRenderer;    // renderer = canvas ( endroit où l'on va déssiner )
    SDL_Event event;            // gestionnaire d'évènements
    Array objects;
    Window window;

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
    SDL_Rect rect_grid = { 0,0,gridWidth, gridHeight };
    SDL_Rect rect_UI = { rect_grid.x * TOKEN_WIDTH + rect_grid.w * TOKEN_WIDTH, 0, 250, rect_grid.h * TOKEN_HEIGHT };

    SDL_Rect rect_screen = {0 ,
                            0 ,
                            (rect_grid.w * TOKEN_WIDTH + rect_grid.x * TOKEN_WIDTH > rect_UI.w + rect_UI.x ) ? rect_grid.w * TOKEN_WIDTH + rect_grid.x * TOKEN_WIDTH : rect_UI.w + rect_UI.x ,
                            (rect_grid.h * TOKEN_HEIGHT + rect_grid.y * TOKEN_HEIGHT > rect_UI.h + rect_UI.y ) ? rect_grid.h * TOKEN_HEIGHT + rect_grid.y * TOKEN_HEIGHT : rect_UI.h + rect_UI.y } ;

    Array_new(&objects);

    pRenderer = InitGame("Candy Crush Clone C", &objects, rect_screen.w, rect_screen.h );
    if ( !pRenderer )
        return;

    Grid *grid1 = NewGrid(rect_grid,nbMove,nbColor,randomizeInsert,nbHelp,nbSuperHelp,nbRevertOnce);

    fprintf(stdout,"game.c -> GameSession(...) -> Window_new return %d.\n", Window_new(&window, NULL, false, 0, 0, screen_width, screen_height));
    fprintf(stdout,"game.c -> GameSession(...) -> UI_label_new return %d.\n", UI_label_new(&label_score, &window, "Test", rect_UI.x + 20 , rect_UI.y + 20 ));
    fprintf(stdout,"game.c -> GameSession(...) -> UI_label_new return %d.\n", UI_label_new(&label_nbMove, &window, "Test", rect_UI.x + 20 , rect_UI.y + 40 ));
    fprintf(stdout,"game.c -> GameSession(...) -> UI_label_new return %d.\n", UI_label_new(&label_mouvements, &window, "Test", rect_UI.x + 20 , rect_UI.y + 60 ));

    sprintf(label_score.text,"Score : %d ",grid1->score);
    sprintf(label_nbMove.text,"NbCoups : %d", grid1->nbMove);
    sprintf(label_mouvements.text,"Mouvement possible : %d",grid1->moveAvailable);

    fprintf(stdout,"game.c -> GameSession(...) -> UI_button_new return %d.\n", UI_button_new(&button_quit, &window, "Quitter", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 50 ));
    fprintf(stdout,"game.c -> GameSession(...) -> UI_button_new return %d.\n", UI_button_new(&button_menu, &window, "Retour menu", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 80 ));
    fprintf(stdout,"game.c -> GameSession(...) -> UI_button_new return %d.\n", UI_button_new(&button_direction, &window, "Direction", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 200 ));
    fprintf(stdout,"game.c -> GameSession(...) -> UI_button_new return %d.\n", UI_button_new(&button_help, &window, "Aide", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 110 ));
    fprintf(stdout,"game.c -> GameSession(...) -> UI_button_new return %d.\n", UI_button_new(&button_superHelp, &window, "Super aide", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 140 ));
    fprintf(stdout,"game.c -> GameSession(...) -> UI_button_new return %d.\n", UI_button_new(&button_revertOnce, &window, "Retour arriere", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 170 ));

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
            Button_menu_event(&button_direction, &event, &draw, &quit);
            //Button_menu_event(&button_help, &event, &draw, &quit);
            //Button_menu_event(&button_superHelp, &event, &draw, &quit);
            //Button_direction_event(&button_revertOnce, &event, &draw, grid1);
        }

        /* logique */
        Game_logic(grid1);

        /* maj des labels */
        sprintf(label_nbMove.text," NbCoups : %d", grid1->nbMove);
        sprintf(label_score.text,"Score : %d ", grid1->score);
        sprintf(label_mouvements.text,"Nombre de mouvement : %d",grid1->moveAvailable);

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
        UI_button_draw(&button_help, pRenderer);
        UI_button_draw(&button_superHelp, pRenderer);
        UI_button_draw(&button_revertOnce, pRenderer);

        SDL_RenderPresent(pRenderer);                                                           // déssine le renderer à l'écran

        /* gestion de la fréquence d'affichage ( pour les animations )*/
        WaitForNextFrame(frameStart);
    }

    /* fin du programme */
    CleanGame(&objects);

    fprintf(stdout, "game.c -> GameSession(...) : end \n");
}

// =========================================================











