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

#ifndef SDL_H_
#define SDL_H_

// bibliotecas
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

// definiciones
#define TITLE "ShootMe"
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define SCREEN_BPP 32
#define FPS 100
#define FONT_NAME "data/font/FreeMono.ttf"
#define FONT_SIZE 18
#define FONT_STYLE TTF_STYLE_BOLD

// estructuras

// prototipos
void sdl_init (SDL_Surface **screen);
TTF_Font *ttf_load (char *name, short int size, Uint8 style);

#endif 
