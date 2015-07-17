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

#include "shootmed.h"

int main (int argc, char **argv) {
	// cargar configuración
	struct conf *conf = conf_load();
	if (!conf)
		return EXIT_FAILURE;
	// crear lista con los jugadores conectados
	Nodo *lista = NULL;
	// declaracion de variables
	struct sockaddr_in server;
	int addrlen, sock, conexion;
	pthread_t threadID; // thread id de pthread_create()
	struct thread_args *args; // puntero a los argumentos del thread
	// definir puerto a utilizar
	conf->PORT = argc==2 ? atoi(argv[1]) : conf->PORT;
	// crear socket
	//int sock = server_init(conf->PORT);
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("No se ha podido crear el socket");
		exit(EXIT_FAILURE);
	}
	// inicializar estructura del servidor
	bzero(&server, sizeof(server));
	server.sin_family = PF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(conf->PORT);
	// enlazar el socket a la/s ip/s en que se escuchara
	if (bind(sock, (struct sockaddr *) &server, sizeof(server))<0) {
		close(sock);
		perror("No se ha podido enlazar el socket");
		exit(EXIT_FAILURE);
	}
	// inicializar la escucha de conexiones entrantes en el socket
	if (listen(sock, 0) < 0) {
		close(sock);
		perror ("No se ha podido iniciar la escucha de conexiones entrantes");
		exit(EXIT_FAILURE);
	}
	// mensaje indicando que se está escuchando
	printf("Servidor ShootMe iniciado, escuchando en puerto %d\n", conf->PORT);
	// crear semaforo/mutex para la seccion critica (memoria compartida)
	pthread_mutex_t mutex;
	pthread_mutex_init (&mutex, NULL);
	// tamaño de la estructura de la variable server
	addrlen = sizeof(struct sockaddr_in);
	// iniciar bucle infinito
	while(1) {
		// aceptar conexion entrante
		conexion = accept(sock, (struct sockaddr *)&server, (socklen_t *)&addrlen);
		// crear nuevo jugador e ingresarlo a la lista
		Player player;
		bzero(&player, sizeof(Player));
		player.id = conexion;
		agregar(&lista, player);
		// crear espacio de memoria para los argumentos del cliente
		args = malloc(sizeof(struct thread_args));
		// colocar argumentos para la hebra
		args->conexion = conexion;
		args->mutex = &mutex;
		args->conf = conf;
		args->players = &lista;
		// crear hebra para el cliente
		pthread_create(&threadID, NULL, thread_main, (void *)args);
		// mostrar mensaje de nuevas hebras creadas por conexion aceptada
		printf("Creada nueva hebra con ID %ld para la conexion desde %s e ID %d\n", (long int) threadID, inet_ntoa(server.sin_addr), conexion);
	}
	// cerrar socket
	close(sock);
	// retorno al sistema
	return EXIT_SUCCESS;
}

