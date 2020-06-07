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


//SEPARAR TODO EN 2 ARCHIVOS, UNO PARA EL BUDDY Y UNO PARA PARTICIONES DINAMICAS y este con las que unen ambas formas

void registrarMensajeEnMemoria(uint32_t idMensaje, paquete* paq, algoritmoMem metodo){

	msgMemoriaBroker* msgNuevo = malloc(sizeof(msgMemoriaBroker)); //todo preguntar cosas repetidas
	msgNuevo->cola          = paq->tipoMensaje;
	msgNuevo->idMensaje     = idMensaje;
	msgNuevo->subsACK       = inicializarListaMutex();
	msgNuevo->subsYaEnviado = inicializarListaMutex();
	msgNuevo->paq 			= paq;

	switch(metodo){
	case PARTICIONES_DINAMICAS:
		pthread_mutex_lock(mutexMemoria);
		registrarEnMemoriaPARTICIONES(msgNuevo);
		pthread_mutex_unlock(mutexMemoria);
		break;
	case BUDDY_SYSTEM:
		pthread_mutex_lock(mutexMemoria);
		registrarEnMemoriaBUDDYSYSTEM(msgNuevo, nodoRaizMemoria);
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
	struct nodoMemoria* backUp = partActual;
	if(intentarRamaIzquierda(mensajeNuevo, partActual) < 0){
		partActual = backUp->hijoDer;
		registrarEnMemoriaBUDDYSYSTEM(mensajeNuevo, partActual);
	}
}

uint32_t intentarRamaIzquierda(msgMemoriaBroker* mensajeNuevo, struct nodoMemoria* partActual){
	struct nodoMemoria* backUp = partActual;
	if(noEsParticionMinima(partActual) && !estaLibre(partActual)){ //o irá solo estado particionado?
		if(!entraEnLaMitad(partActual, mensajeNuevo))
			return -1;
		if( partActual->hijoIzq->header.status == OCUPADO){
			intentarRamaIzquierda(mensajeNuevo, partActual->hijoDer);
		}else{
			intentarRamaIzquierda(mensajeNuevo, partActual->hijoIzq);
		}
	}else{
		return evaluarTamanioParticionYasignar(partActual, mensajeNuevo);
	}
	return -1;
}

bool entraEnLaMitad(struct nodoMemoria* partActual, msgMemoriaBroker* mensajeNuevo){
	return mensajeNuevo->paq->sizeStream <= tamanioParticion(partActual);
}

//	if(noEsParticionMinima(partActual) && !estaLibre(partActual)){ //o irá solo estado particionado?
//		if( partActual->hijoIzq->header.status == OCUPADO){
//			registrarEnMemoriaBUDDYSYSTEM(mensajeNuevo, partActual->hijoDer);
//		}else{
//			registrarEnMemoriaBUDDYSYSTEM(mensajeNuevo, partActual->hijoIzq);}
//	}else{
//		evaluarTamanioParticion(partActual, mensajeNuevo);
//	}

void evaluarTamanioParticionYasignar(struct nodoMemoria* partActual, msgMemoriaBroker* msg){
	uint32_t tamanioMsg = msg->paq->sizeStream;
	if(tamanioParticion(partActual) > tamanioMsg){
		while(tamanioParticion(partActual)/2 > tamanioMsg){
			particionarMemoriaBUDDY(partActual);
			partActual->header.status = PARTICIONADO;
			partActual = partActual->hijoIzq;
		}
		partActual->header.status = OCUPADO;
		partActual->mensaje = msg;
		return 1;
	}
	return -1;
}

void particionarMemoriaBUDDY(struct nodoMemoria* particionActual){
	particionActual->hijoIzq = inicializarNodo();
	particionActual->hijoDer = inicializarNodo();
	uint32_t tamanoHijos = tamanioParticion(particionActual)/2;

	particionActual->hijoIzq->header.status = LIBRE;
	particionActual->hijoDer->header.status = LIBRE;
	particionActual->hijoIzq->header.size   = tamanoHijos;
	particionActual->hijoDer->header.size   = tamanoHijos;

	particionActual->hijoIzq->padre   = particionActual; //nuevo
	particionActual->hijoDer->padre   = particionActual; //nuevo

	particionActual->header.status = PARTICIONADO;
}

//void particionarMemoriaPARTICIONES(listaMutex* partición, msgMemoriaBroker* msg ){ //todo
	//uint32_t tamanio = msg->paq->sizeStream;

//}

void guardarSubEnMemoria(uint32_t idMensaje, uint32_t socket, ListasMemoria lista){

	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(idMensaje);

	if(mensaje == NULL){
		printf("no se encontró el mensaje en memoria, ERROR");
		return;
	}

	if( lista == CONFIRMADO){
		pthread_mutex_lock(mutexMemoria);
		addListaMutex(mensaje->subsYaEnviado, (void*) socket); 		//verificar que no esté ya en la cola
		pthread_mutex_unlock(mutexMemoria);
	}else if(lista == SUBSYAENVIADOS){
		pthread_mutex_lock(mutexMemoria);
		addListaMutex(mensaje->subsACK, (void*) socket); 			//verificar que no esté ya en la cola
		pthread_mutex_unlock(mutexMemoria);
	}
}

msgMemoriaBroker* buscarMensajeEnMemoria(uint32_t idMensajeBuscado){

	/*if(algoritmoMemoria == BUDDY_SYSTEM){*/
		return buscarMensajeEnMemoriaBuddy(idMensajeBuscado);
		/*}else{
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


void guardarConfirmacionEnMemoriaDe(paquete* paq, uint32_t socket){

	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(paq->idCorrelativo);

	if(mensaje == NULL){
		printf("no se encontró el mensaje en memoria, ERROR");
		return;}

	pthread_mutex_lock(mutexMemoria);
	addListaMutex(mensaje->subsACK, (void*) socket); //verificar que no esté ya en la cola
	pthread_mutex_unlock(mutexMemoria);
}



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

void liberarNodo(struct nodoMemoria* nodo){

    free(nodo->mensaje);
    free(nodo->hijoIzq);
    free(nodo->hijoDer);
    free(nodo);

}

listaMutex* iniciarMemoriaPARTICIONES(){
	return inicializarListaMutex();
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

	if (&(unNodo->padre) != &(otroNodo->padre))
		return false;

	return true;
}


