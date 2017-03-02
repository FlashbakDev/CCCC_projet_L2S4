#include "constants.h"
#include "game.h"

int main(int argc, char* argv[]){

    /* Initialisation */

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_Window* pWindow = NULL;
    pWindow = SDL_CreateWindow("Candy Crush Clone C",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,WINDOW_WIDTH,WINDOW_HEIGHT,SDL_WINDOW_SHOWN);
    // void SDL_SetWindowIcon(SDL_Window*  window , SDL_Surface* icon); //ajoute une icône à la fenêtre

    /* initialisation du jeu */
    int gridHeight = 10;
    int gridWidth = 10;
    int nbMove = 20;
    int nbColor = 5;
    int score = 0;

    Grid *grid1 = NewGrid(gridWidth,gridHeight,nbMove,nbColor);
    SDL_SetWindowSize(pWindow,grid1->width * TOKEN_WIDTH, grid1->height * TOKEN_HEIGHT);

    // renderer = canvas ( endroit où l'on va déssiner )
    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 0 ); // pour faire déssiner en blanc

    // curseur
    SDL_Surface *pSurface_Cursor = IMG_Load("./data/MouseOver_blue.png");
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

    // texte, source -> http://gigi.nullneuron.net/gigilabs/displaying-text-in-sdl2-with-sdl_ttf/
    TTF_Font *pFont = TTF_OpenFont("data/fonts/arial.ttf", 25);
    SDL_Color color_texte = { 0, 0, 0 };
    SDL_Surface *pSurface_texte = TTF_RenderText_Solid(pFont, "Score : 0", color_texte);

    // gestionnaire d'évènement
    SDL_Event event;

    /* boucle de jeu */
    bool loop = true;
    while( loop ){

        int frameStart = SDL_GetTicks();

        /* évènements */
        while( SDL_PollEvent( &event ) != 0 ){

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

                            if ( IsTokenMoving(grid1) == false && IsTokenDestructing(grid1) == false ){

                                dragStart.x = (event.motion.x / TOKEN_WIDTH);
                                dragStart.y = (event.motion.y / TOKEN_HEIGHT);

                                dragAndDrop = grid1->tokens[dragStart.y][dragStart.x].type != NONE;
                            }
                        }
                        break;

                        // click droit pour afficher les stats du jeton
                        case SDL_BUTTON_RIGHT:{

                            int posX = (event.motion.x / TOKEN_WIDTH);
                            int posY = (event.motion.y / TOKEN_HEIGHT);

                            fprintf(stdout,"Jeton en posisition (%d,%d) : ",posX, posY);
                            DebugToken(grid1->tokens[posY][posX]);
                            fprintf(stdout,"\n");
                        }
                        break;
                    }
                }
                break;

                case SDL_MOUSEBUTTONUP:{

                    switch(event.button.button){

                        case SDL_BUTTON_LEFT:{

                            if ( dragAndDrop == true ){

                                dragEnd.x = (event.motion.x / TOKEN_WIDTH);
                                dragEnd.y = (event.motion.y / TOKEN_HEIGHT);

                                fprintf(stdout,"Distance du drag : %d, %d.\n",dragEnd.x - dragStart.x, dragEnd.y - dragStart.y);

                                int distX = sqrt( pow( dragEnd.x - dragStart.x, 2) );
                                int distY = sqrt( pow( dragEnd.y - dragStart.y, 2) );

                                if ( ( distX == 1 && distY == 0 ) || ( distX == 0 && distY == 1 ) ){

                                    HardPermuteToken(grid1, dragStart.x, dragStart.y, dragEnd.x, dragEnd.y);

                                    if ( IsLigneOnGrid(grid1) == false ){

                                        HardPermuteToken(grid1, dragEnd.x, dragEnd.y, dragStart.x, dragStart.y);
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

                    position_CursorOver.x = TOKEN_WIDTH * (event.motion.x / TOKEN_WIDTH);
                    position_CursorOver.y = TOKEN_HEIGHT * (event.motion.y / TOKEN_HEIGHT);

                    if ( dragAndDrop == true ){

                        int distX = sqrt( pow( position_CursorOver.x - dragStart.x, 2) );
                        int distY = sqrt( pow( position_CursorOver.y - dragStart.y, 2) );

                        if ( ( distX == 1 && distY == 0 ) || ( distX == 0 && distY == 1 ) ){

                            /*Colors tmp = grid1->tokens[dragStart.y][dragStart.x].color;
                            grid1->tokens[dragStart.y][dragStart.x].color = grid1->tokens[position_CursorOver.y][position_CursorOver.x].color;
                            grid1->tokens[position_CursorOver.y][position_CursorOver.x].color = tmp;*/
                        }
                    }
                }
                break;
            }
        }

        /* logique */
        if ( IsTokenMoving(grid1) == false && IsTokenDestructing(grid1) == false){

            if( IsLigneOnGrid(grid1) == true ){

                // score
                score ++;

                /* libération de l'encien texte et déclaration du nouveau */
                SDL_FreeSurface(pSurface_texte);
                char tmp[] = "";
                sprintf(tmp,"Score : %d",score);
                pSurface_texte = TTF_RenderText_Solid(pFont,tmp,color_texte);

                // détruit les lignes et remplie les cases manquantes du tableau
                fprintf(stdout,"Nombre de jeton detruit(s) : %d\n", DestroyAlignedTokens(grid1) );
            }
            else {

                while( IsTokenOfType(grid1, NONE ) == true ){

                    // regroupe tout les jetons
                    RegroupTokens(grid1, DOWN);

                    // remplie les espaces vides
                    InjectLigne(grid1, UP);
                }
            }

            //fprintf(stdout,"Grille remplie !");
        }

        /* animations - textes */
        if ( IsTokenDestructing(grid1) == false )
            AnimMovingTokens(grid1);
        else
            AnimDestructingTokens(grid1);

        /* affichage */
        SDL_RenderClear(pRenderer);                                                             // efface tout le contenu du renderer

        SDL_RenderCopy(pRenderer,pTexture_CursorOver,NULL,&position_CursorOver);                // pour le curseur de la souris
        DrawGrid(grid1,pRenderer,pSurface_Token);                                               // déssine la grille sur le renderer

        SDL_Texture *pTexture_texte = SDL_CreateTextureFromSurface(pRenderer, pSurface_texte);  // crée une texture avec le texte
        SDL_Rect rect_texte = {10,10,0,0};
        SDL_QueryTexture(pTexture_texte,NULL,NULL,&rect_texte.w,&rect_texte.h);
        SDL_RenderCopy(pRenderer, pTexture_texte, NULL, &rect_texte);                           // déssine le texte sur le renderer

        SDL_RenderPresent(pRenderer);                                                           // déssine le renderer à l'écran

        /* gestion de la fréquence d'affichage ( pour les animations )*/
        int frameEnd = SDL_GetTicks();
        if ( frameEnd - frameStart < 1000 / FRAME_PER_SECOND ){

            SDL_Delay( (1000 / FRAME_PER_SECOND) - (frameEnd - frameStart) );
        }
    }

    /* fin du programme */

    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    TTF_CloseFont(pFont);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
