/*

files.h
-----------

Par Desbouchages Benjamin, Rousseau jérémy, pour le projet CCCC le 24/02/2017.

Rôle : fonctions pour sauvegarder / charger des parties ou des fichiers.

*/

// =========================================================

#ifndef FILES_H_INCLUDED
#define FILES_H_INCLUDED

// =========================================================

#include "core.h"
#include "constants.h"

#ifndef RESDIR
#define RESDIR ""
#endif

#include <sys/types.h>
#ifdef _MSC_VER
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

// =========================================================

#ifdef _MSC_VER
#ifdef _WIN32
typedef struct _stat32 Files_stat;
#else
typedef struct _stat64 Files_stat;
#endif

typedef struct Files_dirent {
	char *d_name;
} Files_dirent;

typedef struct Files_dir {
	ptrdiff_t fhandle;
	struct _finddata_t fdata;
	Files_dirent ent;
	char name[UI_MAX_LENGTH];
} Files_dir;
#else
typedef struct stat Files_stat;
typedef struct dirent Files_dirent;
typedef DIR Files_dir;
#endif

// =========================================================

int Save_grid(Grid *pGrid, char* name);

Grid *Load_grid(char* name);

const char *get_filename_ext(const char *filename);

// =========================================================
// source : https://github.com/actsl/kiss_sdl/blob/master/kiss_posix.c

char *Files_getcwd(char *buf, int size);

int Files_getstat(char *pathName, Files_stat *buf);

Files_dir *Files_opendir(char *name);

Files_dirent *Files_readdir(Files_dir *dirp);

int Files_closedir(Files_dir *dirp);

int Files_isdir(Files_stat s);

int Files_isreg(Files_stat s);

// =========================================================

#endif // FILES_H_INCLUDED
