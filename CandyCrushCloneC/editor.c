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
    error += Image_new(&image_cursorBlue, "data/image_cursorBlue.png", pArray, pRenderer);
    error += Image_new(&image_cursorGreen, "data/image_cursorGreen.png", pArray, pRenderer);
    error += Image_new(&image_cursorRed, "data/image_cursorRed.png", pArray, pRenderer);

    // image des jetons
    error += Image_new(&image_tokens[0], "data/Tokens/Token_red.png", pArray, pRenderer);
    error += Image_new(&image_tokens[1], "data/Tokens/Token_blue.png", pArray, pRenderer);
    error += Image_new(&image_tokens[2], "data/Tokens/Token_green.png", pArray, pRenderer);
    error += Image_new(&image_tokens[3], "data/Tokens/Token_yellow.png", pArray, pRenderer);
    error += Image_new(&image_tokens[4], "data/Tokens/Token_purple.png", pArray, pRenderer);
    error += Image_new(&image_tokens[5], "data/Tokens/Token_orange.png", pArray, pRenderer);

    error += Image_new(&image_tokens[6], "data/Tokens/Token_red_horizontal.png", pArray, pRenderer);
    error += Image_new(&image_tokens[7], "data/Tokens/Token_blue_horizontal.png", pArray, pRenderer);
    error += Image_new(&image_tokens[8], "data/Tokens/Token_green_horizontal.png", pArray, pRenderer);
    error += Image_new(&image_tokens[9], "data/Tokens/Token_yellow_horizontal.png", pArray, pRenderer);
    error += Image_new(&image_tokens[10], "data/Tokens/Token_purple_horizontal.png", pArray, pRenderer);
    error += Image_new(&image_tokens[11], "data/Tokens/Token_orange_horizontal.png", pArray, pRenderer);

    error += Image_new(&image_tokens[12], "data/Tokens/Token_red_vertical.png", pArray, pRenderer);
    error += Image_new(&image_tokens[13], "data/Tokens/Token_blue_vertical.png", pArray, pRenderer);
    error += Image_new(&image_tokens[14], "data/Tokens/Token_green_vertical.png", pArray, pRenderer);
    error += Image_new(&image_tokens[15], "data/Tokens/Token_yellow_vertical.png", pArray, pRenderer);
    error += Image_new(&image_tokens[16], "data/Tokens/Token_purple_vertical.png", pArray, pRenderer);
    error += Image_new(&image_tokens[17], "data/Tokens/Token_orange_vertical.png", pArray, pRenderer);

    error += Image_new(&image_tokens[18], "data/Tokens/Token_multi.png", pArray, pRenderer);

    if ( error > 0 ) {

		Clean(pArray);
		return NULL;
	}

	return pRenderer;
}

// =========================================================

Grid *NewEmptyGrid(SDL_Rect rect){

    Grid *pGrid = malloc(sizeof(Grid));

    pGrid->width = rect.w;
    pGrid->height = rect.h;
    pGrid->nbMove = 0;
    pGrid->nbColor = 0;
    pGrid->direction = DOWN;
    pGrid->is_randomizeInsert = false;
    pGrid->score = 0;
    pGrid->nbHelp = 0;
    pGrid->nbSuperHelp = 0;
    pGrid->nbRevertOnce = 0;

    MakeRect(&pGrid->rect, rect.x, rect.y, rect.w * TOKEN_WIDTH, rect.h * TOKEN_HEIGHT);

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
}

// =========================================================

void ClearToken(Grid *pGrid, Token *token, int x, int y){

    token->type = NONE;
    token->color = NONE_COLOR;
    token->isMoving = false;
    token->isDestruct = false;
    token->startDestructAnim = -1;

    token->textureSize = 100;

    CalculTokenImages(pGrid, token, x, y);
}

// =========================================================

void Editor_event(Grid *pGrid, SDL_Event *pEvent, bool *pQuit){

}

// =========================================================

void Editor_logic(Grid *pGrid){
}


// =========================================================

