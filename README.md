ShootMe
=======

Shooter en 2D usando C y SDL. Básicamente es el diseño de un RPG con las
opciones de un shooter.

**¡¡¡PROYECTO EN DESARROLLO, NO HAY UNA VERSIÓN USABLE!!!**

Compilación
-----------

	# apt-get install build-essential libsdl1.2-dev libsdl-ttf2.0-dev libsdl-mixer1.2-dev
	$ make

Ejecución
---------

Primero se debe ejecutar el servidor:

	$ ./shootmed [PUERTO]

Si no se especifica, por defecto se escuchará en el puerto 5555.

Luego ejecutar cada uno de los clientes que se conectarán al servidor:

	$ ./shootme [IP SERVIDOR] [PUERTO]

Si no se especifica, por defecto se conectará a localhost y al puerto 5555.

Caracteristicas
---------------

- Diseño como RPG.
- Múltiples armas (10).
- Arma principal y secundaria
- Diseño de personajes se pueden crear a partir de los *sprites* existentes.
- Mapas editables mediante archivos de texto plano.
- Objetos editables mediante archivo de texto plano.
- Armas editables mediante archivo de texto plano.

Controles
---------

- Flechas o bien WASD mueve al personaje.
- Scroll del mouse cambia arma principal.
- Botón izquiero dispara arma principal.
- Botón derecho dispara arma secundaria.

Simbología mapa
---------------

	ID	Nombre		Descripción
	--------------------------------
	*	black		color negro
	0	ammo0		municion arma 0
	1	ammo1		municion arma 1
	2	ammo2		municion arma 2
	3	ammo3		municion arma 3
	4	ammo4		municion arma 4
	5	ammo5		municion arma 5
	6	ammo6		municion arma 6
	7	ammo7		municion arma 7
	8	ammo8		municion arma 8
	9	ammo9		municion arma 9
	H	health		paquete de salud
	_	wall_h		pared horizontal
	b	bridge		puente
	g	grass		pasto
	t	tree		árbol
	w	water		agua
	|	wall_v		pared vertical

Se agregarán más objetos en el futuro.

Formato de archivos de texto plano
----------------------------------

# Archivo de objetos del mapa: mapobjects.conf

Columnas:

- Letra ASCII, desde codigo 33 a 126
- nombre, debe coincidir con el nombre del sprite
- si se puede (1) o no (0) pisar el objeto
- si se puede (1) o no (0) pasar por encima del objeto con munición

Ejemplo:

	w water 0 1

# Archivo armas: weapons.conf

Columnas:

- número del arma: 1,2,3,4,5,6,7,8,9 o 0
- nombre
- daño
- munición inicial
- cuanto da cada caja de municion

Ejemplo:

	1 knife 5 200 200

# Archivo mapas

Se recomienda revisar el mapa de ejemplo 01.txt, se utilizan los objetos del
mapa definidos en mapobjects.conf

Pendiente
---------

- Que el servidor envíe el mapa a los clientes para que no usen los locales.
- Hacer que el jugador solo vea en un radio cercano a él, el resto en negro.
- Hacer que personaje muera si se acaban vidas (si esta se configuró a distinto de cero).
- Hacer que partida termine si se configuró tiempo limite.
- Mostrar estadísticas del juego al finalizar o bajo demanda (presionando tecla).
- Chat entre jugadores.
- Dar "vida" a los bots. 
