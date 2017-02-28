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

                pGrid->tokens[i][j].type = TOKEN;
                pGrid->tokens[i][j].color = (Colors)(rand() % pGrid->nbColor);
            }
        }

    }while(IsLigneOnGrid(pGrid) == true );

    fprintf(stdout, "grille trouve en %d fois.\n", i);
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

bool IsTokenOfType(Grid *pGrid, TokenTypes type){

   for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].type == type ){ return true; }
        }
    }

    return false;
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

                            pGrid->tokens[(i + DirectionsVectors[dir][1])][(j + DirectionsVectors[dir][0])].type = TOKEN;
                            pGrid->tokens[(i + DirectionsVectors[dir][1])][(j + DirectionsVectors[dir][0])].color = pGrid->tokens[i][j].color;

                            pGrid->tokens[i][j].type = NONE;

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

                //fprintf(stdout,"! Jeton en posisition (%d,%d) : Couleur = %d, type = %d, aligne = %d.\n",0, i, pGrid->tokens[i][0].color, pGrid->tokens[i][0].type,pGrid->tokens[i][0].aligned);

                if ( pGrid->tokens[i][0].type == NONE ){

                    pGrid->tokens[i][0].type = TOKEN;
                    pGrid->tokens[i][0].color = (Colors)(rand() % pGrid->nbColor);

                    //fprintf(stdout,"Ajout d'un jeton aleatoire en (%d, %d) de couleur %d.\n",0,i,pGrid->tokens[i][0].color);
                }
            }
        }
        break;

        case DOWN:{

            for(int i = 0; i < pGrid->width; i++){

                if ( pGrid->tokens[i][pGrid->height-1].type == NONE ){

                    pGrid->tokens[i][pGrid->height-1].type = TOKEN;
                    pGrid->tokens[i][pGrid->height-1].color = (Colors)(rand() % pGrid->nbColor);
                }
            }
        }
        break;

        case LEFT:{

            for(int i = 0; i < pGrid->height; i++){

                if ( pGrid->tokens[0][i].type == NONE ){

                    pGrid->tokens[0][i].type = TOKEN;
                    pGrid->tokens[0][i].color = (Colors)(rand() % pGrid->nbColor);
                }
            }
        }
        break;

        case RIGHT:{

            for(int i = 0; i < pGrid->height; i++){

                if ( pGrid->tokens[pGrid->width-1][i].type == NONE ){

                    pGrid->tokens[pGrid->width-1][i].type = TOKEN;
                    pGrid->tokens[pGrid->width-1][i].color = (Colors)(rand() % pGrid->nbColor);
                }
            }
        }
        break;
    }
}

// =========================================================

void DrawGrid(Grid *pGrid, SDL_Renderer *pRenderer, SDL_Surface *pSurface_Token[5]){

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].type != NONE ){

                SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer,pSurface_Token[pGrid->tokens[i][j].color]);

                SDL_Rect position;
                SDL_QueryTexture(pTexture, NULL, NULL, &position.w, &position.h);
                position.x = i * position.h;
                position.y = j * position.w;

                SDL_RenderCopy(pRenderer,pTexture,NULL,&position);

                SDL_DestroyTexture(pTexture);
            }
        }
    }
}






















