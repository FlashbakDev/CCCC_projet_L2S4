#include "constants.h"
#include "game.h"

int main(int argc, char* argv[]){

    /* Initialisation */

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* pWindow = NULL;
    pWindow = SDL_CreateWindow("Candy Crush Clone C",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,WINDOW_WIDTH,WINDOW_HEIGHT,SDL_WINDOW_SHOWN);
    // void SDL_SetWindowIcon(SDL_Window*  window , SDL_Surface* icon); //ajoute une icône à la fenêtre

    /* initialisation du jeu */
    int gridHeight = 10;
    int gridWidth = 10;
    int nbMove = 20;
    int nbColor = 5;

    Grid *grid1 = NewGrid(gridWidth,gridHeight,nbMove,nbColor);
    SDL_SetWindowSize(pWindow,grid1->width * TOKEN_WIDTH, grid1->height * TOKEN_HEIGHT);

    // renderer = canvas ( endroit où l'on va déssiner )
    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 0 ); // pour faire déssiner en blanc

    // curseur
    SDL_Surface *pSurface_Cursor = IMG_Load("./data/MouseOver.png");
    SDL_Texture *pTexture_CursorOver = SDL_CreateTextureFromSurface(pRenderer,pSurface_Cursor);
    SDL_Rect position_CursorOver;
    SDL_QueryTexture(pTexture_CursorOver, NULL, NULL, &position_CursorOver.w, &position_CursorOver.h);

    SDL_Point dragStart, dragEnd;
    bool dragAndDrop = false;

    // textures
    SDL_Surface *pSurface_Token[5];
    pSurface_Token[0] = IMG_Load("./data/Token_red.png");
    pSurface_Token[1] = IMG_Load("./data/Token_blue.png");
    pSurface_Token[2] = IMG_Load("./data/Token_green.png");
    pSurface_Token[3] = IMG_Load("./data/Token_yellow.png");
    pSurface_Token[4] = IMG_Load("./data/Token_purple.png");

    // gestionnaire d'évènement
    SDL_Event event;

    /* boucle de jeu */
    bool loop = true;
    while( loop ){

        /* évènements */
        SDL_WaitEvent(&event);
        switch(event.type){

            // appuie d'une touche du clavier
            case SDL_KEYDOWN:{

                switch(event.key.keysym.sym){

                    // appuie sur echap
                    case SDLK_ESCAPE: {

                        fprintf(stdout,"Appuie sur echap, fin de la boucle de jeu");
                        loop = false;
                    }
                    break;
                }
            }
            break;

            // bouttons de souris appuié
            case SDL_MOUSEBUTTONDOWN:{

                switch(event.button.button){

                    // bouton gauche
                    case SDL_BUTTON_LEFT:{

                        //fprintf(stdout,"Bouton de la souris gauche appuiye.\n");

                        dragStart.x = (event.motion.x / TOKEN_WIDTH);
                        dragStart.y = (event.motion.y / TOKEN_HEIGHT);

                        dragAndDrop = grid1->tokens[dragStart.x][dragStart.y].type != NONE;
                    }
                    break;

                    // click droit pour afficher les stats du jeton
                    case SDL_BUTTON_RIGHT:{

                        int posX = (event.motion.x / TOKEN_WIDTH);
                        int posY = (event.motion.y / TOKEN_HEIGHT);
                        fprintf(stdout,"Jeton en posisition (%d,%d) : Couleur = %d, type = %d, aligne = %d.\n",posX, posY, grid1->tokens[posX][posY].color, grid1->tokens[posX][posY].type,grid1->tokens[posX][posY].aligned);
                    }
                    break;
                }
            }
            break;

            case SDL_MOUSEBUTTONUP:{

                switch(event.button.button){

                    case SDL_BUTTON_LEFT:{

                        //fprintf(stdout,"Bouton de la souris gauche relache.\n");

                        if ( dragAndDrop == true ){

                            dragEnd.x = (event.motion.x / TOKEN_WIDTH);
                            dragEnd.y = (event.motion.y / TOKEN_HEIGHT);

                            fprintf(stdout,"Distance du drag : %d, %d.\n",dragEnd.x - dragStart.x, dragEnd.y - dragStart.y);

                            int distX = sqrt( pow( dragEnd.x - dragStart.x, 2) );
                            int distY = sqrt( pow( dragEnd.y - dragStart.y, 2) );

                            if ( ( distX == 1 && distY == 0 ) || ( distX == 0 && distY == 1 ) ){

                                 Colors colorTmp = grid1->tokens[dragEnd.x][dragEnd.y].color;

                                //fprintf(stdout,"changement de couleur du jeton d'arrive ( %d, %d, %d", dragEnd.x, dragEnd.y, grid1->tokens[dragEnd.y][dragEnd.x].color );
                                grid1->tokens[dragEnd.x][dragEnd.y].color = grid1->tokens[dragStart.x][dragStart.y].color;
                                //fprintf(stdout," -> %d)\n", grid1->tokens[dragEnd.x][dragEnd.y].color );

                                grid1->tokens[dragStart.x][dragStart.y].color = colorTmp;

                                if ( IsLigneOnGrid(grid1) == false ){

                                    grid1->tokens[dragStart.x][dragStart.y].color = grid1->tokens[dragEnd.x][dragEnd.y].color;
                                    grid1->tokens[dragEnd.x][dragEnd.y].color = colorTmp;
                                }
                                else{

                                    while( IsLigneOnGrid(grid1) == true){

                                        // détruit les lignes et remplie les cases manquantes du tableau
                                        fprintf(stdout,"Nombre de jeton detruit(s) : %d\n", DestroyAlignedTokens(grid1) );

                                        // regroupe tout les jetons
                                        RegroupTokens(grid1, DOWN);

                                        while( IsTokenOfType(grid1, NONE ) == true ){

                                            // remplie les espaces vides
                                            InjectLigne(grid1, UP);

                                            // regroupe tout les jetons
                                            RegroupTokens(grid1, DOWN);
                                        }
                                    }
                                }
                            }

                            dragAndDrop = false;
                        }
                    }
                    break;
                }
            }
            break;

            // mouvement de souris
            case SDL_MOUSEMOTION:{

                //fprintf(stdout,"Coordonnees de la souris : ( %d , %d )\n", event.motion.x, event.motion.y);

                position_CursorOver.x = TOKEN_WIDTH * (event.motion.x / TOKEN_WIDTH);
                position_CursorOver.y = TOKEN_HEIGHT * (event.motion.y / TOKEN_HEIGHT);
            }
            break;
        }

        /* logique */

        /* affichage */
        SDL_RenderClear(pRenderer);                                                 // efface tout le contenu du renderer

        SDL_RenderCopy(pRenderer,pTexture_CursorOver,NULL,&position_CursorOver);    // pour le curseur de la souris
        DrawGrid(grid1,pRenderer,pSurface_Token);                                   // déssine la grille sur le renderer

        SDL_RenderPresent(pRenderer);                                               // déssine le renderer à l'écran
    }

    /* fin du programme */

    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();

    return 0;
}
