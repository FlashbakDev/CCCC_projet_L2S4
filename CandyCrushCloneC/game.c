#include "game.h"
#include "core.h"

// =========================================================

Grid *NewGrid(SDL_Rect rect, int nbMove, int nbColor){

    /* aléatoire */
    srand(time(NULL));

    Grid *pGrid = malloc(sizeof(Grid));

    pGrid->width = rect.w;
    pGrid->height = rect.h;
    pGrid->nbMove = nbMove;
    pGrid->nbColor = nbColor;
    pGrid->pastTokens = NULL;
    pGrid->direction =DOWN;
    pGrid->isdir_random = false;
    pGrid->score = 0;

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

    fprintf(stdout,"game.c : RandomizeGrid(Grid *pGrid)\n");

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

    CheckGrid(pGrid);

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

void Button_quit_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit ){

    if ( UI_button_event(pButton, pEvent, pDraw) )
        *pQuit = true;
}

void Button_direction_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw,Grid *pGrid ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        if(pGrid->isdir_random==true){

            pGrid->isdir_random = false;
        }
        else {

            pGrid->isdir_random =true;
            ChangeDirectionRandom(pGrid);
        }

        printf("direction  : %d\n",pGrid->isdir_random );
    }
}

// =========================================================

GameSession* Game_init(Array *pArray,Window *pWindow,bool multiplayer, int width, int height, int nbMove, int nbColor){

    fprintf(stdout,"game.c -> GameSession_init(bool multiplayer = %d) : start \n", multiplayer);

    GameSession *pGameSession;

    // création des zone de jeu et d'affichage
    MakeRect(&pGameSession->rect_grid, 0,0,width, height );
    fprintf(stdout,"bug ? \n");
    MakeRect(&pGameSession->rect_UI, pGameSession->rect_grid.x * TOKEN_WIDTH + pGameSession->rect_grid.w * TOKEN_WIDTH, 0, 250, pGameSession->rect_grid.h * TOKEN_HEIGHT );
fprintf(stdout,"bug ? \n");
    MakeRect(&pGameSession->rect_screen,
             0 ,
             0 ,
             (pGameSession->rect_grid.w * TOKEN_WIDTH + pGameSession->rect_grid.x * TOKEN_WIDTH > pGameSession->rect_UI.w + pGameSession->rect_UI.x ) ? pGameSession->rect_grid.w * TOKEN_WIDTH + pGameSession->rect_grid.x * TOKEN_WIDTH : pGameSession->rect_UI.w + pGameSession->rect_UI.x ,
             (pGameSession->rect_grid.h * TOKEN_HEIGHT + pGameSession->rect_grid.y * TOKEN_HEIGHT > pGameSession->rect_UI.h + pGameSession->rect_UI.y ) ? pGameSession->rect_grid.h * TOKEN_HEIGHT + pGameSession->rect_grid.y * TOKEN_HEIGHT : pGameSession->rect_UI.h + pGameSession->rect_UI.y ) ;

    fprintf(stdout,"bug ? \n");

    pGameSession->pWindow = pWindow;
    pGameSession->is_multiplayer = multiplayer;
    pGameSession->pGrid_J1 = NewGrid(pGameSession->rect_grid,nbMove,nbColor);

    fprintf(stdout,"UI_label_new return %d.\n", UI_label_new(&pGameSession->label_score, pGameSession->pWindow, "Test", pGameSession->rect_UI.x + 20 , pGameSession->rect_UI.y + 20 ));
    fprintf(stdout,"UI_label_new return %d.\n", UI_label_new(&pGameSession->label_nbMove, pGameSession->pWindow, "Test", pGameSession->rect_UI.x + 20 , pGameSession->rect_UI.y + 40 ));
    fprintf(stdout,"UI_label_new return %d.\n", UI_label_new(&pGameSession->label_mouvements, pGameSession->pWindow, "Test", pGameSession->rect_UI.x + 20 , pGameSession->rect_UI.y + 60 ));

    sprintf(pGameSession->label_score.text,"Score : %d ",0);
    sprintf(pGameSession->label_nbMove.text,"NbCoups : %d", pGameSession->pGrid_J1->nbMove);
    sprintf(pGameSession->label_mouvements.text,"Mouvement possible : %d",0);

    fprintf(stdout,"UI_button_new return %d.\n", UI_button_new(&pGameSession->button_quit, pGameSession->pWindow, "Quitter", pGameSession->rect_UI.x + ( pGameSession->rect_UI.w / 2 ) - image_normal.w / 2 , pGameSession->rect_UI.h - 50 ));
    fprintf(stdout,"UI_button_new return %d.\n", UI_button_new(&pGameSession->button_direction, pGameSession->pWindow, "Direction", pGameSession->rect_UI.x + ( pGameSession->rect_UI.w / 2 ) - image_normal.w / 2 , pGameSession->rect_UI.h - 80 ));

    fprintf(stdout,"game.c -> GameSession_init(bool multiplayer = %d) : end\n", multiplayer);
}

