/*

UI.h
-----------

Par Desbouchages Benjamin, Rousseau jérémy, pour le projet CCCC le 12/03/2017.

Rôle : gestion de linterface utilisateur au moyen de différents widget.

source : https://github.com/actsl/kiss_sdl

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
	char text[UI_MAX_LENGTH];
	SDL_Color color_text;
	Font font;
	Window *pWindow;
	bool draw;

}UI_label;

typedef struct UI_button {

	bool visible;
	bool focus;
	bool draw;
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

typedef struct UI_toggle {

	bool visible;
	bool focus;
	SDL_Rect rect;
	bool selected;
	SDL_Color color;
	Window *pWindow;

} UI_toggle;

typedef struct UI_verticalScrollbar {

	bool visible;
	bool focus;
	SDL_Rect rect_up;
	SDL_Rect rect_down;
	SDL_Rect rect_slider;
	int maxPos;
	double fraction;
	double step;
	unsigned int lastTick;
	bool mouseClick_down;
	bool mouseClick_up;
	bool mouseClick_slider;
	Image image_up;
	Image image_down;
	Image image_slider;
	Window *pWindow;
	bool draw;

} UI_verticalScrollbar;

typedef struct UI_entry {

	bool visible;
	bool focus;
	SDL_Rect rect;
	bool outline;
	SDL_Point position_text;
	char text[UI_MAX_LENGTH];
	bool active;
	int textWidth;
	int selection[4];
	int cursor[2];
	SDL_Color color_normal;
	SDL_Color color_active;
	SDL_Color color_background;
	Font font;
	Window *pWindow;

} UI_entry;

typedef struct UI_textBox {

	bool visible;
	bool focus;
	SDL_Rect rect;
	bool outline;
	Array *array;
	SDL_Rect rect_text;
	int firstLine;
	int maxLines;
	int textWidth;
	int highlightLine;
	int selectedLine;
	int selection[4];
	int cursor[2];
	SDL_Color color_text;
	SDL_Color color_highlight;
	SDL_Color color_background;
	Font font;
	Window *pWindow;
	bool draw;

} UI_textBox;

// ========================================================= Création des widgets

/* crée un nouveau label ( emplacement de texte ) */
int UI_label_new(UI_label *pLabel, Window *pWindow, char *text, int x, int y);

/* créé un nouveau bouton */
int UI_button_new(UI_button *pButton, Window *pWindow, char *text, int x, int y);

/* crée un nouveau toggle ( bouton séléctionnable ) */
int UI_toggle_new(UI_toggle *pToggle, Window *pWindow, int x, int y,int w, int h, SDL_Color color);

/* crée un nouveau champ de texte */
int UI_entry_new(UI_entry *pEntry, Window *pWindow, char *pText, int x, int y, int w);

/* crée une nouvelle barre de déplacement vertical */
int UI_verticalScrollbar_new(UI_verticalScrollbar *pVerticalScrollbar, Window *pWindow, int x, int y, int h, int w,  SDL_Color color);

int UI_textBox_new(UI_textBox *pTextBox, Window *pWindow, bool outline, Array *pArray, int x, int y, int w, int h);

// ========================================================= évenements sur les widget

int UI_button_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw);

int UI_toggle_event(UI_toggle *pToggle, SDL_Event *pEvent, bool *pDraw);

int UI_entry_event(UI_entry *pEntry, SDL_Event *pEvent, bool *pDraw);

int UI_verticalScrollbar_event(UI_verticalScrollbar *pVerticalScrollbar, SDL_Event *event, int *pDraw);

int UI_textBox_event(UI_textBox *pTextBox, SDL_Event *event, int *pDraw);

// ========================================================= affichage des widget

int UI_label_draw(UI_label *pLabel, SDL_Renderer *pRenderer);

int UI_button_draw(UI_button *pButton, SDL_Renderer *pRenderer);

int UI_toggle_draw(UI_toggle *pToggle, SDL_Renderer *pRenderer);

int UI_entry_draw(UI_entry *pEntry, SDL_Renderer *pRenderer);

int UI_verticalScrollbar_draw(UI_verticalScrollbar *pVerticalScrollbar, SDL_Renderer *pRenderer);

int UI_textBox_draw(UI_textBox *pTextBox, SDL_Renderer *pRenderer);

// ========================================================= Autre

/* crée un cadre autour du widget de la couleur donné */
int UI_outline(SDL_Renderer *pRenderer, SDL_Rect *pRect, SDL_Color color, int edge);

/* remplie un espace donné */
int UI_fillRect(SDL_Renderer *pRenderer, SDL_Rect *pRect, SDL_Color color);

/* change les images d'un bouton */
void UI_set_button_images(UI_button *pButton, Image image_active, Image image_normal, Image image_prelight);

void UI_verticalScrollbar_newPos(UI_verticalScrollbar *pVerticalScrollbar, double step, bool *pDraw);

int textBox_numOfLines(UI_textBox *pTextBox);

// =========================================================

#endif // UI_H_INCLUDED
