/*
 * memoria.c
 *
 *  Created on: 29 may. 2020
 *      Author: utnso
 */

#include "broker.h"
#include "memoria.h"
#include "memoriaParticiones.h"

void definirComienzoDeMemoria(){
	memoria = malloc(tamMemoria);
	switch(algoritmoMemoria){
	case BUDDY_SYSTEM:
		nodoRaizMemoria = crearRaizArbol();
		nodoRaizMemoria->offset = 0;
		nodosOcupados = inicializarListaMutex();
		break;

	case PARTICIONES_DINAMICAS:
		particionesOcupadas = inicializarListaMutex();
		particionesLibres = inicializarListaMutex();
		addListaMutex(particionesLibres,(void*) crearPrimeraParticionLibre());
		break;
	}
}

void registrarMensajeEnMemoria(uint32_t idMensaje, paquete* paq, algoritmoMem metodo){
	if(paq->tipoMensaje == CATCH_POKEMON || paq->tipoMensaje == GET_POKEMON){
		if(yaEstaEnMemoria(paq)){
			log_info(brokerLogger2, "el msj ya esta");
			return;
		}
	}
	msgMemoriaBroker* msgNuevo = malloc(sizeof(msgMemoriaBroker));
	msgNuevo->cola          = paq->tipoMensaje;
	msgNuevo->idMensaje     = idMensaje;
	msgNuevo->subsACK       = inicializarListaMutex();
	msgNuevo->subsYaEnviado = inicializarListaMutex();
	msgNuevo->sizeStream 	= paq->sizeStream;
	msgNuevo->stream=paq->stream;
//	msgNuevo->stream 		= malloc(msgNuevo->sizeStream);
//	memcpy(msgNuevo->stream , paq->stream, msgNuevo->sizeStream);

	msgNuevo->modulo = paq->modulo;
	uint32_t sizePrevio;

	switch(metodo){
	case PARTICIONES_DINAMICAS:
		log_info(brokerLogger2, "ITERACION: %i", iteraciones);
		iteraciones = iteraciones + 1;
		pthread_mutex_lock(mutexMemoria);
//		particion* part = (particion*) getListaMutex(particionesLibres,0);
//		log_info(brokerLogger2, "Mi primera particion tiene offset: %i", part -> offset);
		registrarEnParticiones(msgNuevo);
		log_info(brokerLogger2, "Registre en particiones");
		pthread_mutex_unlock(mutexMemoria);
		break;
	case BUDDY_SYSTEM:
		pthread_mutex_lock(mutexMemoria);
		sizePrevio = sizeListaMutex(nodosOcupados);
		registrarEnMemoriaBUDDYSYSTEM(msgNuevo, nodoRaizMemoria);
		while (sizeListaMutex(nodosOcupados) == sizePrevio){
			elegirVictimaDeReemplazoYeliminarBD();
			sizePrevio = sizeListaMutex(nodosOcupados);
			registrarEnMemoriaBUDDYSYSTEM(msgNuevo, nodoRaizMemoria);
		}
		pthread_mutex_unlock(mutexMemoria);
		break;
	default:
		printf("error al guardar mensaje");
		free(msgNuevo);
		return;
	}
	log_info(brokerLogger2, "Registre el mensaje en memoria.");
	//free(msgNuevo);
}

bool estaEnListaACK(uint32_t socket, msgMemoriaBroker* mensaje){
	for(int i =0; i<sizeListaMutex(mensaje->subsACK); i++){
		if(*(uint32_t*)getListaMutex(mensaje->subsACK, i) == socket){ //tengo que cambiar el void* por un uint32_t
			return true;
		}
	}
	return false;
}

void guardarMensajeACK (paquete* paq, uint32_t socket){
	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(paq->idCorrelativo);
	if(mensaje == NULL){
		printf("No se encontró el mensaje en memoria, ERROR");
		return;
	}
	if(estaEnListaACK (socket, mensaje)){
		printf("Ya esta en la lista de ACK");
		return;
	}
	pthread_mutex_lock(mutexMemoria);
	addListaMutex(mensaje->subsACK, (void*) socket);
	pthread_mutex_unlock(mutexMemoria);
}

