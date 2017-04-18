#include "editor.h"

// =========================================================

SDL_Renderer *InitEditor(char * pChar_name, Array *pArray, int w, int h){

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
    error += Image_new(&image_cursorBlue, "data/image_cursorBlue.png", pArray, pRenderer);
    error += Image_new(&image_cursorGreen, "data/image_cursorGreen.png", pArray, pRenderer);
    error += Image_new(&image_cursorRed, "data/image_cursorRed.png", pArray, pRenderer);

    error += LoadTokenImagesFromPath("data/Tokens/Jewellery/", pArray, pRenderer);

    if ( error > 0 ) {

		Clean(pArray);
		return NULL;
	}

	return pRenderer;
}

// =========================================================

Grid *NewEmptyGrid(int x, int y){

    Grid *pGrid = malloc(sizeof(Grid));

    pGrid->width = x;
    pGrid->height = y;
    pGrid->nbMove = 0;
    pGrid->nbColor = 0;
    pGrid->direction = DOWN;
    pGrid->is_randomizeInsert = false;
    pGrid->score = 0;
    pGrid->nbHelp = 0;
    pGrid->nbSuperHelp = 0;
    pGrid->nbRevertOnce = 0;
    pGrid->moveAvailable = 0;

    MakeRect(&pGrid->rect, 0, 0, x*TOKEN_WIDTH, y*TOKEN_HEIGHT);

    pGrid->is_cursorOnGrid = false;
    pGrid->cursorTokenPosition.x = 0;
    pGrid->cursorTokenPosition.y = 0;
    pGrid->isHelpActive = false;

    /* allocation de la grille et remplissage */
    pGrid->tokens = (Token*)malloc( pGrid->height * sizeof(Token*));
    pGrid->pastTokens = (Token*)malloc( pGrid->height * sizeof(Token*));

    if ( pGrid->tokens == NULL || pGrid->pastTokens == NULL){ return NULL; }

    for(int i = 0; i < pGrid->height; i++){

        pGrid->tokens[i] = (Token*)malloc( pGrid->width * sizeof(Token));
        pGrid->pastTokens[i] = (Token*)malloc( pGrid->width * sizeof(Token));

        /* désallocation de la mémoire en cas d'erreur */
        if ( pGrid->tokens[i] == NULL || pGrid->pastTokens[i] == NULL){

            for(i=i-1 ; i >= 0 ; i--){
                free(pGrid->tokens[i]);
                free(pGrid->pastTokens[i]);
            }
            free(pGrid->tokens);
            free(pGrid->pastTokens);

            return NULL;
        }
    }

    ClearGrid(pGrid);

    return pGrid;
}

// =========================================================

void ClearGrid(Grid *pGrid){

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            ClearToken(pGrid, &pGrid->tokens[i][j], j , i);
        }
    }

    MoveAvailable(pGrid, false);
}

// =========================================================

void ClearToken(Grid *pGrid, Token *token, int x, int y){

    ResetToken(token);

    CalculTokenImages(pGrid, token, x, y);
}

// =========================================================

void ResetToken(Token *token){

    token->type = NONE;
    token->color = NONE_COLOR;
    token->isMoving = false;
    token->isDestruct = false;
    token->startDestructAnim = -1;
    token->drawBackground = false;
    token->textureSize = 100;
    token->image = image_tokens[0];
    token->image_background = image_cursorBlue;
    MakeRect(&token->rect_image,0,0,0,0);
}

// =========================================================

bool Toggle_color_event(UI_toggle *pToggle, SDL_Event *pEvent, bool *pDraw, Grid *pGrid ){

    if ( !pToggle->selected  ){
        if ( UI_toggle_event(pToggle, pEvent, pDraw) ){

            return true;
        }
    }

    return false;
}

// =========================================================

void Entry_nbMove_event(UI_entry *pEntry, SDL_Event *pEvent, bool *pDraw, Grid *pGrid ){

    if ( UI_entry_event(pEntry, pEvent, pDraw) ){

        if ( strcmp( pEntry->text, "") == 0 ) String_copy(pEntry->text, 2, "-", NULL);
        else if( strtoimax(pEntry->text, NULL, 10) == 0 ) String_copy(pEntry->text, 2, "-", NULL);
        else if ( strlen(pEntry->text) > 4 ) String_copy(pEntry->text, 2, "-", NULL);
        else pGrid->nbMove = strtoimax(pEntry->text, NULL, 10);
    }
}

