/*

game.h
-----------

Par Benjamin, pour le projet CCCC le 24/02/2017.

R�le : fonctions g�rants une partie de CCCC.

*/

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "constants.h"

/* Cr�e une grille de largeur X hauteur remplie al�atoirement */
Grid *NewGrid(int width, int height, int nbMove, int nbColor);

/* remplie la grille al�atoirement */
void RandomizeGrid(Grid *pGrid);

/* marque les jetons align�s */
void CheckGrid(Grid *pGrid);

/* v�rifie s'il y a des lignes complete sur la grille */
bool IsLigneOnGrid(Grid *pGrid);

/* v�rifie s'il y a un token du type donn� */
bool IsTokenOfType(Grid *pGrid, TokenTypes type);

/* indique si une animation d'un jeton en train de se d�placer est en cours */
bool IsTokenMoving(Grid *pGrid);

/* renvoie le jeton le plus haut de la colonne donn� ( pour les animations... ) */
Token *GetColumnUpperToken(Grid *pGrid,int x);

/* d�truit les jetons align�s ( retourne le nombre de jetons d�truit ) */
int DestroyAlignedTokens(Grid *pGrid);

/* regroupe les jetons dans la direction donn� */
void RegroupTokens(Grid *pGrid, Directions dir);

/* ins�re de nouveaux jetons al�atoirement */
void InjectLigne(Grid *pGrid, Directions dir);

/* affiche la grille */
void DrawGrid(Grid *pGrid, SDL_Renderer *pRenderer, SDL_Surface *pSurface_Token[5]);

/* permute 2 jetons */
void PermuteToken(Grid *pGrid,int x1,int y1,int x2,int y2);

/* cr�e un nouveau jeton avec une couleur al�atoire parmis celles donn�s */
void InitRandomToken(Token *token, int nbColor, int x, int y);

/* anime les mouvements des jetons */
void AnimTokens(Grid *pGrid);

#endif // GAME_H_INCLUDED
