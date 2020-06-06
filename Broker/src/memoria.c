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


//SEPARAR TODO EN 2 ARCHIVOS, UNO PARA EL BUDDY Y UNO PARA PARTICIONES DINAMICAS

msgMemoriaBroker* buscarMensajeEnMemoria(uint32_t idMensajeBuscado){ //ver que pasa si el mensaje no esta

	if(algoritmoMemoria == BUDDY_SYSTEM){
		return buscarMensajeEnMemoriaBuddy(idMensajeBuscado);
	}/*else{
		return buscarMensajeEnMemoriaParticiones(idMensajeBuscado);
	}*/

}

msgMemoriaBroker* buscarMensajeEnMemoriaBuddy(uint32_t id){

	pthread_mutex_lock(mutexMemoria);
	struct nodoMemoria* nodoActual = nodoRaizMemoria;

	while (!estaLibre(nodoActual) && nodoActual->mensaje->idMensaje != id){ //fijarse que volver a la rama derecha si en la rama izq no hay nada
		if(nodoActual->hijoIzq->header.status == LIBRE){
			nodoActual = nodoActual->hijoDer;
		}else if(nodoActual->hijoDer->header.status == LIBRE){
			nodoActual = nodoActual->hijoIzq;
		}else{
			pthread_mutex_unlock(mutexMemoria);
			printf("no hay ningun mensaje con ese ID");
			nodoActual = NULL;
		}

	}
		pthread_mutex_unlock(mutexMemoria);
		return nodoActual->mensaje;
	}


void guardarSubEnMemoria(uint32_t idMensaje, uint32_t socket, ListasMemoria lista){

	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(idMensaje);

	if(mensaje == NULL){
		printf("no se encontró el mensaje en memoria, ERROR");
	}

	if( lista == CONFIRMADO){
		pthread_mutex_lock(mutexMemoria);
		pushColaMutex(mensaje->subsYaEnviado, (void*) socket); 		//verificar que no esté ya en la cola
		pthread_mutex_unlock(mutexMemoria);
	}else if(lista == SUBSYAENVIADOS){
		pthread_mutex_lock(mutexMemoria);
		pushColaMutex(mensaje->subsACK, (void*) socket); 			//verificar que no esté ya en la cola
		pthread_mutex_unlock(mutexMemoria);
	}
}


void registrarMensajeEnMemoria(uint32_t idMensaje, paquete* paq, algoritmoMem metodo){

	msgMemoriaBroker* msgNuevo = malloc(sizeof(msgMemoriaBroker));
	msgNuevo->cola          = paq->tipoMensaje;
	msgNuevo->idMensaje     = idMensaje;
	msgNuevo->subsACK       = inicializarListaMutex();
	msgNuevo->subsYaEnviado = inicializarListaMutex();
	msgNuevo->paq 			= paq;

	switch(metodo){
	case PARTICIONES_DINAMICAS:
		pthread_mutex_lock(mutexMemoria);
		registrarEnMemoriaPARTICIONES(&msgNuevo);
		pthread_mutex_unlock(mutexMemoria);
		break;
	case BUDDY_SYSTEM:
		pthread_mutex_lock(mutexMemoria);
		registrarEnMemoriaBUDDYSYSTEM(&msgNuevo, nodoRaizMemoria);
		pthread_mutex_unlock(mutexMemoria);
		break;
	default:
		printf("error al guardar mensaje");
		free(msgNuevo);
		return;
	}

	free(msgNuevo);
}

void registrarEnMemoriaPARTICIONES(msgMemoriaBroker* mensajeNuevo){

}

void registrarEnMemoriaBUDDYSYSTEM(msgMemoriaBroker* mensajeNuevo, struct nodoMemoria* partActual){

	if(noEsParticionMinima(partActual) && !estaLibre(partActual)){ //o irá solo estado particionado?

		if( partActual->hijoIzq->header.status == OCUPADO){
			registrarEnMemoriaBUDDYSYSTEM(mensajeNuevo, partActual->hijoDer);
		}else{
			registrarEnMemoriaBUDDYSYSTEM(mensajeNuevo, partActual->hijoIzq);}

	}else{
		evaluarTamanioParticion(partActual, mensajeNuevo);
	}

}

void evaluarTamanioParticion(struct nodoMemoria* partActual, msgMemoriaBroker* msg){

	uint32_t tamanioMsg = msg->paq->sizeStream;

	while(tamanioParticion(&partActual)/2 > tamanioMsg){
	  particionarMemoriaBUDDY(partActual);
	  partActual = partActual->hijoIzq;
	}

	partActual->header.status = OCUPADO;
	partActual->mensaje = msg;

}


/*void guardarConfirmacionEnMemoriaDe(paquete* paq, uint32_t socket){

	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(paq->idCorrelativo); //validar que pasa si ese mensaje no esta

	pthread_mutex_lock(memoria.mutexMemoria); //la estructura memoria va a tener ese contador
	pushColaMutex(mensaje->subsACK, (void*) socket); //verificar que no esté ya en la cola
	pthread_mutex_unlock(memoria.mutexMemoria);
}*/



struct nodoMemoria* crearRaizArbol(void){
	struct nodoMemoria* nodoRaiz = inicializarNodo();    //no estoy liberando malloc
	nodoRaiz->header.size   = tamMemoria;
	nodoRaiz->header.status = LIBRE;

	return nodoRaiz;
}

struct nodoMemoria* inicializarNodo(){
    struct nodoMemoria* nodo = malloc(sizeof(struct nodoMemoria));
	nodo->hijoDer = malloc(sizeof(struct nodoMemoria));
	nodo->hijoIzq = malloc(sizeof(struct nodoMemoria));
	nodo->mensaje = malloc(sizeof(msgMemoriaBroker));
	return nodo;
}

void particionarMemoriaBUDDY(struct nodoMemoria* particionActual){
	particionActual->hijoIzq = inicializarNodo();
	particionActual->hijoDer = inicializarNodo();
	uint32_t tamanoHijos = (particionActual->header.size)/2;

	particionActual->hijoIzq->header.status  = LIBRE;
	particionActual->hijoDer->header.status = LIBRE;
	particionActual->hijoIzq->header.size    = tamanoHijos;
	particionActual->hijoDer->header.size   = tamanoHijos;

	particionActual->hijoIzq->padre   = particionActual; //nuevo
	particionActual->hijoDer->padre   = particionActual; //nuevo


	particionActual->header.status = PARTICIONADO;
}




bool noEsParticionMinima(struct nodoMemoria* particion){

	return !(particion->header.size == particionMinima);
}

bool estaLibre(struct nodoMemoria* particion){
	return particion->header.status == LIBRE;
}

uint32_t tamanioParticion(struct nodoMemoria* part){
	return part->header.size;
}


bool sonBuddies(struct nodoMemoria* unNodo, struct nodoMemoria* otroNodo){
	if ( unNodo->header.size != otroNodo->header.size)
		return false;

	if (&(unNodo->padre) != &(otroNodo->padre) )
		return false;

	return true;
}


