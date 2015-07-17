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

#include "net.h"

void clean(char *string) {
	char *p;
	if((p=strchr(string, 10))) *p = '\0';
	if((p=strchr(string, 13))) *p = '\0';
}

char *query(char *query, int connection) {
	char *buffer = calloc(BUFFER_SIZE, sizeof(char));
	write(connection, query, strlen(query));
	read(connection, buffer, BUFFER_SIZE*sizeof(char));
	clean(buffer);
	return buffer;
}

int client_init(struct conf *conf) {
	int connection;
	struct sockaddr_in client;
	struct hostent *host;
	// iniciar datos del cliente
	bzero(&client, sizeof(client));
	client.sin_family = PF_INET;
	client.sin_port = htons(conf->PORT);
	host = gethostbyname(conf->SERVER);
	bcopy(host->h_addr, &client.sin_addr, host->h_length);
	// crear socket y conectar
	connection = socket(PF_INET, SOCK_STREAM, 0);
	if (connect(connection, (struct  sockaddr *)&client, sizeof(client)) < 0) {
		fprintf(stderr, "[error] No se puede conectar al servidor %s:%d\n", conf->SERVER, conf->PORT);
		return 0;
	}
	return connection;
}
