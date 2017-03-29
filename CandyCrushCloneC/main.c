#include "UI.h"
#include "constants.h"
#include "game.h"
#include "menu.h"
#include "editor.h"

void Button_quit_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit );

int main(int argc, char* argv[]){

    /* Initialisation */
    InitSDL();

    gameState = MENU;

    /* boucle */
    bool quit = false;
    while(quit == false){

        switch(gameState){

            case MENU : {

                MenuSession();
            }
            break;

            case GAMESESSION : {

                GameSession(10,10,6,5,false);
            }
            break;

            case EDITOR : {

                gameState = MENU;
            }
            break;

            case QUIT : {

                quit = true;
            }
            break;
        }
    }

    CleanSDL();

    return 0;
}

// =========================================================

void Button_quit_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        gameState = QUIT;
        *pQuit = true;
    }
}

void Button_menu_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        gameState = MENU;
        *pQuit = true;
    }
}

void Button_play_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        gameState = GAMESESSION;
        *pQuit = true;
    }
}
