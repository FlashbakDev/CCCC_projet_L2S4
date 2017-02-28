/*

constants.h
-----------

Par Benjamin, pour le projet CCCC le 24/02/2017.

R�le : d�finit des constantes pour tout le programme ( taille de la fen�tre... ).

*/

#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_mixer.h>

    #include <stdio.h>
    #include <math.h>
    #include <stdlib.h>
    #include <time.h>

    #include "stdbool.h"

    #define WINDOW_WIDTH        800
    #define WINDOW_HEIGHT       600
    #define TOKEN_WIDTH         64
    #define TOKEN_HEIGHT        64

    typedef enum Directions { UP, DOWN, LEFT, RIGHT }Directions;

    typedef enum TokenTypes { NONE, TOKEN }TokenTypes;

    typedef enum GameStates { MENU, GAME, EDITOR }GameStates;

    typedef enum Colors { RED, BLUE, GREEN, YELLOW, PURPLE }Colors;

    typedef struct Token{

        TokenTypes type;
        Colors color;
        bool aligned;
    }Token;

    typedef struct Grid{

        Token **pastTokens;
        Token **tokens;
        int nbMove;
        int width, height;
        int nbColor;
    }Grid;

#endif // CONSTANTS_H_INCLUDED
