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

    #define WINDOW_WIDTH        800
    #define WINDOW_HEIGHT       600
    #define TOKEN_WIDTH         64
    #define TOKEN_HEIGHT        64

    typedef enum Directions Directions;
    enum Directions { UP, DOWN, LEFT, RIGHT };

    typedef enum TokenTypes TokenTypes;
    enum TokenTypes { VIDE, TOKEN };

    typedef enum GameStates gameStates;
    enum GameStates { MENU, GAME, EDITOR};

    typedef struct Token Token;
    struct Token{
        SDL_Point position;
        TokenTypes type;
        char* color;
    };

    typedef struct Grid Grid;
    struct Grid{

        Token **pastTokens;
        Token **tokens;
        int nbMove;
        int width, height;
    };

#endif // CONSTANTS_H_INCLUDED
