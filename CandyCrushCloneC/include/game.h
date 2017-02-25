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

/* affiche la grille */
void *DrawGrid(Grid *pGrid, SDL_Renderer *pRenderer);

#endif // GAME_H_INCLUDED
