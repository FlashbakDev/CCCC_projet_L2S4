#include "menu.h"

// =========================================================

SDL_Renderer *InitMenu(char * pChar_name, Array *pArray, int w, int h){

    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    SDL_Rect rect_bounds;
    int error = 0;

    SDL_GetDisplayBounds(0, &rect_bounds);

    if ( !pArray || w > rect_bounds.w || h > rect_bounds.h ){

        SDL_Quit();
        return NULL;
    }

    screen_width = w;
    screen_height = h;

    Array_new(pArray);

    pWindow = SDL_CreateWindow(pChar_name, rect_bounds.w / 2 - w / 2, rect_bounds.h / 2 - h / 2, w, h, SDL_WINDOW_SHOWN);
    if ( pWindow ) Array_append(pArray, WINDOW_TYPE , pWindow);
    // void SDL_SetWindowIcon(SDL_Window*  window , SDL_Surface* icon); //ajoute une ic�ne � la fen�tre

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

int CleanMenu(Array *pArray){

    if ( !pArray )
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

    return 0;
}

// =========================================================

void Menu_event(SDL_Event *pEvent, bool *pQuit){

}

// =========================================================

void Menu_logic(){

}

// =========================================================

void MenuSession(){

    fprintf(stdout, "menu.c -> MenuSession() : start \n");

    SDL_Renderer *pRenderer;    // renderer = canvas ( endroit o� l'on va d�ssiner )
    SDL_Event event;            // gestionnaire d'�v�nements
    Array objects;
    Window *pWindow;

    UI_button button_quit = {false};
    UI_button button_play = {false};

    Array_new(&objects);

    pRenderer = InitMenu("Candy Crush Clone C", &objects, WINDOW_WIDTH, WINDOW_HEIGHT );
    if ( !pRenderer )
        return;

    fprintf(stdout,"menu.c -> MenuSession() -> Window_new return %d.\n", Window_new(pWindow, NULL, false, 0, 0, screen_width, screen_height));
    //Array_append(&objects, WINDOW_TYPE, pWindow);
    fprintf(stdout,"menu.c -> MenuSession() -> UI_button_new return %d.\n", UI_button_new(&button_quit, pWindow, "Quitter", screen_width / 2 - image_normal.w / 2 , screen_height / 2 - image_normal.h / 2 + 20 ));
    fprintf(stdout,"menu.c -> MenuSession() -> UI_button_new return %d.\n", UI_button_new(&button_play, pWindow, "Jouer", screen_width / 2 - image_normal.w / 2 , screen_height / 2 - image_normal.h / 2 - 20 ));

    pWindow->visible = true;

    bool draw = true; // non utilis�
    bool quit = false;

    while( !quit ){

        int frameStart = SDL_GetTicks();

        /* �v�nements */
        while( SDL_PollEvent( &event ) != 0 ){

            if( event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)){

                gameState = QUIT;
                quit = true;
            }

            // entr� li� a la grille
            Menu_event( &event, &quit);

            // event UI
            Window_event(pWindow, &event, &draw );
            Button_quit_event(&button_quit, &event, &draw, &quit);
            Button_play_event(&button_play, &event, &draw, &quit);
        }

        /* affichage */
        SDL_RenderClear(pRenderer);                                                             // efface tout le contenu du renderer

        Window_draw(pWindow, pRenderer);

        UI_button_draw(&button_quit, pRenderer);
        UI_button_draw(&button_play, pRenderer);

        SDL_RenderPresent(pRenderer);                                                           // d�ssine le renderer � l'�cran

        /* gestion de la fr�quence d'affichage ( pour les animations )*/
        WaitForNextFrame(frameStart);
    }

    /* fin du programme */
    CleanMenu(&objects);

    fprintf(stdout, "menu.c -> MenuSession() : end \n");
}

// =========================================================