void EditorSession(int gridWidth, int gridHeight){

    fprintf(stdout, "editor.c -> EditorSession(...) : start \n");

    SDL_Renderer *pRenderer;    // renderer = canvas ( endroit où l'on va déssiner )
    SDL_Event event;            // gestionnaire d'évènements
    Array objects;
    Window window;

    UI_label label_score = {false};
    UI_button button_quit = {false};
    UI_button button_direction = {false};
    UI_button button_menu = {false};
    UI_label label_mouvements = {false};
    UI_label label_nbMove = {false};

    // création des zone de jeu et d'affichage
    SDL_Rect rect_grid = { 0,0,gridWidth, gridHeight };
    SDL_Rect rect_UI = { rect_grid.x * TOKEN_WIDTH + rect_grid.w * TOKEN_WIDTH, 0, 250, rect_grid.h * TOKEN_HEIGHT };

    SDL_Rect rect_screen = {0 ,
                            0 ,
                            (rect_grid.w * TOKEN_WIDTH + rect_grid.x * TOKEN_WIDTH > rect_UI.w + rect_UI.x ) ? rect_grid.w * TOKEN_WIDTH + rect_grid.x * TOKEN_WIDTH : rect_UI.w + rect_UI.x ,
                            (rect_grid.h * TOKEN_HEIGHT + rect_grid.y * TOKEN_HEIGHT > rect_UI.h + rect_UI.y ) ? rect_grid.h * TOKEN_HEIGHT + rect_grid.y * TOKEN_HEIGHT : rect_UI.h + rect_UI.y } ;

    Array_new(&objects);

    pRenderer = InitEditor("Candy Crush Clone C", &objects, rect_screen.w, rect_screen.h );
    if ( !pRenderer )
        return;

    int nbMove = 5;
    int nbColor = 6;
    bool randomizeInsert = false;
    int nbHelp = 0;
    int nbSuperHelp = 0;
    int nbRevertOnce = 0;
    Grid *grid1 = NewEmptyGrid(rect_grid);

    fprintf(stdout,"game.c -> GameSession(...) -> Window_new return %d.\n", Window_new(&window, NULL, false, 0, 0, screen_width, screen_height));
    Array_append(&objects, WINDOW_TYPE, &window);
    fprintf(stdout,"game.c -> GameSession(...) -> UI_label_new return %d.\n", UI_label_new(&label_score, &window, "Test", rect_UI.x + 20 , rect_UI.y + 20 ));
    fprintf(stdout,"game.c -> GameSession(...) -> UI_label_new return %d.\n", UI_label_new(&label_nbMove, &window, "Test", rect_UI.x + 20 , rect_UI.y + 40 ));
    fprintf(stdout,"game.c -> GameSession(...) -> UI_label_new return %d.\n", UI_label_new(&label_mouvements, &window, "Test", rect_UI.x + 20 , rect_UI.y + 60 ));

    sprintf(label_score.text,"Score : %d ",grid1->score);
    sprintf(label_nbMove.text,"NbCoups : %d", grid1->nbMove);
    sprintf(label_mouvements.text,"Mouvement possible : %d",grid1->moveAvailable);

    fprintf(stdout,"game.c -> GameSession(...) -> UI_button_new return %d.\n", UI_button_new(&button_quit, &window, "Quitter", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 50 ));
    fprintf(stdout,"game.c -> GameSession(...) -> UI_button_new return %d.\n", UI_button_new(&button_menu, &window, "Retour menu", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 90 ));
    fprintf(stdout,"game.c -> GameSession(...) -> UI_button_new return %d.\n", UI_button_new(&button_direction, &window, "Direction", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 200 ));

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
            Editor_event(grid1, &event, &quit);

            // event UI
            Window_event(&window, &event, &draw );
            Button_quit_event(&button_quit, &event, &draw, &quit);
            Button_menu_event(&button_menu, &event, &draw, &quit);
            Button_direction_event(&button_direction, &event, &draw, grid1);
        }

        /* logique */

        /* maj des labels */
        sprintf(label_nbMove.text," NbCoups : %d", grid1->nbMove);
        sprintf(label_score.text,"Score : %d ", grid1->score);
        sprintf(label_mouvements.text,"Nombre de mouvement : %d",grid1->moveAvailable);

        /* animations */

        /* affichage */
        SDL_RenderClear(pRenderer);                                                             // efface tout le contenu du renderer

        Window_draw(&window, pRenderer);

        Grid_draw(grid1,pRenderer);                                                              // désine la grille sur le renderer

        UI_label_draw(&label_score,pRenderer);
        UI_label_draw(&label_nbMove,pRenderer);
        UI_label_draw(&label_mouvements,pRenderer);
        UI_button_draw(&button_quit, pRenderer);
        UI_button_draw(&button_direction, pRenderer);
        UI_button_draw(&button_menu, pRenderer);

        SDL_RenderPresent(pRenderer);                                                           // déssine le renderer à l'écran

        /* gestion de la fréquence d'affichage ( pour les animations )*/
        WaitForNextFrame(frameStart);
    }

    /* fin du programme */
    Clean(&objects);

    fprintf(stdout, "editor.c -> EditorSession(...) : end \n");
}

// =========================================================
