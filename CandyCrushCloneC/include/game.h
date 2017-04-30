/*

game.h
-----------

Par Desbouchages Benjamin, Rousseau jérémy, pour le projet CCCC le 24/02/2017.

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

/* indique si la grille est stable */
bool IsRandomGridStabilized(Grid *pGrid);

/* retourne me nombre de jeton autrez que Null dans la grille */
int NbTokenOnGrid(Grid *pGrid);

/* met en vert le meillur coup possible */
void HighlightBestMove(Grid *pGrid);

/* détruit les jetons alignés ( retourne le nombre de jetons détruit ) */
int DestroyAlignedTokens(Grid *pGrid);

/* déclanche les fonctions de destructions suivant le tye de jeton */
int Token_special_action(TokenTypes t, Grid *pGrid, int y, int x);

/* Fonctions detruisants des jetons sur la grille suivant une certaine forme ou caracteristique */
int Destruct_color(Colors c, Grid *pGrid);
int Destruct_line(int y, Grid * pGrid);
int Destruct_column(int x, Grid *pGrid);
int Destruct_square(int y,int x,int l,Grid *pGrid);

/* compare la couleur de 2 token */
bool Compare_TokenColor(Token t1, Token t2);

/* compare la couleur d'un token avec la couleur donné */
bool Compare_TokenColor_color(Token t1, Colors c);

/*Change la direction de la grille*/
void ChangeDirection(Grid *pGrid, Directions dir);

/* regroupe les jetons dans la direction actuel de la grille */
void RegroupTokens(Grid *pGrid);

/* insère de nouveaux jetons aléatoires dans le sens de la grille */
void InjectLigne(Grid *pGrid);

/* retourne la valeur inverse de la direction donné */
Directions ReverseDirection(Directions dir);

/* permute 2 jetons */
void PermuteToken(Grid *pGrid,int x1,int y1,int x2,int y2);

/* permute les textures de 2 jetons ( prévisualisation ) */
void PermuteTokenImage(Grid *pGrid,int x1,int y1,int x2,int y2);

/* crée un nouveau jeton avec une couleur aléatoire parmis celles de la grille */
void InitRandomToken(Grid *pGrid, Token *token, int x, int y);

/* anime les mouvements des jetons */
void AnimMovingTokens(Grid *pGrid);

/* anime les déstructions des jetons */
void AnimDestructingTokens(Grid *pGrid);

/* anime la grille */
void Grid_anim(Grid *pGrid);

/* detect les coudées et ajoute les jetons speciaux */
void Token_speciaux(Grid *pGrid);

/* calcul le score */
int Calc_Score(Grid *pGrid );

/* change la couleur de fond des jetons alignés */
void ChangeAlignedTokenBackgroundImage(Grid *pGrid, Image image);
void ChangeColorTokenBackgroundImage(Grid *pGrid, Image image, Colors c);

// =========================================================
// evenements sur les éléments d'interface
// =========================================================

void GameButton_help_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid );

void GameButton_superHelp_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid );

void GameButton_revertOnce_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid );

void GameButton_restart_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit );

void GameButton_return_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit );

void GameButton_quit_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit );

void GameButton_replay_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit );

// =========================================================

/* apelle les evenement d'input de la grille */
void Game_event(Grid *pGrid, SDL_Event *pEvent, bool *pQuit, bool *pDragAndDrop, SDL_Point *pDragStart);

/* gere l'aparition, la destruction des jetons */
void Game_logic(Grid *pGrid, bool *pQuit);

// =========================================================

void GameSessionRandom(int width, int height,int nbColor, int nbMove,bool randomizeInsert,int nbHelp, int nbSuperHelp, int nbRevertOnce);

void GameSessionPuzzle(Grid *pGrid);

// =========================================================

#endif // GAME_H_INCLUDED