void *thread_main(void *targs) {
	// recuperar argumentos pasados a la hebra
	struct thread_args *args = (struct thread_args *) targs;
	// variables
	char buffer[BUFFER_SIZE];
	int len;
	// marcar la hebra como separada, se liberaran los recursos cuando termine la hebra
	pthread_detach(pthread_self());
	// mientras la conexion este establecida y se reciban datos se guarda el mensaje para ser enviado a los otros clientes
	while((len=read(args->conexion, buffer, sizeof(buffer)))>0) {
		// quitar caracteres al final de lo leido (salto de linea y retorno)
		clean(buffer);
		// separar informacion recibida desde el cliente y ubicar donde corresponde
		// formato del protocolo: procedimiento;par1;par2;par3;... etc
		char *ptr;
		ptr = strtok(buffer, ";"); // primer campo es el procedimiento
		// cliente envia su informacion (personaje y nombre)
		if(strcmp(ptr, "info")==0) {
			// actualizar posicion del usuario
			Nodo *aux = *args->players;
			while(aux!=NULL) {
				if(aux->player.id==args->conexion) {
					char *character = strtok(NULL, ";");
					char *name = strtok(NULL, ";");
					aux->player.character = malloc(strlen(character));
					strcpy(aux->player.character, character);
					aux->player.name = malloc(strlen(name));
					strcpy(aux->player.name, name);
					break;
				}
				aux = aux->siguiente;
			}
		}
		// cliente envia su posicion en el mapa
		else if(strcmp(ptr, "position")==0) {
			// actualizar posicion del usuario
			Nodo *aux = *args->players;
 			while(aux!=NULL) {
 				if(aux->player.id==args->conexion) {
				aux->player.y = atoi(strtok(NULL, ";"));
				aux->player.x = atoi(strtok(NULL, ";"));
 					break;
 				}
 				aux = aux->siguiente;
 			}
		}
		// cliente solicita "algo"
		else if(strcmp(ptr, "get")==0) {
			// obtener que solicita el cliente
			ptr = strtok(NULL, ";");
			// cliente solicita id
			if(strcmp(ptr, "id")==0) {
				char *msg = malloc(BUFFER_SIZE);
				sprintf(msg, "%d\n", args->conexion);
				write(args->conexion, msg, strlen(msg));
			}
			// cliente solicita configuracion
			else if(strcmp(ptr, "conf")==0) {
				char *msg = malloc(BUFFER_SIZE);
				sprintf(msg, "%d;%d;%d;%d;%d;%d;%d;%s;%c;%d;%d;%d\n", args->conf->PLAYER_LIFE, args->conf->PLAYER_HEALTH, args->conf->PLAYER_BASE_MOVEMENT, args->conf->PLAYER_HEALTH_PLUS, args->conf->TIME_LIMIT, args->conf->WEAPON_PRIMARY, args->conf->WEAPON_SECONDARY, args->conf->MAP, args->conf->BACKGROUND, args->conf->BOTS, args->conf->BOT_LIFE, args->conf->BOT_HEALTH);
				write(args->conexion, msg, strlen(msg));
			}
			// cliente solicita mapa de juego
			else if(strcmp(ptr, "map")==0) {
				char *msg = "Enviando mapa al cliente\n";
				write(args->conexion, msg, strlen(msg));
			}
			// cliente solicita armas
			else if(strcmp(ptr, "weapons")==0) {
				char *msg = "Enviando armas al cliente\n";
				write(args->conexion, msg, strlen(msg));
			}
			// cliente solicita jugadores
			else if(strcmp(ptr, "players")==0) {
				// enviar los jugadores 1 por linea
				char *msg = calloc(BUFFER_SIZE, sizeof(char));
				char tmp[BUFFER_SIZE];
				Nodo *aux = *args->players;
				while(aux!=NULL) {
					if(aux->player.id!=args->conexion) {
						bzero(tmp, sizeof(tmp));
						sprintf(tmp, "%d;%s;%s;%d;%d|", aux->player.id, aux->player.character, aux->player.name, aux->player.y, aux->player.x);
						strcat(msg, tmp);
					}
					aux = aux->siguiente;
				}
				int len = strlen(msg);
				msg[len-1] = '\n';
				msg[len] = '\0';
				write(args->conexion, msg, len+1);
			}
			// lo solicitado no esta disponible
			else {
				char *msg = "error;get;404\n";
				write(args->conexion, msg, strlen(msg));
			}
		}
		// no se encontró el procedimiento
		else {
			char *msg = "error;procedure;404\n";
			write(args->conexion, msg, strlen(msg));
		}
		// limpiar buffer
		bzero(buffer, sizeof(buffer));
	}
	// fin de la conexion
	printf("Fin de la hebra de ID %ld\n", pthread_self());
	close(args->conexion);
	// eliminar jugador de la lista
	eliminar (args->players, args->conexion);
	// terminar la hebra
	return NULL;
}

void agregar(Nodo **lista, Player player) {
	Nodo *nuevo = (Nodo *) malloc(sizeof(Nodo));
	nuevo->player = player;
	nuevo->siguiente = NULL;
	if(*lista == NULL) {
		*lista = nuevo;
	} else {
		Nodo *aux = *lista;
		while(aux->siguiente!=NULL) aux = aux->siguiente;
		aux->siguiente = nuevo;
	}
}

void eliminar(Nodo **lista, int id) {
	Nodo *aux = *lista;
	Nodo *eliminar = NULL;
	int posicion = buscar_posicion (lista, id);
	if(posicion == 1) {
		eliminar = *lista;
		*lista = aux->siguiente;
	} else {
		int i;
		for(i=1; i<posicion-1; ++i)
			aux = aux->siguiente;
		eliminar = aux->siguiente;
		aux->siguiente = aux->siguiente->siguiente;
	}
	free(eliminar);
}

int buscar_posicion(Nodo **lista, int id) {
	Nodo *aux = *lista;
	int posicion = 0;
	int contador = 0;
	// buscar posicion
	while(aux!=NULL) {
		++contador;
		if(aux->player.id==id) {
			posicion = contador;
			break;
		}
		aux = aux->siguiente;
	}
	return posicion;
}

int contar(Nodo **lista) {
	Nodo *aux = *lista;
	int nodos = 0;
	while(aux!=NULL) {
		++nodos;
		aux = aux->siguiente;
	}
	return nodos;
}
