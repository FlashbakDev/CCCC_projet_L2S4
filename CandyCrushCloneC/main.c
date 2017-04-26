#include "UI.h"
#include "constants.h"
#include "game.h"
#include "menu.h"
#include "editor.h"

int main(int argc, char* argv[]){

    /* Initialisation */
    InitSDL();

    // avec une pile ça serais mieux !
    gameState = States_MENU;
    gameState_prec = States_QUIT;
    gameSessionType = GameTypes_CLASSIC;
    editorSessionType = EditorTypes_NEW;

    /* boucle de la machine d'état */
    bool quit = false;
    while(quit == false){

        switch(gameState){

            case States_MENU : {

                MenuSession();
            }
            break;

            case States_GAME : {

                switch( gameSessionType ){

                    case GameTypes_CLASSIC : GameSessionRandom(10,10,6,5,false,5,2,2); break;
                    case GameTypes_PUZZLE : GameSessionPuzzle(Load_grid(&puzzleName)); break;
                }
            }
            break;

            case States_EDITOR : {


                switch( editorSessionType ){

                    case EditorTypes_NEW : fprintf(stdout,"EDITOR -> NEWPUZZLE\n"); EditorSession(NewEmptyPuzzle(10, 10)); break;
                    case EditorTypes_LOAD : fprintf(stdout,"EDITOR -> LOADPUZZLE\n"); EditorSession(Load_grid(&puzzleName)); break;
                }
            }
            break;

            case States_QUIT : {

                quit = true;
            }
            break;
        }
    }

    // Libération de la SDL
    CleanSDL();

    return 0;
}
