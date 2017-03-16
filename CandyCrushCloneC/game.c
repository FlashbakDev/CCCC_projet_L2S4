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

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            InitRandomToken(pGrid, &pGrid->tokens[i][j], pGrid->nbColor, j, i);
        }
    }

    while(IsLigneOnGrid(pGrid) == true || IsTokenOfType(pGrid, NONE ) == true){

            DestroyAlignedTokens(pGrid);

               // regroupe tout les jetons
            RegroupTokens(pGrid, DOWN);

                // remplie les espaces vides
            InjectLigne(pGrid, UP);

    }
}

// =========================================================

void InitRandomToken(Grid *pGrid, Token *token, int nbColor, int x, int y){

    token->type = TOKEN;
    token->color = (Colors)(rand() % nbColor);
    token->image = image_tokens[token->color];
    token->image_background = image_cursorBlue;
    token->isMoving = false;
    token->isDestruct = false;
    token->startDestructAnim = -1;

    token->textureSize = 100;
    CalculTokenRectTexure(pGrid, token,x,y);

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

    // anime les mouvement
    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].rect_image.x != ((j * TOKEN_WIDTH) + (TOKEN_WIDTH/2)) - (pGrid->tokens[i][j].rect_image.w / 2) ){

                pGrid->tokens[i][j].isMoving = true;

                if ( pGrid->tokens[i][j].rect_image.x > ((j * TOKEN_WIDTH) + (TOKEN_WIDTH/2)) - (pGrid->tokens[i][j].rect_image.w / 2) )
                    pGrid->tokens[i][j].rect_image.x -= FALL_SPEED ;
                else
                    pGrid->tokens[i][j].rect_image.x += FALL_SPEED ;

                if ( sqrt( pow( pGrid->tokens[i][j].rect_image.x - ( ((j * TOKEN_WIDTH) + (TOKEN_WIDTH/2)) - (pGrid->tokens[i][j].rect_image.w / 2)) ,2) ) < FALL_SPEED ){

                    pGrid->tokens[i][j].rect_image.x = ((j * TOKEN_WIDTH) + (TOKEN_WIDTH/2)) - (pGrid->tokens[i][j].rect_image.w / 2);
                    pGrid->tokens[i][j].isMoving = false;
                }
            }

            if ( pGrid->tokens[i][j].rect_image.y != (((i * TOKEN_HEIGHT) + (TOKEN_HEIGHT/2)) - (pGrid->tokens[i][j].rect_image.h / 2)) ){

                pGrid->tokens[i][j].isMoving = true;

                if ( pGrid->tokens[i][j].rect_image.y > (((i * TOKEN_HEIGHT) + (TOKEN_HEIGHT/2)) - (pGrid->tokens[i][j].rect_image.h / 2)) )
                    pGrid->tokens[i][j].rect_image.y -= FALL_SPEED ;
                else
                    pGrid->tokens[i][j].rect_image.y += FALL_SPEED ;

                if ( sqrt( pow( pGrid->tokens[i][j].rect_image.y - (((i * TOKEN_HEIGHT) + (TOKEN_HEIGHT/2)) - (pGrid->tokens[i][j].rect_image.h / 2)) ,2) ) < FALL_SPEED ){

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

                    CalculTokenRectTexure(pGrid, &pGrid->tokens[i][j], j, i);
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

int Calc_Score(Grid *pGrid ){

    /* actualisation de la grille */
    CheckGrid(pGrid);

    int Score = 0;
    int Nb_Align;
    int Multi;
    int Val = 0;
    Colors SaveColor;

    /* traitement au cas par cas */

    //Verification des allignements horizontaux
    for(int i = 0; i < pGrid->height; i++){
        Nb_Align = 1;
        SaveColor = pGrid->tokens[i][0].color;
        Val = pGrid->tokens[i][0].score;
        for(int j = 1; j < pGrid->width; j++){

            if(pGrid->tokens[i][j].color == SaveColor)
            {
                Nb_Align++;
                Val += pGrid->tokens[i][j].score;

            }
            else {

                if(Nb_Align >= 3)
                {

                    Multi = 1;
                    if(Nb_Align>=4)
                    {
                        Multi = 3;

                        if(Nb_Align>=5)
                        {
                            Multi = 10;

                        }
                    }
                    //Faire boucle en ajoutant les scores de jetons individuels
                    //Puis multiplier par Multi

                    Score += Multi * Val;

                    printf("Score de la ligne : %d \n", Score);
                }
                Nb_Align = 1;
                SaveColor = pGrid->tokens[i][j].color;
                Val = pGrid->tokens[i][j].score;
            }



        }

    }
    //Verification des allignements verticaux
    for(int j = 0; j < pGrid->width; j++){
        Nb_Align = 1;
        SaveColor = pGrid->tokens[0][j].color;
         Val = pGrid->tokens[0][j].score;
        for(int i = 1; i < pGrid->height; i++){

            if(pGrid->tokens[i][j].color == SaveColor)
            {
                Nb_Align++;
                Val += pGrid->tokens[i][j].score;
            }
            else {

                if(Nb_Align >= 3)
                {

                    Multi = 1;
                    if(Nb_Align>=4)
                    {
                        Multi = 3;

                        if(Nb_Align>=5)
                        {
                            Multi = 10;

                        }
                    }

                    Score += Multi * Val;
                    printf("Score de la colonne : %d \n", Score);
                }
                Nb_Align = 1;
                SaveColor = pGrid->tokens[i][j].color;
                 Val = pGrid->tokens[i][j].score;
            }
        }
    }
    return Score;
}

//====================================================

bool IsLigneOnGrid(Grid *pGrid){

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

Token *GetColumnUpperToken(Grid *pGrid,int x){

    for(int i = 0; i < pGrid->height; i ++ ){

        if ( pGrid->tokens[i][x].type != NONE ){ return &pGrid->tokens[i][x]; }
    }

    return NULL;
}

// =========================================================

int DestroyAlignedTokens(Grid *pGrid){

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

void RegroupTokens(Grid *pGrid, Directions dir){

    int DirectionsVectors[4][2] = { {0,-1},{0,1},{-1,0},{1,0} };

    bool ok = false;
    while ( ok == false ){

        ok = true;

        for(int i = 0; i < pGrid->height; i++){
            for(int j = 0; j < pGrid->width; j++){

                if ( pGrid->tokens[i][j].type != NONE ){

                    if( i + DirectionsVectors[dir][1] >= 0 && i + DirectionsVectors[dir][1] < pGrid->height &&
                        j + DirectionsVectors[dir][0] >= 0 && j + DirectionsVectors[dir][0] < pGrid->width ){

                        if ( pGrid->tokens[(i + DirectionsVectors[dir][1])][(j + DirectionsVectors[dir][0])].type == NONE ){

                            PermuteToken(pGrid, j + DirectionsVectors[dir][0], i + DirectionsVectors[dir][1], j, i );

                            ok = false;
                        }
                    }
                }
            }
        }
    }
}

// =========================================================

void InjectLigne(Grid *pGrid, Directions dir){

    switch(dir){

        case UP :{

            for(int j = 0; j < pGrid->width; j++){

                if ( pGrid->tokens[0][j].type != TOKEN ){

                    //fprintf(stdout,"GetColumnUpperToken(%d) return (%d, %d).\n",j,GetColumnUpperToken(pGrid,j)->texturePosition.x / TOKEN_WIDTH, GetColumnUpperToken(pGrid,j)->texturePosition.y / TOKEN_HEIGHT);

                    // temporairement, on hard code le fait que la ligne superieur doit être hors champs
                    if (  ( GetColumnUpperToken(pGrid,j)->rect_image.y / TOKEN_HEIGHT ) - 1 < 0 )
                        InitRandomToken(pGrid, &pGrid->tokens[0][j], pGrid->nbColor, j, ( GetColumnUpperToken(pGrid,j)->rect_image.y / TOKEN_HEIGHT ) - 1 );
                    else
                        InitRandomToken(pGrid, &pGrid->tokens[0][j], pGrid->nbColor, j, - 1 );
                }
            }
        }
        break;

        case DOWN :{

        }
        break;

        case LEFT :{

        }
        break;

        case RIGHT :{

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
// évenements
// =========================================================

void Button_quit_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit ){

    if ( UI_button_event(pButton, pEvent, pDraw) )
        *pQuit = true;
}

void GameEvent(Grid *pGrid, SDL_Event *pEvent, bool *pQuit){

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

                        pQuit = true;
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

                        pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].color = pGrid->nbColor-1;
                    }

                }
                break;

                case SDLK_e : {

                    RandomizeGrid(pGrid);
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

                            if ( IsLigneOnGrid(pGrid) == false ){

                                HardPermuteToken(pGrid, dragEnd.x, dragEnd.y, dragStart.x, dragStart.y);
                            }
                            else {

                                pGrid->nbMove --;

                                if ( pGrid->nbMove <= 0 ){

                                    pQuit = true;
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

                pGrid->cursorTokenPosition.x = (pEvent->motion.x / TOKEN_WIDTH);
                pGrid->cursorTokenPosition.y = (pEvent->motion.y / TOKEN_HEIGHT);

                rect_CursorOver.x = pGrid->cursorTokenPosition.x * TOKEN_WIDTH;
                rect_CursorOver.y = pGrid->cursorTokenPosition.y * TOKEN_HEIGHT;
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














