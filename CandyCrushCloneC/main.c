#include <stdio.h>

#include "constants.h"
#include "game.h"
#include "files.h"
#include "editor.h"

int main(int argc, char* argv[]){

    /* Initialisation */

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* pWindow = NULL;
    pWindow = SDL_CreateWindow("Candy Crush Clone C",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,WINDOW_WIDTH,WINDOW_HEIGHT,SDL_WINDOW_SHOWN);
    // void SDL_SetWindowIcon(SDL_Window*  window , SDL_Surface* icon); //ajoute une icône à la fenêtre

    /* initialisation du jeu */
    int gridHeight = 10;
    int gridWidth = 10;
    int nbMove = 20;
    int nbColor = 5;

    Grid *grid1 = NewGrid(gridWidth,gridHeight,nbMove,nbColor);
    SDL_SetWindowSize(pWindow,grid1->width * TOKEN_WIDTH, grid1->height * TOKEN_HEIGHT);

    // renderer = canvas ( endroit où l'on va déssiner )
    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Rect *pBackground = {0,0,grid1->width * TOKEN_WIDTH,grid1->height * TOKEN_HEIGHT};

    // curseur
    SDL_Surface *pSurface_Cursor = IMG_Load("./data/MouseOver.png");
    SDL_Texture *pTexture_CursorOver = SDL_CreateTextureFromSurface(pRenderer,pSurface_Cursor);
    SDL_Rect position_CursorOver;
    SDL_QueryTexture(pTexture_CursorOver, NULL, NULL, &position_CursorOver.w, &position_CursorOver.h);

    // gestionnaire d'évènement
    SDL_Event event;

    /* boucle de jeu */
    bool loop = true;
    while( loop ){

        /* évènements */
        SDL_WaitEvent(&event);
        switch(event.type){

            // appuie d'une touche du clavier
            case SDL_KEYDOWN:{

                switch(event.key.keysym.sym){

                    // appuie sur echap
                    case SDLK_ESCAPE: {

                        fprintf(stdout,"Appuie sur echap, fin de la boucle de jeu");
                        loop = false;
                    }
                    break;
                }
            }
            break;

            // bouttons de souris appuié
            case SDL_MOUSEBUTTONDOWN:{

                switch(event.button.button){

                    // bouton gauche
                    case SDL_BUTTON_LEFT:{

                        fprintf(stdout,"Bouton de la souris gauche appuiye.\n");
                    }
                    break;
                }
            }
            break;

            case SDL_MOUSEBUTTONUP:{

                switch(event.button.button){

                    case SDL_BUTTON_LEFT:{

                        fprintf(stdout,"Bouton de la souris gauche relache.\n");
                    }
                    break;
                }
            }
            break;

            // mouvement de souris
            case SDL_MOUSEMOTION:{

                fprintf(stdout,"Coordonnees de la souris : ( %d , %d )\n", event.motion.x, event.motion.y);

                position_CursorOver.x = TOKEN_WIDTH * (event.motion.x / TOKEN_WIDTH);
                position_CursorOver.y = TOKEN_HEIGHT * (event.motion.y / TOKEN_HEIGHT);
            }
            break;
        }

        /* logique */

        /* affichage */
        SDL_RenderClear(pRenderer);                                                 // efface tout le contenu du renderer

        SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 0 );                       // pour faire déssiner en blanc
        SDL_RenderFillRect(pRenderer,pBackground);                                  // pour déssiner le background
        SDL_RenderCopy(pRenderer,pTexture_CursorOver,NULL,&position_CursorOver);    // pour le curseur de la souris

        DrawGrid(grid1,pRenderer);                                                  // déssine la grille sur le renderer

        SDL_RenderPresent(pRenderer);                                               // déssine le renderer à l'écran
    }

    /* fin du programme */

    SDL_DestroyWindow(pWindow);

    SDL_Quit();

    return 0;
}
