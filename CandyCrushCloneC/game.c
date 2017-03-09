#include "game.h"

// =========================================================

Grid *NewGrid(int width, int height, int nbMove, int nbColor){

    /* al�atoire */
    srand(time(NULL));

    Grid *pGrid = malloc(sizeof(Grid));

    pGrid->width = width;
    pGrid->height = height;
    pGrid->nbMove = nbMove;
    pGrid->nbColor = nbColor;
    pGrid->pastTokens = NULL;

    /* allocation de la grille et remplissage */
    pGrid->tokens = (Token*)malloc( pGrid->height * sizeof(Token*));
    if ( pGrid->tokens == NULL ){ return NULL; }

    for(int i = 0; i < pGrid->height; i++){

        pGrid->tokens[i] = (Token*)malloc( pGrid->width * sizeof(Token));

        /* d�sallocation de la m�moire en cas d'erreur */
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

    int cpt = 0;

    do{

        cpt++;

        for(int i = 0; i < pGrid->height; i++){
            for(int j = 0; j < pGrid->width; j++){

                InitRandomToken(&pGrid->tokens[i][j], pGrid->nbColor, j, i);
            }
        }

    }while(IsLigneOnGrid(pGrid) == true );

    fprintf(stdout, "grille trouve en %d fois.\n", cpt);
}

// =========================================================

void InitRandomToken(Token *token, int nbColor, int x, int y){

    token->type = TOKEN;
    token->color = (Colors)(rand() % nbColor);
    token->isMoving = false;
    token->isDestruct = false;
    token->startDestructAnim = -1;

    token->textureSize = 100;
    CalculTokenRectTexure(token,x,y);
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

                // v�rification verticale
                if ( i > 0 && i < pGrid->height - 1){

                    if ( pGrid->tokens[i][j].color == pGrid->tokens[i-1][j].color && pGrid->tokens[i][j].color == pGrid->tokens[i+1][j].color ){

                        pGrid->tokens[i][j].aligned = true;
                        pGrid->tokens[i-1][j].aligned = true;
                        pGrid->tokens[i+1][j].aligned = true;
                    }
                }

                // v�rification horizontale
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

int Calc_Score(Grid *pGrid ){

    /* actualisation de la grille */
    CheckGrid(pGrid);

    int Score = 0;
    int Nb_Align;
    int Multi;
    Colors SaveColor;
    /* traitement au cas par cas */

    //Verification des allignements horizontaux
    for(int i = 0; i < pGrid->height; i++){
        Nb_Align = 1;
        SaveColor = pGrid->tokens[i][0].color;
        for(int j = 1; j < pGrid->width; j++){

            if(pGrid->tokens[i][j].color == SaveColor)
            {
                Nb_Align++;
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
                    Score += Multi * TOKEN_SCORE * Nb_Align;

                    printf("Score de la ligne : %d \n", Score);
                }
                Nb_Align = 1;
                SaveColor = pGrid->tokens[i][j].color;
            }



        }

    }
    //Verification des allignements verticaux
    for(int j = 0; j < pGrid->width; j++){
        Nb_Align = 1;
        SaveColor = pGrid->tokens[0][j].color;
        for(int i = 1; i < pGrid->height; i++){

            if(pGrid->tokens[i][j].color == SaveColor)
            {
                Nb_Align++;
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
                    Score += Multi * TOKEN_SCORE * Nb_Align;

                    printf("Score de la colonne : %d \n", Score);
                }
                Nb_Align = 1;
                SaveColor = pGrid->tokens[i][j].color;
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

                    // temporairement, on hard code le fait que la ligne superieur doit �tre hors champs
                    if (  ( GetColumnUpperToken(pGrid,j)->rect_texture.y / TOKEN_HEIGHT ) - 1 < 0 )
                        InitRandomToken( &pGrid->tokens[0][j], pGrid->nbColor, j, ( GetColumnUpperToken(pGrid,j)->rect_texture.y / TOKEN_HEIGHT ) - 1 );
                    else
                        InitRandomToken( &pGrid->tokens[0][j], pGrid->nbColor, j, - 1 );
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















