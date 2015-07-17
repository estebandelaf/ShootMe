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

#include "shootme.h"

int main (int argc, char **argv) {

	// variables
	SDL_Surface *screen; // pantalla
	SDL_Event event; // para los eventos de SDL (teclado y/o mouse)
	Uint8 *keystates = SDL_GetKeyState(NULL); // para detectar teclas mantenidas presionadas
	Uint8 salir = 0; // para determinar cuando se cierra la ventana
	int mouse_x=0, mouse_y=0; // coordenadas del mouse
	char info1[100]; // texto para mostrar informacion variada linea 1
	char info2[100]; // texto para mostrar informacion variada linea 2
	short int time; // variable para la hora
	short int ticks; // variable para mover elementos independiente de los FPS

	// cargar configuracion
	struct conf *conf = conf_load();
	if (!conf)
		return EXIT_FAILURE;

	// verificar a donde nos queremos conectar
	// se indicó el servidor como 1er parametro
	if(argc>=2) {
		free(conf->SERVER);
		conf->SERVER = malloc(strlen(argv[1]));
		strcpy(conf->SERVER, argv[1]);
	}
	// se indicó el puerto como 2do parametro
	if(argc>=3) {
		conf->PORT = atoi(argv[2]);
	}

	// conectar al servidor
	int connection = client_init(conf);
	if (!connection)
		return EXIT_FAILURE;
	// buffer para transferencia entre servidor y cliente
	char buffer[BUFFER_SIZE];

	// obtener id del jugador
	int playerID = atoi(query("get;id", connection));

	// recibir y actualizar configuraciones desde el servidor
	conf_update(conf, query("get;conf", connection));

	// inicializar sistema
	sdl_init(&screen);
	// cargar fuente
	TTF_Font *fuente = ttf_load(FONT_NAME, FONT_SIZE, FONT_STYLE);
	// cargar todos los posibles objetos de un mapa
	MapObject *mapobjects = mapobjects_load(screen);
	if (!mapobjects)
		return EXIT_FAILURE;
	// cargar mapa
	char **map = map_open(conf->MAP);
	if (!map)
		return EXIT_FAILURE;
	// cargar armas
	Weapon *weapons = weapons_load();
	if (!weapons)
		return EXIT_FAILURE;
	// crear personaje jugador
	Player *pj = player_create(playerID, conf->PLAYER_NAME, conf->PLAYER_CHARACTER, screen, weapons, conf);
	// asignar armas al personaje
	player_change_weapon(&pj, conf->WEAPON_PRIMARY, 1);
	player_change_weapon(&pj, conf->WEAPON_SECONDARY, 2);
	// ubicar personaje en el mapa
	player_set_position(&pj, map, -1, -1, mapobjects);

	// informar estado del usuario
	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "info;%s;%s", conf->PLAYER_CHARACTER, conf->PLAYER_NAME);
	write(connection, buffer, strlen(buffer));

	// crear bots y ubicar en el mapa
	Player *bots = bots_create(screen, weapons, conf);
	int i;
	for(i=0; i<conf->BOTS; ++i) {
		Player *aux = &bots[i];
		player_set_position(&aux, map, -1, -1, mapobjects);
	}

	// guardar ticks
	ticks = SDL_GetTicks();
	// ciclo infinito para refrescar la imagen de la pantalla
	while (!salir) {

		// solicitar informacion de todos los jugadores (menos el mio)
		char *enemies = query("get;players", connection);

		// enviar posicion del jugador al servidor
		sprintf(buffer, "position;%d;%d", (int)(pj->y), (int)(pj->x));
		write(connection, buffer, strlen(buffer));
		
		// time
		time = SDL_GetTicks()/1000;
		// dibujar mapa
		map_draw(screen, map, mapobjects, conf->BACKGROUND);
		// revisar si existen eventos de SDL
		while (SDL_PollEvent(&event)) {
			// salir del programa
			if(event.type==SDL_QUIT) salir = 1;
			// si se presiona una tecla
			else if(event.type==SDL_KEYDOWN) {
				// cambiar a pantalla completa
				if(event.key.keysym.sym==SDLK_f) SDL_WM_ToggleFullScreen(screen);
				// salir del juego
				else if(event.key.keysym.sym==SDLK_ESCAPE) salir = 1;
				// cambiar arma primaria
				else if(event.key.keysym.sym>=48&&event.key.keysym.sym<=57) player_change_weapon(&pj, event.key.keysym.sym-48, 1);
			}
			// si se mueve el mouse
			else if(event.type==SDL_MOUSEMOTION) {
				// guardar coordenadas del mouse
				mouse_x = event.motion.x;
				mouse_y = event.motion.y;
			}
			// si se presiona el mouse
			else if(event.type == SDL_MOUSEBUTTONDOWN ) {
				// guardar coordenadas del mouse
				mouse_x = event.button.x;
				mouse_y = event.button.y;
				// resetear flag de armas disparadas
				pj->primary->fired = 0;
				pj->secondary->fired = 0;
				// si se presiono el boton izquierdo
				if(event.button.button==SDL_BUTTON_LEFT && pj->primary->ammo) {
					Mix_PlayChannel(-1, pj->primary->sound, 0);
					pj->primary->fired = pj->primary->ammo--;
				}
				// si se presiono el boton derecho
				else if(event.button.button==SDL_BUTTON_RIGHT && pj->secondary->ammo) {
					Mix_PlayChannel(-1, pj->secondary->sound, 0);
					pj->secondary->fired = pj->secondary->ammo--;
				}
				// si se mueve el scroll se cambia el arma principal
				else if (event.button.button==SDL_BUTTON_WHEELUP) {
					player_change_weapon(&pj, WEAPON_PREV, 1);
				}
				else if (event.button.button==SDL_BUTTON_WHEELDOWN) {
					player_change_weapon(&pj, WEAPON_NEXT, 1);
				}
				// determinar si personaje fue herido
				if(mouse_x >= pj->x && mouse_x <= pj->x+SPRITE_SIZE && mouse_y >= pj->y && mouse_y <= pj->y+SPRITE_SIZE) {
					// hacer daño al personaje
					if(event.button.button==SDL_BUTTON_LEFT && pj->primary->fired) {
						pj->health -= pj->primary->damage;
						// reproducir sonido de dolor
						Mix_PlayChannel(-1, pj->sounds[PLAYER_SOUND_DAMAGED], 0);
					}
					else if(event.button.button==SDL_BUTTON_RIGHT && pj->secondary->fired) {
						pj->health -= pj->secondary->damage;
						// reproducir sonido de dolor
						Mix_PlayChannel(-1, pj->sounds[PLAYER_SOUND_DAMAGED], 0);
					}
					// si la salud quedo menor o igual a 0 el personaje muere
					if(pj->health<1) {
						// incrementar contador de muerte
						++pj->dead;
						pj->health = conf->PLAYER_HEALTH;
						// descontar vida
						if(conf->PLAYER_LIFE!=0) {
							--pj->life;
							if(pj->life==0) {
								// GAME OVER
							}
						}
						// reposicionar personaje muerto
						player_set_position(&pj, map, -1, -1, mapobjects);
					}
				}
			}
			// si se suelta el mouse
			else if(event.type==SDL_MOUSEBUTTONUP) {
				// guardar coordenadas del mouse
				mouse_x = event.button.x;
				mouse_y = event.button.y;
				// si se solto el boton izquierdo
				if(event.button.button==SDL_BUTTON_LEFT) {
					
				}
				// si se solto el boton derecho
				else if(event.button.button==SDL_BUTTON_RIGHT) {
					
				}
			}
		}
		// mover jugador
		player_move(&pj, keystates, map, mapobjects, conf, SDL_GetTicks()-ticks);
		player_get_object(&pj, map, conf);
		// primera linea informacion
		sprintf(info1, "time: %02d:%02d, mouse: (%04d,%04d)", time/60, time%60, mouse_x, mouse_y);
		SDL_Surface *s_info1 = TTF_RenderText_Blended(fuente, info1, (SDL_Color) {255,255,255,255});
		SDL_Rect rect = (SDL_Rect) { 10, 0, 100, 20 };
		SDL_BlitSurface(s_info1, NULL, screen, &rect);
		SDL_FreeSurface(s_info1);
		// segunda linea informacion
		sprintf(info2, "[%s] +%03d, life: %02d, dead: %02d, weapon: [%d/%03d/%03d] [%d/%03d/%03d]", conf->PLAYER_NAME, pj->health, pj->life, pj->dead, pj->primary->id, pj->primary->damage, pj->primary->ammo, pj->secondary->id, pj->secondary->damage, pj->secondary->ammo);
		SDL_Surface *s_info2 = TTF_RenderText_Blended(fuente, info2, (SDL_Color) {255,255,255,255});
		rect.y += 12;
		SDL_BlitSurface(s_info2, NULL, screen, &rect);
		SDL_FreeSurface(s_info2);
		// dibujar enemigos
		enemies_draw(screen, enemies);
		// dibujar bots
		/*for(i=0; i<conf->BOTS; ++i) {
			Player *aux = &bots[i];
			SDL_BlitSurface(aux->images[PLAYER_IMAGE_SPRITE], &aux->src, screen, &aux->sprite);
		}*/
		// dibujar jugador
		SDL_BlitSurface(pj->images[PLAYER_IMAGE_SPRITE], &pj->src, screen, &pj->sprite);
		// actualizar screen sdl
		SDL_Flip(screen);
		SDL_Delay(1000/FPS);
		// resetear ticks
		ticks = SDL_GetTicks();
	}

	// cerrar conexion al servidor
	close(connection);

	// liberar jugador
	player_free(&pj);

	// liberar armas
	weapons_free(&weapons);
	
	// libear objetos del mapa
	mapobjects_free(&mapobjects);

	// cerrar fuentes
	TTF_CloseFont(fuente);
	TTF_Quit();

	// cerrar SDL
	SDL_FreeSurface(screen);
	SDL_Quit();

	// retorno del sistema
	return EXIT_SUCCESS;

}

