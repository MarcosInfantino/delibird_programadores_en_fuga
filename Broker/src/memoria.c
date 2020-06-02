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
#include "broker.h"
#include "memoria.h"




/*msgMemoriaBroker* buscarMensajeEnMemoria(uint32_t idMensajeBuscado){ //ver que pasa si el mensaje no esta

    pthread_mutex_lock(memoria.mutexMemoria); //la estructura memoria va a tener ese contador
    return find(memoria.ListaMensajes, id de mensaje es igual a idMensajeBuscado);
    pthread_mutex_unlock(memoria.mutexMemoria);
}*/

/*void guardarSubEnMemoria(uint32_t idMensaje, uint32_t socket, ListasMemoria lista){
	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(idMensaje);  //validar que pasa si ese mensaje no esta

	if( lista == CONFIRMADO){
		pthread_mutex_lock(mutexMemoria);
		pushColaMutex(mensaje->subsYaEnviado, (void*) socket); 		//verificar que no esté ya en la cola
		pthread_mutex_unlock(mutexMemoria);
	}else if(lista == SUBSYAENVIADOS){
		pthread_mutex_lock(mutexMemoria);
		pushColaMutex(mensaje->subsACK, (void*) socket); 			//verificar que no esté ya en la cola
		pthread_mutex_unlock(mutexMemoria);
	}

}*/


void registrarMensajeEnMemoria(uint32_t idMensaje, paquete* paq, algoritmoMem metodo){

	msgMemoriaBroker* msgNuevo = malloc(sizeof(msgMemoriaBroker));
	msgNuevo->cola          = paq->tipoMensaje;
	msgNuevo->idMensaje     = idMensaje;
	msgNuevo->subsACK       = inicializarListaMutex();
	msgNuevo->subsYaEnviado = inicializarListaMutex();

	switch(metodo){
	case PARTICIONES_DINAMICAS:
		pthread_mutex_lock(mutexMemoria);
		registrarEnMemoriaPARTICIONES(msgNuevo);
		pthread_mutex_unlock(mutexMemoria);
		break;
	case BUDDY_SYSTEM:
		pthread_mutex_lock(mutexMemoria);
		registrarEnMemoriaBUDDYSYSTEM(msgNuevo);
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

void registrarEnMemoriaBUDDYSYSTEM(msgMemoriaBroker* mensajeNuevo){

	//nodoRaizMemoria(varGlobal) VER CUANTO PESA EL MENSAJE Y VER SI CABE EN LA PARTICION O HAY QUE SEGUIR PARTICIONANDO
	/*voy a tener que chequear que no este PARTICIONADA La raiz, si no esta veo si al partirlo en dos y si sigue la particion
	 * mas grande que el mensaje sigo partiendo hasta que no, cuando no haya que partir mas ahi guardo el mensaje.
	 * si ya está particionada voy a ir al nodo hijo que este libre, si ninguno esta busco en todos los subhijos cual si y veo
	 * si encaja o tengo que seguir particionando
	*/

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


	particionActual->header.status = PARTICIONADO;
}

/*bool sonBuddies(nodoMemoria* unNodo, nodoMemoria* otroNodo){
	if ( unNodo->header.size != otroNodo->header.size){
		return false;
	}

	2 condiciones más en el ppt

	return false;
}*/

