/*
 * files.c
 *
 *  Created on: 09 jun. 2020
 *      Author: utnso
 */

#include "broker.h"
#include "memoria.h"
#include "files.h"
#include "memoriaParticiones.h"


void iniciarArchivoMutex(){

	archivoSem = malloc(sizeof(archivoMutex));
	archivoSem->mutex = malloc(sizeof(pthread_mutex_t));
	archivoSem->path="dumpDeCache.db";
	pthread_mutex_init(archivoSem->mutex,NULL);
}

void iniciarEscrituraDump(){

	log_info(loggerBroker,"Se solicitó dump de cache.");
	log_info(brokerLogger2,"Se solicitó dump de cache.");

	FILE* f= fopen(archivoSem->path,modoEscrituraEnBinario);
	fwrite("Dump: ", strlen("Dump: "), 1, f);
	char* tiempo=temporal_get_string_time();
	char* tiempoAEscribir=string_new();
	string_append_with_format(&tiempoAEscribir, "%s\n", tiempo);

	fwrite(tiempoAEscribir, strlen(tiempoAEscribir), 1, f);

	free(tiempo);
	free(tiempoAEscribir);
	fclose(f);


	switch(algoritmoMemoria){
		case BUDDY_SYSTEM:
			recorrerArbolYgrabarArchivo();break;
		case PARTICIONES_DINAMICAS:
			registrarParticionesLibresYocupadas();break;
		default: break;
	}

}

void registrarParticionesLibresYocupadas(){
	t_list* particiones = list_create();
	listAddAllMutex(particiones, particionesOcupadas);
	listAddAllMutex(particiones, particionesLibres);
	list_sort(particiones, menorAmayorSegunOffset);

	for(int j =0; j<list_size(particiones); j++){

		particion* particionAEscribir = (particion*)list_get(particiones, j);

		char* buffer = string_new();
		string_append_with_format(&buffer, "Particion %d: ", j);
		if(particionAEscribir->offset==0){
			string_append_with_format(&buffer, "0x0-%p.  ", particionAEscribir->offset+particionAEscribir->sizeParticion);
		}else{
			string_append_with_format(&buffer, "%p-%p.  ",particionAEscribir->offset, particionAEscribir->offset+particionAEscribir->sizeParticion);
		}


		if(particionAEscribir->estadoParticion == PARTICION_LIBRE){
			string_append(&buffer, LIBREP);
			string_append(&buffer, "   ");
			string_append_with_format(&buffer, "Size: %ib\n", particionAEscribir->sizeParticion);
		}else{
			string_append(&buffer, OCUPADA);
			string_append(&buffer, "   ");
			string_append_with_format(&buffer, "Size: %ib   ", particionAEscribir->sizeParticion);
			string_append_with_format(&buffer, "LRU: %s   ", particionAEscribir->lru);
			string_append(&buffer, "Cola: ");
			string_append(&buffer, nombreDeCola(particionAEscribir->mensaje->cola));
			string_append(&buffer, "   ");
			string_append_with_format(&buffer, "ID: %i\n", particionAEscribir->mensaje->idMensaje);
			}
		escribirEnArchivo(buffer);
	}
}

void escribirEnArchivo(char* buffer){
	pthread_mutex_lock(archivoSem->mutex);
	FILE* f=fopen("dumpDeCache.db", "r+");
	fseek(f,0,SEEK_END);
	//if(archivoSem->archivo){
	fwrite(buffer,strlen(buffer), 1, f);
	fclose(f);
	//}
	pthread_mutex_unlock(archivoSem->mutex);
	free(buffer);
}

void recorrerArbolYgrabarArchivo(){
	t_list* particiones = list_create();

	listAddAllMutex(particiones, nodosOcupados);
	listAddAllMutex(particiones, nodosLibres);
	list_sort(particiones, menorAmayorSegunOffset);

	log_info(brokerLogger2, "SIZE OCUPADAS: %i, SIZE LIBRES: %i",sizeListaMutex(nodosOcupados), sizeListaMutex(nodosLibres));
	for(int j = 0; j<list_size(particiones); j++){

	char* buffer = string_new();
	struct nodoMemoria* particionAEscribir = (struct nodoMemoria*) list_get(particiones, j);
	string_append_with_format(&buffer, "Particion %d: ", j);


	if(particionAEscribir->offset==0){
		string_append_with_format(&buffer, "0x0-%p.  ", particionAEscribir->offset+particionAEscribir->header.size);
	}else{
		string_append_with_format(&buffer, "%p-%p.  ",particionAEscribir->offset, particionAEscribir->offset+particionAEscribir->header.size);
	}


	//string_append_with_format(&buffer, "%p-%p.  ",particionAEscribir->offset, particionAEscribir->offset+particionAEscribir->header.size);

	if(particionAEscribir->header.status == LIBRE){
		string_append(&buffer, LIBREP);
		string_append(&buffer, "   ");
		string_append_with_format(&buffer, "Size: %ib\n", particionAEscribir->header.size);
	}else{
		string_append(&buffer, OCUPADA);
		string_append(&buffer, "   ");
		string_append_with_format(&buffer, "Size: %ib   ", particionAEscribir->header.size);
		string_append_with_format(&buffer, "LRU: %s   ", particionAEscribir->header.ultimoAcceso);
		string_append(&buffer, "Cola: ");
		string_append(&buffer, nombreDeCola(particionAEscribir->mensaje->cola));
		string_append(&buffer, "   ");
		string_append_with_format(&buffer, "ID: %i\n", particionAEscribir->mensaje->idMensaje);
		}
	escribirEnArchivo(buffer);

	}
}

char* estadoEnString(uint32_t estado){
	if(estado == 1)
	return LIBREP;

	return OCUPADA;
}

//TODO NO SÉ SI SE USA POR LAS DUDAS NO LO BORRÉ. ATTE:cAMI
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
