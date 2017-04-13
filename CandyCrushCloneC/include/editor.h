/*

editor.h
-----------

Par Benjamin, pour le projet CCCC le 24/02/2017.

Rôle : fonctions pour l'édition de niveau.

*/

// =========================================================

#ifndef EDITOR_H_INCLUDED
#define EDITOR_H_INCLUDED

// =========================================================

#include "core.h"
#include "UI.h"
#include "constants.h"

// =========================================================

/* initailisation et chargement des ressources */
SDL_Renderer *InitEditor(char * pChar_name, Array *pArray, int w, int h);

/* crée une grille vide */
Grid *NewEmptyGrid(SDL_Rect rect);

/* suprime tout les token de la grille */
void ClearGrid(Grid *pGrid);

/* suprime le token */
void ClearToken(Grid *pGrid, Token *token, int x, int y);

// =========================================================

void Editor_event(Grid *pGrid, SDL_Event *pEvent, bool *pQuit);

void Editor_logic(Grid *pGrid);

void EditorSession(int gridWidth, int gridHeight);

// =========================================================

#endif // EDITOR_H_INCLUDED