/**
 * Crea un jugador
 */
Player *player_create (int id, char *name, char *character, SDL_Surface *screen, Weapon *weapons, struct conf *conf) {
	// crear jugador e inicializar en ceros
	Player *player = calloc(1, sizeof(Player));
	// cargar datos básicos del personaje
	player->id = id;
	strcpy(player->name, name);
	player->life = conf->PLAYER_LIFE;
	player->health = conf->PLAYER_HEALTH;
	player->dead = 0;
	// cargar imagenes
	char *file = malloc(100);
	sprintf(file, "data/image/player/%s/sprite.bmp", character);
	player->images[PLAYER_IMAGE_SPRITE] = SDL_DisplayFormat(SDL_LoadBMP(file));
	SDL_SetColorKey(player->images[PLAYER_IMAGE_SPRITE], SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(screen->format, 255, 0, 255));
	sprintf(file, "data/image/player/%s/dead.bmp", character);
	player->images[PLAYER_IMAGE_DEAD] = SDL_DisplayFormat(SDL_LoadBMP(file));
	SDL_SetColorKey(player->images[PLAYER_IMAGE_DEAD], SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(screen->format, 255, 0, 255));
	// cargar sonidos
	sprintf(file, "data/sound/player/%s/damaged.wav", character);
	player->sounds[PLAYER_SOUND_DAMAGED] = Mix_LoadWAV(file);
	sprintf(file, "data/sound/player/%s/health.wav", character);
	player->sounds[PLAYER_SOUND_HEALTH] = Mix_LoadWAV(file);
// 	sprintf(file, "data/sound/player/%s/walking.wav", character);
// 	player->sounds[PLAYER_SOUND_WALKING] = Mix_LoadWAV(file);
	free(file);
	// asignar armas
	player->weapons = weapons;
	// retornar jugador
	return player;
}