// =========================================================

bool Button_tokenType_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid ){

    if( UI_button_event(pButton, pEvent, pDraw) ){

        return true;
    }

    return false;
}

// =========================================================

void Button_reset_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, Grid *pGrid ){

   if( UI_button_event(pButton, pEvent, pDraw) ){

        ClearGrid(pGrid);
    }
}

// =========================================================

void Editor_event(Grid *pGrid, SDL_Event *pEvent, bool *pQuit, Token tokenToPaste){

    switch(pEvent->type){

        // bouttons de souris appuié
        case SDL_MOUSEBUTTONDOWN:{

            switch(pEvent->button.button){

                // bouton gauche
                case SDL_BUTTON_LEFT:{

                    if ( pGrid->is_cursorOnGrid ){

                        pGrid->tokens[ pGrid->cursorTokenPosition.y ][ pGrid->cursorTokenPosition.x ] = tokenToPaste;
                        CalculTokenImages(pGrid, &pGrid->tokens[ pGrid->cursorTokenPosition.y ][ pGrid->cursorTokenPosition.x ], pGrid->cursorTokenPosition.x, pGrid->cursorTokenPosition.y );
                    }
                }
                break;

                case SDL_BUTTON_RIGHT:{

                    if ( pGrid->is_cursorOnGrid ){

                        ClearToken(pGrid, &pGrid->tokens[ pGrid->cursorTokenPosition.y ][ pGrid->cursorTokenPosition.x ], pGrid->cursorTokenPosition.x, pGrid->cursorTokenPosition.y);
                    }
                }
                break;
            }
        }
        break;

        case SDL_MOUSEBUTTONUP:{

            switch(pEvent->button.button){

                // bouton gauche
                case SDL_BUTTON_LEFT:{

                    fprintf( stdout, "click gauche relache\n");
                }
                break;

                case SDL_BUTTON_RIGHT:{

                    fprintf( stdout, "click droit relache\n");
                }
                break;
            }
        }
        break;

        // mouvement de souris
        case SDL_MOUSEMOTION:{

            pGrid->is_cursorOnGrid = PointInRect(pEvent->motion.x, pEvent->motion.y, &pGrid->rect);

            if ( pGrid->is_cursorOnGrid == true ){

                SDL_Point cursorTokenPositionTemp;
                cursorTokenPositionTemp.x = pGrid->cursorTokenPosition.x;
                cursorTokenPositionTemp.y = pGrid->cursorTokenPosition.y;

                pGrid->cursorTokenPosition.x = (pEvent->motion.x / TOKEN_WIDTH);
                pGrid->cursorTokenPosition.y = (pEvent->motion.y / TOKEN_HEIGHT);

                for(int i = 0; i < pGrid->height; i++){
                    for(int j = 0; j < pGrid->width; j++){

                        pGrid->tokens[i][j].drawBackground = false;
                    }
                }
                pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].drawBackground = true;
            }
            else{

                pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x].drawBackground = false;
            }
        }
        break;
    }
}

// =========================================================

void Editor_logic(Grid *pGrid){
}

// =========================================================

