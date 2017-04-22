/*

files.h
-----------

Par Benjamin, pour le projet CCCC le 24/02/2017.

Rôle : fonctions pour sauvegarder / charger des parties ou des fichiers.

*/

// =========================================================

#ifndef FILES_H_INCLUDED
#define FILES_H_INCLUDED

// =========================================================

#include "core.h"
#include "constants.h"

// =========================================================

int Save_grid(Grid *pGrid, char* name);

Grid *Load_grid(char* name);

// =========================================================

#endif // FILES_H_INCLUDED
