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

    // nom du fichier grille à chargé / éditer / jouer
    char puzzleName[UI_MAX_LENGTH];

    /* boucle de la machine d'état */
    bool quit = false;
    while(quit == false){

        switch(gameState){

            case States_MENU : MenuSession(&puzzleName); break;

            case States_GAME_CLASSIC : GameSessionRandom(10,10,6,2,false,5,2,2); break;

            case States_GAME_PUZZLE : GameSessionPuzzle(Load_grid(&puzzleName)); break;

            case States_EDITOR_NEW : EditorSession(&puzzleName, true); break;

            case States_EDITOR_LOAD : EditorSession(&puzzleName, false); break;

            case States_QUIT : quit = true; break;
        }
    }

    // Libération de la SDL
    CleanSDL();

    return 0;
}
