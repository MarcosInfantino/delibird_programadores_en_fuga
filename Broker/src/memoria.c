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

void registrarMensajeEnMemoria(paquete* paq, algoritmoMem metodo){
	if(paq->tipoMensaje == CATCH_POKEMON || paq->tipoMensaje == GET_POKEMON){
		if(yaEstaEnMemoria(paq)){
			log_info(brokerLogger2, "El mensaje ya se encontraba en memoria, no se registra.");
			return;
		}
	}
	msgMemoriaBroker* msgNuevo = malloc(sizeof(msgMemoriaBroker));
	msgNuevo->cola          = paq->tipoMensaje;
	msgNuevo->idMensaje     = paq->id;
	msgNuevo->idCorrelativo = paq->idCorrelativo;
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
		registrarEnParticiones(msgNuevo);
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
	log_info(brokerLogger2, "Registré el mensaje en memoria.");
}

bool estaEnListaACK(uint32_t idProceso, msgMemoriaBroker* mensaje){
	for(int i =0; i<sizeListaMutex(mensaje->subsACK); i++){
		uint32_t actual=*((uint32_t*)getListaMutex(mensaje->subsACK, i));
		if(actual == idProceso){
			return true;
		}
	}
	return false;
}

void guardarMensajeACK (paquete* paq){
	uint32_t* idProceso=malloc(sizeof(uint32_t));
	*idProceso=obtenerIdProcesoDeAck(paq->stream);
	log_info(brokerLogger2, "----------------------Guardo ACK del proceso %i",obtenerIdProcesoDeAck(paq->stream) );
	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(paq->idCorrelativo);
	if(mensaje == NULL){
		printf("No se encontró el mensaje en memoria, ERROR");
		return;
	}

//	if(estaEnListaACK (socket, mensaje)){//esto en realidad no puede pasar
//		printf("El sub ya está en la lista de ACK");
//		return;
//	}
	pthread_mutex_lock(mutexMemoria);
	addListaMutex(mensaje->subsACK, (void*) idProceso);
	pthread_mutex_unlock(mutexMemoria);
}

//DEPRECATED
//bool estaEnListaEnviados (uint32_t socket, msgMemoriaBroker* mensaje){
//	log_info(brokerLogger2, "SOCKET: %i", socket);
//	for(int i =0; i<sizeListaMutex(mensaje->subsYaEnviado); i++){
//		log_info(brokerLogger2, "sizeStream: %i", mensaje->sizeStream);
//		uint32_t* intActual=((uint32_t*)getListaMutex(mensaje->subsYaEnviado, i));
//		log_info(brokerLogger2, "intActual: %i", intActual);
//		if(*intActual == socket){ //tengo que cambiar el void* por un uint32_t
//			return true;
//		}
//	}
//	return false;
//}

void guardarYaEnviados (paquete* paq, uint32_t idProceso){
	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(paq->id);
	if(mensaje == NULL){
		printf("No se encontró el mensaje en memoria, ERROR");
	}
	else{
		uint32_t* idProcesoAgregar=malloc(sizeof(uint32_t));
		*idProcesoAgregar=idProceso;
		addListaMutex(mensaje->subsYaEnviado, (void*) idProcesoAgregar);
	}



}

//void guardarEnListaMemoria(uint32_t idMensaje, uint32_t socket, ListasMemoria lista){
//	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(idMensaje);
//
//	if(mensaje == NULL){
//		printf("No se encontró el mensaje en memoria, ERROR");
//		return;
//	}

//if(estaEnLista(socket,lista, mensaje ))
//		{	printf("Ya está en la lista");
//			return;
//		}
//
//	if( lista == CONFIRMADO){
//
//		pthread_mutex_lock(mutexMemoria);
//		addListaMutex(mensaje->subsYaEnviado, (void*) socket);
//		pthread_mutex_unlock(mutexMemoria);
//	}else if(lista == SUBSYAENVIADOS){
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
	log_info(brokerLogger2,"ALMACENO mensaje en posición: %p (de memoria) -.-", 0 + offset);
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
		log_info(brokerLogger2, "Busco si Pokemon: %s ya está en memoria", msg->pokemon);
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
	log_info(brokerLogger2, "Veo si existe mensaje en particiones.");
	for(int i = 0; i < sizeListaMutex(particionesOcupadas); i++){
		particion* partActual = (particion*)getListaMutex (particionesOcupadas, i);

		mensajeCatch* msg = deserializarCatch(partActual->mensaje->stream);
		log_info(brokerLogger2, "Partición deserializada %s", msg->pokemon); //TODO no está mostrando el nombre del pokemon
		if(msgCatch != NULL && partActual->mensaje->cola == CATCH_POKEMON){
			if(compararCatch(deserializarCatch(partActual->mensaje->stream), msgCatch)){
				return true;
			}
		}else if(msgGet != NULL && partActual->mensaje->cola == GET_POKEMON){
			if(compararGet(deserializarGet(partActual->mensaje->stream), msgGet)){
				return true;
			}
		}
	}
	return false;
}


bool yaSeGuardoEnMemoria(mensajeCatch* msgCatch, mensajeGet* msgGet){
	switch(algoritmoMemoria){
	case BUDDY_SYSTEM:
		return existeMensajeEnMemoriaBuddy(msgGet, msgCatch);
	case PARTICIONES_DINAMICAS:
		return existeMensajeEnParticionesDinamicas(msgCatch, msgGet);
	}
	return false;
}


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

void enviarMensajesPreviosEnMemoria(uint32_t socket, uint32_t idProceso, uint32_t cola){
	switch(algoritmoMemoria){
	case BUDDY_SYSTEM:
		enviarMsjsASuscriptorNuevoBuddySystem (cola, socket, idProceso);
		break;
	case PARTICIONES_DINAMICAS:
		//TODO: Revisar esto, fijarse en buddy tema id correlativos y id del mensaje
		enviarMsjsASuscriptorNuevoParticiones (cola,socket,  idProceso);
		break;
	default:
		printf("Error en algoritmo memoria");
	}
}
