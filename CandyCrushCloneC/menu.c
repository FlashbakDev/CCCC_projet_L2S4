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
    error += Image_new(&image_arrow_down, "data/UI/image_arrow_down.png", pArray, pRenderer);
    error += Image_new(&image_arrow_up, "data/UI/image_arrow_up.png", pArray, pRenderer);
    error += Image_new(&image_verticalSlider, "data/UI/image_verticalScrollbar.png", pArray, pRenderer);

    if ( error > 0 ) {

		Clean(pArray);
		return NULL;
	}

	return pRenderer;
}

// =========================================================

void Text_reset(UI_textBox *pTextBox, UI_verticalScrollbar *pVerticalScrollbar){

	qsort(pTextBox->array->tab_data, pTextBox->array->length, sizeof(void *), strcmp);
	pVerticalScrollbar->step = 0.;

	if (pTextBox->array->length - pTextBox->maxLines > 0)
		pVerticalScrollbar->step = 1. / (pTextBox->array->length - pTextBox->maxLines);

	pTextBox->firstLine = 0;
	pTextBox->highlightLine = -1;
	pVerticalScrollbar->fraction = 0.;
}

// =========================================================

void dirent_read(UI_textBox *pTextBox, UI_verticalScrollbar *pVerticalScrollbar){

	Files_dirent *ent;
	Files_stat s;
	Files_dir *dir;

	Array_free(pTextBox->array);
	Array_new(pTextBox->array);

	dir = Files_opendir(".\\data\\puzzles");

    if ( dir != NULL ){

        fprintf(stdout,"menu.c -> dirent_read(...) : dir = %s\n", dir->dd_name);

        while ((ent = Files_readdir(dir))) {

            if (!ent->d_name)
                continue;

            Files_getstat(ent->d_name, &s);

            if (strcmp( get_filename_ext(ent->d_name), "puz") == 0 )
                Array_append_string(pTextBox->array, 0, ent->d_name, NULL);
        }
    }
    else{

        fprintf(stdout,"menu.c : dirent_read(...) : dir = NULL");
    }

    Files_closedir(dir);
    Text_reset(pTextBox, pVerticalScrollbar);
}

// =========================================================

void Textbox_files_event(UI_textBox *pTextBox, SDL_Event *pEvent, UI_verticalScrollbar *pVerticalScrollbar, UI_label *pLabel, UI_button *pButton, bool *pDraw){

	int index;

	if (UI_textBox_event(pTextBox, pEvent, pDraw)) {

		index = pTextBox->firstLine + pTextBox->selectedLine;

		if (strcmp((char *) Array_GET_data(pTextBox->array, index),"")) {

			String_copy(pLabel->text, UI_MAX_LENGTH, (char *) Array_GET_data(pTextBox->array, index), NULL);
			String_copy(&puzzleName, UI_MAX_LENGTH, pLabel->text, NULL);
			pButton->draw = true;
			*pDraw = true;
		}
	}
}

// =========================================================

void VerticalScrollbar_files_event(UI_verticalScrollbar *pVerticalScrollbar, SDL_Event *pEvent, UI_textBox *pTextBox, bool *pDraw){

	int firstLine;

	if (UI_verticalScrollbar_event(pVerticalScrollbar, pEvent, pDraw) &&pTextBox->array->length) {

		firstLine = (int) ((pTextBox->array->length - pTextBox->maxLines) * pVerticalScrollbar->fraction + 0.5);

		if (firstLine >= 0)
            pTextBox->firstLine = firstLine;

		*pDraw = true;
	}
}

// =========================================================

void Menu_event(SDL_Event *pEvent, bool *pQuit){

}

// =========================================================

void Button_menu_play_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        *pState = MenuState_PLAY;
    }
}

// =========================================================

void Button_menu_play_load_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        *pState = MenuState_PLAY_LOAD;
    }
}

// =========================================================

void Button_menu_play_classic_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        gameState_prec = gameState;
        gameState = GAME;
        gameSessionType = RANDOM;
        *pQuit = true;
    }
}

// =========================================================

void Button_menu_play_custom_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        *pState = MenuState_PLAY_CUSTOM;
    }
}

// =========================================================

void Button_menu_editor_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        *pState = MenuState_EDITOR;
    }
}

// =========================================================

void Button_menu_editor_load_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        *pState = MenuState_EDITOR_LOAD;
    }
}

// =========================================================

void Button_menu_editor_new_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        gameState_prec = gameState;
        gameState = EDITOR;
        editorSessionType = NEWPUZZLE;
        *pQuit = true;
    }
}

// =========================================================

void Button_menu_return_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        switch( *pState ){

            case MenuState_EDITOR_LOAD : *pState = MenuState_EDITOR; break;
            case MenuState_EDITOR : *pState = MenuState_MENU; break;
            case MenuState_PLAY : *pState = MenuState_MENU; break;
            case MenuState_PLAY_CUSTOM : *pState = MenuState_PLAY; break;
            case MenuState_PLAY_LOAD : *pState = MenuState_PLAY; break;
        }
    }
}

