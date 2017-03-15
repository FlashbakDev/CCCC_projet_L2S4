/*

core.h
-----------

Par Benjamin, pour le projet CCCC le 09/03/2017.

R�le : fonctions g�n�rales du jeu, aussi bien affichage que logique.

*/

// =========================================================

#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

// =========================================================

#include "constants.h"

// =========================================================

typedef enum Directions { UP, DOWN, LEFT, RIGHT }Directions;

typedef enum TokenTypes { NONE, TOKEN }TokenTypes;

typedef enum Colors { RED, BLUE, GREEN, YELLOW, PURPLE, ORANGE }Colors;

typedef enum ObjectTypes {OTHER_TYPE, WINDOW_TYPE, RENDERER_TYPE, TEXTURE_TYPE, SURFACE_TYPE, FONT_TYPE, ARRAY_TYPE} ObjectTypes;

typedef struct Token{

    TokenTypes type;
    Colors color;
    bool aligned;
    int score;

    SDL_Rect rect_texture;
    int textureSize;

    // animations
    bool isMoving;
    bool isDestruct;
    int startDestructAnim;

}Token;

typedef struct Grid{

    Token **pastTokens; // historique
    Token **tokens;
    int nbMove;
    int width, height;
    int nbColor;

}Grid;

typedef struct Array{

    void **tab_data;
    int *tab_id;
    int length;
    int size;
    int ref;

}Array;

typedef struct Image{

    SDL_Texture *pTexture;
    int w;
    int h;

}Image;

typedef struct Font{

    TTF_Font *pFont;
	int fontHeight;
	int spacing;
	int lineHeight;
	int advance;
	int ascent;

}Font;

typedef struct Window {

	bool visible;
	bool focus;
	SDL_Rect rect;
	bool outline;
	SDL_Color color_background;
    struct Window *pWindow;

}Window;

// =========================================================

extern Font font_default;
extern Image image_normal, image_prelight, image_active, image_selected, image_unselected;
extern int screen_width, screen_height;

// ========================================================= Logique

/* initailisation et chargement des ressources */
SDL_Renderer *InitGame(char * pChar_name, Array *pArray, int w, int h);

/* fermeture du jeu / d�chargement */
int CleanGame(Array *pArray);

/* permute 2 jetons et place leurs texture au bon endroit */
void HardPermuteToken(Grid *pGrid,int x1,int y1,int x2,int y2);

/* affiche les informations d'un jeton donn� en console */
void DebugToken(Token token);

/* Calcul le rect_texture du jeton par raport � textureSize et � une position donn� */
void CalculTokenRectTexure(Token *token, int x, int y);

/* cr�e un rect avec les donn�es fournis */
void MakeRect(SDL_Rect *pRect, int x, int y, int w, int h);

/* */
int TextWidth(Font font, char *str1, char *str2);

/* Test le point dans le rect */
bool PointInRect(int x, int y, SDL_Rect *pRect);

/* copie une chaine de charactere */
char *String_copy(char *dest, size_t size, char *str1, char *str2);

int kiss_utf8fix(char *str);

// ========================================================= Chargement de ressources

/* cr��e une nouvelle fen�tre */
int Window_new(Window *pWindow, Window *pWindow_parent, bool outline, int x, int y, int w, int h);

/* cr�e une nouvelle image */
int Image_new(Image *pImage, char *pChar_name, Array *pArray, SDL_Renderer* pRenderer);

/* cr�e une nouvelle police de texte */
int Font_new(Font *pFont, char *pChar_name, Array *pArray, int size);

// ========================================================= Evenements

int Window_event(Window *pWindow, SDL_Event *pEvent, bool *pDraw);

// ========================================================= Affichage

/* gestion des fps */
void WaitForNextFrame(int frameStart);

/* affiche la grille */
void DrawGrid(Grid *pGrid, SDL_Renderer *pRenderer, SDL_Surface *pSurface_Token[5]);

/* affiche une image */
int RenderImage(SDL_Renderer *pRenderer, Image image, int x, int y, SDL_Rect *pRect);

/* affiche un texte */
int RenderText(SDL_Renderer *pRenderer, char *pChar_text, int x, int y, Font font, SDL_Color color);

/* affiche la fenetre */
int Window_draw(Window *pWindow, SDL_Renderer *pRenderer);

// ========================================================= Champs

/* systeme de liste avec id */

/* cr�� une nouvelle liste */
int Array_new(Array *pArray);

/* acesseur GET sur les donn�es */
void *Array_GET_data(Array *pArray, int index);

/* acesseur GET sur les id */
int Array_GET_id(Array *pArray, int index);

/* accesseur SET sur les champs */
int Array_SET(Array *pArray, int index, int id, void *pData);

/* ajoute un item � la liste */
int Array_append(Array *pArray, int id, void *pData);

/* Insertion d'un �l�ment dans le champ */
int Array_insert(Array *pArray, int index, int id, void *pData);

/* suppression d'un �l�ment du champs */
int Array_remove(Array *pArray, int index);

/* suppression du champs */
int Array_free(Array *pArray);

// =========================================================

#endif // CORE_H_INCLUDED
