#include "UI.h"

// =========================================================

int UI_label_new(UI_label *pLabel, Window *pWindow, char *text, int x, int y){

    if (!pLabel || !text)
        return -1;

    SDL_Color black = { 0, 0, 0, 255 };

    pLabel->font = font_default;
    pLabel->color_text = black;
    MakeRect(&pLabel->rect, x, y, 0, 0);
    String_copy(pLabel->text, UI_MAX_LENGTH, text, NULL);
    pLabel->visible = false;
    pLabel->pWindow = pWindow;
    pLabel->draw = true;

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
    pButton->draw = true;
    pButton->pWindow = pWindow;

    return 0;
}

void UI_set_button_images(UI_button *pButton, Image image_active, Image image_normal, Image image_prelight){

    pButton->image_normal = image_normal;
    pButton->image_active = image_active;
    pButton->image_prelight = image_prelight;

    MakeRect(&pButton->rect, pButton->rect.x, pButton->rect.y, pButton->image_normal.w, pButton->image_normal.h);

    pButton->position_text.x = pButton->rect.x + pButton->image_normal.w / 2 - TextWidth(pButton->font, pButton->text, NULL) / 2;
    pButton->position_text.y = pButton->rect.y + pButton->image_normal.h / 2 - pButton->font.fontHeight / 2;
}

// =========================================================

int UI_toggle_new(UI_toggle *pToggle, Window *pWindow, int x, int y,int w, int h, SDL_Color color){

    if (!pToggle)
        return -1;

	MakeRect(&pToggle->rect, x, y, w, h);
	pToggle->selected = false;
	pToggle->focus = false;
	pToggle->pWindow = pWindow;
	pToggle->color = color;

	return 0;
}

// =========================================================

/*int UI_textbox_new(UI_textBox *pTextBox, UI_window *pWindow, Array *pArray, int x, int y, int w, int h){

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color black = { 0, 0, 0, 0 };
    SDL_Color blue = { 0, 0, 255, 255 };

	if (!pTextBox || !pArray) return -1;

	pTextBox->font = font_default;

	if (h - 2 * 6 < pTextBox->font.lineheight) return -1;

	pTextBox->color_background = white;
	pTextBox->color_text = black;
	pTextBox->color_highlight = blue;
	Makerect(&pTextBox->rect, x, y, w, h);
	Makerect(&pTextBox->rect_text, x + 6,y + 6, w - 2 * 6, h - 2 * 6);
	pTextBox->array = pArray;
	pTextBox->firstLine = 0;
	pTextBox->maxLines = (h - 2 * 6) / pTextBox->font.lineheight;
	pTextBox->textWidth = w - 2 * 6;
	pTextBox->highlightLine = -1;
	pTextBox->selectedLine = -1;
	pTextBox->visible = 0;
	pTextBox->focus = 0;
	pTextBox->pWindow = pWindow;

	return 0;
}*/

// =========================================================

int UI_entry_new(UI_entry *pEntry, Window *pWindow, char *pText, int x, int y, int w){

    int border = 6;
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color black = { 0, 0, 0, 0 };
    SDL_Color blue = { 0, 0, 255, 255 };

	if (!pEntry || !pText) return -1;

	pEntry->font = font_default;

	if (w < 2 * border + pEntry->font.advance) return -1;

	pEntry->color_background = white;
	pEntry->color_normal = black;
	pEntry->color_active = blue;
	pEntry->textWidth = w - 2 * border;
	String_copy(pEntry->text, MaxLength(pEntry->font,pEntry->textWidth, pText, NULL), pText, NULL);
	MakeRect(&pEntry->rect, x, y, w, pEntry->font.fontHeight +2 * border);
	pEntry->position_text.x = x + border;
	pEntry->position_text.y = y + border;
	pEntry->active = 0;
	pEntry->visible = 0;
	pEntry->focus = 0;
	pEntry->pWindow = pWindow;
	pEntry->outline = true;

	return 0;
}

// =========================================================

