#include "UI.h"
#include "constants.h"
#include "game.h"
#include "menu.h"
#include "editor.h"

void Button_quit_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit );
void Button_menu_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit );
void Button_play_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit );
void Button_editor_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit );
void Button_return_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit );

int main(int argc, char* argv[]){

    /* Initialisation */
    InitSDL();

    // avec une pile Ã§a serais mieux !
    gameState = MENU;
    gameState_prec = QUIT;
    gameSessionType = RANDOM;

    /* boucle */
    bool quit = false;
    while(quit == false){

        switch(gameState){

            case MENU : {

                MenuSession();
            }
            break;

            case GAME : {

                switch( gameSessionType ){

                    case RANDOM : GameSessionRandom(10,10,6,5,false,5,2,2); break;
                    case PUZZLE : GameSessionPuzzle(&loadedGrid); break;
                }
            }
            break;

            case EDITOR : {

                /* a faire par chargement d'un fichier ou par création d'un nouveau */
                loadedGrid = *NewEmptyGrid(10, 10);

                EditorSession(&loadedGrid);
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

        gameState_prec = gameState;
        gameState = QUIT;
        *pQuit = true;
    }
}

void Button_menu_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        gameState_prec = gameState;
        gameState = MENU;
        *pQuit = true;
    }
}

void Button_play_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        gameState_prec = gameState;
        gameState = GAME;
        gameSessionType = RANDOM;
        *pQuit = true;
    }
}

void Button_return_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        gameState = gameState_prec;
        *pQuit = true;
    }
}

void Button_editor_event(UI_button *pButton, SDL_Event *pEvent, bool *pDraw, bool *pQuit ){

    if ( UI_button_event(pButton, pEvent, pDraw) ){

        gameState_prec = gameState;
        gameState = EDITOR;
        *pQuit = true;
    }
}
