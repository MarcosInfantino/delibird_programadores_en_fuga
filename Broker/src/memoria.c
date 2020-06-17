/*
 * memoria.c
 *
 *  Created on: 29 may. 2020
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "broker.h"
#include "memoria.h"

void registrarMensajeEnMemoria(uint32_t idMensaje, paquete* paq, algoritmoMem metodo){

	msgMemoriaBroker* msgNuevo = malloc(sizeof(msgMemoriaBroker)); //todo preguntar cosas repetidas
	msgNuevo->cola          = paq->tipoMensaje;
	msgNuevo->idMensaje     = idMensaje;
	msgNuevo->subsACK       = inicializarListaMutex();
	msgNuevo->subsYaEnviado = inicializarListaMutex();
	msgNuevo->paq 			= paq;
	log_info(brokerLogger2, "Comienzo a registrar el mensaje en memoria.");
	switch(metodo){
	case PARTICIONES_DINAMICAS:
		pthread_mutex_lock(mutexMemoria);
		registrarEnMemoriaPARTICIONES(msgNuevo);
		pthread_mutex_unlock(mutexMemoria);
		break;
	case BUDDY_SYSTEM:
		pthread_mutex_lock(mutexMemoria);
		log_info(brokerLogger2, "Selecciono administración de memoria con buddy system.");
		registrarEnMemoriaBUDDYSYSTEM(msgNuevo, nodoRaizMemoria);
		pthread_mutex_unlock(mutexMemoria);
		break;
	default:
		printf("error al guardar mensaje");
		free(msgNuevo);
		return;
	}
	log_info(brokerLogger2, "Registre el mensaje en memoria.");
	free(msgNuevo);
}

void guardarSubEnMemoria(uint32_t idMensaje, uint32_t socket, ListasMemoria lista){

	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(idMensaje);

	if(mensaje == NULL){
		printf("no se encontró el mensaje en memoria, ERROR");
		return;
	}

	if( lista == CONFIRMADO){
		if(estaEnLista(socket,lista, mensaje )){return;}
		pthread_mutex_lock(mutexMemoria);
		addListaMutex(mensaje->subsYaEnviado, (void*) socket);
		pthread_mutex_unlock(mutexMemoria);
	}else if(lista == SUBSYAENVIADOS){
		if(estaEnLista(socket,lista, mensaje )){return;}
		pthread_mutex_lock(mutexMemoria);
		addListaMutex(mensaje->subsACK, (void*) socket);
		pthread_mutex_unlock(mutexMemoria);
	}
}

msgMemoriaBroker* buscarMensajeEnMemoria(uint32_t idMensajeBuscado){ //todo poner validaciones

	/*if(algoritmoMemoria == BUDDY_SYSTEM){*/
		return buscarMensajeEnMemoriaBuddy(idMensajeBuscado);
		/*}else{
		return buscarMensajeEnMemoriaParticiones(idMensajeBuscado);
	}*/

}


//pthread_mutex_lock(mutexMemoria);
//	struct nodoMemoria* nodoActual = nodoRaizMemoria;
//	struct nodoMemoria* backUp = nodoActual;
//
//	while (!estaLibre(nodoActual) && nodoActual->mensaje->idMensaje != id){
//		if(nodoActual->hijoIzq->header.status == LIBRE){
//			nodoActual = nodoActual->hijoDer;
//		}else if(nodoActual->hijoDer->header.status == LIBRE){
//			nodoActual = nodoActual->hijoIzq;
//		}else{
//			pthread_mutex_unlock(mutexMemoria);
//
//
//			printf("no hay ningun mensaje con ese ID");
//
//			return NULL;
//		}
//
//	}
//		pthread_mutex_unlock(mutexMemoria);
//		return nodoActual->mensaje;

//msgMemoriaBroker* buscarPorRama(uint32_t id, struct nodoMemoria* nodoActual ){
//	while (!estaLibre(nodoActual) && nodoActual->mensaje->idMensaje != id){
//
//		if(nodoActual->hijoIzq->header.status == LIBRE){
//			nodoActual = nodoActual->hijoDer;
//		}else if(nodoActual->hijoDer->header.status == LIBRE){
//			nodoActual = nodoActual->hijoIzq;
//		}else if (nodoActual->header.status == OCUPADO || nodoActual->header.status == LIBRE){
//				return NULL;
//		} //podría regresar al padre->hijoder? pero entraría en bucle
//		}
//	return nodoActual->mensaje;
//}

bool estaEnLista(uint32_t socket, ListasMemoria lista, msgMemoriaBroker* mensaje){
	listaMutex* list = malloc(sizeof(listaMutex));
	if(lista == CONFIRMADO){
		list =  mensaje->subsACK;
	}else{
		list =  mensaje->subsYaEnviado;
	}

	for(uint32_t i=0; i< sizeListaMutex(list); i++ ){
		if(socket == (uint32_t)getListaMutex(list, i)){
			free(list);
			return true;
		}
	}

	free(list);
    return false;
}


void crearDumpDeCache(){
  printf("toma la señal y la trata");
}