int UI_verticalScrollbar_new(UI_verticalScrollbar *pVerticalScrollbar, Window *pWindow, int x, int y, int h, int w,  SDL_Color color){

    int edge = 2;
    int slider_padding = 2;

	if (!pVerticalScrollbar)
        return -1;

    pVerticalScrollbar->image_up = image_arrow_up;
    pVerticalScrollbar->image_down = image_arrow_down;
    pVerticalScrollbar->image_slider = image_verticalSlider;

	if (pVerticalScrollbar->image_up.h + pVerticalScrollbar->image_down.h + 2 * edge + 2 * slider_padding + pVerticalScrollbar->image_slider.h > h)
		return -1;

	MakeRect(&pVerticalScrollbar->rect_up, x, y + edge, pVerticalScrollbar->image_up.w, pVerticalScrollbar->image_up.h + slider_padding);
	MakeRect(&pVerticalScrollbar->rect_down, x, y + h - pVerticalScrollbar->image_down.h - slider_padding - edge, pVerticalScrollbar->image_down.w, pVerticalScrollbar->image_down.h + slider_padding);
	MakeRect(&pVerticalScrollbar->rect_slider, x, y + pVerticalScrollbar->rect_up.h + edge, pVerticalScrollbar->image_slider.w, pVerticalScrollbar->image_slider.h);

	pVerticalScrollbar->maxPos = h - 2 * slider_padding - 2 * edge - pVerticalScrollbar->image_up.h - pVerticalScrollbar->image_down.h - pVerticalScrollbar->image_slider.h;
	pVerticalScrollbar->fraction = 0.;
	pVerticalScrollbar->step = 0.1;
	pVerticalScrollbar->mouseClick_up = false;
	pVerticalScrollbar->mouseClick_down = false;
	pVerticalScrollbar->mouseClick_slider = false;
	pVerticalScrollbar->lastTick = 0;
	pVerticalScrollbar->visible = 0;
	pVerticalScrollbar->focus = 0;
	pVerticalScrollbar->pWindow = pWindow;

	return 0;
}

// =========================================================

