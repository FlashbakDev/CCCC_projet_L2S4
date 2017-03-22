#include "UI.h"
#include "constants.h"
#include "game.h"
#include "menu.h"
#include "editor.h"

int main(int argc, char* argv[]){

    /* Initialisation */
    fprintf(stdout,"Initialisation begin : \n");

    SDL_Renderer *pRenderer;    // renderer = canvas ( endroit o� l'on va d廥siner )
    SDL_Event event;            // gestionnaire d'憝鋝ement
    Array objects;
    Window window;

    UI_label label_score = {false};
    UI_button button_quit = {false};
    UI_label label_mouvements = {false};
    UI_label label_nbMove = {false};

    /* initialisation du jeu */
    int gridHeight = 10;
    int gridWidth = 10;
    int nbColor = 6;
    int nbMove = 5;

    // cr嶧tion des zone de jeu et d'affichage
    SDL_Rect rect_grid = { 0,0,gridWidth, gridHeight };
    SDL_Rect rect_UI = { rect_grid.x * TOKEN_WIDTH + rect_grid.w * TOKEN_WIDTH, 0, 250, rect_grid.h * TOKEN_HEIGHT };

    SDL_Rect rect_screen = {0 ,
                            0 ,
                            (rect_grid.w * TOKEN_WIDTH + rect_grid.x * TOKEN_WIDTH > rect_UI.w + rect_UI.x ) ? rect_grid.w * TOKEN_WIDTH + rect_grid.x * TOKEN_WIDTH : rect_UI.w + rect_UI.x ,
                            (rect_grid.h * TOKEN_HEIGHT + rect_grid.y * TOKEN_HEIGHT > rect_UI.h + rect_UI.y ) ? rect_grid.h * TOKEN_HEIGHT + rect_grid.y * TOKEN_HEIGHT : rect_UI.h + rect_UI.y } ;

    Array_new(&objects);

    pRenderer = InitGame("Candy Crush Clone C", &objects, rect_screen.w, rect_screen.h );
    if ( !pRenderer )
        return 1;

    Grid *grid1 = NewGrid(rect_grid,nbMove,nbColor);

    fprintf(stdout,"debug\n");

    fprintf(stdout,"Window_new return %d.\n", Window_new(&window, NULL, false, 0, 0, screen_width, screen_height));
    fprintf(stdout,"UI_label_new return %d.\n", UI_label_new(&label_score, &window, "Test", rect_UI.x + 20 , rect_UI.y + 20 ));
    fprintf(stdout,"UI_label_new return %d.\n", UI_label_new(&label_nbMove, &window, "Test", rect_UI.x + 20 , rect_UI.y + 40 ));
    fprintf(stdout,"UI_label_new return %d.\n", UI_label_new(&label_mouvements, &window, "Test", rect_UI.x + 20 , rect_UI.y + 60 ));

    sprintf(label_score.text,"Score : %d ",0);
    sprintf(label_nbMove.text,"NbCoups : %d", nbMove);
    sprintf(label_mouvements.text,"Mouvement possible : %d",0);

    fprintf(stdout,"UI_button_new return %d.\n", UI_button_new(&button_quit, &window, "Quitter", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 50 ));
    window.visible = true;

    fprintf(stdout,"Initialisation end.\n");

    /* boucle de jeu */

    int score = 0;
    bool draw = true; // placebo
    bool quit = false;

    while( !quit ){

        int frameStart = SDL_GetTicks();

        /* 憝鋝ements */
        while( SDL_PollEvent( &event ) != 0 ){

            if( event.type == SDL_QUIT){

                quit = true;
            }

            if ( event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE ){

                fprintf(stdout,"Appuie sur echap, fin de la boucle de jeu");
                quit = true;
            }

            // entr矇 li矇 au jeu
            Game_event(grid1, &event, &quit);

            // event UI
            Window_event(&window, &event, &draw );
            Button_quit_event(&button_quit, &event, &draw, &quit);
        }

        /* logique */
        Game_logic(grid1);

        /* maj des mlabels */
        sprintf(label_nbMove.text," NbCoups : %d", grid1->nbMove);
        sprintf(label_score.text,"Score : %d ", grid1->score);
        sprintf(label_mouvements.text,"Nombre de mouvement : %d",grid1->moveAvailable);

        /* animations - textes */
        Grid_anim(grid1);

        /* affichage */
        SDL_RenderClear(pRenderer);                                                             // efface tout le contenu du renderer

        Window_draw(&window, pRenderer);

        Grid_draw(grid1,pRenderer);                                                              // d廥sine la grille sur le renderer

        UI_label_draw(&label_score,pRenderer);
        UI_label_draw(&label_nbMove,pRenderer);
        UI_label_draw(&label_mouvements,pRenderer);
        UI_button_draw(&button_quit, pRenderer);

        SDL_RenderPresent(pRenderer);                                                           // d廥sine le renderer � l'嶰ran

        /* gestion de la fr廦uence d'affichage ( pour les animations )*/
        WaitForNextFrame(frameStart);
    }

    printf("\n\n Score Total : %d", score);

    /* fin du programme */
    CleanGame(&objects);

    return 0;
}
