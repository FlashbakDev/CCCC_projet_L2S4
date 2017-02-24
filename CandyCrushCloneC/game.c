#include "game.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// =========================================================

Grid *NewGrid(int width, int height, int nbMove){

    Grid *grid;

    grid->width = width;
    grid->height = height;
    grid->nbMove = nbMove;

    grid->pastTokens = NULL;

    grid->tokens = malloc(sizeof(*grid->tokens) * height);

    if ( grid->tokens == NULL ){ return NULL; }

    for(int i = 0; i < height; i++){

        grid->tokens[i] = malloc(sizeof(*grid->tokens[i]) * width);

        if ( grid->tokens[i] == NULL ){ return NULL; }
    }

    // aléatoire
    srand(time(NULL));

    for(int i = 0; i < grid->height; i++){
        for(int j = 0; j < grid->width; j++){

            grid->tokens[i][j].position.x = j;
            grid->tokens[i][j].position.y = i;
            grid->tokens[i][j].type = TOKEN;

            int r = rand() % 5;

            switch(r){

                case 0:{

                    grid->tokens[i][j].color = "red";
                }
                break;

                case 1:{

                    grid->tokens[i][j].color = "blue";
                }
                break;

                case 2:{

                    grid->tokens[i][j].color = "green";
                }
                break;

                case 3:{

                    grid->tokens[i][j].color = "yellow";
                }
                break;

                case 4:{

                    grid->tokens[i][j].color = "purple";
                }
                break;
            }
        }
    }

    return grid;
}

// =========================================================

void *DrawGrid(Grid *pGrid, SDL_Renderer *pRenderer){

    SDL_Surface *pSurface_Token_Blue = IMG_Load("./data/Token_blue.png");
    SDL_Surface *pSurface_Token_Red = IMG_Load("./data/Token_red.png");
    SDL_Surface *pSurface_Token_Green = IMG_Load("./data/Token_green.png");
    SDL_Surface *pSurface_Token_Yellow = IMG_Load("./data/Token_yellow.png");
    SDL_Surface *pSurface_Token_Purple = IMG_Load("./data/Token_purple.png");

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            SDL_Texture *pTexture = NULL;

            if( strcmp( pGrid->tokens[i][j].color, "blue") == 0 ){

                pTexture = SDL_CreateTextureFromSurface(pRenderer,pSurface_Token_Blue);
            }

            SDL_Rect position;
            SDL_QueryTexture(pTexture, NULL, NULL, &position.w, &position.h);
            position.x = i * position.h;
            position.y = j * position.w;

            SDL_RenderCopy(pRenderer,pTexture,NULL,&position);

            SDL_free(pTexture);

        }
    }

    SDL_free(pSurface_Token_Blue);
    SDL_free(pSurface_Token_Red);
    SDL_free(pSurface_Token_Green);
    SDL_free(pSurface_Token_Yellow);
    SDL_free(pSurface_Token_Purple);
}






















