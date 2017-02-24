#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <stdio.h>
#include "stdbool.h"

#include "constants.h"
#include "game.h"
#include "files.h"
#include "editor.h"

int main(int argc, char* argv[]){

    /* Initialisation */

    if ( SDL_Init(SDL_INIT_VIDEO) != 0 ){

        fprintf( stdout, "Echec de l'initialisation de la SDL (%s)\n", SDL_GetError());
        return -1;
    }

    /* création de la fenêtre */

    SDL_Window* pWindow = NULL;
    pWindow = SDL_CreateWindow("Ma premiere application SDL2",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,640,480,SDL_WINDOW_SHOWN);

    if( pWindow )
    {
        /* programme */

        // renderer
        SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // Création du renderer
        if(pRenderer == NULL){

            printf("Erreur lors de la creation d'un renderer : %s",SDL_GetError());
            return -1;
        }

        // surface
        SDL_Surface *pSurface = IMG_Load("./data/test.jpg");
        if ( !pSurface ){

            printf("Erreur de chargement de la surface : %s",SDL_GetError());
            return -1;
        }

        // texture
        SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer,pSurface);

        // affichage

        SDL_Rect position;
        position.x = 0;
        position.y = 0;
        SDL_QueryTexture(pTexture, NULL, NULL, &position.w, &position.h);

        // redimensionement de la fenetre
        SDL_SetWindowSize(pWindow, position.w, position.h );

        SDL_RenderCopy(pRenderer,pTexture,NULL,&position);
        SDL_RenderPresent(pRenderer);

        SDL_Delay(3000); /* Attendre trois secondes, que l'utilisateur voie la fenêtre */

        /* fin du programme */

        SDL_DestroyWindow(pWindow);
    }
    else
    {
        fprintf(stderr,"Erreur de création de la fenêtre: %s\n",SDL_GetError());
    }

    SDL_Quit();

    return 0;
}
