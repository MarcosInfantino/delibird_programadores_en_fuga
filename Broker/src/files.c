/*
 * files.c
 *
 *  Created on: 09 jun. 2020
 *      Author: utnso
 */

#include "broker.h"
#include "memoria.h"
#include "files.h"

archivoMutex* iniciarArchivoMutex(char* version){
	archivoMutex* archivo = malloc(sizeof(archivoMutex));
	archivo->mutex = malloc(sizeof(pthread_mutex_t));
	archivo->archivo = malloc(sizeof(FILE));

	time_t t;
	t = time(NULL);
	struct tm tiempoActual = *localtime(&t);

	char * nombre = "dumpDeCache";
	char * extension = ".db";
	char* nombreCompleto = malloc(sizeof(nombre) + sizeof(version) + sizeof(extension) + 1);
	strcpy(nombreCompleto,nombre);
	strcat(nombreCompleto, version);
	strcat(nombreCompleto, extension);

	archivoSem->archivo = fopen(nombreCompleto,modoEscrituraEnBinario);
	//fwrite(tiempoActual, sizeof(struct tm), 1, archivoSem->archivo);

	return archivo;
}

void almacenarParticionEnArchivo(lineaFile* particion){

	pthread_mutex_lock(archivoSem->mutex);
	archivoSem->archivo = fopen("dumpDeCache.db",modoEscrituraEnBinario);
	if(archivoSem->archivo){

		fwrite(particion, sizeof(lineaFile), 1, archivoSem->archivo);
		fclose(archivoSem->archivo);
	}
	pthread_mutex_unlock(archivoSem->mutex);;
}

//VALIDAR RQUE PONER EN PARTICIONES QUE ESTÉN LIBRES

void recorrerArbol(){ //TODO recorrer arbol y por cada particion que este ocupada o libre pero NO particionada recolecto datos y mando
	lineaFile* datosParticion = malloc(sizeof(lineaFile));
	struct nodoMemoria* particionActual;

	pthread_mutex_lock(mutexMemoria);
	//datosParticion->idMensaje = particionActual->mensaje->idMensaje;
	datosParticion->base      = particionActual->offset + &memoria;
	datosParticion->lru       = particionActual->header.tiempo;
	datosParticion->tamanio   = particionActual->header.size;
	datosParticion->limite    = particionActual->offset + &memoria + particionActual->header.size;
	//strcpy(datosParticion->estado,estadoEnString(particionActual->header.status));
	pthread_mutex_unlock(mutexMemoria);

	almacenarParticionEnArchivo(datosParticion);
	free(datosParticion);
}

void registrarParticionesLibresYocupadas(){
	lineaFile* datosParticion = malloc(sizeof(lineaFile));
	particionLibre* partLibre = malloc(sizeof(particionLibre));
	particionOcupada* partOcupada = malloc(sizeof(particionOcupada));

	pthread_mutex_lock(mutexMemoria);
	for(int i=0; i< sizeof(particionesLibres);i++){
	 partLibre = getListaMutex(particionesLibres, i);
	 datosParticion->idMensaje = -1;
	 datosParticion->base      = &memoria + partLibre->offset;
	// datosParticion->lru       = 0; //creo que va tipo tiempo aca
	 datosParticion->tamanio   = partLibre->sizeParticion;
	 datosParticion->limite    = (partLibre->offset + &memoria) + partLibre->sizeParticion;
	 strcpy(datosParticion->estado,LIBREP);

	 almacenarParticionEnArchivo(datosParticion);}
	pthread_mutex_unlock(mutexMemoria);

	pthread_mutex_lock(mutexMemoria);
	for(int i=0; i< sizeof(memoriaPARTICIONES);i++){
	 partOcupada = getListaMutex(memoriaPARTICIONES, i);
	 datosParticion->idMensaje = partOcupada->mensaje->idMensaje;
	datosParticion->base      = partOcupada->offset + &memoria;
	 datosParticion->lru       = partOcupada->lru; //creo que va tipo tiempo aca
	 datosParticion->tamanio   = partOcupada->mensaje->sizeStream;
	 datosParticion->limite    = (partOcupada->offset + &memoria) + partOcupada->mensaje->sizeStream;
	 strcpy(datosParticion->estado,OCUPADA);

	 almacenarParticionEnArchivo(datosParticion);}

	pthread_mutex_unlock(mutexMemoria);

	free(partLibre);
	free(partOcupada);
	free(datosParticion);
}

char estadoEnString(uint32_t estado){
	if(estado == 1){
		return LIBREP;
	}else{
		return OCUPADA;}
}

//DEPRECATED
/*void almacenarEnArchivoMensaje(msgMemoriaBroker* mensajeNuevo){
	pthread_mutex_lock(archivoSem->mutex);
	archivoSem->archivo = fopen("backupFile.db",modoEscrituraEnBinario);
	if(archivoSem->archivo){
		fwrite(mensajeNuevo, sizeof(mensajeNuevo), 1, archivoSem->archivo);
		fclose(archivoSem->archivo);
	}
	pthread_mutex_unlock(archivoSem->mutex);;
}*/


//DEPRECATED
/*listaMutex * leerNodosEnArchivoMensaje(){
	listaMutex * lista = inicializarListaMutex();
	msgMemoriaBroker* mensajeAAlmacenar = malloc(sizeof(msgMemoriaBroker));
	pthread_mutex_lock(archivoSem->mutex);
	archivoSem->archivo = fopen("backupFile.db",modoLecturaEnBinario);
	if(archivoSem->archivo){
		fread(mensajeAAlmacenar, sizeof(mensajeAAlmacenar), 1, archivoSem->archivo);
		addListaMutex(lista,(void*) mensajeAAlmacenar);
		if(!feof(archivoSem->archivo)){
			fread(mensajeAAlmacenar, sizeof(mensajeAAlmacenar), 1, archivoSem->archivo);
			addListaMutex(lista,(void*) mensajeAAlmacenar);
		}
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
}*/
