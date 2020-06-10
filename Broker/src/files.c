/*
 * files.c
 *
 *  Created on: 09 jun. 2020
 *      Author: utnso
 */

#include "broker.h"
#include "memoria.h"
#include "files.h"

const char* modoLecturaEnBinario = "rb";
const char* modoEscrituraEnBinario = "wb";

void almacenarEnArchivo(msgMemoriaBroker* mensajeNuevo){
	pthread_mutex_lock(archivoSem->mutex);
	archivoSem->archivo = fopen("backupFile.db",modoEscrituraEnBinario);
	if(archivoSem->archivo){
		fwrite(mensajeNuevo, sizeof(mensajeNuevo), 1, archivoSem->archivo);
		fclose(archivoSem->archivo);
	}
	pthread_mutex_unlock(archivoSem->mutex);;
}

listaMutex * leerNodosEnArchivo(){
	listaMutex * lista = inicializarListaMutex();
	msgMemoriaBroker* mensajeAAlmacenar = malloc(sizeof(msgMemoriaBroker));
	pthread_mutex_lock(archivoSem->mutex);
	archivoSem->archivo = fopen("backupFile.db",modoLecturaEnBinario);
	if(archivoSem->archivo){
		while(fread(mensajeAAlmacenar, sizeof(mensajeAAlmacenar), 1, archivoSem->archivo) != 0){
		addListaMutex(lista,(void*) mensajeAAlmacenar);
		}
		if(fread(mensajeAAlmacenar, sizeof(mensajeAAlmacenar), 1, archivoSem->archivo) != 0){
			addListaMutex(lista,(void*) mensajeAAlmacenar);
		}
		fclose(archivoSem->archivo);
	}
	pthread_mutex_unlock(archivoSem->mutex);
	return lista;
}

void agregarListaABuddySystem(listaMutex * lista, struct nodoMemoria* partActual){
	for( int i = 0; i<sizeListaMutex(lista);i++){
		registrarEnMemoriaBUDDYSYSTEM(getListaMutex(lista, i), partActual);
	}
}

archivoMutex* iniciarArchivoMutex(){
	archivoMutex* archivo = malloc(sizeof(archivoMutex));
	archivo->mutex = malloc(sizeof(pthread_mutex_t));
	archivo->archivo = malloc(sizeof(FILE));
	return archivo;
}

