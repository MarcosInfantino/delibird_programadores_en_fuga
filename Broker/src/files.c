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
	//archivoMutex* archivo = malloc(sizeof(archivoMutex));
//	archivo->mutex = malloc(sizeof(pthread_mutex_t));
//	archivo->archivo = malloc(sizeof(FILE));

	archivoSem = malloc(sizeof(archivoMutex));
	archivoSem->mutex = malloc(sizeof(pthread_mutex_t));
	archivoSem->archivo = malloc(sizeof(FILE));

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
	fclose(archivoSem->archivo);
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
	pthread_mutex_lock(archivoSem->mutex);
	archivoSem->archivo = fopen("dumpDeCache.db", modoEscrituraEnBinario);
	if(archivoSem->archivo){
		fwrite(particionVacia, sizeof(lineaFileLibre), 1, archivoSem->archivo);
		fclose(archivoSem->archivo);
	}
	pthread_mutex_unlock(archivoSem->mutex);
}
/*char* posicionCantidadToString(posicionCantidad* pos){
    char* posX=string_itoa((pos->posicion).x);
    char* posY=string_itoa((pos->posicion).y);
    char* cantidad=string_itoa(pos->cantidad);
    char * buffer=string_new();
    string_append(&buffer,posX );
    string_append(&buffer,"-");
    string_append(&buffer,posY );
    string_append(&buffer, "=" );
    string_append(&buffer,cantidad);
    string_append(&buffer,"\n" );

    free(posX);
    free(posY);
    free(cantidad);

    return buffer;
}*/

char* serializarLineaOcupada (lineaFile* particionE){
	t_list* particiones = list_create();
	list_add_all(particiones, particionesOcupadas);
	list_add_all(particiones, particionesLibres);
	list_sort(particiones, menorAmayorSegunOffset);
	for(int i =0; i<sizeListaMutex(particiones); i++){
		particion* particionAEscribir = (particion*)list_get(particiones, i);
		char* buffer = string_new();
		char* nroParticion = string_from_format("Particion %d:", i);
		string_append(&buffer, nroParticion);
		char* inicioParticion = string_from_format("%p", memoria + particionAEscribir->offset);
		string_append(&buffer, inicioParticion);

	}


}

void recorrerArbolYgrabarArchivo(){ //TODO recorrer arbol y por cada particion que este ocupada o libre pero NO particionada recolecto datos y mando
	lineaFile* datosParticion = malloc(sizeof(lineaFile));
	struct nodoMemoria* particionActual = malloc(sizeof(struct nodoMemoria));
	lineaFileLibre* datosParticionVacia = malloc(sizeof(lineaFileLibre));

	pthread_mutex_lock(mutexMemoria);
	datosParticion->idMensaje = particionActual->mensaje->idMensaje;
	datosParticion->base      = particionActual->offset + memoria;
	datosParticion->lru       = particionActual->header.ultimoAcceso;
	datosParticion->tamanio   = particionActual->header.size;
	datosParticion->limite    = particionActual->offset + memoria + particionActual->header.size;
	strcpy(datosParticion->estado,OCUPADA);
	//strcpy(datosParticion->estado,estadoEnString(particionActual->header.status));
	pthread_mutex_unlock(mutexMemoria);

	almacenarParticionEnArchivo(datosParticion);

	//y si está libre sólo esto
	datosParticionVacia->base    = particionActual->offset + memoria;
	datosParticionVacia->limite  = particionActual->offset + memoria + particionActual->header.size;
	datosParticionVacia->tamanio = particionActual->header.size;
	strcpy(datosParticionVacia->estado,LIBREP);

	almacenarParticionLibreEnArchivo(datosParticionVacia);


	free(datosParticion);
}

void registrarParticionesLibresYocupadas(){ //TODO esto debería ir por orden de offset
	lineaFile* datosParticion = malloc(sizeof(lineaFile));
	lineaFileLibre* datosParticionVacia = malloc(sizeof(lineaFileLibre));
	particionLibre* partLibre;
	particionOcupada* partOcupada;

	pthread_mutex_lock(mutexMemoria);
	for(int i=0; i< sizeListaMutex(particionesLibres);i++){
		partLibre = getListaMutex(particionesLibres, i);
		datosParticionVacia->base    = memoria + partLibre->offset;
		datosParticionVacia->tamanio = partLibre->sizeParticion;
		datosParticionVacia->limite  = partLibre->offset + memoria + partLibre->sizeParticion;
		strcpy(datosParticionVacia->estado, LIBREP);

		almacenarParticionLibreEnArchivo(datosParticionVacia);
	}

	pthread_mutex_unlock(mutexMemoria);

	pthread_mutex_lock(mutexMemoria);
	for(int i=0; i< sizeListaMutex(particionesOcupadas);i++){
		partOcupada = getListaMutex(particionesOcupadas, i);
		datosParticion->idMensaje = partOcupada->mensaje->idMensaje;
		datosParticion->base      = partOcupada->offset + memoria;
		datosParticion->lru       = partOcupada->lru;
		datosParticion->tamanio   = partOcupada->mensaje->sizeStream;
		datosParticion->limite    = partOcupada->offset + memoria + partOcupada->mensaje->sizeStream;
		strcpy(datosParticion->estado, OCUPADA);

		almacenarParticionEnArchivo(datosParticion);
	}

	pthread_mutex_unlock(mutexMemoria);

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
