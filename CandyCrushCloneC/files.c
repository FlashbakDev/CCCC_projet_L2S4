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

    if ( strcmp( get_filename_ext(name), "puz") == 0 )
        sprintf(filename,"data/puzzles/%s",name);
    else
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

const char *get_filename_ext(const char *filename) {

    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

// =========================================================

char *Files_getcwd(char *buf, int size){

#ifdef _MSC_VER
	return _getcwd(buf, size);
#else
	return getcwd(buf, size);
#endif

}

int Files_chdir(char *path){

#ifdef _MSC_VER
	return _chdir(path);
#else
	return chdir(path);
#endif

}

int Files_getstat(char *pathName, Files_stat *buf){

#ifdef _MSC_VER
#ifdef _WIN32
	return _stat32(pathName, buf);
#else
	return _stat64(pathName, buf);
#endif
#endif
#ifndef _MSC_VER
	return stat(pathName, buf);
#endif

}

Files_dir *Files_opendir(char *name){

#ifdef _MSC_VER
	Files_dir *dir;

	if (!name || strlen(name) > UI_MAX_LENGTH - 2) return NULL;
	dir = malloc(sizeof(Files_dir));
	dir->ent.d_name = NULL;
	strcpy(dir->name, name);
	dir->fhandle = (ptrdiff_t) _findfirst(dir->name, &dir->fdata);
	if ((int) dir->fhandle == -1) {
		free (dir);
		return NULL;
	}
	return dir;
#else
	return opendir(name);
#endif

}

Files_dirent *Files_readdir(Files_dir *dirp){

#ifdef _MSC_VER
	if(dirp->ent.d_name &&
		(int) _findnext(dirp->fhandle, &dirp->fdata) == -1)
		return NULL;
	dirp->ent.d_name = dirp->fdata.name;
	return &dirp->ent;
#else
	return readdir(dirp);
#endif

}

int Files_closedir(Files_dir *dirp){

#ifdef _MSC_VER
	int n;

	if (!dirp || dirp->fhandle == -1) return -1;
	n = (int) _findclose(dirp->fhandle);
	free(dirp);
	return n;
#else
	return closedir(dirp);
#endif

}

int Files_isdir(Files_stat s){

#ifdef _MSC_VER
	return s.st_mode & _S_IFDIR;
#else
	return S_ISDIR(s.st_mode);
#endif

}

int Files_isreg(Files_stat s){

#ifdef _MSC_VER
	return s.st_mode & _S_IFREG;
#else
	return S_ISREG(s.st_mode);
#endif

}