int UI_textBox_new(UI_textBox *pTextBox, Window *pWindow, bool outline, Array *pArray, int x, int y, int w, int h){

	int border = 6;

	if (!pTextBox || !pArray)
        return -1;

	pTextBox->font = font_default;

	if (h - 2 * border < pTextBox->font.lineHeight)
        return -1;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color lightBlue = {200, 225, 255, 255};

	pTextBox->color_background = white;
	pTextBox->color_text = black;
	pTextBox->color_highlight = lightBlue;
	MakeRect(&pTextBox->rect, x, y, w, h);
	MakeRect(&pTextBox->rect_text, x + border, y + border, w - 2 * border, h - 2 * border);
	pTextBox->outline = outline;
	pTextBox->array = pArray;
	pTextBox->firstLine = 0;
	pTextBox->maxLines = (h - 2 * border) / pTextBox->font.lineHeight;
	pTextBox->textWidth = w - 2 * border;
	pTextBox->highlightLine = -1;
	pTextBox->selectedLine = -1;
	pTextBox->visible = false;
	pTextBox->focus = false;
	pTextBox->pWindow = pWindow;

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

void UI_verticalScrollbar_newPos(UI_verticalScrollbar *pVerticalScrollbar, double step, bool *pDraw){

	*pDraw = true;
	pVerticalScrollbar->fraction += step;
	pVerticalScrollbar->lastTick = SDL_GetTicks();

	if (pVerticalScrollbar->fraction < -0.000001)
        pVerticalScrollbar->fraction = 0.;

	if (pVerticalScrollbar->fraction > 0.999999)
        pVerticalScrollbar->fraction = 1.;

	pVerticalScrollbar->rect_slider.y = pVerticalScrollbar->rect_down.y - pVerticalScrollbar->rect_slider.h - (int) (pVerticalScrollbar->fraction * pVerticalScrollbar->maxPos + 0.5);

	if (pVerticalScrollbar->fraction > 0.000001 && pVerticalScrollbar->fraction < 0.999999)
		return;

	pVerticalScrollbar->mouseClick_up = false;
	pVerticalScrollbar->mouseClick_down = false;
}

// =========================================================

int textBox_numOfLines(UI_textBox *pTextBox){

	int numOfLines = pTextBox->maxLines;

	if (pTextBox->array->length - pTextBox->firstLine < pTextBox->maxLines)
		numOfLines = pTextBox->array->length - pTextBox->firstLine;

	return numOfLines;
}

// =========================================================
// évènements sur les widget
// =========================================================

int UI_button_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw){

    if (!pButton || !pButton->visible || !pEvent || !pButton->draw )
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

int UI_entry_event(UI_entry *pEntry, SDL_Event *pEvent, bool *pDraw){

	if (!pEntry || !pEntry->visible || !pEvent) return 0;

	if (pEvent->type == SDL_WINDOWEVENT && pEvent->window.event == SDL_WINDOWEVENT_EXPOSED) *pDraw = true;

	if (!pEntry->focus && (!pEntry->pWindow || (pEntry->pWindow && !pEntry->pWindow->focus))) return 0;

	if (pEvent->type == SDL_MOUSEBUTTONDOWN && !pEntry->active && PointInRect(pEvent->button.x, pEvent->button.y, &pEntry->rect)) {

		pEntry->active = true;

		if (pEntry->pWindow) pEntry->pWindow->focus = false;

		pEntry->focus = true;
		*pDraw = true;

	} else if (pEvent->type == SDL_KEYDOWN && pEntry->active && pEvent->key.keysym.scancode == SDL_SCANCODE_RETURN) {

	    pEntry->active = false;

		if (pEntry->pWindow) pEntry->pWindow->focus = true;

		pEntry->focus = false;
		*pDraw = true;

		return 1;

	} else if (pEvent->type == SDL_TEXTINPUT && pEntry->active) {

		if (TextWidth(pEntry->font, pEntry->text, pEvent->text.text) < pEntry->textWidth && strlen(pEntry->text) + strlen(pEvent->text.text) < UI_MAX_LENGTH )
			strcat(pEntry->text, pEvent->text.text);

		*pDraw = true;

	} else if (pEvent->type == SDL_KEYDOWN && pEntry->active && pEvent->key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {

	    Backspace(pEntry->text);
		*pDraw = true;

	} else if (pEvent->type == SDL_KEYDOWN && pEntry->active && (pEvent->key.keysym.mod & KMOD_CTRL) && pEvent->key.keysym.scancode == SDL_SCANCODE_U) {

		strcpy(pEntry->text, "");
		*pDraw = true;
	}

	return 0;
}

// =========================================================

int UI_verticalScrollbar_event(UI_verticalScrollbar *pVerticalScrollbar, SDL_Event *event, int *pDraw){

    int clickInterval = 70;

	if (!pVerticalScrollbar || !pVerticalScrollbar->visible)
        return 0;

	if (!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))) {

		pVerticalScrollbar->mouseClick_up = false;
		pVerticalScrollbar->mouseClick_down = false;
		pVerticalScrollbar->lastTick = 0;

	} else if (pVerticalScrollbar->mouseClick_up && SDL_GetTicks() > pVerticalScrollbar->lastTick + clickInterval) {

		UI_verticalScrollbar_newPos(pVerticalScrollbar, -pVerticalScrollbar->step, pDraw);
		return 1;

	} else if (pVerticalScrollbar->mouseClick_down && SDL_GetTicks() > pVerticalScrollbar->lastTick + clickInterval) {

		UI_verticalScrollbar_newPos(pVerticalScrollbar, pVerticalScrollbar->step, pDraw);
		return 1;
	}

	if (!event)
        return 0;

	if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*pDraw = true;

	if (!pVerticalScrollbar->focus && (!pVerticalScrollbar->pWindow || (pVerticalScrollbar->pWindow && !pVerticalScrollbar->pWindow->focus)))
		return 0;

	if (event->type == SDL_MOUSEBUTTONDOWN && PointInRect(event->button.x, event->button.y, &pVerticalScrollbar->rect_up) && pVerticalScrollbar->step > 0.000001) {

		if (pVerticalScrollbar->fraction > 0.000001) {

			pVerticalScrollbar->mouseClick_up = true;

			if (pVerticalScrollbar->pWindow)
                pVerticalScrollbar->pWindow->focus = false;

			pVerticalScrollbar->focus = true;
		}

		pVerticalScrollbar->lastTick = SDL_GetTicks() - clickInterval - 1;

	} else if (event->type == SDL_MOUSEBUTTONDOWN && PointInRect(event->button.x, event->button.y, &pVerticalScrollbar->rect_down) && pVerticalScrollbar->step > 0.000001) {

		if (pVerticalScrollbar->fraction < 0.999999) {

			pVerticalScrollbar->mouseClick_down = true;

			if (pVerticalScrollbar->pWindow)
                pVerticalScrollbar->pWindow->focus = false;

			pVerticalScrollbar->focus = true;
		}

		pVerticalScrollbar->lastTick = SDL_GetTicks() - clickInterval - 1;

	} else if (event->type == SDL_MOUSEBUTTONDOWN && PointInRect(event->button.x, event->button.y, &pVerticalScrollbar->rect_slider) && pVerticalScrollbar->step > 0.000001) {

		if (pVerticalScrollbar->pWindow)
            pVerticalScrollbar->pWindow->focus = false;

		pVerticalScrollbar->focus = true;
		pVerticalScrollbar->mouseClick_slider = true;

	} else if (event->type == SDL_MOUSEBUTTONUP) {

		pVerticalScrollbar->mouseClick_up = false;
		pVerticalScrollbar->mouseClick_down = false;
		pVerticalScrollbar->mouseClick_slider = false;
		pVerticalScrollbar->lastTick = 0;

		if (pVerticalScrollbar->pWindow)
            pVerticalScrollbar->pWindow->focus = true;

		pVerticalScrollbar->focus = false;

	} else if (event->type == SDL_MOUSEMOTION && (event->motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) && pVerticalScrollbar->mouseClick_slider) {

		UI_verticalScrollbar_newPos(pVerticalScrollbar, 1. * event->motion.yrel / pVerticalScrollbar->maxPos, pDraw);
		return 1;
	}

	return 0;
}

