#include "core.h"

// =========================================================

Font font_default;
Image image_normal, image_prelight, image_active, image_selected, image_unselected;
int screen_width, screen_height;

// =========================================================

SDL_Renderer *InitGame(char * pChar_name, Array *pArray, int w, int h){

    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    SDL_Rect rect_bounds;
    int error = 0;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GetDisplayBounds(0, &rect_bounds);

    if ( !pArray || w > rect_bounds.w || h > rect_bounds.h ){

        SDL_Quit();
        return NULL;
    }

    screen_width = w;
    screen_height = h;

    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    SDL_StartTextInput();

    Array_new(pArray);

    pWindow = SDL_CreateWindow(pChar_name, rect_bounds.w / 2 - w / 2, rect_bounds.h / 2 - h / 2, w, h, SDL_WINDOW_SHOWN);
    if ( pWindow ) Array_append(pArray, WINDOW_TYPE , pWindow);
    // void SDL_SetWindowIcon(SDL_Window*  window , SDL_Surface* icon); //ajoute une icône à la fenêtre

    pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if ( pRenderer ) Array_append(pArray, RENDERER_TYPE, pRenderer);

    // init des ressources
    error += Font_new(&font_default, "data/fonts/arial.ttf", pArray, 15);
    error += Image_new(&image_active, "data/UI/image_active.png", pArray, pRenderer);
    error += Image_new(&image_prelight, "data/UI/image_prelight.png", pArray, pRenderer);
    error += Image_new(&image_normal, "data/UI/image_normal.png", pArray, pRenderer);

    if ( error > 0 ) {

		CleanGame(pArray);
		return NULL;
	}

	return pRenderer;
}

// =========================================================

int CleanGame(Array *pArray){

    if ( pArray )
        return -1;

    if ( pArray->length ){

        for(int i = pArray->length - 1; i >= 0; i--){

            switch( Array_GET_id(pArray, i) ){

                case FONT_TYPE :{ TTF_CloseFont((TTF_Font*)Array_GET_data(pArray,i)); } break;
                case TEXTURE_TYPE : { SDL_DestroyTexture((SDL_Texture*)Array_GET_data(pArray,i)); } break;
                case RENDERER_TYPE : { SDL_DestroyRenderer((SDL_Renderer*)Array_GET_data(pArray,i)); } break;
                case WINDOW_TYPE : { SDL_DestroyWindow((SDL_Window*)Array_GET_data(pArray,i)); } break;
                case ARRAY_TYPE : { Array_free((Array*)Array_GET_data(pArray,i)); } break;
                default : { free( pArray->tab_data[i]); }
            }
        }
    }

    pArray->length = 0;
    Array_free(pArray);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}

// =========================================================

void WaitForNextFrame(int frameStart){

    int frameEnd = SDL_GetTicks();
    if ( frameEnd - frameStart < 1000 / FRAME_PER_SECOND ){

        SDL_Delay( (1000 / FRAME_PER_SECOND) - (frameEnd - frameStart) );
    }
}

// =========================================================

void DrawGrid(Grid *pGrid, SDL_Renderer *pRenderer, SDL_Surface *pSurface_Token[]){

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].type != NONE || pGrid->tokens[i][j].isDestruct ){

                SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer,pSurface_Token[pGrid->tokens[i][j].color]);

                SDL_RenderCopy(pRenderer,pTexture,NULL,&pGrid->tokens[i][j].rect_texture);

                SDL_DestroyTexture(pTexture);
            }
        }
    }
}

// =========================================================

int RenderImage(SDL_Renderer *pRenderer, Image image, int x, int y, SDL_Rect *pRect){
	SDL_Rect distance;

	if (!pRenderer || !image.pTexture)
        return -1;

	MakeRect(&distance, x, y, image.w, image.h);
	if (pRect) distance.w = pRect->w;
	if (pRect) distance.h = pRect->h;
	SDL_RenderCopy(pRenderer, image.pTexture, pRect, &distance);

	return 0;
}

