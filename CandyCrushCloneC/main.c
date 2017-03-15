#include "UI.h"
#include "constants.h"
#include "game.h"
#include "menu.h"
#include "editor.h"

int main(int argc, char* argv[]){

    /* Initialisation */
    fprintf(stdout,"Initialisation begin : \n");

    SDL_Renderer *pRenderer;    // renderer = canvas ( endroit où l'on va déssiner )
    SDL_Event event;            // gestionnaire d'évènement
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

    Grid *grid1 = NewGrid(gridWidth,gridHeight,nbMove,nbColor);

    // création des zone de jeu et d'affichage
    SDL_Rect rect_UI = { grid1->rect.x + grid1->rect.w, 0, 250, grid1->rect.h };
    SDL_Rect rect_screen = {0 ,
                            0 ,
                            (grid1->rect.w + grid1->rect.x > rect_UI.w + rect_UI.x ) ? grid1->rect.w + grid1->rect.x : rect_UI.w + rect_UI.x ,
                            (grid1->rect.h + grid1->rect.y > rect_UI.h + rect_UI.y ) ? grid1->rect.h + grid1->rect.y : rect_UI.h + rect_UI.y } ;

    Array_new(&objects);

    pRenderer = InitGame("Candy Crush Clone C", &objects, rect_screen.w, rect_screen.h );
    if ( !pRenderer )
        return 1;

    fprintf(stdout,"debug\n");

    fprintf(stdout,"Window_new return %d.\n", Window_new(&window, NULL, false, 0, 0, screen_width, screen_height));
    fprintf(stdout,"UI_label_new return %d.\n", UI_label_new(&label_score, &window, "Test", rect_UI.x + 20 , rect_UI.y + 20 ));
    fprintf(stdout,"UI_label_new return %d.\n", UI_label_new(&label_nbMove, &window, "Test", rect_UI.x + 40 , rect_UI.y + 40 ));
    fprintf(stdout,"UI_label_new return %d.\n", UI_label_new(&label_mouvements, &window, "Test", rect_UI.x + 60 , rect_UI.y + 60 ));

    sprintf(label_score.text,"Score : %d ",0);
    sprintf(label_nbMove.text,"NbCoups : %d", nbMove);
    sprintf(label_mouvements.text,"Mouvement possible : %d",0);

    fprintf(stdout,"UI_button_new return %d.\n", UI_button_new(&button_quit, &window, "Quitter", rect_UI.x + ( rect_UI.w / 2 ) - image_normal.w / 2 , rect_UI.h - 50 ));
    window.visible = true;

    fprintf(stdout,"Initialisation end.\n");

    // curseur
    SDL_Surface *pSurface_Cursor = IMG_Load("./data/MouseOver_blue.png");
    SDL_Texture *pTexture_CursorOver = SDL_CreateTextureFromSurface(pRenderer,pSurface_Cursor);
    SDL_QueryTexture(pTexture_CursorOver, NULL, NULL, &rect_CursorOver.w, &rect_CursorOver.h);

    // textures
    SDL_Surface *pSurface_Token[6];
    pSurface_Token[0] = IMG_Load("data/Tokens/Token_red.png");
    pSurface_Token[1] = IMG_Load("data/Tokens/Token_blue.png");
    pSurface_Token[2] = IMG_Load("data/Tokens/Token_green.png");
    pSurface_Token[3] = IMG_Load("data/Tokens/Token_yellow.png");
    pSurface_Token[4] = IMG_Load("data/Tokens/Token_purple.png");
    pSurface_Token[5] = IMG_Load("data/Tokens/Token_orange.png");

    /* boucle de jeu */

    int score = 0;
    bool draw = true; // placebo
    bool quit = false;

    while( !quit ){

        int frameStart = SDL_GetTicks();

        /* évènements */
        while( SDL_PollEvent( &event ) != 0 ){

            if( event.type == SDL_QUIT){

                quit = true;
            }

            if ( event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE ){

                fprintf(stdout,"Appuie sur echap, fin de la boucle de jeu");
            }
            else{
                  
              switch(event.key.keysym.sym){

                    case SDLK_KP_1 :
                        nbMove +=3;
                        break;


                      case SDLK_KP_2 :
                          if(grid1->tokens[cursorTokenPosition.y][cursorTokenPosition.x].color < nbColor-1)
                          grid1->tokens[cursorTokenPosition.y][cursorTokenPosition.x].color ++;
                          else grid1->tokens[cursorTokenPosition.y][cursorTokenPosition.x].color = 0;

                      break;

                      case SDLK_KP_3 :
                          if(grid1->tokens[cursorTokenPosition.y][cursorTokenPosition.x].color >0)
                          grid1->tokens[cursorTokenPosition.y][cursorTokenPosition.x].color --;
                          else grid1->tokens[cursorTokenPosition.y][cursorTokenPosition.x].color = nbColor-1;


                      break;

                      case SDLK_KP_4 :
                          printf("deplacement possible %d",MoveAvailable(grid1));
                          break;

                      case SDLK_KP_5 :
                          RandomizeGrid(grid1);
                          break;

              }
            }

            // entré lié au jeu
            GameEvent(grid1, &event, &quit);

            // event UI
            Window_event(&window, &event, &draw );
            Button_quit_event(&button_quit, &event, &draw, &quit);
        }

        /* logique */
        sprintf(label_nbMove.text," NbCoups : %d", nbMove);
        sprintf(label_score.text,"Score : %d ",score);
        if ( IsTokenMoving(grid1) == false && IsTokenDestructing(grid1) == false){

            if( IsLigneOnGrid(grid1) == true ){

                // score
                score += Calc_Score(grid1);




                // détruit les lignes et remplie les cases manquantes du tableau
                fprintf(stdout,"Nombre de jeton detruit(s) : %d\n", DestroyAlignedTokens(grid1) );
            }
            else {

                if(IsTokenOfType(grid1, NONE ) == true ){
                while( IsTokenOfType(grid1, NONE ) == true ){

                    // regroupe tout les jetons
                    RegroupTokens(grid1, DOWN);

                    // remplie les espaces vides
                    InjectLigne(grid1, UP);
                }
                }else {
                    sprintf(label_mouvements.text,"Nombre de mouvement : %d",MoveAvailable(grid1));

                 if(MoveAvailable(grid1) == 0)
                 {
                    RandomizeGrid(grid1);
                 }

                }

            }

            //fprintf(stdout,"Grille remplie !\n");
        }

        /* animations - textes */
        if ( IsTokenDestructing(grid1) == false )
            AnimMovingTokens(grid1);
        else
            AnimDestructingTokens(grid1);

        /* affichage */
        SDL_RenderClear(pRenderer);                                                             // efface tout le contenu du renderer

        Window_draw(&window, pRenderer);

        if ( grid1->is_cursorOnGrid == true )
            SDL_RenderCopy(pRenderer,pTexture_CursorOver,NULL,&rect_CursorOver);            // pour le curseur de la souris

        DrawGrid(grid1,pRenderer,pSurface_Token);                                               // déssine la grille sur le renderer

        UI_label_draw(&label_score,pRenderer);
        UI_label_draw(&label_nbMove,pRenderer);
        UI_label_draw(&label_mouvements,pRenderer);
        UI_button_draw(&button_quit, pRenderer);

        SDL_RenderPresent(pRenderer);                                                           // déssine le renderer à l'écran

        /* gestion de la fréquence d'affichage ( pour les animations )*/
        WaitForNextFrame(frameStart);
    }

    printf("\n\n Score Total : %d", score);

    /* fin du programme */
    CleanGame(&objects);

    return 0;
}
