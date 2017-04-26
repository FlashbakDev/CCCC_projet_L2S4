/*

editor.h
-----------

Par Desbouchages Benjamin, Rousseau jérémy, pour le projet CCCC le 24/02/2017.

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

// =========================================================

bool EditorToggle_color_event(UI_toggle *pToggle, SDL_Event *pEvent, bool *pDraw, Grid *pGrid );

void EditorEntry_nbMove_event(UI_entry *pEntry, SDL_Event *pEvent, bool *pDraw, Grid *pGrid );

void EditorEntry_name_event(UI_entry *pEntry, SDL_Event *pEvent, bool *pDraw, Grid *pGrid, char *puzzleName);

bool EditorButton_tokenType_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid );

void EditorButton_save_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid, char *puzzleName);

void EditorButton_test_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid, bool *pQuit, char *puzzleName );

void EditorButton_reset_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid );

void EditorButton_quit_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit );

void EditorButton_menu_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit );

void Editor_event(Grid *pGrid, SDL_Event *pEvent, bool *pQuit, Token tokenToPaste);

void Editor_logic(Grid *pGrid);

void EditorSession(char *puzzleName, bool newPuzzle);

// =========================================================

#endif // EDITOR_H_INCLUDED
