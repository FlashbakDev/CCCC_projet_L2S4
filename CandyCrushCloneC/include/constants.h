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
    #define FRAME_PER_SECOND    60
    #define FALL_SPEED          20       // en pixel par frame
    #define DESTRUCT_SPEED      1200    // en ms ( pour aller de 100% à 0% de taille de texture )

    #define Score_Default 10
    typedef enum Directions { UP, DOWN, LEFT, RIGHT }Directions;

    typedef enum TokenTypes { NONE, TOKEN }TokenTypes;

    typedef enum GameStates { MENU, GAME, EDITOR }GameStates;

    typedef enum Colors { RED, BLUE, GREEN, YELLOW, PURPLE, ORANGE }Colors;

    typedef struct Token{

        TokenTypes type;
        Colors color;
        bool aligned;

        SDL_Rect rect_texture;
        int textureSize;

        // animations
        bool isMoving;
        bool isDestruct;
        int startDestructAnim;

    }Token;

    typedef struct Grid{

        Token **pastTokens; // historique
        Token **tokens;
        int nbMove;
        int width, height;
        int nbColor;

    }Grid;

#endif // CONSTANTS_H_INCLUDED