void player_free (Player **player) {
	int i;
	// liberar imagenes
	for(i=0;i<PLAYER_IMAGES;++i)
		SDL_FreeSurface((*player)->images[i]);
	// liberar sonidos
	for(i=0;i<PLAYER_SOUNDS;++i)
		Mix_FreeChunk((*player)->sounds[i]);
	free(*player);
}

/**
 * Posiciona el jugador en coordenadas de frente
 */
void player_set_position (Player **player, char **map, Uint8 y, Uint8 x, MapObject *mapobjects) {
	// si no se indica posicion determinar random
	if(y==255 || x==255) {
		do {
			y = random() % (SCREEN_HEIGHT/SPRITE_SIZE);
			x = random() % (SCREEN_WIDTH/SPRITE_SIZE);
		} while(mapobjects[((int)map[y][x]-OBJECT_OFFSET)].over==0);
	}
	// ubicar personaje
	(*player)->x = x*SPRITE_SIZE;
	(*player)->y = y*SPRITE_SIZE;
	(*player)->src.x = 32;
	(*player)->src.y = 0;
	(*player)->src.w = SPRITE_SIZE;
	(*player)->src.h = SPRITE_SIZE;
}

/**
 * Mueve el personaje por la pantalla, cambiando la imagen para causar la animacion
 * y evitando que el mismo se salga de los bordes
 */
