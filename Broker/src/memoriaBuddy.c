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

void registrarEnMemoriaBUDDYSYSTEM(msgMemoriaBroker* mensajeNuevo, struct nodoMemoria* partActual){
	struct nodoMemoria* backUp = partActual;
	if(intentarRamaIzquierda(mensajeNuevo, partActual) < 0){
		partActual = backUp->hijoDer;
		registrarEnMemoriaBUDDYSYSTEM(mensajeNuevo, partActual);
	}
}

uint32_t intentarRamaIzquierda(msgMemoriaBroker* mensajeNuevo,struct nodoMemoria* partActual){
	if (estaOcupado(partActual)){
	return -1;
	} else if (estaParticionado(partActual)){
		uint32_t retorno = intentarRamaIzquierda(mensajeNuevo,partActual->hijoIzq);
		if (retorno != 1)
			return intentarRamaIzquierda(mensajeNuevo,partActual->hijoDer);
		return retorno;
	}else{
		return evaluarTamanioParticionYasignar (partActual, mensajeNuevo);
	}
}

bool ambosHijosOcupados(struct nodoMemoria* padre){
	return estaOcupado(padre->hijoDer) && estaOcupado(padre->hijoIzq);
}

bool esHijoDerecho(struct nodoMemoria* particion){
	return particion==(particion->padre->hijoDer);
}

bool estaOcupado(struct nodoMemoria* partActual){
	return (partActual->header).status==OCUPADO;
}

bool estaParticionado(struct nodoMemoria* partActual){
	return (partActual->header).status==PARTICIONADO;
}
bool entraEnLaMitad(struct nodoMemoria* partActual, msgMemoriaBroker* mensajeNuevo){
	return mensajeNuevo->sizeStream <= tamanioParticion(partActual);
}

//	if(noEsParticionMinima(partActual) && !estaLibre(partActual)){ //o irá solo estado particionado?
//		if( partActual->hijoIzq->header.status == OCUPADO){
//			registrarEnMemoriaBUDDYSYSTEM(mensajeNuevo, partActual->hijoDer);
//		}else{
//			registrarEnMemoriaBUDDYSYSTEM(mensajeNuevo, partActual->hijoIzq);}
//	}else{
//		evaluarTamanioParticion(partActual, mensajeNuevo);
//	}

uint32_t tamanioMinimo(struct nodoMemoria* partActual){
	return (partActual->header).size;
}