// =========================================================

int UI_textBox_event(UI_textBox *pTextBox, SDL_Event *event, int *pDraw){

	int textY, textMaxY, numOfLines;

	if (!pTextBox || !pTextBox->visible || !event || !pTextBox->array || !pTextBox->array->length)
		return 0;

	if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*pDraw = true;

	if (!pTextBox->focus && (!pTextBox->pWindow || (pTextBox->pWindow && !pTextBox->pWindow->focus)))
		return 0;

	if (event->type == SDL_MOUSEBUTTONDOWN && PointInRect(event->button.x, event->button.y, &pTextBox->rect_text)) {

		numOfLines = textBox_numOfLines(pTextBox);
		textY = event->button.y - pTextBox->rect_text.y;
		textMaxY = numOfLines * pTextBox->font.lineHeight;

		if (textY < textMaxY) {

			pTextBox->selectedLine = textY / pTextBox->font.lineHeight;
			return 1;
		}

	} else if (event->type == SDL_MOUSEMOTION && PointInRect(event->motion.x, event->motion.y, &pTextBox->rect_text)) {

		numOfLines = textBox_numOfLines(pTextBox);
		textY = event->motion.y - pTextBox->rect_text.y;
		textMaxY = numOfLines * pTextBox->font.lineHeight;
		pTextBox->highlightLine = -1;

		if (textY < textMaxY)
			pTextBox->highlightLine = textY / pTextBox->font.lineHeight;

		*pDraw = true;

	} else if (event->type == SDL_MOUSEMOTION && !PointInRect(event->motion.x, event->motion.y, &pTextBox->rect_text)) {

		pTextBox->highlightLine = -1;
		*pDraw = true;
	}

	return 0;
}

// =========================================================
// affichage des widgets
// =========================================================

int UI_label_draw(UI_label *pLabel, SDL_Renderer *pRenderer){

    char buf[UI_MAX_LENGTH], *p;
	int len, y;

	if (pLabel && pLabel->pWindow)
        pLabel->visible = pLabel->pWindow->visible;

	if (!pLabel || !pLabel->visible || !pRenderer || !pLabel->draw )
        return 0;

	y = pLabel->rect.y + pLabel->font.spacing / 2;
	len = strlen(pLabel->text);

	if (len > UI_MAX_LENGTH - 2)
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

	if (!pButton || !pButton->visible || !pRenderer || !pButton->draw )
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

	if (pToggle->selected){

		//RenderImage(pRenderer, pToggle->image_selected, pToggle->rect.x, pToggle->rect.y, NULL);
		UI_fillRect(pRenderer,&pToggle->rect,pToggle->color);
	}
	else{

		//RenderImage(pRenderer, pToggle->image_unselected, pToggle->rect.x, pToggle->rect.y, NULL);
		UI_outline(pRenderer,&pToggle->rect,pToggle->color,1);
	}

	return 1;
}

