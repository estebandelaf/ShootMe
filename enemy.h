/**
 * ShootMe
 * Copyright (C) 2015  Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ENEMY_H_
#define ENEMY_H_

// bibliotecas
#include "sdl.h"

// definiciones
#define ENEMY_IMAGES 2
#define ENEMY_IMAGE_SPRITE 0
#define ENEMY_IMAGE_DEAD 1

// estructuras
typedef struct {
	int id;
	char *character;
	char *name;
	int y;
	int x;
	char direction;
	SDL_Rect sprite;
	SDL_Rect src;
	SDL_Surface *images[ENEMY_IMAGES];
} Enemy; 

// prototipos
Enemy *enemy_from_char (char *string);
void enemies_draw (SDL_Surface *screen, char *enemies);
void enemy_draw (SDL_Surface *screen, Enemy *enemy);

#endif 