void player_move (Player **player, Uint8 *keystates, char **map, MapObject *mapobjects, struct conf *conf, int deltaTicks) {
	// cuadrante en el que se quiere posicionar el personaje
	int x1, y1, x2, y2;
	// determinar velocidad
	(*player)->speed = conf->PLAYER_BASE_MOVEMENT;
// 	(*player)->speed = PLAYER_BASE_MOVEMENT * (deltaTicks/1000.f); // http://lazyfoo.net/SDL_tutorials/lesson32/index.php
	// mover hacia arriba
	if(keystates[SDLK_UP]||keystates[SDLK_w]) {
		// elegir imagen para la direccion del movimiento
		(*player)->src.y = 96;
		(*player)->src.x = ((*player)->src.x+32) % 96;
		// mover personaje
		(*player)->y -= (*player)->speed;
		// revisar que no se salga de los bordes
		if((*player)->y<0) (*player)->y = 0;
		// revisar que el personaje no este sobre un objeto que no se puede "pisar"
		x1 = (*player)->x / SPRITE_SIZE;
		y1 = (*player)->y / SPRITE_SIZE;
		x2 = ((*player)->x+SPRITE_SIZE-1) / SPRITE_SIZE;
		if(mapobjects[((int)map[y1][x1]-OBJECT_OFFSET)].over==0||mapobjects[((int)map[y1][x2]-OBJECT_OFFSET)].over==0) (*player)->y = y1*SPRITE_SIZE + SPRITE_SIZE;
		// reproducir sonido caminando
		Mix_PlayChannel(-1, (*player)->sounds[PLAYER_SOUND_WALKING], 0);
	}
	// mover hacia abajo
	else if(keystates[SDLK_DOWN]||keystates[SDLK_s]) {
		// elegir imagen para la direccion del movimiento
		(*player)->src.y = 0;
		(*player)->src.x = ((*player)->src.x+32) % 96;
		// mover personaje
		(*player)->y += (*player)->speed;
		// revisar que no se salga de los bordes
		if((*player)->y>(SCREEN_HEIGHT-SPRITE_SIZE)) (*player)->y = SCREEN_HEIGHT-SPRITE_SIZE;
		// revisar que el personaje no este sobre un objeto que no se puede "pisar"
		x1 = (*player)->x / SPRITE_SIZE;
		y1 = ((*player)->y+SPRITE_SIZE-1) / SPRITE_SIZE;
		x2 = ((*player)->x+SPRITE_SIZE-1) / SPRITE_SIZE;
		if(mapobjects[((int)map[y1][x1]-OBJECT_OFFSET)].over==0||mapobjects[((int)map[y1][x2]-OBJECT_OFFSET)].over==0) (*player)->y = y1*SPRITE_SIZE - (SPRITE_SIZE-2) - conf->PLAYER_BASE_MOVEMENT;
		// reproducir sonido caminando
		Mix_PlayChannel(-1, (*player)->sounds[PLAYER_SOUND_WALKING], 0);
	}
	// mover hacia la izquierda
	else if(keystates[SDLK_LEFT]||keystates[SDLK_a]) {
		// elegir imagen para la direccion del movimiento
		(*player)->src.y = 32;
		(*player)->src.x = ((*player)->src.x+32) % 96;
		// mover personaje
		(*player)->x -= (*player)->speed;
		// revisar que no se salga de los bordes
		if((*player)->x<0) (*player)->x = 0;
		// revisar que el personaje no este sobre un objeto que no se puede "pisar"
		x1 = (*player)->x / SPRITE_SIZE;
		y1 = (*player)->y / SPRITE_SIZE;
		y2 = ((*player)->y+SPRITE_SIZE-1) / SPRITE_SIZE;
		if(mapobjects[((int)map[y1][x1]-OBJECT_OFFSET)].over==0||mapobjects[((int)map[y2][x1]-OBJECT_OFFSET)].over==0) (*player)->x = x1*SPRITE_SIZE + SPRITE_SIZE;
		// reproducir sonido caminando
		Mix_PlayChannel(-1, (*player)->sounds[PLAYER_SOUND_WALKING], 0);
	}
	// mover hacia la derecha
	else if(keystates[SDLK_RIGHT]||keystates[SDLK_d]) {
		// elegir imagen para la direccion del movimiento
		(*player)->src.y = 64;
		(*player)->src.x = ((*player)->src.x+32) % 96;
		// mover personaje
		(*player)->x += (*player)->speed;
		// revisar que no se salga de los bordes
		if((*player)->x>(SCREEN_WIDTH-SPRITE_SIZE)) (*player)->x = SCREEN_WIDTH-SPRITE_SIZE;
		// revisar que el personaje no este sobre un objeto que no se puede "pisar"
		x1 = ((*player)->x+SPRITE_SIZE-1) / SPRITE_SIZE;
		y1 = ((*player)->y) / SPRITE_SIZE;
		y2 = ((*player)->y+SPRITE_SIZE-1) / SPRITE_SIZE;
		if(mapobjects[((int)map[y1][x1]-OBJECT_OFFSET)].over==0||mapobjects[((int)map[y2][x1]-OBJECT_OFFSET)].over==0) (*player)->x = x1*SPRITE_SIZE - (SPRITE_SIZE-2) - conf->PLAYER_BASE_MOVEMENT;
		// reproducir sonido caminando
		Mix_PlayChannel(-1, (*player)->sounds[PLAYER_SOUND_WALKING], 0);
	}
	(*player)->sprite.x = (*player)->x;
	(*player)->sprite.y = (*player)->y;
}

