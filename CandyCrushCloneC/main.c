#include <stdio.h>
#include "stdbool.h"

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
    // void SDL_SetWindowIcon(SDL_Window*  window , SDL_Surface* icon); //ajoute une icône à la fenêtre

    /* programme */

    Grid *grid1 = NewGrid(10,10,20);

    /* affichage */
    /*SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // Création du renderer
    DrawGrid(grid1,pRenderer);*/
    SDL_Delay(3000); /* Attendre trois secondes, que l'utilisateur voie la fenêtre */

    /* fin du programme */

    SDL_DestroyWindow(pWindow);

    SDL_Quit();

    return 0;
}
