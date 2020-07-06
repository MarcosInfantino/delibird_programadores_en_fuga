/*
 * files.h
 *
 *  Created on: Jun 9, 2020
 *      Author: utnso
 */

#ifndef FILES_H_
#define FILES_H_
#define modoLecturaEnBinario "rb"
#define modoEscrituraEnBinario "wb"
#define OCUPADA "[X]"
#define LIBREP "[L]"

//#include <mathcalls.h>

typedef struct{
	FILE* archivo;
	pthread_mutex_t* mutex;
}archivoMutex;

typedef struct{
	char* nombre;
	void* base;
	void* limite;
	uint32_t tamanio;
	struct tm lru;
	uint32_t cola;
	uint32_t idMensaje;
	char estado[3];    //LIBRE - OCUPADO
}lineaFile;

typedef struct{
	char* nombre;
	void* base;
	void* limite;
	uint32_t tamanio;
	char estado[3];    //LIBRE - OCUPADO
}lineaFileLibre;

uint32_t contadorFile;

void iniciarArchivoMutex();
void almacenarEnArchivo(msgMemoriaBroker* mensajeNuevo);
listaMutex * leerNodosEnArchivo();
void agregarListaABuddySystem(listaMutex * lista, struct nodoMemoria* partActual);
void almacenarParticionEnArchivo(lineaFile* particion);
void almacenarParticionLibreEnArchivo(lineaFileLibre* particionVacia);
void registrarParticionesLibresYocupadas();
void recorrerArbolYgrabarArchivo();
char* estadoEnString(uint32_t estado);

#endif /* FILES_H_ */