// =========================================================

void Game_event(GameSession *pGameSession, SDL_Event *pEvent,bool *pDraw ,bool *pQuit){

    switch(pEvent->type){

        case SDL_KEYDOWN :{

            switch( pEvent->key.keysym.sym ){

                case SDLK_d :{

                    pGameSession->pGrid_J1->nbMove ++;
                }
                break;

                case SDLK_q :{

                    pGameSession->pGrid_J1->nbMove --;

                    if ( pGameSession->pGrid_J1->nbMove <= 0 )
                        *pQuit = true;

                }
                break;

                case SDLK_z :{

                    if(pGameSession->pGrid_J1->tokens[pGameSession->pGrid_J1->cursorTokenPosition.y][pGameSession->pGrid_J1->cursorTokenPosition.x].color < pGameSession->pGrid_J1->nbColor-1){

                        pGameSession->pGrid_J1->tokens[pGameSession->pGrid_J1->cursorTokenPosition.y][pGameSession->pGrid_J1->cursorTokenPosition.x].color ++;
                    }
                    else {

                        pGameSession->pGrid_J1->tokens[pGameSession->pGrid_J1->cursorTokenPosition.y][pGameSession->pGrid_J1->cursorTokenPosition.x].color = 0;
                    }

                }
                break;

                case SDLK_s :{

                    if(pGameSession->pGrid_J1->tokens[pGameSession->pGrid_J1->cursorTokenPosition.y][pGameSession->pGrid_J1->cursorTokenPosition.x].color >0){

                        pGameSession->pGrid_J1->tokens[pGameSession->pGrid_J1->cursorTokenPosition.y][pGameSession->pGrid_J1->cursorTokenPosition.x].color --;
                    }
                    else{

                        pGameSession->pGrid_J1->tokens[pGameSession->pGrid_J1->cursorTokenPosition.y][pGameSession->pGrid_J1->cursorTokenPosition.x].color = pGameSession->pGrid_J1->nbColor-1;
                    }

                }
                break;

                case SDLK_e : {

                    RandomizeGrid(pGameSession->pGrid_J1);
                }
                break;

                  case SDLK_a : {

                    pGameSession->pGrid_J1->direction = UP;
                }
                break;
            }
        }

        // bouttons de souris appuié
        case SDL_MOUSEBUTTONDOWN:{

            switch(pEvent->button.button){

                // bouton gauche
                case SDL_BUTTON_LEFT:{

                    if ( IsTokenMoving(pGameSession->pGrid_J1) == false && IsTokenDestructing(pGameSession->pGrid_J1) == false && pGameSession->pGrid_J1->is_cursorOnGrid == true ){

                        dragStart.x = pGameSession->pGrid_J1->cursorTokenPosition.x;
                        dragStart.y = pGameSession->pGrid_J1->cursorTokenPosition.y;

                        dragAndDrop = pGameSession->pGrid_J1->tokens[dragStart.y][dragStart.x].type != NONE;
                    }
                }
                break;

                // click droit pour afficher les stats du jeton
                case SDL_BUTTON_RIGHT:{

                    if ( pGameSession->pGrid_J1->is_cursorOnGrid == true ){

                        fprintf(stdout,"Jeton en posisition (%d,%d) : ",pGameSession->pGrid_J1->cursorTokenPosition.x, pGameSession->pGrid_J1->cursorTokenPosition.y);
                        DebugToken(pGameSession->pGrid_J1->tokens[pGameSession->pGrid_J1->cursorTokenPosition.y][pGameSession->pGrid_J1->cursorTokenPosition.x]);
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

                        dragEnd.x = pGameSession->pGrid_J1->cursorTokenPosition.x;
                        dragEnd.y = pGameSession->pGrid_J1->cursorTokenPosition.y;

                        fprintf(stdout,"Distance du drag : %d, %d.\n",dragEnd.x - dragStart.x, dragEnd.y - dragStart.y);

                        int distX = sqrt( pow( dragEnd.x - dragStart.x, 2) );
                        int distY = sqrt( pow( dragEnd.y - dragStart.y, 2) );

                        if ( ( distX == 1 && distY == 0 ) || ( distX == 0 && distY == 1 ) ){

                            HardPermuteToken(pGameSession->pGrid_J1, dragStart.x, dragStart.y, dragEnd.x, dragEnd.y);

                            if ( IsLineOnGrid(pGameSession->pGrid_J1) == false ){

                                HardPermuteToken(pGameSession->pGrid_J1, dragEnd.x, dragEnd.y, dragStart.x, dragStart.y);
                            }
                            else {

                                pGameSession->pGrid_J1->nbMove --;


                                if ( pGameSession->pGrid_J1->nbMove <= 0 ){

                                    *pQuit = true;
                                }
                                //printf("direction  : %d\n",pGameSession->pGrid_J1->isdir_random );
                                if(pGameSession->pGrid_J1->isdir_random == true)
                                {
                                    ChangeDirectionRandom(pGameSession->pGrid_J1);
                                    printf("changement direction: direction = %d ", pGameSession->pGrid_J1->direction);
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

            pGameSession->pGrid_J1->is_cursorOnGrid = PointInRect(pEvent->motion.x, pEvent->motion.y, &pGameSession->pGrid_J1->rect);

            if ( pGameSession->pGrid_J1->is_cursorOnGrid == true ){

                SDL_Point cursorTokenPositionTemp;
                cursorTokenPositionTemp.x = pGameSession->pGrid_J1->cursorTokenPosition.x;
                cursorTokenPositionTemp.y = pGameSession->pGrid_J1->cursorTokenPosition.y;

                pGameSession->pGrid_J1->cursorTokenPosition.x = (pEvent->motion.x / TOKEN_WIDTH);
                pGameSession->pGrid_J1->cursorTokenPosition.y = (pEvent->motion.y / TOKEN_HEIGHT);

                // si drag and drop et changement de case
                if ( dragAndDrop && ( cursorTokenPositionTemp.x != pGameSession->pGrid_J1->cursorTokenPosition.x || cursorTokenPositionTemp.y != pGameSession->pGrid_J1->cursorTokenPosition.y ) ){

                    int distX = sqrt( pow( pGameSession->pGrid_J1->cursorTokenPosition.x - dragStart.x, 2) );
                    int distY = sqrt( pow( pGameSession->pGrid_J1->cursorTokenPosition.y - dragStart.y, 2) );

                    if ( ( distX == 1 && distY == 0 ) || ( distX == 0 && distY == 1 ) ){

                        PermuteToken(pGameSession->pGrid_J1, dragStart.x, dragStart.y, pGameSession->pGrid_J1->cursorTokenPosition.x, pGameSession->pGrid_J1->cursorTokenPosition.y);

                        if ( IsLineOnGrid(pGameSession->pGrid_J1) == false ){

                            //fprintf(stdout,"game.c -> Game_event(Grid *pGameSession->pGrid_J1, SDL_Event *pEvent, bool *pQuit) -> switch(pEvent->type) -> case SDL_MOUSEMOTION -> !IsLineOnGrid(pGrid) ");
                            pGameSession->pGrid_J1->tokens[pGameSession->pGrid_J1->cursorTokenPosition.y][pGameSession->pGrid_J1->cursorTokenPosition.x].image_background = image_cursorRed;
                            pGameSession->pGrid_J1->tokens[pGameSession->pGrid_J1->cursorTokenPosition.y][pGameSession->pGrid_J1->cursorTokenPosition.x].drawBackground = true;
                        }

                        ChangeAlignedTokenBackgroundImage(pGameSession->pGrid_J1, image_cursorGreen);

                        PermuteToken(pGameSession->pGrid_J1, dragStart.x, dragStart.y, pGameSession->pGrid_J1->cursorTokenPosition.x, pGameSession->pGrid_J1->cursorTokenPosition.y);

                        PermuteTokenImage(pGameSession->pGrid_J1, dragStart.x, dragStart.y, pGameSession->pGrid_J1->cursorTokenPosition.x, pGameSession->pGrid_J1->cursorTokenPosition.y);
                    }
                    else{

                        ResetTokenImages(pGameSession->pGrid_J1);
                        pGameSession->pGrid_J1->tokens[pGameSession->pGrid_J1->cursorTokenPosition.y][pGameSession->pGrid_J1->cursorTokenPosition.x].drawBackground = true;

                        if  ( !( distX == 1 && distY == 0 ) && !( distX == 0 && distY == 1 ) && !( distX == 0 && distY == 0 ) )
                            dragAndDrop = false;
                    }
                }
                else if ( dragAndDrop == false ){

                    for(int i = 0; i < pGameSession->pGrid_J1->height; i++){
                        for(int j = 0; j < pGameSession->pGrid_J1->width; j++){

                            pGameSession->pGrid_J1->tokens[i][j].drawBackground = false;
                        }
                    }
                    pGameSession->pGrid_J1->tokens[pGameSession->pGrid_J1->cursorTokenPosition.y][pGameSession->pGrid_J1->cursorTokenPosition.x].drawBackground = true;
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

    // boutons :

    Button_quit_event(&pGameSession->button_quit, pEvent, pDraw, pQuit);
    Button_direction_event(&pGameSession->button_direction, pEvent, pDraw, pGameSession->pGrid_J1);
}

// =========================================================
// Logiques
// =========================================================

void Game_logic(GameSession *pGameSession){

    //fprintf(stdout,"game.c : Game_logic(Grid *pGameSession->pGrid_J1)\n");

    if ( IsTokenMoving(pGameSession->pGrid_J1) == false && IsTokenDestructing(pGameSession->pGrid_J1) == false){

        if( IsLineOnGrid(pGameSession->pGrid_J1) == true ){

            // score
            Calc_Score(pGameSession->pGrid_J1);

            // détruit les lignes et remplie les cases manquantes du tableau
            fprintf(stdout,"Nombre de jeton detruit(s) : %d\n", DestroyAlignedTokens(pGameSession->pGrid_J1) );
        }
        else {

            if(IsTokenOfType(pGameSession->pGrid_J1, NONE ) == true ){

                while( IsTokenOfType(pGameSession->pGrid_J1, NONE ) == true ){

                    // regroupe tout les jetons
                    RegroupTokens(pGameSession->pGrid_J1);

                    // remplie les espaces vides
                   InjectLigne(pGameSession->pGrid_J1);
                }



                // recalcul les mouvements possibles
                MoveAvailable(pGameSession->pGrid_J1);

            }else {

                if(pGameSession->pGrid_J1->moveAvailable == 0){

                    RandomizeGrid(pGameSession->pGrid_J1);
                }
            }
        }
    }

    /* maj des mlabels */
    sprintf(pGameSession->label_nbMove.text," NbCoups : %d", pGameSession->pGrid_J1->nbMove);
    sprintf(pGameSession->label_score.text,"Score : %d ", pGameSession->pGrid_J1->score);
    sprintf(pGameSession->label_mouvements.text,"Nombre de mouvement : %d",pGameSession->pGrid_J1->moveAvailable);

    /* animations */
    Grid_anim(pGameSession->pGrid_J1);
}

// =========================================================

void Game_draw(GameSession *pGameSession, SDL_Renderer *pRenderer){

    Grid_draw(pGameSession->pGrid_J1,pRenderer);     // désine la grille sur le renderer

    UI_label_draw(&pGameSession->label_score,pRenderer);
    UI_label_draw(&pGameSession->label_nbMove,pRenderer);
    UI_label_draw(&pGameSession->label_mouvements,pRenderer);
    UI_button_draw(&pGameSession->button_quit, pRenderer);
    UI_button_draw(&pGameSession->button_direction, pRenderer);
}

// =========================================================