// =========================================================

int UI_entry_draw(UI_entry *pEntry, SDL_Renderer *pRenderer){

	SDL_Color color;
	SDL_Color blue = { 0, 0, 255, 255 };
	SDL_Color green = { 0, 255, 0, 255 };

	if (pEntry && pEntry->pWindow) pEntry->visible = pEntry->pWindow->visible;

	if (!pEntry || !pEntry->visible || !pRenderer) return 0;

	UI_fillRect(pRenderer, &pEntry->rect, pEntry->color_background);
	color = blue;

	if (pEntry->active) color = green;

    if (pEntry->outline) UI_outline(pRenderer, &pEntry->rect, color, 1);

	color = pEntry->color_normal;

	if (pEntry->active) color = pEntry->color_active;

	RenderText(pRenderer, pEntry->text, pEntry->position_text.x, pEntry->position_text.y, pEntry->font, color);

	return 1;
}

// =========================================================

int UI_verticalScrollbar_draw(UI_verticalScrollbar *pVerticalScrollbar, SDL_Renderer *pRenderer){

    int slider_padding = 2;

	if (pVerticalScrollbar && pVerticalScrollbar->pWindow)
		pVerticalScrollbar->visible = pVerticalScrollbar->pWindow->visible;

	if (!pVerticalScrollbar || !pVerticalScrollbar->visible || !pRenderer || !pVerticalScrollbar->draw )
        return 0;

	pVerticalScrollbar->rect_slider.y = pVerticalScrollbar->rect_up.y + pVerticalScrollbar->rect_up.h + (int) (pVerticalScrollbar->fraction * pVerticalScrollbar->maxPos);

	RenderImage(pRenderer, pVerticalScrollbar->image_up, pVerticalScrollbar->rect_up.x, pVerticalScrollbar->rect_up.y, NULL);
	RenderImage(pRenderer, pVerticalScrollbar->image_down, pVerticalScrollbar->rect_down.x, pVerticalScrollbar->rect_down.y + slider_padding, NULL);
	RenderImage(pRenderer, pVerticalScrollbar->image_slider, pVerticalScrollbar->rect_slider.x, pVerticalScrollbar->rect_slider.y, NULL);

	return 1;
}

// =========================================================

int UI_textBox_draw(UI_textBox *pTextBox, SDL_Renderer *pRenderer){

	SDL_Rect highlightRect;
	char buf[UI_MAX_LENGTH];
	int numOfLines, i;

	SDL_Color blue = { 0, 0, 255, 255 };
	int edge = 2;

	if ( pTextBox && pTextBox->pWindow )
		pTextBox->visible = pTextBox->pWindow->visible;

	if ( !pTextBox || !pTextBox->visible || !pRenderer || !pTextBox->draw )
        return 0;

	UI_fillRect(pRenderer, &pTextBox->rect, pTextBox->color_background);

	if (pTextBox->outline)
		UI_outline(pRenderer, &pTextBox->rect, blue, edge);

	if (pTextBox->highlightLine >= 0) {

		MakeRect(&highlightRect, pTextBox->rect_text.x, pTextBox->rect_text.y + pTextBox->highlightLine * pTextBox->font.lineHeight, pTextBox->rect_text.w, pTextBox->font.lineHeight);
		UI_fillRect(pRenderer, &highlightRect, pTextBox->color_highlight);
	}

	if (!pTextBox->array || !pTextBox->array->length)
        return 0;

	numOfLines = textBox_numOfLines(pTextBox);

	for (i = 0; i < numOfLines; i++) {

		String_copy(buf, MaxLength(pTextBox->font, pTextBox->textWidth, (char *) Array_GET_data(pTextBox->array, pTextBox->firstLine + i), NULL), (char *) Array_GET_data(pTextBox->array, pTextBox->firstLine + i), NULL);
		RenderText(pRenderer, buf, pTextBox->rect_text.x, pTextBox->rect_text.y + i * pTextBox->font.lineHeight + pTextBox->font.spacing / 2, pTextBox->font, pTextBox->color_text);
	}

	return 1;
}

// =========================================================