// =========================================================

int RenderText(SDL_Renderer *pRenderer, char *pChar_text, int x, int y, Font font, SDL_Color color){

    SDL_Surface *pSurface;
    Image image;

    if ( !pRenderer ||!pChar_text || !font.pFont )
        return -1;

    pSurface = TTF_RenderUTF8_Blended(font.pFont, pChar_text, color);
    image.pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_QueryTexture(image.pTexture, NULL, NULL, &image.w, &image.h);

    if ( pSurface ) SDL_FreeSurface(pSurface);

    RenderImage(pRenderer, image, x, y, NULL);

    SDL_DestroyTexture(image.pTexture);

    return 0;
}

// =========================================================

int Window_draw(Window *pWindow, SDL_Renderer *pRenderer){

    if (pWindow && pWindow->pWindow)
        pWindow->visible = pWindow->pWindow->visible;

	if (!pWindow || !pWindow->visible || !pRenderer)
        return 0;

	UI_fillRect(pRenderer, &pWindow->rect, pWindow->color_background);

	SDL_Color blue = { 0, 0, 255, 255 };

	if (pWindow->outline)
		UI_outline(pRenderer, &pWindow->rect, blue, 2);

	return 1;
}

// =========================================================

void HardPermuteToken(Grid *pGrid,int x1,int y1,int x2,int y2){

    Token tmp = pGrid->tokens[y1][x1];
    pGrid->tokens[y1][x1] = pGrid->tokens[y2][x2];
    pGrid->tokens[y2][x2] = tmp;

    CalculTokenRectTexure( &pGrid->tokens[y1][x1], x1, y1 );
    CalculTokenRectTexure( &pGrid->tokens[y2][x2], x2, y2 );
}

// =========================================================

void DebugToken(Token token){

    fprintf(stdout,
            "\n- color = %d\n- type = %d\n- aligne = %d\n- isMoving = %d\n- isDestruct = %d\n- textureSize = %d\n- rect_texture w = %d, h = %d, x = %d, y = %d",
            token.color, token.type,token.aligned,token.isMoving, token.isDestruct, token.textureSize, token.rect_texture.w, token.rect_texture.h, token.rect_texture.x, token.rect_texture.y );
}

// =========================================================

void CalculTokenRectTexure(Token *token, int x, int y){

    token->rect_texture.w = (float)(TOKEN_WIDTH / 100.0 * token->textureSize);
    token->rect_texture.h = (float)(TOKEN_HEIGHT / 100.0 * token->textureSize);
    token->rect_texture.x = ((x * TOKEN_WIDTH) + (TOKEN_WIDTH/2)) - (token->rect_texture.w / 2);
    token->rect_texture.y = ((y * TOKEN_HEIGHT) + (TOKEN_HEIGHT/2)) - (token->rect_texture.h / 2);
}

// =========================================================

void MakeRect(SDL_Rect *pRect, int x, int y, int w, int h){

    pRect->x = x;
    pRect->y = y;
    pRect->w = w;
    pRect->h = h;
}

// =========================================================

int TextWidth(Font font, char *str1, char *str2){

    char buf[UI_MAX_LENGTH];
	int width;

	if ( !str1 && !str2 )
        return -1;

	String_copy(buf, UI_MAX_LENGTH, str1, str2);
	TTF_SizeUTF8(font.pFont, buf, &width, NULL);

	return width;
}

// =========================================================

bool PointInRect(int x, int y, SDL_Rect *pRect){

    return x >= pRect->x && x < pRect->x + pRect->w && y >= pRect->y && y < pRect->y + pRect->h;
}

// =========================================================

