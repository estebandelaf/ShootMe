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

#ifndef SHOTME_H_
#define SHOTME_H_

// bibliotecas
#include <stdio.h>
#include <stdlib.h>

#include "conf.h"
#include "net.h"
#include "sdl.h"
#include "enemy.h"

// definiciones
#define SPRITE_SIZE 32
#define PLAYER_IMAGES 2
#define PLAYER_IMAGE_SPRITE 0
#define PLAYER_IMAGE_DEAD 1
#define PLAYER_SOUNDS 3
#define PLAYER_SOUND_DAMAGED 0
#define PLAYER_SOUND_HEALTH 1
#define PLAYER_SOUND_WALKING 2
#define WEAPON_PREV 11
#define WEAPON_NEXT 12
#define OBJECT_OFFSET 33

#define WEAPONS_FILE "data/weapons.conf"
#define MAPOBJECTS_FILE "data/mapobjects.conf"

// estructuras
typedef struct {
	Uint8 id;
	Uint8 damage;
	float speed;
	Uint8 fire4sec;
	Uint8 ammo;
	Uint8 reload;
	Uint8 fired;
	Uint8 hand;
	Mix_Chunk *sound;
	Mix_Chunk *reloadsound;
} Weapon;

typedef struct {
	int id;
	char name[20];
	Uint8 life;
	short int health;
	Uint8 dead;
	float x;
	float y;
	float speed;
	SDL_Rect sprite;
	SDL_Rect src;
	SDL_Surface *images[PLAYER_IMAGES];
	Mix_Chunk *sounds[PLAYER_SOUNDS];
	Weapon *weapons;
	Weapon *primary;
	Weapon *secondary;
	
} Player;

typedef struct {
	SDL_Surface *image;
	Uint8 over;
	Uint8 through;
} MapObject;

// prototipos
Player *player_create (int id, char *name, char *character, SDL_Surface *screen, Weapon *weapons, struct conf *conf);
void player_free (Player **player);
void player_set_position (Player **player, char **map, Uint8 y, Uint8 x, MapObject *mapobjects);
void player_move (Player **player, Uint8 *keystates, char **map, MapObject *mapobjects, struct conf *conf, int deltaTicks);
void player_get_object(Player **player, char **map, struct conf *conf);
void player_change_weapon(Player **player, Uint8 weapon, Uint8 hand);
Weapon *weapons_load();
void weapons_free(Weapon **weapons);
MapObject *mapobjects_load(SDL_Surface *screen);
void mapobjects_free(MapObject **mapobjects);
char **map_open(char *map);
void map_draw(SDL_Surface *screen, char **map, MapObject *mapobjects, char background);
Player *bots_create(SDL_Surface *screen, Weapon *weapons, struct conf *conf);

#endif 
