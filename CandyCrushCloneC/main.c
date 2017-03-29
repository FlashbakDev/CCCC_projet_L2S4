#include "UI.h"
#include "constants.h"
#include "game.h"
#include "menu.h"
#include "editor.h"

int main(int argc, char* argv[]){

    /* Initialisation */

    SDL_Renderer *pRenderer;    // renderer = canvas ( endroit où l'on va déssiner )
    SDL_Event event;            // gestionnaire d'évènements
    Array objects;
    Window window;

    Array_new(&objects);

    GameSession *pGameSession = Game_init(&objects,&window,false,10,10,5,6);

    pRenderer = InitGame("Candy Crush Clone C", &objects, pGameSession->rect_screen.w, pGameSession->rect_screen.h );
    if ( !pRenderer )
        return 1;

    //printf("direction  : %d\n",grid1->isdir_random );
    fprintf(stdout,"Window_new return %d.\n", Window_new(&window, NULL, false, 0, 0, screen_width, screen_height));
    window.visible = true;

    /* boucle de jeu */

    bool draw = true; // non utilisé
    bool quit = false;

    while( !quit ){

        int frameStart = SDL_GetTicks();

        /* évènements */
        while( SDL_PollEvent( &event ) != 0 ){

            if( event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
                quit = true;

            // entré lié au jeu
            Game_event(pGameSession, &event,&draw, &quit);

            // event UI
            Window_event(&window, &event, &draw );
        }

        /* logique */
        Game_logic(pGameSession);

        /* affichage */
        SDL_RenderClear(pRenderer);                                                             // efface tout le contenu du renderer

        Window_draw(&window, pRenderer);

        Game_draw(pGameSession,pRenderer);

        SDL_RenderPresent(pRenderer);                                                           // déssine le renderer à l'écran

        /* gestion de la fréquence d'affichage ( pour les animations )*/
        WaitForNextFrame(frameStart);
    }

    printf("\n\n Score Total : %d", pGameSession->pGrid_J1->score);

    /* fin du programme */
    CleanGame(&objects);

    return 0;
}
