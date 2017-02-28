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

    #include <stdio.h>
    #include <math.h>
    #include <stdlib.h>
    #include <time.h>

    #include "stdbool.h"

    #define WINDOW_WIDTH        800
    #define WINDOW_HEIGHT       600
    #define TOKEN_WIDTH         64
    #define TOKEN_HEIGHT        64
    #define FRAME_PER_SECOND    60
    #define FALL_SPEED          5

    typedef enum Directions { UP, DOWN, LEFT, RIGHT }Directions;

    typedef enum TokenTypes { NONE, TOKEN }TokenTypes;

    typedef enum GameStates { MENU, GAME, EDITOR }GameStates;

    typedef enum Colors { RED, BLUE, GREEN, YELLOW, PURPLE }Colors;

    typedef struct Token{

        TokenTypes type;
        Colors color;
        bool aligned;

        //SDL_Texture texture;
        SDL_Rect texturePosition;

        bool isMoving;

    }Token;

    typedef struct Grid{

        Token **pastTokens; // historique
        Token **tokens;
        int nbMove;
        int width, height;
        int nbColor;

    }Grid;

#endif // CONSTANTS_H_INCLUDED
