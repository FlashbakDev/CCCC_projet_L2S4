#include "files.h"

// =========================================================

int Save_grid(Grid *pGrid, char* name){

    fprintf(stdout , "files.c : Save_grid(Grid *pGrid, char* name = %s) -> start\n", name);

    char filename[UI_MAX_LENGTH];
    sprintf(filename,"data/puzzles/%s.puz",name);

    FILE *f = fopen(filename, "w");
    if (f == NULL){

        printf("files.c : Save_grid(Grid *pGrid) -> Error opening file!\n");
        return -1;
    }

    fprintf(f, "%d %d\n", pGrid->width, pGrid->height);

    /* stats de la grille */
    fprintf(f, "%d %d\n", pGrid->nbMove, pGrid->nbColor);

    for(int i=0; i< pGrid->height; i++){
        for(int j=0;j < pGrid->width; j++){

            fprintf(f,"%d %d    ", pGrid->tokens[i][j].type, pGrid->tokens[i][j].color);
        }

        fprintf(f,"\n");
    }

    fclose(f);

    fprintf(stdout , "files.c : Save_grid(Grid *pGrid, char* name = %s) -> end\n", name);

    return 0;
}

// =========================================================

Grid *Load_grid(char* name){

    fprintf(stdout , "files.c : Load_grid(char* name = %s) -> start\n", name);

    char filename[UI_MAX_LENGTH];
    sprintf(filename,"data/puzzles/%s.puz",name);

    FILE *f = fopen(filename, "r");
    if (f == NULL){

        printf("files.c : Load_grid(char* name = %s, Grid *pGrid) -> Error opening file!\n");
        return NULL;
    }

    int width;
    int height;

    /* nom et dimmension */
    fscanf(f, "%d %d", &width, &height);

    Grid *pGrid = NewEmptyPuzzle(width,height);

    /* stats de la grille */
    fscanf(f, "%d %d", &pGrid->nbMove, &pGrid->nbColor);

    //fprintf(stdout , "files.c : Load_grid(char* name = %s) -> pGrid->name = %s\n", pGrid->name);

    for(int i=0; i< pGrid->height; i++){
        for(int j=0;j < pGrid->width; j++){

            fscanf(f,"%d", &pGrid->tokens[i][j].type);
            fscanf(f,"%d", &pGrid->tokens[i][j].color);
        }
    }

    fclose(f);

    fprintf(stdout , "files.c : Load_grid(char* name = %s) -> end\n", name);

    return pGrid;
}

// =========================================================
