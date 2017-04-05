/*

game.h
-----------

Par Benjamin, pour le projet CCCC le 24/02/2017.

Rôle : fonctions gérants une partie de CCCC.

*/

// =========================================================

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

// =========================================================

#include "core.h"
#include "UI.h"
#include "constants.h"

// =========================================================

/* initailisation et chargement des ressources */
SDL_Renderer *InitGame(char * pChar_name, Array *pArray, int w, int h);

/* fermeture du jeu / déchargement */
int CleanGame(Array *pArray);

/* Crée une grille de largeur X hauteur remplie aléatoirement */
Grid *NewGrid(SDL_Rect rect, int nbMove, int nbColor,bool randomizeInsert,int nbHelp, int nbSuperHelp, int nbRevertOnce);

/* remplie la grille aléatoirement */
void RandomizeGrid(Grid *pGrid);

/* marque les jetons alignés */
void CheckGrid(Grid *pGrid);

/* vérifie s'il y a des lignes complete sur la grille */
bool IsLineOnGrid(Grid *pGrid);

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

/*Change la direction de la grille aléatoirement */
void ChangeDirectionRandom(Grid *pGrid);

/*Change la direction de la grille*/
void ChangeDirection(Grid *pGrid, Directions dir);

/* regroupe les jetons dans la direction donné */
void RegroupTokens(Grid *pGrid);

/* insère de nouveaux jetons aléatoirement */
void InjectLigne(Grid *pGrid);

/*Inverse la direction*/
Directions ReverseDirection(Directions dir);

/* permute 2 jetons */
void PermuteToken(Grid *pGrid,int x1,int y1,int x2,int y2);

/* permute les textures de 2 jetons ( prévisualisation ) */
void PermuteTokenImage(Grid *pGrid,int x1,int y1,int x2,int y2);

/* crée un nouveau jeton avec une couleur aléatoire parmis celles donnés */
void InitRandomToken(Grid *pGrid, Token *token, int nbColor, int x, int y);

/* anime les mouvements des jetons */
void AnimMovingTokens(Grid *pGrid);

/* anime les déstructions des jetons */
void AnimDestructingTokens(Grid *pGrid);

/* anime la grille */
void Grid_anim(Grid *pGrid);
/* detect les coudées et ajoute les jetons speciaux */
void Token_speciaux(Grid *pGrid);

TokenTypes TypeRandom(int borne);

/* calcul le score */
int Calc_Score(Grid *pGrid );

/* change la couleur de fond des jetons alignés */
void ChangeAlignedTokenBackgroundImage(Grid *pGrid, Image image);

// =========================================================

void Button_direction_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid );

void Game_event(Grid *pGrid, SDL_Event *pEvent, bool *pQuit);

void Game_logic(Grid *pGrid);

void GameSession(int width, int height,int nbColor, int nbMove,bool randomizeInsert,int nbHelp, int nbSuperHelp, int nbRevertOnce);

// =========================================================

#endif // GAME_H_INCLUDED
