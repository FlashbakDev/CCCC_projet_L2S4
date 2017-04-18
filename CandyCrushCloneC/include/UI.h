/*

UI.h
-----------

Par Benjamin, pour le projet CCCC le 12/03/2017.

R�le : gestion de linterface utilisateur au moyen de diff�rents widget.

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
	char text[UI_MAX_LABEL];
	SDL_Color color_text;
	Font font;
	Window *pWindow;

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
	Image image_verticalSlider;
	Window *pWindow;

} UI_verticalScrollbar;

typedef struct UI_horizontalScrollbar {

	bool visible;
	bool focus;
	SDL_Rect rect_up;
	SDL_Rect rect_down;
	SDL_Rect rect_slider;
	int maxPos;
	double fraction;
	double step;
	unsigned int lastTick;
	bool mouseClick_left;
	bool mouseClick_right;
	bool mouseClick_slider;
	Image image_left;
	Image image_right;
	Image image_horizontalSlider;
	Window *pWindow;

} UI_horizontalScrollbar;

typedef struct UI_progressBar {

	bool visible;
	SDL_Rect rect;
	SDL_Rect rect_bar;
	int width;
	double fraction;
	double step;
	SDL_Color color_background;
	unsigned int lastTick;
	bool run;
	Image image_bar;
	Window *pWindow;

} kiss_progressbar;

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

} UI_textBox;

typedef struct UI_combobox {

	bool visible;
	char text[UI_MAX_LENGTH];
	UI_entry entry;
	Window window;
	UI_verticalScrollbar verticalScrollbar;
	UI_textBox textBox;
	Image image_combo;
	Window *pWindow;

} UI_combobox;

// ========================================================= Cr�ation des widgets

/* cr�e un nouveau label ( emplacement de texte ) */
int UI_label_new(UI_label *pLabel, Window *pWindow, char *text, int x, int y);

/* cr�� un nouveau bouton */
int UI_button_new(UI_button *pButton, Window *pWindow, char *text, int x, int y);

/* change les images d'un bouton */
void UI_set_button_images(UI_button *pButton, Image image_active, Image image_normal, Image image_prelight);

/* cr�e un nouveau toggle ( bouton s�l�ctionnable ) */
UI_toggle_new(UI_toggle *pToggle, Window *pWindow, int x, int y,int w, int h, SDL_Color color);

/* cr�e un nouveau champ de texte */
int UI_entry_new(UI_entry *pEntry, Window *pWindow, char *pText, int x, int y, int w);

// ========================================================= �venements sur les widget

int UI_button_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw);

int UI_toggle_event(UI_toggle *pToggle, SDL_Event *pEvent, bool *pDraw);

int UI_entry_event(UI_entry *pEntry, SDL_Event *pEvent, bool *pDraw);

// ========================================================= affichage des widget

int UI_label_draw(UI_label *pLabel, SDL_Renderer *pRenderer);

int UI_button_draw(UI_button *pButton, SDL_Renderer *pRenderer);

int UI_toggle_draw(UI_toggle *pToggle, SDL_Renderer *pRenderer);

int UI_entry_draw(UI_entry *pEntry, SDL_Renderer *pRenderer);

// ========================================================= Autre

/* cr�e un cadre autour du widget de la couleur donn� */
int UI_outline(SDL_Renderer *pRenderer, SDL_Rect *pRect, SDL_Color color, int edge);

/* remplie un espace donn� */
int UI_fillRect(SDL_Renderer *pRenderer, SDL_Rect *pRect, SDL_Color color);

// =========================================================

#endif // UI_H_INCLUDED
