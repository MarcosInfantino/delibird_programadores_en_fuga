/*
 * memoria.c
 *
 *  Created on: 29 may. 2020
 *      Author: utnso
 */

#include "broker.h"
#include "memoria.h"
#include "memoriaParticiones.h"
#include "files.h"

void definirComienzoDeMemoria(){
	memoria = malloc(tamMemoria);
	switch(algoritmoMemoria){
	case BUDDY_SYSTEM:
		nodoRaizMemoria = crearRaizArbol();
		nodoRaizMemoria->offset = 0;
		nodosOcupados = inicializarListaMutex();
		nodosLibres   = inicializarListaMutex();

		addListaMutex(nodosLibres, nodoRaizMemoria);
		log_info(brokerLogger2,"agrego a la lista de libres a la raiz: status %i",nodoRaizMemoria->header.status);
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
	for(int i = 0; i<sizeListaMutex(mensaje->subsACK); i++){
		uint32_t actual=*((uint32_t*)getListaMutex(mensaje->subsACK, i));
		if(actual == idProceso){
			return true;
		}
	}
	return false;
}

void guardarMensajeACK (paquete* paq){
	uint32_t* idProceso = malloc(sizeof(uint32_t));
	*idProceso = obtenerIdProcesoDeAck(paq->stream);

	log_info(loggerBroker, "Me llegó el ACK del proceso de id %i (Mensaje %i).",obtenerIdProcesoDeAck(paq->stream), paq->idCorrelativo);

	log_info(brokerLogger2, "----------------------Guardo ACK del proceso %i",obtenerIdProcesoDeAck(paq->stream) );
	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(paq->idCorrelativo);
	if(mensaje == NULL){
		printf("No se encontró el mensaje en memoria, ERROR");
		return;
	}
	pthread_mutex_lock(mutexMemoria);
	addListaMutex(mensaje->subsACK, (void*) idProceso);
	pthread_mutex_unlock(mutexMemoria);
}

void guardarYaEnviados (paquete* paq, uint32_t idProceso){
	msgMemoriaBroker* mensaje = buscarMensajeEnMemoria(paq->id);
	if(mensaje == NULL){
		printf("No se encontró el mensaje en memoria, ERROR");
	}
	else{
		uint32_t* idProcesoAgregar = malloc(sizeof(uint32_t));
		*idProcesoAgregar = idProceso;
		addListaMutex(mensaje->subsYaEnviado, (void*) idProcesoAgregar);
	}
}


void asignarPuntero(uint32_t offset, void* stream, uint32_t sizeStream){
	log_info(brokerLogger2,"ALMACENO mensaje en posición: %itim (de memoria)", offset);
	log_info(loggerBroker, "Se almacena mensaje en memoria en posición: %i", offset);
	memcpy(memoria + offset, stream, sizeStream);
}

void crearDumpDeCache(){
	log_info(loggerBroker,"Se solicitó dump de cache.");
	log_info(brokerLogger2,"Se solicitó dump de cache.");
	iniciarEscrituraDump();


}

bool yaEstaEnMemoria(paquete* paq){
	switch(paq->tipoMensaje){
	case CATCH_POKEMON:;

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
		enviarMsjsASuscriptorNuevoParticiones (cola,socket,  idProceso);
		break;
	default:
		printf("Error en algoritmo memoria");
	}
}


bool lruNodos(struct nodoMemoria* nodo1, struct nodoMemoria* nodo2){
	return menorTiempo((nodo1->header).ultimoAcceso,(nodo2->header).ultimoAcceso );
}

bool fifoNodos(struct nodoMemoria* nodo1, struct nodoMemoria* nodo2){
	return menorTiempo((nodo1->header).tiempoDeCarga,(nodo2->header).tiempoDeCarga );
}

bool menorTiempo(char* tiempo1, char* tiempo2){
	char** time1Separado = string_split(tiempo1, ":");
	char** time2Separado = string_split(tiempo2, ":");

	uint32_t hora1 = atoi(time1Separado[0]);
	uint32_t hora2 = atoi(time2Separado[0]);

	uint32_t min1 = atoi(time1Separado[1]);
	uint32_t min2 = atoi(time2Separado[1]);

	uint32_t sec1 = atoi(time1Separado[2]);
	uint32_t sec2 = atoi(time2Separado[2]);

	uint32_t ms1 = atoi(time1Separado[3]);
	uint32_t ms2 = atoi(time2Separado[3]);

	if(hora1<hora2){
		return true;
	}else if((hora1==hora2) && (min1<min2)){
		return true;
	}else if((hora1==hora2) && (min1==min2) && (sec1<sec2)){
		return true;
	}else if((hora1==hora2) && (min1==min2) && (sec1==sec2) && (ms1<ms2)){
		return true;
	}
	return false;
}
