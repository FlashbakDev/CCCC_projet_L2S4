#include "UI.h"

// =========================================================

int UI_label_new(UI_label *pLabel, Window *pWindow, char *text, int x, int y){

    if (!pLabel || !text)
        return -1;

    SDL_Color black = { 0, 0, 0, 255 };

    pLabel->font = font_default;
    pLabel->color_text = black;
    MakeRect(&pLabel->rect, x, y, 0, 0);
    String_copy(pLabel->text, UI_MAX_LABEL, text, NULL);
    pLabel->visible = false;
    pLabel->pWindow = pWindow;

    return 0;
}

// =========================================================

int UI_button_new(UI_button *pButton, Window *pWindow, char *text, int x, int y){

    if (!pButton || !text)
        return -1;

    SDL_Color white = { 255, 255, 255, 255 };

    pButton->font = font_default;
    pButton->image_normal = image_normal;
    pButton->image_active = image_active;
    pButton->image_prelight = image_prelight;

    MakeRect(&pButton->rect, x, y, pButton->image_normal.w, pButton->image_normal.h);
    pButton->color_text = white;
    String_copy(pButton->text, UI_MAX_LENGTH, text, NULL);
    pButton->position_text.x = x + pButton->image_normal.w / 2 - TextWidth(pButton->font, text, NULL) / 2;
    pButton->position_text.y = y + pButton->image_normal.h / 2 - pButton->font.fontHeight / 2;
    pButton->active = false;
    pButton->prelight = false;
    pButton->visible = false;
    pButton->focus = false;
    pButton->pWindow = pWindow;

    return 0;
}

// =========================================================

int UI_toggle_new(UI_toggle *pToggle, Window *pWindow, int x, int y){

    if (!pToggle)
        return -1;

    pToggle->image_selected = image_selected;
    pToggle->image_unselected = image_unselected;

	MakeRect(&pToggle->rect, x, y, pToggle->image_selected.w, pToggle->image_selected.h);
	pToggle->selected = false;
	pToggle->focus = false;
	pToggle->pWindow = pWindow;

	return 0;
}

// =========================================================

int UI_outline(SDL_Renderer *pRenderer, SDL_Rect *pRect, SDL_Color color, int edge){

    SDL_Rect rect_outline;
    int d;

    d = 2* edge;

    if ( !pRenderer || !pRect || pRect->w < d + 6 || pRect->h < d + 6 )
        return -1;

    SDL_SetRenderDrawColor(pRenderer, color.r, color.g, color.b, color.a);
    for(int i = 0 ; i < 2; i++){

        MakeRect(&rect_outline,
                 pRect->x + edge + i,
                 pRect->y + edge + i,
                 pRect->w - d - i - i,
                 pRect->h - d - i - i );

        SDL_RenderDrawRect(pRenderer, &rect_outline);
    }

    return 0;
}

// =========================================================

int UI_fillRect(SDL_Renderer *pRenderer, SDL_Rect *pRect, SDL_Color color){

    if ( !pRenderer || !pRect )
        return -1;

    SDL_SetRenderDrawColor(pRenderer, color.r, color.g, color.b, color.a );
    SDL_RenderFillRect(pRenderer, pRect);

    return 0;
}

// =========================================================
// évènements sur les widget
// =========================================================

int UI_button_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw){

    if (!pButton || !pButton->visible || !pEvent)
        return 0;

    if (pEvent->type == SDL_WINDOWEVENT && pEvent->window.event == SDL_WINDOWEVENT_EXPOSED)
        *pDraw = true;

    if (!pButton->focus && (!pButton->pWindow || (pButton->pWindow && !pButton->pWindow->focus)))
        return 0;

    if (pEvent->type == SDL_MOUSEBUTTONDOWN && PointInRect(pEvent->button.x, pEvent->button.y, &pButton->rect)) {

        pButton->active = true;
        *pDraw = true;

    } else if (pEvent->type == SDL_MOUSEBUTTONUP && PointInRect(pEvent->button.x, pEvent->button.y, &pButton->rect) && pButton->active) {

        pButton->active = false;
        *pDraw = true;
        return 1;

    } else if (pEvent->type == SDL_MOUSEMOTION && PointInRect(pEvent->motion.x, pEvent->motion.y, &pButton->rect)) {

        pButton->prelight = true;
        *pDraw = true;

    } else if (pEvent->type == SDL_MOUSEMOTION && !PointInRect(pEvent->motion.x, pEvent->motion.y, &pButton->rect)) {

        pButton->prelight = false;
        *pDraw = true;

        if (pButton->active) {

            pButton->active = false;
            return 1;
        }
    }

    return 0;
}

