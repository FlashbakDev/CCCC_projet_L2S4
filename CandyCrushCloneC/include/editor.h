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
Grid *NewEmptyPuzzle(int x, int y);

/* suprime tout les token de la grille */
void ClearGrid(Grid *pGrid);

/* suprime le token */
void ClearToken(Grid *pGrid, Token *token, int x, int y);

/* met tout les parrametres de la structure aux valeurs par default */
void ResetToken(Token *token);

// =========================================================

bool Toggle_color_event(UI_toggle *pToggle, SDL_Event *pEvent, bool *pDraw, Grid *pGrid );

void Entry_nbMove_event(UI_entry *pEntry, SDL_Event *pEvent, bool *pDraw, Grid *pGrid );

void Entry_name_event(UI_entry *pEntry, SDL_Event *pEvent, bool *pDraw, Grid *pGrid );

bool Button_tokenType_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid );

void Button_save_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid );

void Button_test_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid, bool *pQuit );

void Button_reset_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid );

void Editor_event(Grid *pGrid, SDL_Event *pEvent, bool *pQuit, Token tokenToPaste);

void Editor_logic(Grid *pGrid);

void EditorSession(Grid *pGrid);

// =========================================================

#endif // EDITOR_H_INCLUDED
