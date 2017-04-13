/*

menu.h
-----------

Par Benjamin, pour le projet CCCC le 09/03/2017.

Rôle : fonctions liées au menu, boutons etc.

*/

// =========================================================

#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

// =========================================================

#include "core.h"
#include "UI.h"
#include "constants.h"

// =========================================================


// =========================================================

/* initailisation et chargement des ressources */
SDL_Renderer *InitMenu(char * pChar_name, Array *pArray, int w, int h);

void Menu_event(SDL_Event *pEvent, bool *pQuit);

void Menu_logic();

void MenuSession();

#endif // MENU_H_INCLUDED
