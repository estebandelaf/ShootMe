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

#ifndef SHOTMED_H_
#define SHOTMED_H_

// bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "conf.h"
#include "net.h"

// estructuras
typedef struct {
	int id;
	int y;
	int x;
	char *character;
	char *name;
} Player;

typedef struct nodo {
	Player player;
	struct nodo *siguiente;
} Nodo;

struct thread_args {
	int conexion;
	struct conf *conf;
	Nodo **players;
	pthread_mutex_t *mutex;
};

// prototipos
void *thread_main(void *args);
void agregar(Nodo **lista, Player player);
void eliminar(Nodo **lista, int posicion);
int buscar_posicion(Nodo **lista, int conexion);
int contar(Nodo **lista);

#endif
