#include <stdio.h>

#include "constants.h"
#include "game.h"
#include "files.h"
#include "editor.h"

int main(int argc, char* argv[]){

    SDL_Surface *pScreen = NULL;
    SDL_Surface *pMenu = NULL;

    SDL_Rect coordMenu;

    SDL_Event event;

    bool loop = true;

    /* Initialisation */

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* pWindow = NULL;
    pWindow = SDL_CreateWindow("Candy Crush Clone C",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,WINDOW_WIDTH,WINDOW_HEIGHT,SDL_WINDOW_SHOWN);
    // void SDL_SetWindowIcon(SDL_Window*  window , SDL_Surface* icon); //ajoute une ic�ne � la fen�tre

    /* initialisation du jeu */
    int gridHeight = 10;
    int gridWidth = 10;
    int nbMove = 20;
    int nbColor = 5;

    Grid *grid1 = NewGrid(gridWidth,gridHeight,nbMove,nbColor);
    SDL_SetWindowSize(pWindow,grid1->width * TOKEN_WIDTH, grid1->height * TOKEN_HEIGHT);

    /* boucle de jeu */
    while( loop ){

        /* �v�nements */

        /* logique */

        /* affichage */

        // renderer = canvas ( endroit o� l'on va d�ssiner )
        SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        DrawGrid(grid1,pRenderer); // d�ssine la grille sur le renderer

        SDL_RenderPresent(pRenderer); // d�ssine le renderer � l'�cran

        //SDL_Delay(3000); /* Attendre trois secondes, que l'utilisateur voie la fen�tre */

        SDL_RenderClear(pRenderer);

        //loop = false;
    }

    /* fin du programme */

    SDL_DestroyWindow(pWindow);

    SDL_Quit();

    return 0;
}
