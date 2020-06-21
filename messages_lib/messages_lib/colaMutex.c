/*
 * colaMutex.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "messages_lib.h"

colaMutex* inicializarColaMutex(){
	colaMutex* cola = malloc(sizeof(colaMutex));
	cola->cola=queue_create();
	cola->mutex = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(cola->mutex,NULL);
	return cola;
}

void pushColaMutex(colaMutex* cola, void* infoEntrenador){
	pthread_mutex_lock(cola->mutex);
	queue_push(cola->cola,infoEntrenador);
	pthread_mutex_unlock(cola->mutex);

}

void* popColaMutex(colaMutex* cola){
	pthread_mutex_lock(cola->mutex);
	void* elemento=queue_pop(cola->cola);
	pthread_mutex_unlock(cola->mutex);
	return elemento;

}

uint32_t sizeColaMutex(colaMutex* cola){
	pthread_mutex_lock(cola->mutex);
	uint32_t size=queue_size(cola->cola);
	pthread_mutex_unlock(cola->mutex);
	return size;
}

void destruirColaMutexYElementos(colaMutex* cola, void(*element_destroyer)(void*)){
	free(cola->mutex);
	queue_destroy_and_destroy_elements(cola->cola,element_destroyer);
}

void destruirColaMutex(colaMutex* cola){
	free(cola->mutex);
	queue_destroy(cola->cola);
}
