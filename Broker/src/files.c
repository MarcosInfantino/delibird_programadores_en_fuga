/*
 * files.c
 *
 *  Created on: 09 jun. 2020
 *      Author: utnso
 */

#include "broker.h"
#include "memoria.h"
#include "files.h"

void iniciarArchivoMutex(){
	archivoMutex* archivo = malloc(sizeof(archivoMutex));
	archivo->mutex = malloc(sizeof(pthread_mutex_t));
	archivo->archivo = malloc(sizeof(FILE));

   time_t rawtime;
   struct tm *info;
   time( &rawtime );
   info = localtime( &rawtime );

//	time_t t;
//	t = time(NULL);
//	struct tm *tiempoActual = *localtime(&t);

	//char * nombre = "dumpDeCache";
	//char * extension = ".db";
	//char* nombreCompleto = malloc(sizeof(nombre) + sizeof(version) + sizeof(extension) + 1);
	//strcpy(nombreCompleto,nombre);
	//strcat(nombreCompleto, version);
	//strcat(nombreCompleto, extension);

	archivoSem->archivo = fopen("dumpDeCache.db",modoEscrituraEnBinario);
	fwrite(info, sizeof(struct tm), 1, archivoSem->archivo);

	//return archivo;
}

void almacenarParticionEnArchivo(lineaFile* particion){
	pthread_mutex_lock(archivoSem->mutex);
	archivoSem->archivo = fopen("dumpDeCache.db",modoEscrituraEnBinario);
	if(archivoSem->archivo){
		fwrite(particion, sizeof(lineaFile), 1, archivoSem->archivo);
		fclose(archivoSem->archivo);
	}
	pthread_mutex_unlock(archivoSem->mutex);
}

void almacenarParticionLibreEnArchivo(lineaFileLibre* particionVacia){

	archivoSem->archivo = fopen("dumpDeCache.db",modoEscrituraEnBinario);
		if(archivoSem->archivo){
		 fwrite(particionVacia, sizeof(lineaFileLibre), 1, archivoSem->archivo);
		 fclose(archivoSem->archivo);
		}
	return;
}


void recorrerArbolYgrabarArchivo(){ //TODO recorrer arbol y por cada particion que este ocupada o libre pero NO particionada recolecto datos y mando
	lineaFile* datosParticion = malloc(sizeof(lineaFile));
	struct nodoMemoria* particionActual = malloc(sizeof(struct nodoMemoria));
	lineaFileLibre* datosParticionVacia = malloc(sizeof(lineaFileLibre));

	pthread_mutex_lock(mutexMemoria);
	datosParticion->idMensaje = particionActual->mensaje->idMensaje;
	datosParticion->base      = particionActual->offset + &memoria;
	datosParticion->lru       = particionActual->header.tiempo;
	datosParticion->tamanio   = particionActual->header.size;
	datosParticion->limite    = particionActual->offset + &memoria + particionActual->header.size;
	strcpy(datosParticion->estado,OCUPADA);
	//strcpy(datosParticion->estado,estadoEnString(particionActual->header.status));
	pthread_mutex_unlock(mutexMemoria);

	almacenarParticionEnArchivo(datosParticion);

	//y si está libre sólo esto
	datosParticionVacia->base    = particionActual->offset + &memoria;
	datosParticionVacia->limite  = particionActual->offset + &memoria + particionActual->header.size;
	datosParticionVacia->tamanio = particionActual->header.size;
	strcpy(datosParticionVacia->estado,LIBREP);

	almacenarParticionLibreEnArchivo(datosParticionVacia);


	free(datosParticion);
}

void registrarParticionesLibresYocupadas(){ //TODO esto debería ir por orden de offset
	lineaFile* datosParticion = malloc(sizeof(lineaFile));
	lineaFileLibre* datosParticionVacia = malloc(sizeof(lineaFileLibre));
	particionLibre* partLibre = malloc(sizeof(particionLibre));
	particionOcupada* partOcupada = malloc(sizeof(particionOcupada));


	pthread_mutex_lock(mutexMemoria);
	for(int i=0; i< sizeListaMutex(particionesLibres);i++){
	 partLibre = getListaMutex(particionesLibres, i);
	 datosParticionVacia->base    = &memoria + partLibre->offset;
	 datosParticionVacia->tamanio = partLibre->sizeParticion;
	 datosParticionVacia->limite  = (partLibre->offset + &memoria) + partLibre->sizeParticion;
	 strcpy(datosParticionVacia->estado,LIBREP);

	 almacenarParticionLibreEnArchivo(datosParticionVacia);
	}
	pthread_mutex_unlock(mutexMemoria);

	pthread_mutex_lock(mutexMemoria);
	for(int i=0; i< sizeListaMutex(memoriaPARTICIONES);i++){
	 partOcupada = getListaMutex(memoriaPARTICIONES, i);
	 datosParticion->idMensaje = partOcupada->mensaje->idMensaje;
	 datosParticion->base      = partOcupada->offset + &memoria;
	 datosParticion->lru       = partOcupada->lru;
	 datosParticion->tamanio   = partOcupada->mensaje->sizeStream;
	 datosParticion->limite    = (partOcupada->offset + &memoria) + partOcupada->mensaje->sizeStream;
	 strcpy(datosParticion->estado,OCUPADA);

	 almacenarParticionEnArchivo(datosParticion);}

	pthread_mutex_unlock(mutexMemoria);

	free(partLibre);
	free(partOcupada);
	free(datosParticion);
	free(datosParticionVacia);
}

char* estadoEnString(uint32_t estado){
	if(estado == 1){
		return LIBREP;
	}else{
		return OCUPADA;}
}

/*char* intToVecChar(uint32_t intAConvertir){
    uint32_t lengthDeInt = (uint32_t)floor(log10(abs(intAConvertir))) + 1;
    uint32_t array[lengthDeInt];
    char * cadena = "";
    char digitoActual;
    for (uint32_t i = lengthDeInt-1; i >= 0; i--) {
        array[i] = intAConvertir % 10;
        intAConvertir /= 10;
    }
    for (uint32_t i = 0; i < lengthDeInt; i--) {
        digitoActual = array[i] + '0';
        strcpy(cadena+i,digitoActual);
    }
    return cadena;
}
*/
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