bool estaEnListaEnviados (uint32_t socket, msgMemoriaBroker* mensaje){
	log_info(brokerLogger2, "SOCKET: %i", socket);
	for(int i =0; i<sizeListaMutex(mensaje->subsYaEnviado); i++){
		log_info(brokerLogger2, "sizeStream: %i", mensaje->sizeStream);
		uint32_t* intActual=((uint32_t*)getListaMutex(mensaje->subsYaEnviado, i));
		log_info(brokerLogger2, "intActual: %i", intActual);
		if(*intActual == socket){ //tengo que cambiar el void* por un uint32_t
			log_info(brokerLogger2, "hola");
			return true;
		}
	}
	return false;
}

void guardarYaEnviados (paquete* paq, uint32_t socket){
	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(paq->id);
	log_info(brokerLogger2, "ENCIMA LOS VOLVIERON A VOTAR");
	if(mensaje == NULL){
		printf("No se encontró el mensaje en memoria, ERROR");
	}
//	else if(estaEnListaEnviados (socket, mensaje)){
//		printf("Ya esta en la lista de enviados");
//
//	}
	else{
		addListaMutex(mensaje->subsYaEnviado, (void*) socket);
	}



}

//void guardarEnListaMemoria(uint32_t idMensaje, uint32_t socket, ListasMemoria lista){
//	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(idMensaje);
//
//	if(mensaje == NULL){
//		printf("No se encontró el mensaje en memoria, ERROR");
//		return;
//	}
//
//	if( lista == CONFIRMADO){
//		if(estaEnLista(socket,lista, mensaje ))
//		{	printf("Ya está en la lista");
//			return;
//		}
//		pthread_mutex_lock(mutexMemoria);
//		addListaMutex(mensaje->subsYaEnviado, (void*) socket);
//		pthread_mutex_unlock(mutexMemoria);
//	}else if(lista == SUBSYAENVIADOS){
//		if(estaEnLista(socket,lista, mensaje ))
//		{
//			printf("Ya está en la lista");
//			return;
//		}
//		pthread_mutex_lock(mutexMemoria);
//		addListaMutex(mensaje->subsACK, (void*) socket);
//		pthread_mutex_unlock(mutexMemoria);
//	}
//}

//bool estaEnLista(uint32_t socket, ListasMemoria lista, msgMemoriaBroker* mensaje){
//	log_info(brokerLogger2, "Numero 5");
//	listaMutex* list = inicializarListaMutex();
//	if(lista == CONFIRMADO){
//		list =  mensaje->subsACK;
//	}else{
//		list =  mensaje->subsYaEnviado;
//	}
//
//	for(uint32_t i=0; i< sizeListaMutex(list); i++ ){
//		if(socket == (uint32_t)getListaMutex(list, i)){
//			free(list);
//			return true;
//		}
//	}
//
//	free(list);
//    return false;
//}


void asignarPuntero(uint32_t offset, void* stream, uint32_t sizeStream){
	log_info(brokerLogger2,"ALMACENO: mensaje en posición: %p (de memoria) -.-", 0 + offset);
	log_info(loggerBroker, "Se almacena mensaje en memoria en posición: %p -.-", 0 + offset);
	memcpy(memoria + offset, stream, sizeStream);
}

void crearDumpDeCache(){
	log_info(loggerBroker,"Se solicitó dump de cache.");
	log_info(brokerLogger2,"Se solicitó dump de cache.");

	iniciarArchivoMutex();
	if(algoritmoMemoria == BUDDY_SYSTEM){
		//recorrerArbolYgrabarArchivo();
	}else{
		registrarParticionesLibresYocupadas();
	}
}

bool yaEstaEnMemoria(paquete* paq){
	switch(paq->tipoMensaje){
	case CATCH_POKEMON:;
		mensajeCatch* msg=deserializarCatch(paq->stream);
		log_info(brokerLogger2, "POKEMOOOOOOOOOOOOOOON: %s", msg->pokemon);
		return yaSeGuardoEnMemoria(deserializarCatch(paq->stream), NULL);
		break;
	case GET_POKEMON:
		return yaSeGuardoEnMemoria(NULL, deserializarGet(paq->stream));
		break;
	default:
		printf("ERROR");
		return false;
	}
}

