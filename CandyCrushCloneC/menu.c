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
    // void SDL_SetWindowIcon(SDL_Window*  window , SDL_Surface* icon); //ajoute une icône à la fenêtre

    pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if ( pRenderer ) Array_append(pArray, RENDERER_TYPE, pRenderer);

    // init des ressources
    error += Font_new(&font_default, "data/fonts/arial.ttf", pArray, 15);
    error += Image_new(&image_active, "data/UI/image_active.png", pArray, pRenderer);
    error += Image_new(&image_prelight, "data/UI/image_prelight.png", pArray, pRenderer);
    error += Image_new(&image_normal, "data/UI/image_normal.png", pArray, pRenderer);
    error += Image_new(&image_selected, "data/UI/image_selected.png", pArray, pRenderer);
    error += Image_new(&image_unselected, "data/UI/image_unselected.png", pArray, pRenderer);

    if ( error > 0 ) {

		Clean(pArray);
		return NULL;
	}

	return pRenderer;
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

    SDL_Renderer *pRenderer;    // renderer = canvas ( endroit où l'on va déssiner )
    SDL_Event event;            // gestionnaire d'évènements
    Array objects;
    Window window;

    UI_button button_quit = {false};
    UI_button button_play = {false};
    UI_button button_editor = {false};

    Array_new(&objects);

    pRenderer = InitMenu("Candy Crush Clone C", &objects, WINDOW_WIDTH, WINDOW_HEIGHT );
    if ( !pRenderer )
        return;

    fprintf(stdout,"menu.c -> MenuSession() -> Window_new return %d.\n", Window_new(&window, NULL, false, 0, 0, screen_width, screen_height));
    Array_append(&objects, WINDOW_TYPE, &window);
    fprintf(stdout,"menu.c -> MenuSession() -> UI_button_new return %d.\n", UI_button_new(&button_quit, &window, "Quitter", screen_width / 2 - image_normal.w / 2 , screen_height / 2 - image_normal.h / 2 + 20 ));
    fprintf(stdout,"menu.c -> MenuSession() -> UI_button_new return %d.\n", UI_button_new(&button_play, &window, "Jouer", screen_width / 2 - image_normal.w / 2 , screen_height / 2 - image_normal.h / 2 - 60 ));
    fprintf(stdout,"menu.c -> MenuSession() -> UI_button_new return %d.\n", UI_button_new(&button_editor, &window, "Editeur", screen_width / 2 - image_normal.w / 2 , screen_height / 2 - image_normal.h / 2 - 20 ));

    window.visible = true;

    bool draw = true; // non utilisé
    bool quit = false;
    while( !quit ){

        int frameStart = SDL_GetTicks();

        /* évènements */
        while( SDL_PollEvent( &event ) != 0 ){

            if( event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)){

                gameState = QUIT;
                quit = true;
            }

            // entré lié a la grille
            Menu_event( &event, &quit);

            // event UI
            Window_event(&window, &event, &draw );
            Button_quit_event(&button_quit, &event, &draw, &quit);
            Button_play_event(&button_play, &event, &draw, &quit);
            Button_editor_event(&button_editor, &event, &draw, &quit);
        }

        /* affichage */
        SDL_RenderClear(pRenderer);                              // efface tout le contenu du renderer

        Window_draw(&window, pRenderer);

        UI_button_draw(&button_quit, pRenderer);
        UI_button_draw(&button_play, pRenderer);
        UI_button_draw(&button_editor, pRenderer);

        SDL_RenderPresent(pRenderer);                           // déssine le renderer à l'écran

        /* gestion de la fréquence d'affichage ( pour les animations )*/
        WaitForNextFrame(frameStart);
    }

    /* fin du programme */
    Clean(&objects);

    fprintf(stdout, "menu.c -> MenuSession() : end \n");
}

// =========================================================