uint32_t evaluarTamanioParticionYasignar(struct nodoMemoria* partActual, msgMemoriaBroker* msg){
	log_info(brokerLogger2,"Entre a particionar.");

	uint32_t tamanioMsg = msg->sizeStream;

	log_info(brokerLogger2, "Comienzo a evaluar el tamaño de la particion y asignar.");
	if(tamanioParticion(partActual) >= tamanioMsg){
		while(tamanioParticion(partActual)/2 >= tamanioMsg && tamanioParticion(partActual)/2>=particionMinima){
			particionarMemoriaBUDDY(partActual);
			partActual->header.status = PARTICIONADO;
			partActual = partActual->hijoIzq;
		}
		partActual->header.status = OCUPADO;
		partActual->mensaje = msg;
		time_t t;
		t=time(NULL);
		partActual->header.tiempo = *localtime(&t);
		asignarPuntero(partActual->offset, partActual->mensaje->stream, partActual->mensaje->sizeStream);
		log_info(brokerLogger2,"ASIGNE: Size: %i. Id mensaje: %i. Size del mensaje: %i.", (partActual->header).size, partActual->mensaje->idMensaje, partActual->mensaje->sizeStream);
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

	particionActual->hijoIzq->padre   = particionActual;
	particionActual->hijoDer->padre   = particionActual;

	particionActual->hijoIzq->offset = particionActual->offset;
	particionActual->hijoDer->offset = particionActual->offset + tamanioParticion(particionActual)/2;

	particionActual->header.status = PARTICIONADO;


}

msgMemoriaBroker* buscarMensajeEnMemoriaBuddy(uint32_t id){
	pthread_mutex_lock(mutexMemoria);
	//struct nodoMemoria* backUp = nodoActual;
	struct nodoMemoria* nodoActual = nodoRaizMemoria;

	if(estaLibre(nodoActual)) {
		pthread_mutex_unlock(mutexMemoria);
		return NULL;}

	if(nodoActual->mensaje->idMensaje != id){
		msgMemoriaBroker* mensajeEnRamaIzq = buscarPorRama(id, nodoActual->hijoIzq);
		msgMemoriaBroker* mensajeEnRamaDer;

		if(mensajeEnRamaIzq == NULL){
			mensajeEnRamaDer = buscarPorRama(id, nodoActual->hijoDer);
			pthread_mutex_unlock(mutexMemoria);
			return mensajeEnRamaDer;
		}
		pthread_mutex_unlock(mutexMemoria);
		return mensajeEnRamaIzq;
	}
	pthread_mutex_unlock(mutexMemoria);
	return nodoActual->mensaje;
}

bool yaEstaEnMemoriaBuddy(mensajeGet* msgGet, mensajeCatch*  msgCatch){
	pthread_mutex_lock(mutexMemoria);
	struct nodoMemoria* nodoActual = nodoRaizMemoria;

	if(estaLibre(nodoActual)) {
		pthread_mutex_unlock(mutexMemoria);
		return false;}

	if(msgGet == NULL){
		if(compararCatch(deserializarCatch(nodoActual->mensaje->stream),msgCatch) == 0){
		msgMemoriaBroker* mensajeEnRamaIzq = buscarPorRama(id, nodoActual->hijoIzq); //todo modificar el buscar por rama
		msgMemoriaBroker* mensajeEnRamaDer;

		if(mensajeEnRamaIzq == NULL){

			mensajeEnRamaDer = buscarPorRama(id, nodoActual->hijoDer);
			pthread_mutex_unlock(mutexMemoria);
			return compararCatch(deserializarCatch(mensajeEnRamaDer->stream), msgCatch);
		}
		pthread_mutex_unlock(mutexMemoria);
		return compararCatch(deserializarCatch(mensajeEnRamaIzq->stream), msgCatch);
	}
	pthread_mutex_unlock(mutexMemoria);
	return true;

	}else{
		if(compararGet(deserializarGet(nodoActual->mensaje->stream),msgGet) == 0){
			msgMemoriaBroker* mensajeEnRamaIzq = buscarPorRama(id, nodoActual->hijoIzq); //todo modificar el buscar por rama
			msgMemoriaBroker* mensajeEnRamaDer;

					if(mensajeEnRamaIzq == NULL){

						mensajeEnRamaDer = buscarPorRama(id, nodoActual->hijoDer);
						pthread_mutex_unlock(mutexMemoria);
						return compararGet(deserializarGet(mensajeEnRamaDer->stream), msgGet);
					}
					pthread_mutex_unlock(mutexMemoria);
					return compararGet(deserializarGet(mensajeEnRamaIzq->stream), msgGet);
				}
				pthread_mutex_unlock(mutexMemoria);
				return true;
	}
}

msgMemoriaBroker* buscarPorRama(uint32_t id, struct nodoMemoria* partActual ){
	if (estaOcupado(partActual) && (partActual->mensaje->idMensaje)==id){
		return partActual->mensaje;
	}else if (estaParticionado(partActual)){
		msgMemoriaBroker* retorno = buscarPorRama(id,partActual->hijoIzq);
		if (retorno == NULL)
			return buscarPorRama(id,partActual->hijoDer);
		return retorno;
	}else{
		return NULL;
	}
}

struct nodoMemoria* crearRaizArbol(void){
	struct nodoMemoria* nodoRaiz = inicializarNodo();    //no estoy liberando malloc
	nodoRaiz->header.size   = tamMemoria;
	nodoRaiz->header.status = LIBRE;
	nodoRaiz->offset = 0;

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

bool esParticionMinima(struct nodoMemoria* particion){

	return (particion->header.size == particionMinima);
}

bool estaLibre(struct nodoMemoria* particion){
	return particion->header.status == LIBRE;
}

uint32_t tamanioParticion(struct nodoMemoria* part){
	return part->header.size;
}
