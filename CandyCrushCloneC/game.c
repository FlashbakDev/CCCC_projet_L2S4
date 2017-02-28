#include "game.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

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

            if ( pGrid->tokens[i][j].type == TOKEN ){

                // vérification verticale
                if ( i > 0 && i < pGrid->height - 1){

                    if ( pGrid->tokens[i][j].color == pGrid->tokens[i-1][j].color && pGrid->tokens[i][j].color == pGrid->tokens[i+1][j].color ){

                        pGrid->tokens[i][j].aligned = true;
                    }
                }

                // vérification horizontale
                if ( j > 0 && j < pGrid->width - 1){

                    if ( pGrid->tokens[i][j].color == pGrid->tokens[i][j-1].color && pGrid->tokens[i][j].color == pGrid->tokens[i][j+1].color ){

                        pGrid->tokens[i][j].aligned = true;
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

void *DrawGrid(Grid *pGrid, SDL_Renderer *pRenderer){

    SDL_Surface *pSurface_Token[5];
    pSurface_Token[0] = IMG_Load("./data/Token_red.png");
    pSurface_Token[1] = IMG_Load("./data/Token_blue.png");
    pSurface_Token[2] = IMG_Load("./data/Token_green.png");
    pSurface_Token[3] = IMG_Load("./data/Token_yellow.png");
    pSurface_Token[4] = IMG_Load("./data/Token_purple.png");

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer,pSurface_Token[pGrid->tokens[i][j].color]);

            SDL_Rect position;
            SDL_QueryTexture(pTexture, NULL, NULL, &position.w, &position.h);
            position.x = i * position.h;
            position.y = j * position.w;

            SDL_RenderCopy(pRenderer,pTexture,NULL,&position);

            SDL_free(pTexture);
        }
    }

    for(int i = 0; i < 5; i ++){ SDL_free(pSurface_Token[i]); }
    SDL_free(pSurface_Token);
}






















