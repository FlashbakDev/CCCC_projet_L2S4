/*

core.h
-----------

Par Desbouchages Benjamin, Rousseau jérémy, pour le projet CCCC le 09/03/2017.

Rôle : fonctions générales du jeu, aussi bien affichage que logique.

*/

// =========================================================

#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

// =========================================================

#include "constants.h"

// =========================================================

typedef enum Directions { Directions_UP, Directions_DOWN, Directions_LEFT, Directions_RIGHT }Directions;

typedef enum TokenTypes { TokenTypes_NORMAL, TokenTypes_HORIZONTAL, TokenTypes_VERTICAL, TokenTypes_BOMB, TokenTypes_MULTI, TokenTypes_BLOCK, TokenTypes_NONE }TokenTypes;

typedef enum Colors { Colors_RED, Colors_BLUE, Colors_GREEN, Colors_YELLOW, Colors_PURPLE, Colors_ORANGE, Colors_NONE }Colors;

typedef enum ObjectTypes {ObjectTypes_OTHER, ObjectTypes_WINDOW, ObjectTypes_RENDERER, ObjectTypes_TEXTURE, ObjectTypes_SURFACE, ObjectTypes_FONT, ObjectTypes_ARRAY} ObjectTypes;

typedef enum States { States_MENU, States_GAME_CLASSIC, States_GAME_PUZZLE, States_EDITOR_NEW, States_EDITOR_LOAD, States_QUIT }States;

// =========================================================

typedef struct Image{

    SDL_Texture *pTexture;
    int w;
    int h;

}Image;

typedef struct Token{

    TokenTypes type;
    Colors color;
    bool aligned;
    int score;

    bool canBeMoved;

    // graphics
    Image image;
    SDL_Rect rect_image;
    int textureSize;
    Image image_background;
    bool drawBackground;

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

    bool is_cursorOnGrid;
    SDL_Point cursorTokenPosition;
    SDL_Rect rect;

    Directions direction;
    bool is_randomizeInsert;
    int score;
    int moveAvailable;
    int nbHelp;
    int nbSuperHelp;
    int nbRevertOnce;

    bool isHelpActive;
    bool isSuperHelpActive;

    bool isCalc;

    bool is_puzzle;
    bool outline;

}Grid;

typedef struct Array{

    void **tab_data;
    int *tab_id;
    int length;
    int size;
    int ref;

}Array;

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

// ressources
extern Font font_default, font_hight;
extern Image image_normal, image_prelight, image_active, image_selected, image_unselected, image_arrow_up, image_arrow_down,
image_cursorBlue, image_cursorRed, image_cursorGreen, image_tokens[26], image_verticalSlider;
extern int screen_width, screen_height;

// variable machine d'etat
extern States gameState, gameState_prec;

// ========================================================= Logique

/* initialisae la sdl */
void InitSDL();

/* libère la sdl */
void CleanSDL();

/* libere le champs */
int Clean(Array *pArray);

/* permute 2 jetons et place leurs texture au bon endroit */
void HardPermuteToken(Grid *pGrid,int x1,int y1,int x2,int y2);

/* affiche les informations d'un jeton donné en console */
void DebugToken(Token token);

/* Calcul le rect_texture du jeton par raport à textureSize et à une position donné */
void CalculTokenImages(Grid *pGrid, Token *token, int x, int y);

/* assigne l'image en fonction des parrametre du token */
void AssignImageToToken(Token *token);

/* remet les bonnes images aux bon jetons */
void ResetTokenImages(Grid *pGrid);

/* crée un rect avec les données fournis */
void MakeRect(SDL_Rect *pRect, int x, int y, int w, int h);

/* */
int TextWidth(Font font, char *str1, char *str2);

/* Test le point dans le rect */
bool PointInRect(int x, int y, SDL_Rect *pRect);

/* copie une chaine de charactere */
char *String_copy(char *dest, size_t size, char *str1, char *str2);

int Utf8Fix(char *str);

int Utf8next(char *str, int index);

char *Backspace(char *str);

void MoveAvailable(Grid * pGrid);

/* copie les token actuel dans la sauvegarde */
void SaveTokensInPastTokens(Grid *pGrid);

/* copie les token de la sauvegarde dans l'actuel */
void LoadTokensInPastTokens(Grid *pGrid);

/* met tout les parrametres de la structure aux valeurs par default */
void ResetToken(Token *token);

// ========================================================= Chargement de ressources

/* charge les images des jetons du dossier donné ( renvoie le nombre d'erreur ) */
int LoadTokenImagesFromPath(char *folder, Array *pArray, SDL_Renderer *pRenderer);

/* créée une nouvelle fenêtre */
int Window_new(Window *pWindow, Window *pWindow_parent, bool outline, int x, int y, int w, int h);

/* crée une nouvelle image */
int Image_new(Image *pImage, char *pChar_name, Array *pArray, SDL_Renderer* pRenderer);

/* crée une nouvelle police de texte */
int Font_new(Font *pFont, char *pChar_name, Array *pArray, int size);

// ========================================================= Evenements

int Window_event(Window *pWindow, SDL_Event *pEvent, bool *pDraw);

// ========================================================= Affichage

/* gestion des fps */
void WaitForNextFrame(int frameStart);

/* affiche la grille */
void Grid_draw(Grid *pGrid, SDL_Renderer *pRenderer);

/* affiche une image */
int RenderImage(SDL_Renderer *pRenderer, Image image, int x, int y, SDL_Rect *pRect);

/* affiche un texte */
int RenderText(SDL_Renderer *pRenderer, char *pChar_text, int x, int y, Font font, SDL_Color color);

/* affiche la fenetre */
int Window_draw(Window *pWindow, SDL_Renderer *pRenderer);

// ========================================================= Champs

/* systeme de liste avec id */

/* créé une nouvelle liste */
int Array_new(Array *pArray);

/* acesseur GET sur les données */
void *Array_GET_data(Array *pArray, int index);

/* acesseur GET sur les id */
int Array_GET_id(Array *pArray, int index);

/* accesseur SET sur les champs */
int Array_SET(Array *pArray, int index, int id, void *pData);

/* ajoute un item à la liste */
int Array_append(Array *pArray, int id, void *pData);

int Array_append_string(Array *a, int id, char *pText1, char *pText2);

/* Insertion d'un élément dans le champ */
int Array_insert(Array *pArray, int index, int id, void *pData);

/* suppression d'un élément du champs */
int Array_remove(Array *pArray, int index);

/* suppression du champs */
int Array_free(Array *pArray);

// =========================================================

#endif // CORE_H_INCLUDED
