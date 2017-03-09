/*

core.h
-----------

Par Benjamin, pour le projet CCCC le 09/03/2017.

Rôle : fonctions générales du jeu, aussi bien affichage que logique.

*/

#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

#include "constants.h"

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

/* initailisation et création de la fenêtre */
SDL_Window *InitSDL(char * windowName);

/* affiche la grille */
void DrawGrid(Grid *pGrid, SDL_Renderer *pRenderer, SDL_Surface *pSurface_Token[5]);

/* permute 2 jetons et place leurs texture au bon endroit */
void HardPermuteToken(Grid *pGrid,int x1,int y1,int x2,int y2);

/* affiche les informations d'un jeton donné en console */
void DebugToken(Token token);

/* Calcul le rect_texture du jeton par raport à textureSize et à une position donné */
void CalculTokenRectTexure(Token *token, int x, int y);

#endif // CORE_H_INCLUDED
