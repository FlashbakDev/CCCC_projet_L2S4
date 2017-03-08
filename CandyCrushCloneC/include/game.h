/*

game.h
-----------

Par Benjamin, pour le projet CCCC le 24/02/2017.

Rôle : fonctions gérants une partie de CCCC.

*/

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "constants.h"

/* Crée une grille de largeur X hauteur remplie aléatoirement */
Grid *NewGrid(int width, int height, int nbMove, int nbColor);

/* remplie la grille aléatoirement */
void RandomizeGrid(Grid *pGrid);

/* marque les jetons alignés */
void CheckGrid(Grid *pGrid);

/* vérifie s'il y a des lignes complete sur la grille */
bool IsLigneOnGrid(Grid *pGrid);

/* vérifie s'il y a un token du type donné */
bool IsTokenOfType(Grid *pGrid, TokenTypes type);

/* indique si une animation d'un jeton en train de se déplacer est en cours */
bool IsTokenMoving(Grid *pGrid);

/* indique si un jeton a une animation de destruction en cours */
bool IsTokenDestructing(Grid *pGrid);

/* renvoie le jeton le plus haut de la colonne donné ( pour les animations... ) */
Token *GetColumnUpperToken(Grid *pGrid,int x);

/* détruit les jetons alignés ( retourne le nombre de jetons détruit ) */
int DestroyAlignedTokens(Grid *pGrid);

/* regroupe les jetons dans la direction donné */
void RegroupTokens(Grid *pGrid, Directions dir);

/* insère de nouveaux jetons aléatoirement */
void InjectLigne(Grid *pGrid, Directions dir);

/* affiche la grille */
void DrawGrid(Grid *pGrid, SDL_Renderer *pRenderer, SDL_Surface *pSurface_Token[5]);

/* permute 2 jetons */
void PermuteToken(Grid *pGrid,int x1,int y1,int x2,int y2);

/* permute 2 jetons et place leurs texture au bon endroit */
void HardPermuteToken(Grid *pGrid,int x1,int y1,int x2,int y2);

/* crée un nouveau jeton avec une couleur aléatoire parmis celles donnés */
void InitRandomToken(Token *token, int nbColor, int x, int y);

/* anime les mouvements des jetons */
void AnimMovingTokens(Grid *pGrid);

/* anime les déstructions des jetons */
void AnimDestructingTokens(Grid *pGrid);

/* affiche les informations d'un jeton donné en console */
void DebugToken(Token token);

/* Calcul le rect_texture du jeton par raport à textureSize et à une position donné */
void CalculTokenRectTexure(Token *token, int x, int y);


int Calc_Score(Grid *pGrid );
#endif // GAME_H_INCLUDED
