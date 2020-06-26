/*
 * gamecard.h
 *
 *  Created on: 12 may. 2020
 *      Author: juancito
 */
#include <commons/collections/queue.h>
#include <string.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <messages_lib/messages_lib.h>
#include <stdbool.h>
#include <commons/bitarray.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#ifndef GAMECARD_H_
#define GAMECARD_H_

#define pathFiles "/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/TALL_GRASS/Files/"
#define pathBlocks "/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/TALL_GRASS/Blocks/"

typedef struct{
	uint32_t block_size;
	uint32_t blocks;
	char* magic_number;
}tallGrassHeader;

typedef enum{
	ARCHIVO,
	DIRECTORIO
}tipoMetadata;

typedef struct{
	char esDirectorio;
	uint32_t tamanioArchivo;
	t_list* bloquesUsados;
	bool estaAbierto;
	tipoMetadata tipo;
	//FILE* archivoMetadata;
	t_list* archivosHijos;
	char* pathArchivo;
	//struct archivoHeader* archivoPadre;
}archivoHeader;



typedef enum{
	LIBRE,
	OCUPADO
}estadoBloque;

typedef struct{
	FILE* file;
	uint32_t bytesLeft;
	uint32_t id;
}blockHeader;


typedef struct{
	char* pokemon;
	posicion posicion;
	uint32_t cantidad;
	uint32_t id;
} pokemonEnPosicion;

typedef struct{
	char* pokemon;
	uint32_t id;
	uint32_t cantPosiciones;
	posicion* posicion;
} pokemonADevolver;

typedef struct{
	char* pokemon;
	uint32_t id;
	posicion* posicion;
	bool resultado;
} pokemonAAtrapar;

uint32_t tamanioBloque;
uint32_t cantidadBloques;
t_bitarray* bitmap;
char* puntoMontaje;
tallGrassHeader tallGrass;
char* mmapBitmap;
t_list* listaBloques;

void* suscribirseCola(void* msgSuscripcion);
void* suscribirseColasBroker(void* config);
void* iniciarServidorGameboy(void* arg);
int crearHiloServidorGameboy(pthread_t* hilo);
void esperar_cliente(uint32_t servidor);
void* atenderCliente(void* sock);
void* atenderNew(void* paquete);
void* atenderGet(void* paquete);
void* atenderCatch(void* paquete);
void enviarLocalized(pokemonADevolver* pokeADevolver);
void enviarAppeared (pokemonEnPosicion* pokeEnPosicion);
void enviarCaught (pokemonAAtrapar* pokeAAtrapar);
char* buscarPath (char *path);
void iniciarFileSystem();
void iniciarMetadata();
void iniciarBitmap();
uint32_t directorioExiste (char *path);
uint32_t archivoExiste(char* path);
void actualizarArchivoBitmap();
int32_t crearArchivoBloque(blockHeader* bloque);
bool poseeArchivo(blockHeader* bloque);
void inicializarListaBloques();
bool estaLibre(uint32_t idBloque);
void ocuparBloque(uint32_t idBloque);
void liberarBloque(uint32_t idBloque);
blockHeader* obtenerBloquePorId(uint32_t id);
int32_t crearDirectorio(char* nombre, char* pathDestino, uint32_t tipo);
void eliminarDirectorio(char* path);
void crearMetadata(uint32_t tipo, char* direccion);
void escribirMetadata(archivoHeader* metadata, FILE* archivoMetadata);
FILE* abrirArchivo(archivoHeader* metadata);
void cerrarArchivo(archivoHeader* metadata, FILE* archivo);
bool estaAbierto(archivoHeader* metadata);
#endif /* GAMECARD_H_ */
