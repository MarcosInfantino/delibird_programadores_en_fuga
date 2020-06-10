/*
 * filesColas.c
 *
 *  Created on: 09 jun. 2020
 *      Author: utnso
 */

#include "broker.h"
#include "memoria.h"
#include "files.h"

void almacenarSuscriptorEnArchivoCola(uint32_t socketSuscrito, char* nombreArchivo){
	archivoMutex * archivoAUsar = iniciarArchivoMutex();
	pthread_mutex_lock(archivoAUsar->mutex);
	archivoAUsar->archivo = fopen(nombreArchivo,modoEscrituraEnBinario);
	if(archivoSem->archivo){
		fwrite(&socketSuscrito, sizeof(uint32_t), 1, archivoAUsar->archivo);
		fclose(archivoAUsar->archivo);
	}
	pthread_mutex_unlock(archivoAUsar->mutex);;
}

listaMutex * leerEnArchivoSuscriptoresDeCola(char* nombreArchivo){
	archivoMutex * archivoAUsar = iniciarArchivoMutex();
	listaMutex * lista = inicializarListaMutex();
	uint32_t * socketAAlmacenar = malloc(sizeof(uint32_t));
	pthread_mutex_lock(archivoAUsar->mutex);
	archivoAUsar->archivo = fopen(nombreArchivo,modoLecturaEnBinario);
	if(archivoAUsar->archivo){
		while(fread(socketAAlmacenar, sizeof(uint32_t), 1, archivoAUsar->archivo) != 0){
		addListaMutex(lista,(void*) socketAAlmacenar);
		}
		if(fread(socketAAlmacenar, sizeof(uint32_t), 1, archivoAUsar->archivo) != 0){
			addListaMutex(lista,(void*) socketAAlmacenar);
		}
		fclose(archivoAUsar->archivo);
	}
	pthread_mutex_unlock(archivoAUsar->mutex);
	return lista;
}

void recuperarSuscriptoresDeCola(colaMensajes * cola, char* nombreArchivo){
	cola->suscriptores = leerEnArchivoSuscriptoresDeCola(nombreArchivo);
}

void almacenarPaqueteEnArchivoCola(paquete * paq, char* nombreArchivo){
	archivoMutex * archivoAUsar = iniciarArchivoMutex();
	pthread_mutex_lock(archivoAUsar->mutex);
	archivoAUsar->archivo = fopen(nombreArchivo,modoEscrituraEnBinario);
	if(archivoSem->archivo){
		fwrite(paq, sizeof(paquete), 1, archivoAUsar->archivo);
		fclose(archivoAUsar->archivo);
	}
	pthread_mutex_unlock(archivoAUsar->mutex);;
}

colaMutex * leerEnArchivoPaquetesDeCola(char* nombreArchivo){
	archivoMutex * archivoAUsar = iniciarArchivoMutex();
	colaMutex * cola = inicializarColaMutex();
	paquete * paq = malloc(sizeof(paquete));
	pthread_mutex_lock(archivoAUsar->mutex);
	archivoAUsar->archivo = fopen(nombreArchivo,modoLecturaEnBinario);
	if(archivoAUsar->archivo){
		while(fread(paq, sizeof(paq), 1, archivoAUsar->archivo) != 0){
		pushColaMutex(cola,(void*) paq);
		}
		if(fread(paq, sizeof(paq), 1, archivoAUsar->archivo) != 0){
			pushColaMutex(cola,(void*) paq);
		}
		fclose(archivoAUsar->archivo);
	}
	pthread_mutex_unlock(archivoAUsar->mutex);
	return cola;
}

void recuperarPaquetesDeCola(colaMensajes * cola, char* nombreArchivo){
	cola->cola = leerEnArchivoPaquetesDeCola(nombreArchivo);
	//NO SE QUE JORACA PONER PARA MENSAJES EN COLA, DEBERIA SER 0
}
