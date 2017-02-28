#include "game.h"

// =========================================================

Grid *NewGrid(int width, int height, int nbMove, int nbColor){

    /* aléatoire */
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

    int i = 0;

    do{

        i++;

        for(int i = 0; i < pGrid->height; i++){
            for(int j = 0; j < pGrid->width; j++){

                InitRandomToken(&pGrid->tokens[i][j], pGrid->nbColor, i, j);
            }
        }

    }while(IsLigneOnGrid(pGrid) == true );

    fprintf(stdout, "grille trouve en %d fois.\n", i);
}

// =========================================================

void InitRandomToken(Token *token, int nbColor, int x, int y){

    token->type = TOKEN;
    token->color = (Colors)(rand() % nbColor);
    token->isMoving = false;

    token->texturePosition.h = TOKEN_HEIGHT;
    token->texturePosition.w = TOKEN_WIDTH;
    token->texturePosition.x = x * token->texturePosition.h;
    token->texturePosition.y = y * token->texturePosition.w;
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
                cpt++;
            }
        }
    }

    return cpt;
}

// =========================================================

void RegroupTokens(Grid *pGrid, Directions dir){

    int DirectionsVectors[4][2] = { {-1,0},{1,0},{0,-1},{0,1} };

    bool ok = false;
    while ( ok == false ){

        ok = true;

        for(int i = 0; i < pGrid->height; i++){
            for(int j = 0; j < pGrid->width; j++){

                if ( pGrid->tokens[i][j].type != NONE ){

                    if( i + DirectionsVectors[dir][1] >= 0 && i + DirectionsVectors[dir][1] < pGrid->height &&
                        j + DirectionsVectors[dir][0] >= 0 && j + DirectionsVectors[dir][0] < pGrid->width ){

                        if ( pGrid->tokens[(i + DirectionsVectors[dir][1])][(j + DirectionsVectors[dir][0])].type == NONE ){

                            PermuteToken(pGrid, i + DirectionsVectors[dir][1], j + DirectionsVectors[dir][0], i , j );

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

            for(int i = 0; i < pGrid->width; i++){

                if ( pGrid->tokens[i][0].type == NONE ){

                    //fprintf(stdout,"GetColumnUpperToken return (%d, %d).\n",GetColumnUpperToken(pGrid,i)->texturePosition.x, GetColumnUpperToken(pGrid,i)->texturePosition.y);

                    InitRandomToken( &pGrid->tokens[i][0], pGrid->nbColor, i , -( GetColumnUpperToken(pGrid,i)->texturePosition.y / TOKEN_HEIGHT )-1 );
                }
            }
        }
        break;

        /*case DOWN:{

            for(int i = 0; i < pGrid->width; i++){

                if ( pGrid->tokens[i][pGrid->height-1].type == NONE ){

                    InitRandomToken( &pGrid->tokens[i][pGrid->height-1], pGrid->nbColor, pGrid->height-1, i);
                }
            }
        }
        break;

        case LEFT:{

            for(int i = 0; i < pGrid->height; i++){

                if ( pGrid->tokens[0][i].type == NONE ){

                    InitRandomToken( &pGrid->tokens[0][i], pGrid->nbColor, i, 0);
                }
            }
        }
        break;

        case RIGHT:{

            for(int i = 0; i < pGrid->height; i++){

                if ( pGrid->tokens[pGrid->width-1][i].type == NONE ){

                    InitRandomToken( &pGrid->tokens[pGrid->width-1][i], pGrid->nbColor, i, pGrid->width-1);
                }
            }
        }
        break;*/
    }
}

// =========================================================

void DrawGrid(Grid *pGrid, SDL_Renderer *pRenderer, SDL_Surface *pSurface_Token[5]){

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].type != NONE ){

                SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer,pSurface_Token[pGrid->tokens[i][j].color]);

                SDL_RenderCopy(pRenderer,pTexture,NULL,&pGrid->tokens[i][j].texturePosition);

                SDL_DestroyTexture(pTexture);
            }
        }
    }
}

// =========================================================

void PermuteToken(Grid *pGrid,int x1,int y1,int x2,int y2){

    Token tmp = pGrid->tokens[x1][y1];
    pGrid->tokens[x1][y1] = pGrid->tokens[x2][y2];
    pGrid->tokens[x2][y2] = tmp;
}

// =========================================================

void AnimTokens(Grid *pGrid){

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].texturePosition.y != j * TOKEN_WIDTH ){

                pGrid->tokens[i][j].isMoving = true;

                if ( pGrid->tokens[i][j].texturePosition.y > j * TOKEN_WIDTH )
                    pGrid->tokens[i][j].texturePosition.y -= FALL_SPEED ;
                else
                    pGrid->tokens[i][j].texturePosition.y += FALL_SPEED ;

                if ( sqrt( pow( pGrid->tokens[i][j].texturePosition.y - j * TOKEN_WIDTH ,2) ) < FALL_SPEED ){

                    pGrid->tokens[i][j].texturePosition.y = j * TOKEN_WIDTH;
                    pGrid->tokens[i][j].isMoving = false;
                }
            }

            if ( pGrid->tokens[i][j].texturePosition.x != i * TOKEN_HEIGHT ){

                pGrid->tokens[i][j].isMoving = true;

                if ( pGrid->tokens[i][j].texturePosition.x > i * TOKEN_HEIGHT )
                    pGrid->tokens[i][j].texturePosition.x -= FALL_SPEED ;
                else
                    pGrid->tokens[i][j].texturePosition.x += FALL_SPEED ;

                if ( sqrt( pow( pGrid->tokens[i][j].texturePosition.x - i * TOKEN_HEIGHT ,2) ) < FALL_SPEED ){

                    pGrid->tokens[i][j].texturePosition.x = i * TOKEN_HEIGHT;
                    pGrid->tokens[i][j].isMoving = false;
                }
            }
        }
    }
}

// =========================================================



















