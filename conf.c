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

#include "conf.h"

struct conf *conf_load() {
	struct conf *conf = calloc(sizeof(struct conf), 1);
	char variable[30], valor[30];
	char linea[100];
	int i,j, k;
	unsigned char comilla;
	FILE *gestor;
	// abrir archivo de configuración
	gestor = fopen(CONF_FILE, "r");
	if(!gestor) {
		fprintf(stderr, "[error] no se puede abrir el archivo de configuración %s\n", CONF_FILE);
		return NULL;
	}
	// leer archivo de configuración
	while(!feof(gestor)) {
		// leer una linea del archivo
		fscanf(gestor, "%[^\n] \n", linea);
		// si la linea esta descomentada se procesa
		if(linea[0]!='#') {
			// limpiar variables
			bzero(variable, 30);
			bzero(valor, 30);
			// extraer nombre de la variable
			for(i=0; i<strlen(linea); ++i) {
				if(linea[i]=='=') break;
				variable[i]=linea[i];
			}
			// extraer valor de la variable
			j = 0;
			comilla = 0;
			k = i + 1;
			for(i=k; i<strlen(linea); ++i) {
				if(linea[i]=='#' || linea[i]=='\n' || (!comilla&&linea[i]==' ') || (comilla&&linea[i]=='"')) break;
				if(k==i && linea[k]=='"') comilla = 1;
				else valor[j++] = linea[i];
			}
			// asignar valor al parametro que corresponde dentro
			// de la estructura de configuracion
			// configuracion de la red
			if(!strcmp(variable, "SERVER")) {
				conf->SERVER = malloc(sizeof(strlen(valor)));
				strcpy(conf->SERVER, valor);
			}
			else if(!strcmp(variable, "PORT")) conf->PORT = atoi(valor);
			// opciones del personaje
			else if(!strcmp(variable, "PLAYER_CHARACTER")) {
				conf->PLAYER_CHARACTER = malloc(sizeof(strlen(valor)));
				strcpy(conf->PLAYER_CHARACTER, valor);
			}
			else if(!strcmp(variable, "PLAYER_NAME")) {
				conf->PLAYER_NAME = malloc(sizeof(strlen(valor)));
				strcpy(conf->PLAYER_NAME, valor);
			}
			else if(!strcmp(variable, "PLAYER_LIFE")) conf->PLAYER_LIFE = atoi(valor);
			else if(!strcmp(variable, "PLAYER_HEALTH")) conf->PLAYER_HEALTH = atoi(valor);
			else if(!strcmp(variable, "PLAYER_HEALTH_PLUS")) conf->PLAYER_HEALTH_PLUS = atoi(valor);
			else if(!strcmp(variable, "PLAYER_BASE_MOVEMENT")) conf->PLAYER_BASE_MOVEMENT = atoi(valor);
			// opciones del juego
			else if(!strcmp(variable, "TIME_LIMIT")) conf->TIME_LIMIT = atoi(valor);
			// opciones de armas
			else if(!strcmp(variable, "WEAPON_PRIMARY")) conf->WEAPON_PRIMARY = atoi(valor);
			else if(!strcmp(variable, "WEAPON_SECONDARY")) conf->WEAPON_SECONDARY = atoi(valor);
			// opciones del mapa
			else if(!strcmp(variable, "MAP")) {
				conf->MAP = malloc(sizeof(strlen(valor)));
				strcpy(conf->MAP, valor);
			}
			else if(!strcmp(variable, "BACKGROUND")) conf->BACKGROUND = valor[0];
			// opciones de los bots
			else if(!strcmp(variable, "BOTS")) conf->BOTS = atoi(valor);
			else if(!strcmp(variable, "BOT_LIFE")) conf->BOT_LIFE = atoi(valor);
			else if(!strcmp(variable, "BOT_HEALTH")) conf->BOT_HEALTH = atoi(valor);
			// en caso que la variable no este en la estructura
			else fprintf(stderr, "[warning] configuración %s existe en archivo pero no se reconoce\n", variable);
		}
	}
	fclose(gestor);
	if (!conf_check(conf))
		return NULL;
	return conf;
} 

