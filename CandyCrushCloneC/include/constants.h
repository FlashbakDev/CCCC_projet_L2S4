/*

constants.h
-----------

Par Benjamin, pour le projet CCCC le 24/02/2017.

Rôle : définit des constantes pour tout le programme ( taille de la fenêtre... ).

*/

#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

    #define TAILLE_TOKEN        34
    #define NB_TOKEN_WIDTH      10
    #define NB_TOKEN_HEIGHT     10
    #define WINDOW_WIDTH        800
    #define WINDOW_HEIGHT       600

    typedef enum Directions Directions;
    enum Directions { UP, DOWN, LEFT, RIGHT };

    typedef enum TokenTypes TokenTypes;
    enum TokenTypes { VIDE, TOKEN };

    typedef enum GameStates gameStates;
    enum GameStates { MENU, GAME, EDITOR};

#endif // CONSTANTS_H_INCLUDED
