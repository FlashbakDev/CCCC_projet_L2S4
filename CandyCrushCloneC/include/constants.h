/*

constants.h
-----------

Par Benjamin, pour le projet CCCC le 24/02/2017.

Rôle : définit des constantes pour tout le programme ( taille de la fenêtre... ).

*/

#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_mixer.h>
    #include <SDL2/SDL_ttf.h>

    #include <stdio.h>
    #include <math.h>
    #include <stdlib.h>
    #include <time.h>

    #include "stdbool.h"

    #define WINDOW_WIDTH        800
    #define WINDOW_HEIGHT       600
    #define TOKEN_WIDTH         64
    #define TOKEN_HEIGHT        64
    #define FRAME_PER_SECOND    60      // rafraichissement de l'écran
    #define FALL_SPEED          6       // en pixel par frame
    #define DESTRUCT_SPEED      1200    // en ms ( pour aller de 100% à 0% de taille de texture )
    #define TOKEN_SCORE         10      // score de base par jeton détruit

    #define UI_MAX_TEXT         100     // longueur du texte maximal affiché à l'écran

#endif // CONSTANTS_H_INCLUDED
