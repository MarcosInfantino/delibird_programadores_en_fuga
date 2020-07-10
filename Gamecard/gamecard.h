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
#include <math.h>
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
	char* pathArchivo;
	char* nombreArchivo;
	pthread_mutex_t* mutex;//despues ver de sacarlo
	//struct archivoHeader* archivoPadre;
}archivoHeader;


struct nodoArbolDirectorio{
	archivoHeader* dataNodo;
	struct nodoArbolDirectorio* nodoPadre;
	t_list* nodosHijos;  //Lista de nodoArbolDirectorio
};

archivoHeader* var;

typedef enum{
	LIBRE,
	OCUPADO
}estadoBloque;

typedef struct{
	FILE* file;
	uint32_t bytesLeft;
	uint32_t id;
	uint32_t pos;
}blockHeader;


typedef struct{
	char* pokemon;
	posicion posicion;
	uint32_t cantidad;
	uint32_t id;
} pokemonEnPosicion;

//typedef struct{
//	posicion posicion;
//	uint32_t cantidad;
//}posicionCantidad;

typedef struct{
	char* pokemon;
	uint32_t id;
	uint32_t cantPosiciones;
	posicion* posicion;
} pokemonADevolver;

typedef struct{
	char* pokemon;
	uint32_t id;
	posicion posicion;
	uint32_t resultado;
} pokemonAAtrapar;

uint32_t tamanioBloque;
uint32_t cantidadBloques;
t_bitarray* bitmap;
char* puntoMontaje;
tallGrassHeader tallGrass;
char* mmapBitmap;
t_list* listaBloques;
t_log* gamecardLogger2;

struct nodoArbolDirectorio* raizDirectorio;

listaMutex* listaArchivos;//lista de archivoHeader
uint32_t tiempoRetardoGC;
uint32_t tiempoReintentoOperacion;
pthread_mutex_t* mutexPrueba;

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
bool archivoExiste(char* path);
void actualizarArchivoBitmap();
int32_t crearArchivoBloque(blockHeader* bloque);
bool poseeArchivo(blockHeader* bloque);
void inicializarListaBloques();
bool estaLibre(uint32_t idBloque);
void ocuparBloque(uint32_t idBloque);
void liberarBloque(uint32_t idBloque);
blockHeader* obtenerBloquePorId(uint32_t id);
archivoHeader* crearDirectorio(char* nombre, char* pathDestino, uint32_t tipo);
void eliminarDirectorio(char* path);
archivoHeader* crearMetadata(char* nombre, uint32_t tipo, char* direccion);
void escribirMetadata(archivoHeader* metadata);
FILE* abrirArchivo(archivoHeader* metadata);
void cerrarArchivo(archivoHeader* metadata, FILE* archivo);
bool estaAbierto(archivoHeader* metadata);
void modificarSize(archivoHeader* metadata,uint32_t size);
char* agregarIdBloque(char* string, uint32_t id);
char* obtenerStringListaBloques(archivoHeader* metadata);
void agregarBloque(archivoHeader* metadata, blockHeader* bloque);
int32_t obtenerPosicionBloqueEnLista(t_list* listaBloques , uint32_t idBloque);
void removerBloque(archivoHeader* metadata, uint32_t idBloque);
int32_t escribirBloque(int32_t bloque, int32_t offset, int32_t longitud,char* buffer);
void escribirBloque2(int32_t bloque, char* buffer);
struct nodoArbolDirectorio* crearNodoDirectorio(archivoHeader* archivo, struct nodoArbolDirectorio* nodoDirectorioPadre);
void disminuirCapacidad(blockHeader* bloque, int32_t bytes);
bool tieneCapacidad(blockHeader* bloque, int32_t capacidad);
archivoHeader* buscarArchivoHeaderPokemon(char* pokemon);
bool archivoHeaderYaRegistrado(char* pokemon);
archivoHeader* obtenerArchivoPokemon(char* nombre);
char* leerBloque(blockHeader* bloque);
uint32_t posBloque(blockHeader* bloque);
char* obtenerStringArchivo(char* pokemon);
char* pathBloque(uint32_t idBloque);
t_list* obtenerListaPosicionesString(char* posiciones);
posicionCantidad* obtenerPosicionCantidadDeString(char* stringPos);
t_list * obtenerListaPosicionCantidad(t_list* listaString);
t_list* obtenerListaPosicionCantidadDeString(char* string);
t_list* obtenerPosicionesDeArchivo(char* pokemon);
uint32_t cantidadBloquesArchivo(archivoHeader* archivo);
uint32_t capacidadTotalArchivo(archivoHeader* archivo);
void removerUnBloque(archivoHeader* metadata);
blockHeader* encontrarBloqueLibre();
void agregarUnBloque(archivoHeader* metadata);
void reescribirArchivo(char* pokemon, char* stringAEscribir);
FILE* verificarApertura(archivoHeader* archivo);
char* leerArchivo(char* pokemon);
posicionCantidad* buscarPosicionCantidad(t_list* lista, posicion pos);
t_list* obtenerListaPosicionCantidadDeArchivo(archivoHeader* archivo);
char* posicionCantidadToString(posicionCantidad* pos);
char* listaPosicionCantidadToString(t_list* lista);
void actualizarPosicionesArchivo(archivoHeader* archivo, t_list* listaPosicionCantidad);
t_list* arrayBloquesStringToList(char** listaArray);
archivoHeader* cargarMetadata(char* path, uint32_t tipo, char* nombre);
uint32_t buscarIdCantidad(t_list* lista, posicion pos);
void reiniciarBloquesDeArchivo(archivoHeader* headerPoke);
void reiniciarArchivoBloque(uint32_t idBloque);
void setearSize(archivoHeader* archivo,uint32_t size);
void obtenerListaBloquesConfig(archivoHeader* archivo, char* estado);
#endif /* GAMECARD_H_ */