void conf_update(struct conf *conf, char *string) {
	char *ptr;
	conf->PLAYER_LIFE = atoi(strtok(string, ";"));
	conf->PLAYER_HEALTH = atoi(strtok(NULL, ";"));
	conf->PLAYER_BASE_MOVEMENT = atoi(strtok(NULL, ";"));
	conf->PLAYER_HEALTH_PLUS = atoi(strtok(NULL, ";"));
	conf->TIME_LIMIT = atoi(strtok(NULL, ";"));
	conf->WEAPON_PRIMARY = atoi(strtok(NULL, ";"));
	conf->WEAPON_SECONDARY = atoi(strtok(NULL, ";"));
	free(conf->MAP); ptr = strtok(NULL, ";"); conf->MAP = malloc(strlen(ptr)); strcpy(conf->MAP, ptr);
	ptr = strtok(NULL, ";"); conf->BACKGROUND = ptr[0];
	conf->BOTS = atoi(ptr = strtok(NULL, ";"));
	conf->BOT_LIFE = atoi(ptr = strtok(NULL, ";"));
	conf->BOT_HEALTH = atoi(ptr = strtok(NULL, ";"));
}

short int conf_check(struct conf *conf)
{
	if (conf->SERVER==NULL) {
		conf->SERVER = malloc(sizeof(strlen("localhost")));
		strcpy(conf->SERVER, "localhost");
	}
	if (!conf->PORT)
		conf->PORT = 5555;
	if (conf->PLAYER_CHARACTER==NULL) {
		conf->PLAYER_CHARACTER = malloc(sizeof(strlen("warrior")));
		strcpy(conf->PLAYER_CHARACTER, "warrior");
	}
	if (conf->PLAYER_NAME==NULL) {
		struct passwd *p = getpwuid(getuid());
		conf->PLAYER_NAME = p->pw_name;
	}
	if (!conf->PLAYER_LIFE)
		conf->PLAYER_LIFE = 3;
	if (!conf->PLAYER_HEALTH)
		conf->PLAYER_HEALTH = 100;
	if (!conf->PLAYER_HEALTH_PLUS)
		conf->PLAYER_HEALTH_PLUS = 30;
	if (!conf->PLAYER_BASE_MOVEMENT)
		conf->PLAYER_BASE_MOVEMENT = 5;
	if (!conf->TIME_LIMIT)
		conf->TIME_LIMIT = 190;
	if (!conf->WEAPON_PRIMARY)
		conf->WEAPON_PRIMARY = 3;
	if (!conf->WEAPON_SECONDARY)
		conf->WEAPON_SECONDARY = 6;
	if (conf->MAP==NULL) {
		conf->MAP = malloc(sizeof(strlen("01.txt")));
		strcpy(conf->MAP, "01.txt");
	}
	if (!conf->BACKGROUND)
		conf->BACKGROUND = 'g';
	if (!conf->BOTS)
		conf->BOTS = 0;
	if (!conf->BOT_LIFE)
		conf->BOT_LIFE = 3;
	if (!conf->BOT_HEALTH)
		conf->BOT_HEALTH = 100;
	return 1;
}

void conf_print(struct conf *conf) {
	printf("SERVER               => %s\n", conf->SERVER);
	printf("PORT                 => %d\n", conf->PORT);
	printf("PLAYER_CHARACTER     => %s\n", conf->PLAYER_CHARACTER);
	printf("PLAYER_NAME          => %s\n", conf->PLAYER_NAME);
	printf("PLAYER_LIFE          => %d\n", conf->PLAYER_LIFE);
	printf("PLAYER_HEALTH        => %d\n", conf->PLAYER_HEALTH);
	printf("PLAYER_HEALTH_PLUS   => %d\n", conf->PLAYER_HEALTH_PLUS);
	printf("PLAYER_BASE_MOVEMENT => %d\n", conf->PLAYER_BASE_MOVEMENT);
	printf("TIME_LIMIT           => %d\n", conf->TIME_LIMIT);
	printf("WEAPON_PRIMARY       => %d\n", conf->WEAPON_PRIMARY);
	printf("WEAPON_SECONDARY     => %d\n", conf->WEAPON_SECONDARY);
	printf("MAP                  => %s\n", conf->MAP);
	printf("BACKGROUND           => %c\n", conf->BACKGROUND);
	printf("BOTS                 => %d\n", conf->BOTS);
	printf("BOT_LIFE             => %d\n", conf->BOT_LIFE);
	printf("BOT_HEALTH           => %d\n", conf->BOT_HEALTH);
}