void player_get_object(Player **player, char **map, struct conf *conf) {
	// determinar coordenadas del jugador
	Uint8 x1 = (*player)->x / SPRITE_SIZE;
	Uint8 y1 = (*player)->y / SPRITE_SIZE;
	Uint8 x2 = ((*player)->x + SPRITE_SIZE - 1) / SPRITE_SIZE;
	Uint8 y2 = ((*player)->y + SPRITE_SIZE - 1) / SPRITE_SIZE;
	// determinar si se cogio armamento
	if ((map[y1][x1]>=48 && map[y1][x1]<=57) ) {
		Uint8 arma = (int)map[y1][x1]-48;
		if((*player)->weapons[arma].ammo!=255) {
			if (((*player)->weapons[arma].ammo+(*player)->weapons[arma].reload)>255)
				(*player)->weapons[arma].ammo = 255;
			else
				(*player)->weapons[arma].ammo += (*player)->weapons[arma].reload;
			map[y1][x1] = conf->BACKGROUND;
			Mix_PlayChannel(-1, (*player)->weapons[arma].reloadsound, 0);
		}
	}
	if ((map[y1][x2]>=48 && map[y1][x2]<=57) ) {
		Uint8 arma = (int)map[y1][x2]-48;
		if((*player)->weapons[arma].ammo!=255) {
			if (((*player)->weapons[arma].ammo+(*player)->weapons[arma].reload)>255)
				(*player)->weapons[arma].ammo = 255;
			else
				(*player)->weapons[arma].ammo += (*player)->weapons[arma].reload;
			map[y1][x2] = conf->BACKGROUND;
			Mix_PlayChannel(-1, (*player)->weapons[arma].reloadsound, 0);
		}
	}
	if ((map[y2][x1]>=48 && map[y2][x1]<=57) ) {
		Uint8 arma = (int)map[y2][x1]-48;
		if((*player)->weapons[arma].ammo!=255) {
			if (((*player)->weapons[arma].ammo+(*player)->weapons[arma].reload)>255)
				(*player)->weapons[arma].ammo = 255;
			else
				(*player)->weapons[arma].ammo += (*player)->weapons[arma].reload;
			map[y2][x1] = conf->BACKGROUND;
			Mix_PlayChannel(-1, (*player)->weapons[arma].reloadsound, 0);
		}
	}
	if ((map[y2][x2]>=48 && map[y2][x2]<=57) ) {
		Uint8 arma = (int)map[y2][x2]-48;
		if((*player)->weapons[arma].ammo!=255) {
			if (((*player)->weapons[arma].ammo+(*player)->weapons[arma].reload)>255)
				(*player)->weapons[arma].ammo = 255;
			else
				(*player)->weapons[arma].ammo += (*player)->weapons[arma].reload;
			map[y2][x2] = conf->BACKGROUND;
			Mix_PlayChannel(-1, (*player)->weapons[arma].reloadsound, 0);
		}
	}
	// determinar si se cogio salud
	if((*player)->health!=255) {
		if (map[y1][x1]=='H') {
			if (((*player)->health+conf->PLAYER_HEALTH_PLUS)>255)
				(*player)->health = 255;
			else
				(*player)->health += conf->PLAYER_HEALTH_PLUS;
			map[y1][x1] = conf->BACKGROUND;
			Mix_PlayChannel(-1, (*player)->sounds[PLAYER_SOUND_HEALTH], 0);
		}
		if (map[y1][x2]=='H') {
			if (((*player)->health+conf->PLAYER_HEALTH_PLUS)>255)
				(*player)->health = 255;
			else
				(*player)->health += conf->PLAYER_HEALTH_PLUS;
			map[y1][x2] = conf->BACKGROUND;
			Mix_PlayChannel(-1, (*player)->sounds[PLAYER_SOUND_HEALTH], 0);
		}
		if (map[y2][x1]=='H') {
			if (((*player)->health+conf->PLAYER_HEALTH_PLUS)>255)
				(*player)->health = 255;
			else
				(*player)->health += conf->PLAYER_HEALTH_PLUS;
			map[y2][x1] = conf->BACKGROUND;
			Mix_PlayChannel(-1, (*player)->sounds[PLAYER_SOUND_HEALTH], 0);
		}
		if (map[y2][x2]=='H') {
			if (((*player)->health+conf->PLAYER_HEALTH_PLUS)>255)
				(*player)->health = 255;
			else
				(*player)->health += conf->PLAYER_HEALTH_PLUS;
			map[y2][x2] = conf->BACKGROUND;
			Mix_PlayChannel(-1, (*player)->sounds[PLAYER_SOUND_HEALTH], 0);
		}
	}
}

