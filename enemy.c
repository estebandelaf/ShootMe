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

#include "enemy.h"

Enemy *enemy_from_char (char *string) {
	// variables para el enemigo
	Enemy *enemy = malloc(sizeof(Enemy));
	bzero(enemy, sizeof(Enemy));
	// obtener id desde el string
	enemy->id = atoi(strtok(string, ";"));
	// obtener caracter del enemigo
	char *character = strtok(NULL, ";");
	enemy->character = malloc(strlen(character));
	strcpy(enemy->character, character);
// 	free(character);
	// obtener nombre del enemigo
	char *name = strtok(NULL, ";");
	enemy->name = malloc(strlen(name));
	strcpy(enemy->name, name);
// 	free(name);
	// obtener coordenadas
	enemy->y = atoi(strtok(NULL, ";"));
	enemy->x = atoi(strtok(NULL, ";"));
	// obtener orientacion
	enemy->direction = 'd';
	// retornar enemigo
	return enemy;
}

void enemies_draw (SDL_Surface *screen, char *enemies) {
	// obtener listado de enemigos a partir del string
	char *ptr = strtok(enemies, "|"); // obtener enemigo 1
	// si existe un enemigo se procesa
	if(ptr!=NULL) {
		// dibujar enemigo 1
		Enemy *enemy = enemy_from_char(ptr);
		enemy_draw(screen, enemy);
		// si hay mas enemigos se procesan
		/*while((ptr=strtok(NULL, "|"))!=NULL) {
			// dibujar otros enemigos
			enemy_draw(screen, enemy_from_char(ptr)); // obtener otros enemigos
		}*/
	}
}

void enemy_draw (SDL_Surface *screen, Enemy *enemy) {
	// definir imagen a mostrar
	/*if(enemy->direction=='u') {
		(*enemy).src.y = 96;
		(*enemy).src.x = 32;
// 		(*enemy)->src.x = ((*player)->src.x+32) % 96;
	}
	else if(enemy->direction=='d') {
		(*enemy).src.y = 0;
		(*enemy).src.x = 32;
	}
	else if(enemy->direction=='l') {
		(*enemy).src.y = 32;
		(*enemy).src.x = 32;
	}
	else if(enemy->direction=='r') {
		(*enemy).src.y = 64;
		(*enemy).src.x = 32;
	}
	(*enemy).src.w = 32;
	(*enemy).src.h = 32;*/
// 	enemy->src = (SDL_Rect) { 32, 0, 32, 32 }; // x, y, w, h
	// dibujar enemigo
	enemy->sprite = (SDL_Rect) { enemy->x, enemy->y, 32, 32 }; // x, y, w, h
// 	SDL_BlitSurface(enemy->images[ENEMY_IMAGE_SPRITE], &enemy->src, screen, &enemy->sprite);
	SDL_FillRect(screen, &enemy->sprite, SDL_MapRGBA(screen->format,255,0,255,0));
}