// =========================================================

void Button_ok_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit, MenuState *pState){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        if ( *pState == MenuState_EDITOR_LOAD ){

            gameState_prec = gameState;
            gameState = EDITOR;
            editorSessionType = LOADPUZZLE;
            *pQuit = true;
        }
        else if( *pState == MenuState_PLAY_LOAD ){

            gameState_prec = gameState;
            gameState = GAME;
            gameSessionType = PUZZLE;
            *pQuit = true;
        }
    }
}

// =========================================================

void Menu_logic(){

}

// =========================================================

void MenuSession(){

    fprintf(stdout, "menu.c -> MenuSession() : start \n");

    SDL_Renderer *pRenderer;    // renderer = canvas ( endroit où l'on va déssiner )
    SDL_Event event;            // gestionnaire d'évènements
    Array objects, files;
    Window window;
    MenuState menuState = MenuState_MENU;

    UI_button button_quit = {false};
    UI_button button_play = {false};
    UI_button button_editor = {false};
    UI_button button_editor_load = {false};
    UI_button button_play_load = {false};
    UI_button button_play_classic = {false};
    UI_button button_play_custom = {false};
    UI_button button_editor_new = {false};
    UI_button button_return = {false};
    UI_label label_puzzleName = {false};
    UI_button button_ok = {false};

    UI_textBox textBox_files = {false};
    UI_verticalScrollbar verticalScrollbar_files = {false};
    SDL_Color color_blue = {0,0,255,255};

    pRenderer = InitMenu("Candy Crush Clone C", &objects, WINDOW_WIDTH, WINDOW_HEIGHT );
    if ( !pRenderer )
        return;

    Array_append(&objects, ARRAY_TYPE, &files);

    fprintf(stdout,"menu.c -> MenuSession() -> UI_button_new return %d.\n", UI_button_new(&button_return, &window, "Retour", screen_width / 2 - image_normal.w / 2 , screen_height - image_normal.h - 20 ));

    fprintf(stdout,"menu.c -> MenuSession() -> Window_new return %d.\n", Window_new(&window, NULL, false, 0, 0, screen_width, screen_height));
    fprintf(stdout,"menu.c -> MenuSession() -> UI_button_new return %d.\n", UI_button_new(&button_quit, &window, "Quitter", screen_width / 2 - image_normal.w / 2 , screen_height / 2 - image_normal.h / 2 + 20 ));
    fprintf(stdout,"menu.c -> MenuSession() -> UI_button_new return %d.\n", UI_button_new(&button_play, &window, "Jouer", screen_width / 2 - image_normal.w / 2 , screen_height / 2 - image_normal.h / 2 - 60 ));
    fprintf(stdout,"menu.c -> MenuSession() -> UI_button_new return %d.\n", UI_button_new(&button_editor, &window, "Editeur", screen_width / 2 - image_normal.w / 2 , screen_height / 2 - image_normal.h / 2 - 20 ));

    fprintf(stdout,"menu.c -> MenuSession() -> UI_button_new return %d.\n", UI_button_new(&button_play_classic, &window, "Partie classique", screen_width / 2 - image_normal.w / 2 , screen_height / 2 - image_normal.h / 2 - 100 ));
    fprintf(stdout,"menu.c -> MenuSession() -> UI_button_new return %d.\n", UI_button_new(&button_play_load, &window, "Puzzle", screen_width / 2 - image_normal.w / 2 , screen_height / 2 - image_normal.h / 2 - 60 ));
    fprintf(stdout,"menu.c -> MenuSession() -> UI_button_new return %d.\n", UI_button_new(&button_play_custom, &window, "Partie custom", screen_width / 2 - image_normal.w / 2 , screen_height / 2 - image_normal.h / 2 - 20 ));

    fprintf(stdout,"menu.c -> MenuSession() -> UI_button_new return %d.\n", UI_button_new(&button_editor_new, &window, "Nouveau puzzle", screen_width / 2 - image_normal.w / 2 , screen_height / 2 - image_normal.h / 2 - 60 ));
    fprintf(stdout,"menu.c -> MenuSession() -> UI_button_new return %d.\n", UI_button_new(&button_editor_load, &window, "Editer un puzzle", screen_width / 2 - image_normal.w / 2 , screen_height / 2 - image_normal.h / 2 - 20 ));

    fprintf(stdout,"menu.c -> MenuSession() -> UI_textBox_new return %d.\n", UI_textBox_new(&textBox_files, &window, true, &files, 20, 20, screen_width - 20 - image_arrow_up.w - 20 - 2 , screen_height - 200 ));
    fprintf(stdout,"menu.c -> MenuSession() -> UI_verticalScrollbar_new return %d.\n", UI_verticalScrollbar_new(&verticalScrollbar_files, &window, textBox_files.rect.x + textBox_files.rect.w + 2 , textBox_files.rect.y , textBox_files.rect.h, image_arrow_up.w, color_blue ));

    fprintf(stdout,"menu.c -> MenuSession() -> UI_label_new return %d.\n", UI_label_new(&label_puzzleName, &window, "", 20 , screen_height - 200 + 5 + font_default.fontHeight ));
    fprintf(stdout,"menu.c -> MenuSession() -> UI_button_new return %d.\n", UI_button_new(&button_ok, &window, "Ok", 20 , label_puzzleName.rect.y + font_default.fontHeight + 5 ));

    window.visible = true;
    button_editor_new.draw = false;
    button_editor_load.draw = false;
    button_play_classic.draw = false;
    button_play_custom.draw = false;
    button_play_load.draw = false;
    button_return.draw = false;
    textBox_files.draw = false;
    verticalScrollbar_files.draw = false;
    button_ok.draw = false;

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

            MenuState menuState_temp = menuState;
            Button_menu_return_event(&button_return, &event, &draw, &quit, &menuState);
            Button_menu_play_event(&button_play, &event, &draw, &quit, &menuState);
            Button_menu_play_classic_event(&button_play_classic, &event, &draw, &quit, &menuState);
            Button_menu_play_custom_event(&button_play_custom, &event, &draw, &quit, &menuState);
            Button_menu_play_load_event(&button_play_load, &event, &draw, &quit, &menuState);
            Button_menu_editor_event(&button_editor, &event, &draw, &quit, &menuState);
            Button_menu_editor_load_event(&button_editor_load, &event, &draw, &quit, &menuState);
            Button_menu_editor_new_event(&button_editor_new, &event, &draw, &quit, &menuState);
            Textbox_files_event(&textBox_files, &event, &verticalScrollbar_files, &label_puzzleName, &button_ok, &draw);
            VerticalScrollbar_files_event(&verticalScrollbar_files, &event, &textBox_files, &draw);
            Button_ok_event(&button_ok, &event, &draw, &quit, &menuState);

            if ( menuState_temp !=  menuState){

                fprintf(stdout, "menu.c -> MenuSession() : menuState = %d \n", menuState);

                button_quit.draw = false;
                button_editor.draw = false;
                button_editor_new.draw = false;
                button_editor_load.draw = false;
                button_play.draw = false;
                button_play_classic.draw = false;
                button_play_custom.draw = false;
                button_play_load.draw = false;
                button_return.draw = false;
                textBox_files.draw = false;
                verticalScrollbar_files.draw = false;
                label_puzzleName.draw = false;
                button_ok.draw = false;

                switch(menuState){

                    case MenuState_MENU:{

                        button_play.draw = true;
                        button_editor.draw = true;
                        button_quit.draw = true;
                    }
                    break;

                    case MenuState_PLAY:{

                        button_return.draw = true;
                        button_play_classic.draw = true;
                        button_play_custom.draw = true;
                        button_play_load.draw = true;
                    }
                    break;

                    case MenuState_PLAY_CUSTOM:{

                        button_return.draw = true;
                    }
                    break;

                    case MenuState_PLAY_LOAD:{

                        button_return.draw = true;
                        textBox_files.draw = true;
                        verticalScrollbar_files.draw = true;
                        label_puzzleName.draw = true;

                        dirent_read(&textBox_files, &verticalScrollbar_files);
                    }
                    break;

                    case MenuState_EDITOR:{

                        button_return.draw = true;
                        button_editor_new.draw = true;
                        button_editor_load.draw = true;
                    }
                    break;

                    case MenuState_EDITOR_LOAD :{

                        button_return.draw = true;
                        textBox_files.draw = true;
                        verticalScrollbar_files.draw = true;
                        label_puzzleName.draw = true;

                        dirent_read(&textBox_files, &verticalScrollbar_files);
                    }
                    break;
                }
            }
        }

        VerticalScrollbar_files_event(&verticalScrollbar_files, NULL, &textBox_files, &draw);

        Menu_logic();

        /* affichage */
        SDL_RenderClear(pRenderer);                              // efface tout le contenu du renderer

        Window_draw(&window, pRenderer);

        UI_button_draw(&button_quit, pRenderer);
        UI_button_draw(&button_return, pRenderer);
        UI_button_draw(&button_play, pRenderer);
        UI_button_draw(&button_play_classic, pRenderer);
        UI_button_draw(&button_play_custom, pRenderer);
        UI_button_draw(&button_play_load, pRenderer);
        UI_button_draw(&button_editor, pRenderer);
        UI_button_draw(&button_editor_load, pRenderer);
        UI_button_draw(&button_editor_new, pRenderer);
        UI_verticalScrollbar_draw(&verticalScrollbar_files, pRenderer);
        UI_textBox_draw(&textBox_files, pRenderer);
        UI_button_draw(&button_ok, pRenderer);
        UI_label_draw(&label_puzzleName, pRenderer);

        SDL_RenderPresent(pRenderer);                           // déssine le renderer à l'écran

        /* gestion de la fréquence d'affichage ( pour les animations )*/
        WaitForNextFrame(frameStart);
    }

    /* fin du programme */
    Clean(&objects);

    fprintf(stdout, "menu.c -> MenuSession() : end \n");
}

// =========================================================