Weapon *weapons_load () {
	// variables para almacenar lo leido
	char name[30], file[50];
	int id, damage, ammo, reload;
	// abrir archivo de armas
	FILE *fd = fopen(WEAPONS_FILE, "r");
	if (!fd) {
		fprintf(stderr, "[error] no se puede abrir el archivo de armas %s\n", WEAPONS_FILE);
		return NULL;
	}
	// arreglo de armas
	Weapon *weapons = calloc(10, sizeof(Weapon));
	// cargar armas
	while(fscanf(fd, "%d %s %d %d %d\n", &id, name, &damage, &ammo, &reload)==5) {
		weapons[id].id = id;
		weapons[id].damage = damage;
		weapons[id].ammo = ammo;
		weapons[id].reload = reload;
		sprintf(file, "data/sound/weapon/%s.wav", name);
		weapons[id].sound = Mix_LoadWAV(file);
		sprintf(file, "data/sound/weapon/%s_reload.wav", name);
		weapons[id].reloadsound = Mix_LoadWAV(file);
	}
	// retornar armas
	return weapons;
}

void weapons_free(Weapon **weapons) {
	
}

void player_change_weapon(Player **player, Uint8 weapon, Uint8 hand) {
	// se quiere mover el arma a la anterior o siguiente disponible
	// se debe determinar el arma por la que se debe cambiar la actual
	if(weapon==WEAPON_PREV || weapon==WEAPON_NEXT) {
		if (hand==1) {
			if(weapon==WEAPON_PREV) { 
				if((*player)->primary->id==0) weapon = 9;
				else weapon = (*player)->primary->id - 1;
				if((*player)->weapons[weapon].hand!=0) {
					if(weapon==0) weapon = 9;
					else --weapon;
				}
			}
			else {
				if((*player)->primary->id==9) weapon = 0;
				else weapon = (*player)->primary->id + 1;
				if((*player)->weapons[weapon].hand!=0) {
					if(weapon==9) weapon = 0;
					else ++weapon;
				}
			}
		}
		else if (hand==2) {
			if(weapon==WEAPON_PREV) { 
				if((*player)->secondary->id==0) weapon = 9;
				else weapon = (*player)->secondary->id - 1;
				if((*player)->weapons[weapon].hand!=0) {
					if(weapon==0) weapon = 9;
					else --weapon;
				}
			}
			else {
				if((*player)->secondary->id==9) weapon = 0;
				else weapon = (*player)->secondary->id + 1;
				if((*player)->weapons[weapon].hand!=0) {
					if(weapon==9) weapon = 0;
					else ++weapon;
				}
			}
		}
	}
	// verificar que el arma no este en alguna mano
	if((*player)->weapons[weapon].hand==0) {
		// poner arma en mano principal
		if(hand==1) {
			// quitar arma actual
			if((*player)->primary!=NULL) (*player)->primary->hand = 0;
			// poner arma nueva
			(*player)->primary = &(*player)->weapons[weapon];
			(*player)->primary->hand = hand;
		}
		// poner arma en mano secundaria
		else if(hand==2) {
			// quitar arma actual
			if((*player)->secondary!=NULL) (*player)->secondary->hand = 0;
			// poner arma nueva
			(*player)->secondary = &(*player)->weapons[weapon];
			(*player)->secondary->hand = hand;
		}
	}
}