// =========================================================

int UI_toggle_event(UI_toggle *pToggle, SDL_Event *pEvent, bool *pDraw){

	if (!pToggle || !pToggle->visible || !pEvent)
        return 0;

	if (pEvent->type == SDL_WINDOWEVENT && pEvent->window.event == SDL_WINDOWEVENT_EXPOSED)
		*pDraw = true;

	if (!pToggle->focus && (!pToggle->pWindow || (pToggle->pWindow && !pToggle->pWindow->focus)))
		return 0;

	if (pEvent->type == SDL_MOUSEBUTTONDOWN && PointInRect(pEvent->button.x, pEvent->button.y, &pToggle->rect)) {
		pToggle->selected ^= true;
		*pDraw = true;
		return 1;
	}

	return 0;
}

// =========================================================
// affichage des widgets
// =========================================================

int UI_label_draw(UI_label *pLabel, SDL_Renderer *pRenderer){

    char buf[UI_MAX_LABEL], *p;
	int len, y;

	if (pLabel && pLabel->pWindow)
        pLabel->visible = pLabel->pWindow->visible;

	if (!pLabel || !pLabel->visible || !pRenderer)
        return 0;

	y = pLabel->rect.y + pLabel->font.spacing / 2;
	len = strlen(pLabel->text);

	if (len > UI_MAX_LABEL - 2)
		pLabel->text[len - 1] = '\n';
	else
		strcat(pLabel->text, "\n");

	for (p = pLabel->text; *p; p = strchr(p, '\n') + 1) {

		String_copy(buf, strcspn(p, "\n") + 1, p, NULL);
		RenderText(pRenderer, buf, pLabel->rect.x, y, pLabel->font, pLabel->color_text);
		y += pLabel->font.lineHeight;
	}

	pLabel->text[len] = 0;

	return 1;
}

// =========================================================

int UI_button_draw(UI_button *pButton, SDL_Renderer *pRenderer){

    if (pButton && pButton->pWindow)
        pButton->visible = pButton->pWindow->visible;

	if (!pButton || !pButton->visible || !pRenderer)
        return 0;

	if (pButton->active)
		RenderImage(pRenderer, pButton->image_active, pButton->rect.x, pButton->rect.y, NULL);
	else if (pButton->prelight && !pButton->active)
		RenderImage(pRenderer, pButton->image_prelight, pButton->rect.x, pButton->rect.y, NULL);
	else
		RenderImage(pRenderer, pButton->image_normal, pButton->rect.x, pButton->rect.y, NULL);

	RenderText(pRenderer, pButton->text, pButton->position_text.x, pButton->position_text.y, pButton->font, pButton->color_text);

	return 1;
}

// =========================================================

int UI_toggle_draw(UI_toggle *pToggle, SDL_Renderer *pRenderer){

	if (pToggle && pToggle->pWindow)
		pToggle->visible = pToggle->pWindow->visible;

	if (!pToggle || !pToggle->visible || !pRenderer)
        return 0;

	if (pToggle->selected)
		RenderImage(pRenderer, pToggle->image_selected, pToggle->rect.x, pToggle->rect.y, NULL);
	else
		RenderImage(pRenderer, pToggle->image_unselected, pToggle->rect.x, pToggle->rect.y, NULL);

	return 1;
}

// =========================================================