bool existeMensajeEnParticionesDinamicas(mensajeCatch* msgCatch, mensajeGet* msgGet){
	log_info(brokerLogger2, "entro a ver si existe mensaje");
	for(int i = 0; i < sizeListaMutex(particionesOcupadas); i++){
		particion* partActual = (particion*)getListaMutex (particionesOcupadas, i);

		mensajeCatch* msg=deserializarCatch(partActual->mensaje->stream);
		log_info(brokerLogger2, "PARTiCION DESERIALIZADA %s", msg->pokemon);
		if(msgCatch != NULL && partActual->mensaje->cola == CATCH_POKEMON){
			printf("A NISMAN LO MATARON\n");
			if(compararCatch(deserializarCatch(partActual->mensaje->stream), msgCatch)){
				printf("Catch\n");
				return true;
			}

		}else if(msgGet != NULL && partActual->mensaje->cola == GET_POKEMON){
			printf("A NISMAN LO MATARON\n");
			if(compararGet(deserializarGet(partActual->mensaje->stream), msgGet)){
				printf("get\n");
				return true;
			}

		}
	}

	printf("CRISTINA KIRCHNER\n");
	return false;
}

//bool yaSeGuardoEnMemoria(paquete* paq){
//	switch(paq->tipoMensaje){
//
//	}
//}

bool yaSeGuardoEnMemoria(mensajeCatch* msgCatch, mensajeGet* msgGet){
	switch(algoritmoMemoria){
	case BUDDY_SYSTEM:
		return existeMensajeEnMemoriaBuddy(msgGet, msgCatch);
	case PARTICIONES_DINAMICAS:
		return existeMensajeEnParticionesDinamicas(msgCatch, msgGet);
	}
	return false;
}

//	if(algoritmoMemoria == BUDDY_SYSTEM){
//		if(msgCatch != NULL){
//			return existeMensajeEnMemoriaBuddy(NULL, msgCatch);
//		}else{
//			return existeMensajeEnMemoriaBuddy(msgGet,NULL);
//		}
//	}else{
//		for(int i=0; i<sizeListaMutex(particionesOcupadas); i++){
//		  partOcupada = getListaMutex(particionesOcupadas, i);
//		  if(partOcupada->mensaje->cola == CATCH_POKEMON && msgCatch != NULL){
//			  if(compararCatch(deserializarCatch(partOcupada->mensaje->stream), msgCatch))
//				  return true;
//		  }else if(partOcupada->mensaje->cola == GET_POKEMON && msgGet != NULL){
//			  if( compararGet(deserializarGet(partOcupada->mensaje->stream), msgGet))
//				  return true;
//		  }
//		}
//		return false;
//	  }
//}

bool compararCatch(mensajeCatch*  elemLista, mensajeCatch*  msgCatch){
	if(strcmp(elemLista->pokemon, msgCatch->pokemon) == 0){
		if(elemLista->posX == msgCatch->posX && elemLista->posY == msgCatch->posY){

			return true;
		}
	}

		return false;
}

bool compararGet(mensajeGet* elemLista, mensajeGet* msgGet){
	if(strcmp(elemLista->pokemon, msgGet->pokemon) == 0){
		return true;
	}
	return false;
}

msgMemoriaBroker* buscarMensajeEnMemoria(uint32_t idMensajeBuscado){
	switch(algoritmoMemoria){
	case BUDDY_SYSTEM:
		return buscarMensajeEnMemoriaBuddy(idMensajeBuscado);
	case PARTICIONES_DINAMICAS:
		return buscarMensajeEnMemoriaParticiones(idMensajeBuscado);
	}
	return NULL;
}

void enviarMensajesPreviosEnMemoria(uint32_t* socket, uint32_t cola){
	switch(algoritmoMemoria){
	case BUDDY_SYSTEM:
		enviarMsjsASuscriptorNuevoBuddySystem (cola, socket);
		break;
	case PARTICIONES_DINAMICAS:
		enviarMsjsASuscriptorNuevoParticiones (cola, socket);
		break;
	default:
		printf("Error en algoritmo memoria");
	}
}
