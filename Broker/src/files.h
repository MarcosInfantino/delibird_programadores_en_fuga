/*
 * files.h
 *
 *  Created on: Jun 9, 2020
 *      Author: utnso
 */

#ifndef FILES_H_
#define FILES_H_


typedef struct{
	FILE* archivo;
	pthread_mutex_t* mutex;
}archivoMutex;

archivoMutex* iniciarArchivoMutex();
void almacenarEnArchivo(msgMemoriaBroker* mensajeNuevo);
listaMutex * leerNodosEnArchivo();
void agregarListaABuddySystem(listaMutex * lista, struct nodoMemoria* partActual);

#endif /* FILES_H_ */
