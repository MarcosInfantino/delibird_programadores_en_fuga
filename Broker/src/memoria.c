/*
 * memoria.c
 *
 *  Created on: 29 may. 2020
 *      Author: utnso
 */

#include "broker.h"
#include "memoria.h"




/*msgMemoriaBroker* buscarMensajeEnMemoria(uint32_t idMensajeBuscado){ //ver que pasa si el mensaje no esta

    pthread_mutex_lock(memoria.mutexMemoria); //la estructura memoria va a tener ese contador
    return find(memoria.ListaMensajes, id de mensaje es igual a idMensajeBuscado);
    pthread_mutex_unlock(memoria.mutexMemoria);
}*/

/*void guardarSubEnMemoria(uint32_t idMensaje, uint32_t socket, ListasMemoria lista){
	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(idMensaje); //validar que pasa si ese mensaje no esta

	if( lista == CONFIRMADO){
		pthread_mutex_lock(memoria.mutexMemoria); //la estructura memoria va a tener ese contador
		pushColaMutex(mensaje->subsYaEnviado, (void*) socket); //verificar que no esté ya en la cola
		pthread_mutex_unlock(memoria.mutexMemoria);
	}else if(lista == SUBSYAENVIADOS){
		pthread_mutex_lock(memoria.mutexMemoria); //la estructura memoria va a tener ese contador
		pushColaMutex(mensaje->subsACK, (void*) socket); //verificar que no esté ya en la cola
		pthread_mutex_unlock(memoria.mutexMemoria);
	}

}*/


/*void registrarMensajeEnMemoria(uint32_t idMensaje, paquete* paq, ){

	msgMemoriaBroker* msgNuevo = malloc(sizeof(msgMemoriaBroker));
	msgNuevo->cola          = paq->tipoMensaje;
	msgNuevo->idMensaje     = idMensaje;
	msgNuevo->subsACK       = inicializarListaMutex();
	msgNuevo->subsYaEnviado = inicializarListaMutex();

	pthread_mutex_lock(memoria.mutexMemoria);
	pushColaMutex(memoria.ListaMensajes, (void*) msgNuevo);
	pthread_mutex_unlock(memoria.mutexMemoria);

	free(msgNuevo);

}*/


/*void almacenarSubEnMemoriaPara(uint32_t idmensaje, uint32_t socket){
	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(paq->idCorrelativo); //validar que pasa si ese mensaje no esta

	pthread_mutex_lock(memoria.mutexMemoria); //la estructura memoria va a tener ese contador
	pushColaMutex(mensaje->subsYaEnviado, (void*) socket); //verificar que no esté ya en la cola
	pthread_mutex_unlock(memoria.mutexMemoria);

}*/

//ESTA CREO QUE NO VA REPITE LOGICA

/*void guardarConfirmacionEnMemoriaDe(paquete* paq, uint32_t socket){

	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(paq->idCorrelativo); //validar que pasa si ese mensaje no esta

	pthread_mutex_lock(memoria.mutexMemoria); //la estructura memoria va a tener ese contador
	pushColaMutex(mensaje->subsACK, (void*) socket); //verificar que no esté ya en la cola
	pthread_mutex_unlock(memoria.mutexMemoria);
}*/


