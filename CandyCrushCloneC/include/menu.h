/*

menu.h
-----------

Par Benjamin, pour le projet CCCC le 09/03/2017.

Rôle : fonctions liées au menu, boutons etc.

*/

#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "core.h"
#include "constants.h"

typedef enum ButtonsActions { PLAY, PLAYPERSO, PLAYMULTI, OPTIONS, QUIT }ButtonsActions;

typedef struct UI_Toggle{

    bool visible;
    bool focus;
    SDL_Rect rect;
    bool selected;

    SDL_Texture *texture_normal;
    SDL_Texture *texture_selected;
    SDL_Window *window;

}UI_Toggle;

typedef struct UI_Button{

    bool visible;
    SDL_Rect rect;

    ButtonsActions action;
    char text[UI_MAX_TEXT];
    bool active;
    bool highlight;
    SDL_Color color_text;
    TTF_Font *font;

    SDL_Texture *texture_normal;
    SDL_Texture *texture_active;
    SDL_Texture *texture_highlight;
    SDL_Window *window;

}UI_Button;

/* créé le menu et les différents boutons */

#endif // MENU_H_INCLUDED
