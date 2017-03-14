/*

UI.h
-----------

Par Benjamin, pour le projet CCCC le 12/03/2017.

Rôle : gestion de linterface utilisateur au moyen de différents widget.

*/

// =========================================================

#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

// =========================================================

#include "core.h"
#include "constants.h"

// =========================================================

typedef struct UI_label {

	bool visible;
	SDL_Rect rect;
	char text[UI_MAX_LABEL];
	SDL_Color color_text;
	Font font;
	Window *pWindow;

}UI_label;

typedef struct UI_button {

	bool visible;
	bool focus;
	SDL_Rect rect;
	SDL_Point position_text;
	char text[UI_MAX_LENGTH];
	bool active;
	bool prelight;
	SDL_Color color_text;
	Font font;
	Image image_normal;
	Image image_active;
	Image image_prelight;
	Window *pWindow;

} UI_button;

// =========================================================

/* crée un nouveau label ( emplacement de texte ) */
int UI_label_new(UI_label *pLabel, Window *pWindow, char *text, int x, int y);

/* créé un nouveau bouton */
int UI_button_new(UI_button *pButton, Window *pWindow, char *text, int x, int y);

/* crée un cadre autour du widget de la couleur donné */
int UI_outline(SDL_Renderer *pRenderer, SDL_Rect *pRect, SDL_Color color, int edge);

/* remplie un espace donné */
int UI_fillRect(SDL_Renderer *pRenderer, SDL_Rect *pRect, SDL_Color color);

// ========================================================= évenements sur les widget

int UI_button_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw);

// ========================================================= affichage

int UI_label_draw(UI_label *pLabel, SDL_Renderer *pRenderer);

int UI_button_draw(UI_button *pButton, SDL_Renderer *pRenderer);

// =========================================================

#endif // UI_H_INCLUDED
