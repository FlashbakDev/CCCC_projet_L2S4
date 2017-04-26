/*

menu.h
-----------

Par Desbouchages Benjamin, Rousseau jérémy, pour le projet CCCC le 09/03/2017.

Rôle : fonctions liées au menu, boutons etc.

*/

// =========================================================

#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

// =========================================================

#include "core.h"
#include "UI.h"
#include "constants.h"
#include "files.h"

// =========================================================

typedef enum MenuState {MenuState_MENU, MenuState_PLAY, MenuState_EDITOR, MenuState_PLAY_CUSTOM, MenuState_PLAY_LOAD, MenuState_EDITOR_LOAD }MenuState;

// =========================================================
// Affichages des puzzles

void Text_reset(UI_textBox *pTextBox, UI_verticalScrollbar *pVerticalScrollbar);

void dirent_read(UI_textBox *pTextBox, UI_verticalScrollbar *pVerticalScrollbar);

// =========================================================

/* initailisation et chargement des ressources */
SDL_Renderer *InitMenu(char * pChar_name, Array *pArray, int w, int h);

void Menu_event(SDL_Event *pEvent, bool *pQuit);

void MenuButton_editor_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState );
void MenuButton_editor_load_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState );
void MenuButton_editor_new_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState );

void MenuButton_play_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState );
void MenuButton_play_load_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState );
void MenuButton_play_classic_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState );
void MenuButton_play_custom_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState );

void MenuButton_return_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState );
void MenuButton_ok_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState );
void MenuButton_quit_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit );

void MenuTextbox_files_event(UI_textBox *pTextBox, SDL_Event *pEvent, UI_verticalScrollbar *pVerticalScrollbar, UI_label *pLabel, UI_button *pButton, bool *pDraw);
void MenuVerticalScrollbar_files_event(UI_verticalScrollbar *pVerticalScrollbar, SDL_Event *pEvent, UI_textBox *pTextBox, bool *pDraw);

void Menu_logic();

void MenuSession();

#endif // MENU_H_INCLUDED