char *String_copy(char *dest, size_t size, char *str1, char *str2){

    unsigned int len;
	char *p;

	if (!dest)
        return NULL;

	strcpy(dest, "");

	if (size < 2)
        return dest;

	if (str1)
        strncpy(dest, str1, size);

	dest[size - 1] = 0;
	len = strlen(dest);

	if (!str2 || size - 1 <= len)
        return dest;

	p = dest;
	strncpy(p + len, str2, size - len);
	dest[size - 1] = 0;
	kiss_utf8fix(dest);

	return dest;
}

// =========================================================

int kiss_utf8fix(char *str){

    int len, i;

    if (!str || !str[0])
        return -1;

    len = strlen(str);

    for (i = len - 1; i >= 0 && len - 1 - i < 3; i--) {

        if ((str[i] & 224) == 192 && len - 1 - i < 1) str[i] = 0;
        if ((str[i] & 240) == 224 && len - 1 - i < 2) str[i] = 0;
        if ((str[i] & 248) == 240 && len - 1 - i < 3) str[i] = 0;
    }

    return 0;
}

// =========================================================

int Window_new(Window *pWindow, Window *pWindow_parent, bool outline, int x, int y, int w, int h){

    if (!pWindow)
        return -1;

    SDL_Color White = {255, 255, 255, 255};
	pWindow->color_background = White;
	MakeRect(&pWindow->rect, x, y, w, h);
	pWindow->outline = outline;
	pWindow->visible = false;
	pWindow->focus = true;
	pWindow->pWindow = pWindow_parent;

	return 0;
}

// =========================================================

int Image_new(Image *pImage, char *pChar_name, Array *pArray, SDL_Renderer* pRenderer){

    char buf[UI_MAX_LENGTH];

    if ( !pImage || !pChar_name )
        return -1;

    String_copy(buf, UI_MAX_LENGTH, "", pChar_name);
    if ( !(pImage->pTexture = IMG_LoadTexture(pRenderer, buf)) ){

        fprintf(stderr, "Cannot load image %s\n", pChar_name);
		return -1;
    }

    if ( pArray ){

        Array_append(pArray, TEXTURE_TYPE, pImage->pTexture);
    }

    SDL_QueryTexture(pImage->pTexture, NULL, NULL, &pImage->w, &pImage->h);

    return 0;
}

// =========================================================

int Font_new(Font *pFont, char *pChar_name, Array *pArray, int size){

    char buf[UI_MAX_LENGTH];

    if ( !pFont || !pChar_name )
        return -1;

    String_copy(buf, UI_MAX_LENGTH, "", pChar_name);
    if ( !(pFont->pFont = TTF_OpenFont(buf, size)) ){

        fprintf(stderr, "Cannot load font %s\n", pChar_name);
		return -1;
    }

    if ( pArray ){

        Array_append(pArray, FONT_TYPE, pFont->pFont);
        pFont->fontHeight = TTF_FontHeight(pFont->pFont);
        pFont->spacing = (int) 0.5 * pFont->fontHeight;
        pFont->lineHeight = pFont->fontHeight + pFont->spacing;
        pFont->ascent = TTF_FontAscent(pFont->pFont);
        TTF_GlyphMetrics(pFont->pFont, 'W', NULL, NULL, NULL, NULL, &(pFont->advance));

        return 0;
    }
}

// =========================================================

int Window_event(Window *pWindow, SDL_Event *pEvent, bool *pDraw){

    if (!pWindow || !pWindow->visible || !pEvent)
        return 0;

	if ( pEvent->type == SDL_WINDOWEVENT && pEvent->window.event == SDL_WINDOWEVENT_EXPOSED){

		*pDraw = true;
	}

	if (!pWindow->focus && (!pWindow->pWindow || (pWindow->pWindow && !pWindow->pWindow->focus)))
		return 0;

	if (pEvent->type == SDL_MOUSEBUTTONDOWN && PointInRect(pEvent->button.x, pEvent->button.y, &pWindow->rect))
		return 1;

	return 0;
}

// =========================================================
// ARRAY
// =========================================================