void EditorSession(Grid *pGrid){

    fprintf(stdout, "editor.c -> EditorSession(...) : start \n");

    SDL_Renderer *pRenderer;    // renderer = canvas ( endroit où l'on va déssiner )
    SDL_Event event;            // gestionnaire d'évènements
    Array objects;
    Window window;

    UI_label label_score = {false};
    UI_label label_direction = {false};
    UI_button button_quit = {false};
    UI_toggle toggle_direction = {false};
    UI_button button_menu = {false};
    UI_button button_reset = {false};
    UI_label label_mouvements = {false};
    UI_label label_nbMove = {false};
    UI_entry entry_nbMove = {false};

    // création des zone de jeu et d'affichage
    SDL_Rect rect_grid = { 0,0,pGrid->width * TOKEN_WIDTH, pGrid->height * TOKEN_HEIGHT };
    SDL_Rect rect_UI = { rect_grid.x + rect_grid.w, 0, 300, rect_grid.h };

    SDL_Rect rect_screen = {0 ,
                            0 ,
                            (rect_grid.w + rect_grid.x > rect_UI.w + rect_UI.x ) ? rect_grid.w + rect_grid.x : rect_UI.w + rect_UI.x ,
                            (rect_grid.h + rect_grid.y > rect_UI.h + rect_UI.y ) ? rect_grid.h + rect_grid.y : rect_UI.h + rect_UI.y } ;

    Array_new(&objects);

    pRenderer = InitEditor("Candy Crush Clone C", &objects, rect_screen.w, rect_screen.h );
    if ( !pRenderer )
        return;

    fprintf(stdout,"editor.c -> GameSession(...) -> Window_new return %d.\n", Window_new(&window, NULL, false, 0, 0, screen_width, screen_height));
    Array_append(&objects, WINDOW_TYPE, &window);

    fprintf(stdout,"editor.c -> GameSession(...) -> UI_entry_new return %d.\n", UI_entry_new(&entry_nbMove, &window, "", rect_UI.x + 20 , rect_UI.y + 25, 50 ));
    fprintf(stdout,"editor.c -> GameSession(...) -> UI_label_new return %d.\n", UI_label_new(&label_nbMove, &window, "Coups alloues", rect_UI.x + 20 + entry_nbMove.rect.w + 5 , rect_UI.y + 30 ));
    sprintf(entry_nbMove.text,"%d",pGrid->nbMove);
    if ( pGrid->nbMove <= 0 ) String_copy(entry_nbMove.text,2,"-",NULL);

    fprintf(stdout,"editor.c -> GameSession(...) -> UI_label_new return %d.\n", UI_label_new(&label_mouvements, &window, "", rect_UI.x + 20 , rect_UI.y + 120 ));
    sprintf(label_mouvements.text,"Mouvements possibles : %d",pGrid->moveAvailable);

    /*fprintf(stdout,"editor.c -> GameSession(...) -> UI_label_new return %d.\n", UI_label_new(&label_direction, &window, "Direction aleatoire", rect_UI.x + 20 + image_selected.w + 5 , rect_UI.y + 90 ));
    fprintf(stdout,"editor.c -> GameSession(...) -> UI_toggle_new return %d.\n", UI_toggle_new(&toggle_direction, &window, rect_UI.x + 20 , rect_UI.y + 90 ));
    toggle_direction.selected = pGrid->is_randomizeInsert;*/

    fprintf(stdout,"editor.c -> GameSession(...) -> UI_button_new return %d.\n", UI_button_new(&button_quit, &window, "Quitter", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 50 ));
    fprintf(stdout,"editor.c -> GameSession(...) -> UI_button_new return %d.\n", UI_button_new(&button_menu, &window, "Retour menu", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 90 ));
    fprintf(stdout,"editor.c -> GameSession(...) -> UI_button_new return %d.\n", UI_button_new(&button_reset, &window, "Vider grille", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 130 ));

    /* boutons de séléction de token */
    UI_button button_token[4];
    UI_toggle toggle_token_colors[7];

    int spacer = 5;
    for(int i = 0; i < 4; i++){

        fprintf(stdout,"editor.c -> GameSession(...) -> UI_button_new return %d.\n", UI_button_new(&button_token[i], &window, "", rect_UI.x + spacer*i+spacer + TOKEN_WIDTH*i  , rect_UI.y + 150 ));
        UI_set_button_images(&button_token[i], image_tokens[6*i], image_tokens[6*i], image_tokens[6*i]);
    }

    SDL_Color red = {255,0,0,255};
    SDL_Color blue = {0,0,255,255};
    SDL_Color green = {0,255,0,255};
    SDL_Color yellow = {255,255,0,255};
    SDL_Color purple = {127,0,255,255};
    SDL_Color orange = {255,127,0,255};
    SDL_Color black = {0,0,0,255};

    SDL_Color colors[7];
    colors[0] = red;
    colors[1] = blue;
    colors[2] = green;
    colors[3] = yellow;
    colors[4] = purple;
    colors[5] = orange;
    colors[6] = black;

    for(int i = 0; i < 7; i ++ ){

        fprintf(stdout,"editor.c -> GameSession(...) -> UI_toggle_new return %d.\n", UI_toggle_new(&toggle_token_colors[i], &window, rect_UI.x + (rect_UI.w / 2 - 7*25/2) + 5*i + 20*i , rect_UI.y + 150 + TOKEN_HEIGHT + 25 , 20, 20, colors[i]));
    }
    toggle_token_colors[0].selected = true;

    window.visible = true;

    Token tokenToPaste;
    ResetToken(&tokenToPaste);
    tokenToPaste.color = RED;
    tokenToPaste.type = TOKEN;
    AssignImageToToken(&tokenToPaste);

    SDL_Point selectedTokenType = { button_token[0].rect.x , button_token[0].rect.y};

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
            Editor_event(pGrid, &event, &quit, tokenToPaste);

            // event UI
            Window_event(&window, &event, &draw );
            Button_quit_event(&button_quit, &event, &draw, &quit);
            Button_menu_event(&button_menu, &event, &draw, &quit);
            Entry_nbMove_event(&entry_nbMove, &event, &draw, pGrid);
            Button_reset_event(&button_reset, &event, &draw, pGrid);

            for(int i = 0; i < 7; i++){

                if ( Toggle_color_event( &toggle_token_colors[i], &event, &draw, pGrid ) ){

                    // changer les couleurs de token ici
                    tokenToPaste.color = (Colors)i;
                    AssignImageToToken(&tokenToPaste);

                    // actualise les images
                    if ( i < 6 ){

                        for(int j = 0; j < 4; j++){

                            UI_set_button_images(&button_token[j], image_tokens[j*6+i], image_tokens[j*6+i], image_tokens[j*6+i]);
                            button_token[j].draw = true;
                        }
                    }
                    else{

                        selectedTokenType.x = button_token[0].rect.x;
                        selectedTokenType.y = button_token[0].rect.y;
                        UI_set_button_images(&button_token[0], image_tokens[24], image_tokens[24], image_tokens[24]);
                        button_token[1].draw = false;
                        button_token[2].draw = false;
                        button_token[3].draw = false;
                        tokenToPaste.type = MULTI;
                    }

                    for(int j = 0; j < 7 ; j++){

                        if ( j != i )
                            toggle_token_colors[j].selected = false;
                    }
                    break;
                }
            }
        }

        /* logique */
        Editor_logic(pGrid);

        /* maj des labels */
        sprintf(label_mouvements.text,"Mouvements possibles : %d",pGrid->moveAvailable);

        /* animations */

        /* affichage */
        SDL_RenderClear(pRenderer);                                                             // efface tout le contenu du renderer

        Window_draw(&window, pRenderer);

        if ( pGrid->is_cursorOnGrid ){

            Token token_save = pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x];
            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x] = tokenToPaste;
            CalculTokenImages(pGrid, &pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x], pGrid->cursorTokenPosition.x, pGrid->cursorTokenPosition.y);

            //RenderImage(pRenderer, tokenToPaste.image, pGrid->cursorTokenPosition.x * TOKEN_WIDTH, pGrid->cursorTokenPosition.y * TOKEN_HEIGHT, NULL );

            Grid_draw(pGrid,pRenderer); // désine la grille sur le renderer

            pGrid->tokens[pGrid->cursorTokenPosition.y][pGrid->cursorTokenPosition.x] = token_save;
        }
        else{

            Grid_draw(pGrid,pRenderer);
        }

        RenderImage(pRenderer, image_cursorBlue, selectedTokenType.x, selectedTokenType.y, NULL);

        UI_label_draw(&label_nbMove,pRenderer);
        UI_label_draw(&label_mouvements,pRenderer);
        UI_button_draw(&button_quit, pRenderer);
        UI_button_draw(&button_menu, pRenderer);
        UI_entry_draw(&entry_nbMove, pRenderer);
        UI_button_draw(&button_reset, pRenderer);

        for(int i = 0; i < 4; i++)
            UI_button_draw(&button_token[i], pRenderer);

        for(int i = 0; i < 7; i ++ )
            UI_toggle_draw(&toggle_token_colors[i], pRenderer);


        UI_outline(pRenderer, &rect_UI, black, -1);
        UI_outline(pRenderer, &rect_grid, black, -1);

        SDL_RenderPresent(pRenderer);                                                           // déssine le renderer à l'écran

        /* gestion de la fréquence d'affichage ( pour les animations )*/
        WaitForNextFrame(frameStart);
    }

    /* fin du programme */
    Clean(&objects);

    fprintf(stdout, "editor.c -> EditorSession(...) : end \n");
}

// =========================================================
