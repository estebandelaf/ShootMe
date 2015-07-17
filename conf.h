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

#ifndef CONF_H_
#define CONF_H_

// bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

// definiciones
#define CONF_FILE "data/shootme.conf"

// estructuras
struct conf {
	/// OPCIONES LOCALES (TOMADAS DESDE EL ARCHIVO DE CONFIGURACIÓN)
	// configuración de la red
	char *SERVER;
	unsigned short int PORT;
	// opciones del personaje
	char *PLAYER_CHARACTER;
	char *PLAYER_NAME;
	/// OPCIONES GLOBALES (TOMADAS DESDE EL SERVIDOR)
	unsigned char PLAYER_LIFE;
	unsigned char PLAYER_HEALTH;
	unsigned char PLAYER_HEALTH_PLUS;
	unsigned char PLAYER_BASE_MOVEMENT;
	// opciones del juego
	short int TIME_LIMIT;
	// opciones de armas
	unsigned char WEAPON_PRIMARY;
	unsigned char WEAPON_SECONDARY;
	// opciones del mapa
	char *MAP;
	char BACKGROUND;
	// opciones de los bots
	unsigned char BOTS;
	unsigned char BOT_LIFE;
	unsigned char BOT_HEALTH;
}; 

// prototipos
struct conf *conf_load();
void conf_update(struct conf *conf, char *string);
short int conf_check(struct conf *conf);
void conf_print(struct conf *conf);

#endif