int Array_new(Array *pArray){

    if( !pArray )
        return -1;

    pArray->size = UI_MIN_LENGTH;
    pArray->length = 0;
    pArray->ref = 1;
    pArray->tab_data = (void **)malloc(UI_MIN_LENGTH * sizeof(void *));
    pArray->tab_id = (int *)malloc(UI_MIN_LENGTH * sizeof(int));

    return 0;
}

// =========================================================

void *Array_GET_data(Array *pArray, int index){

    if ( index < 0 || index >= pArray->size || !pArray )
        return NULL;

    return pArray->tab_data[index];
}

// =========================================================

int Array_GET_id(Array *pArray, int index){

    if ( !pArray || index < 0 || index >= pArray->size )
        return 0;

    return pArray->tab_id[index];
}

// =========================================================

int Array_SET(Array *pArray, int index, int id, void *pData){

    if( !pArray || index < 0 || index >= pArray->length )
        return -1;

    free(pArray->tab_data[index]);
    pArray->tab_data[index] = pData;
    pArray->tab_id[index] = id;

    return 0;
}

// =========================================================

int Array_append(Array *pArray, int id, void *pData){

	if (!pArray)
        return -1;

	if (pArray->length >= pArray->size) {

		pArray->size *= 2;
		pArray->tab_data = (void **) realloc(pArray->tab_data, pArray->size * sizeof(void *));
		pArray->tab_id = (int *) realloc(pArray->tab_id, pArray->size * sizeof(int));

		for (int i = pArray->length; i < pArray->size; i++) {

			pArray->tab_data[i] = NULL;
			pArray->tab_id[i] = 0;
		}
	}

	pArray->tab_data[pArray->length] = pData;
	pArray->tab_id[pArray->length] = id;

	pArray->length ++;

	return 0;
}

// =========================================================

int Array_insert(Array *pArray, int index, int id, void *pData){

    if ( !pArray || index < 0 || index >= pArray->length )
        return -1;

    if ( pArray->length >= pArray->size ){

        pArray->size *= 2;
        pArray->tab_data = (void **)realloc(pArray->tab_data, pArray->size * sizeof(void *));
        pArray->tab_id = (int *)realloc(pArray->tab_id, pArray->size * sizeof(int));

        for(int i = pArray->length; i < pArray->size; i++){

            pArray->tab_data[i] = NULL;
            pArray->tab_id[i] = 0;
        }
    }

    for( int i = pArray->length - 1; i >= index; i-- ){

        pArray->tab_data[i+1] = pArray->tab_data[i];
        pArray->tab_id[i+1] = pArray->tab_id[i];
    }

    pArray->tab_data[index] = pData;
    pArray->tab_id[index] = id;

    pArray->length ++;

    return 0;
}

// =========================================================

int Array_remove(Array *pArray, int index){

    if( !pArray || index < 0 || index >= pArray->length )
        return -1;

    free(pArray->tab_data[index]);

    for(int i = index; i < pArray->length - 1; i++){

        pArray->tab_data[i] = pArray->tab_data[i+1];
        pArray->tab_id[i] = pArray->tab_id[i+1];
    }

    pArray->tab_data[pArray->length - 1] = NULL;
    pArray->tab_id[pArray->length - 1] = 0;

    pArray->length --;

    return 0;
}

// =========================================================

int Array_free(Array *pArray){

    if ( !pArray || !pArray->ref )
        return -1;

    if ( pArray->ref > 1 ){

        pArray->ref --;
        return 0;
    }

    if( pArray->length ){

        for(int i = 0; i < pArray->length; i++){

            free(pArray->tab_data[i]);
        }
    }

    free( pArray->tab_data );
    free( pArray->tab_id );
    pArray->tab_data = NULL;
    pArray->tab_id = NULL;
    pArray->size = 0;
    pArray->length = 0;
    pArray->ref = 0;

    return 0;
}

// =========================================================

