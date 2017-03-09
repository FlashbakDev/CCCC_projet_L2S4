#include "core.h"

// =========================================================
SDL_Window *InitSDL(char * windowName){

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    return SDL_CreateWindow(windowName,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,WINDOW_WIDTH,WINDOW_HEIGHT,SDL_WINDOW_SHOWN);
    // void SDL_SetWindowIcon(SDL_Window*  window , SDL_Surface* icon); //ajoute une icône à la fenêtre
}

// =========================================================

void DrawGrid(Grid *pGrid, SDL_Renderer *pRenderer, SDL_Surface *pSurface_Token[]){

    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].type != NONE || pGrid->tokens[i][j].isDestruct ){

                SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer,pSurface_Token[pGrid->tokens[i][j].color]);

                SDL_RenderCopy(pRenderer,pTexture,NULL,&pGrid->tokens[i][j].rect_texture);

                SDL_DestroyTexture(pTexture);
            }
        }
    }
}

// =========================================================

void HardPermuteToken(Grid *pGrid,int x1,int y1,int x2,int y2){

    Token tmp = pGrid->tokens[y1][x1];
    pGrid->tokens[y1][x1] = pGrid->tokens[y2][x2];
    pGrid->tokens[y2][x2] = tmp;

    CalculTokenRectTexure( &pGrid->tokens[y1][x1], x1, y1 );
    CalculTokenRectTexure( &pGrid->tokens[y2][x2], x2, y2 );
}

// =========================================================

void AnimMovingTokens(Grid *pGrid){

    // anime les mouvement
    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].rect_texture.x != ((j * TOKEN_WIDTH) + (TOKEN_WIDTH/2)) - (pGrid->tokens[i][j].rect_texture.w / 2) ){

                pGrid->tokens[i][j].isMoving = true;

                if ( pGrid->tokens[i][j].rect_texture.x > ((j * TOKEN_WIDTH) + (TOKEN_WIDTH/2)) - (pGrid->tokens[i][j].rect_texture.w / 2) )
                    pGrid->tokens[i][j].rect_texture.x -= FALL_SPEED ;
                else
                    pGrid->tokens[i][j].rect_texture.x += FALL_SPEED ;

                if ( sqrt( pow( pGrid->tokens[i][j].rect_texture.x - ( ((j * TOKEN_WIDTH) + (TOKEN_WIDTH/2)) - (pGrid->tokens[i][j].rect_texture.w / 2)) ,2) ) < FALL_SPEED ){

                    pGrid->tokens[i][j].rect_texture.x = ((j * TOKEN_WIDTH) + (TOKEN_WIDTH/2)) - (pGrid->tokens[i][j].rect_texture.w / 2);
                    pGrid->tokens[i][j].isMoving = false;
                }
            }

            if ( pGrid->tokens[i][j].rect_texture.y != (((i * TOKEN_HEIGHT) + (TOKEN_HEIGHT/2)) - (pGrid->tokens[i][j].rect_texture.h / 2)) ){

                pGrid->tokens[i][j].isMoving = true;

                if ( pGrid->tokens[i][j].rect_texture.y > (((i * TOKEN_HEIGHT) + (TOKEN_HEIGHT/2)) - (pGrid->tokens[i][j].rect_texture.h / 2)) )
                    pGrid->tokens[i][j].rect_texture.y -= FALL_SPEED ;
                else
                    pGrid->tokens[i][j].rect_texture.y += FALL_SPEED ;

                if ( sqrt( pow( pGrid->tokens[i][j].rect_texture.y - (((i * TOKEN_HEIGHT) + (TOKEN_HEIGHT/2)) - (pGrid->tokens[i][j].rect_texture.h / 2)) ,2) ) < FALL_SPEED ){

                    pGrid->tokens[i][j].rect_texture.y = (((i * TOKEN_HEIGHT) + (TOKEN_HEIGHT/2)) - (pGrid->tokens[i][j].rect_texture.h / 2));
                    pGrid->tokens[i][j].isMoving = false;
                }
            }
        }
    }
}

// =========================================================

void AnimDestructingTokens(Grid *pGrid){

    // anime les destructions
    for(int i = 0; i < pGrid->height; i++){
        for(int j = 0; j < pGrid->width; j++){

            if ( pGrid->tokens[i][j].isMoving == false && pGrid->tokens[i][j].isDestruct == true ){

                if ( pGrid->tokens[i][j].startDestructAnim == -1 ){

                    pGrid->tokens[i][j].startDestructAnim = SDL_GetTicks();

                    //fprintf(stdout, "animation de destruction commence.\n");
                }

                // animationd de destruction
                if (  SDL_GetTicks() - pGrid->tokens[i][j].startDestructAnim < DESTRUCT_SPEED ){

                    //fprintf(stdout, "nombre de tick ecoule : %d / %d, textureSize = %d.\n", SDL_GetTicks() - pGrid->tokens[i][j].startDestructAnim, DESTRUCT_SPEED, pGrid->tokens[i][j].textureSize);

                    // perte en une frame
                    pGrid->tokens[i][j].textureSize = 100 - ( 100.0 / DESTRUCT_SPEED * ( SDL_GetTicks() - pGrid->tokens[i][j].startDestructAnim ) ) ;

                    CalculTokenRectTexure(&pGrid->tokens[i][j], j, i);
                }
                else {

                    pGrid->tokens[i][j].isDestruct = false;
                    pGrid->tokens[i][j].startDestructAnim = -1;

                    //fprintf(stdout, "animation de destruction termine a %d/100 de la texture.\n", pGrid->tokens[i][j].textureSize );
                }
            }
        }
    }
}

// =========================================================

void DebugToken(Token token){

    fprintf(stdout,
            "\n- color = %d\n- type = %d\n- aligne = %d\n- isMoving = %d\n- isDestruct = %d\n- textureSize = %d\n- rect_texture w = %d, h = %d, x = %d, y = %d",
            token.color, token.type,token.aligned,token.isMoving, token.isDestruct, token.textureSize, token.rect_texture.w, token.rect_texture.h, token.rect_texture.x, token.rect_texture.y );
}

// =========================================================

void CalculTokenRectTexure(Token *token, int x, int y){

    token->rect_texture.w = (float)(TOKEN_WIDTH / 100.0 * token->textureSize);
    token->rect_texture.h = (float)(TOKEN_HEIGHT / 100.0 * token->textureSize);
    token->rect_texture.x = ((x * TOKEN_WIDTH) + (TOKEN_WIDTH/2)) - (token->rect_texture.w / 2);
    token->rect_texture.y = ((y * TOKEN_HEIGHT) + (TOKEN_HEIGHT/2)) - (token->rect_texture.h / 2);
}