MapObject *mapobjects_load(SDL_Surface *screen) {
	// variables para almacenar lo leido
	char id, name[30], file[50];
	int over, through;
	// abrir archivo de objetos
	FILE *fd = fopen(MAPOBJECTS_FILE, "r");
	if (!fd) {
		fprintf(stderr, "[error] no se puede abrir el archivo de objetos del mapa %s\n", MAPOBJECTS_FILE);
		return NULL;
	}
	// color para "transparencia"
	int colorkey = SDL_MapRGB(screen->format, 255, 0, 255);
	// arreglo de objetos
	MapObject *mapobjects = calloc(94, sizeof(MapObject));
	// cargar objetos
	while(fscanf(fd, "%c %s %d %d\n", &id, name, &over, &through)==4) {
		sprintf(file, "data/image/map/%s.bmp", name);
		mapobjects[((int)id-OBJECT_OFFSET)].image = SDL_DisplayFormat(SDL_LoadBMP(file));
		SDL_SetColorKey(mapobjects[((int)id-OBJECT_OFFSET)].image, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
		mapobjects[((int)id-OBJECT_OFFSET)].over = over;
		mapobjects[((int)id-OBJECT_OFFSET)].through = through;
	}
	// retornar objectos
	return mapobjects;
}

void mapobjects_free(MapObject **mapobjects) {
	
}

char **map_open(char *name) {
	char **map = calloc(SCREEN_HEIGHT/SPRITE_SIZE, sizeof(char *));
	int x, y;
	char uri[50];
	char object;
	sprintf(uri, "data/map/%s", name);
	FILE *fd;
	fd = fopen(uri, "r");
	if (!fd) {
		fprintf(stderr, "[error] no se puede abrir el archivo del mapa %s\n", uri);
		return NULL;
	}
	for (y = 0; y < SCREEN_HEIGHT / SPRITE_SIZE; ++y) {
		char *fila = calloc(SCREEN_WIDTH/SPRITE_SIZE, sizeof(char));
		map[y] = fila;
		for (x = 0; x < SCREEN_WIDTH / SPRITE_SIZE; ++x) {
			object = fgetc(fd);
			if(object!='\n') fila[x] = object;
			else --x;
		}
	}
	fclose(fd);
	return map;
}

void map_draw(SDL_Surface *screen, char **map, MapObject *mapobjects, char background) {
	int x, y;
	SDL_Rect rcBackground;
	for (y = 0; y < SCREEN_HEIGHT / SPRITE_SIZE; ++y) {
		for (x = 0; x < SCREEN_WIDTH / SPRITE_SIZE; ++x) {
			rcBackground.x = x * SPRITE_SIZE;
			rcBackground.y = y * SPRITE_SIZE;
			if(map[y][x]!=background) SDL_BlitSurface(mapobjects[((int)background-OBJECT_OFFSET)].image, NULL, screen, &rcBackground);
			SDL_BlitSurface(mapobjects[((int)map[y][x]-OBJECT_OFFSET)].image, NULL, screen, &rcBackground);
		}
	}
}

Player *bots_create(SDL_Surface *screen, Weapon *weapons, struct conf *conf) {
	// crear bots e inicializar en ceros
	Player *bots = calloc(conf->BOTS, sizeof(Player));
	char *character = "celeste";
	int i;
	// cargar datos básicos del bot
	for(i=0; i<conf->BOTS; ++i) {
		bots[i].id = 0;
		strcpy(bots[i].name, "BoT");
		bots[i].life = conf->BOT_LIFE;
		bots[i].health = conf->BOT_HEALTH;
		bots[i].dead = 0;
		// cargar imagenes
		char *file = malloc(100);
		sprintf(file, "data/image/player/%s/sprite.bmp", character);
		bots[i].images[PLAYER_IMAGE_SPRITE] = SDL_DisplayFormat(SDL_LoadBMP(file));
		SDL_SetColorKey(bots[i].images[PLAYER_IMAGE_SPRITE], SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(screen->format, 255, 0, 255));
		sprintf(file, "data/image/player/%s/dead.bmp", character);
		bots[i].images[PLAYER_IMAGE_DEAD] = SDL_DisplayFormat(SDL_LoadBMP(file));
		SDL_SetColorKey(bots[i].images[PLAYER_IMAGE_DEAD], SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(screen->format, 255, 0, 255));
		// cargar sonidos
		sprintf(file, "data/sound/player/%s/damaged.wav", character);
		bots[i].sounds[PLAYER_SOUND_DAMAGED] = Mix_LoadWAV(file);
		sprintf(file, "data/sound/player/%s/health.wav", character);
		bots[i].sounds[PLAYER_SOUND_HEALTH] = Mix_LoadWAV(file);
		free(file);
		// asignar armas
		bots[i].weapons = weapons;
	}
	// retornar bots
	return bots;
} 
