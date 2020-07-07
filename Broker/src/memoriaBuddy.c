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
	uint32_t response = intentarRamaIzquierda(mensajeNuevo, partActual);

	if(response == 0){
		partActual = backUp->hijoDer;
		registrarEnMemoriaBUDDYSYSTEM(mensajeNuevo, partActual);
	}

	if(response == 0){ //TODO VER que condición poner para que entre acá
		elegirVictimaDeReemplazoYeliminarBD();
		registrarEnMemoriaBUDDYSYSTEM(mensajeNuevo, nodoRaizMemoria);
	}
}

void elegirVictimaDeReemplazoYeliminarBD(){
	struct nodoMemoria* victima;

	if(algoritmoReemplazo == FIFO){
	 victima = buscarVictimaPor(tiempoDeCargaMenor);
	}else{ //LRU
	 victima = buscarVictimaPor(tiempoDeUsoMenor);
	}
	modificarNodoAlibre(victima);
}

struct nodoMemoria* buscarVictimaPor(bool(*condition)(struct nodoMemoria*,struct nodoMemoria*)){
	struct nodoMemoria* minimo = getListaMutex(nodosOcupados, 0);
	struct nodoMemoria* aux;

	for(int i=1; i<sizeListaMutex(nodosOcupados);i++){
		aux = getListaMutex(nodosOcupados, i);

		if(condition(aux,minimo)){
			minimo = aux;
		}
	}
	return minimo;
}

void modificarNodoAlibre(struct nodoMemoria* victima){
	victima->header.status = LIBRE;

	removerDeListaOcupados(victima);
	evaluarConsolidacion(victima);
}

void evaluarConsolidacion(struct nodoMemoria* nodo){
	struct nodoMemoria* buddie;

	if(nodo->padre->hijoDer == nodo){
		buddie = nodo->padre->hijoIzq;
	}else{
		buddie = nodo->padre->hijoDer;
	}

	if(estaLibre(buddie)){
		nodo->padre->header.status = LIBRE;
		liberarNodo(nodo);
		liberarNodo(buddie);
		evaluarConsolidacion(nodo->padre);
	}else{return;}

}

uint32_t intentarRamaIzquierda(msgMemoriaBroker* mensajeNuevo,struct nodoMemoria* partActual){
	if (estaOcupado(partActual)){
	return 0;
	} else if (estaParticionado(partActual)){
		uint32_t retorno = intentarRamaIzquierda(mensajeNuevo,partActual->hijoIzq);
		if (retorno != 1)
			return intentarRamaIzquierda(mensajeNuevo,partActual->hijoDer);
		return retorno;
	}else{
		return evaluarTamanioParticionYasignar (partActual, mensajeNuevo);
	}
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

		addListaMutex(nodosOcupados,partActual);

		time_t t;
		t=time(NULL);
		partActual->header.tiempoDeCarga = *localtime(&t);
		asignarPuntero(partActual->offset, partActual->mensaje->stream, partActual->mensaje->sizeStream);
		log_info(brokerLogger2,"ASIGNE: Size: %i. Id mensaje: %i. Size del mensaje: %i.", (partActual->header).size, partActual->mensaje->idMensaje, partActual->mensaje->sizeStream);
		return 1;
	}

	return 0;
}

uint32_t tamanioMinimo(struct nodoMemoria* partActual){
	return (partActual->header).size;}


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

	time_t t;
	t=time(NULL);
	nodoActual->header.ultimoAcceso = *localtime(&t); //actualizo la fecha de acceso

	return nodoActual->mensaje;
}

bool existeMensajeEnMemoriaBuddy(mensajeGet* msgGet, mensajeCatch*  msgCatch){
	//struct nodoMemoria* backUp = nodoActual;
	pthread_mutex_lock(mutexMemoria);
	struct nodoMemoria* nodoActual = nodoRaizMemoria;

	if(estaLibre(nodoActual)) {
		pthread_mutex_unlock(mutexMemoria);
		return NULL;}

	if(msgGet != NULL){
		if(!compararGet(deserializarGet(nodoActual->mensaje->stream),msgGet)){
				bool mensajeEnRamaIzq = buscarPorRamaGet(msgGet, nodoActual->hijoIzq);
				bool mensajeEnRamaDer;

				if(!mensajeEnRamaIzq){
					mensajeEnRamaDer = buscarPorRamaGet(msgGet, nodoActual->hijoDer);
					pthread_mutex_unlock(mutexMemoria);
					return mensajeEnRamaDer;
				}
				pthread_mutex_unlock(mutexMemoria);
				return mensajeEnRamaIzq;
		}
	}else if(msgCatch != NULL){
		if(!compararCatch(deserializarCatch(nodoActual->mensaje->stream),msgCatch)){
				bool mensajeEnRamaIzq = buscarPorRamaCatch(msgCatch, nodoActual->hijoIzq);
				bool mensajeEnRamaDer;

				if(!mensajeEnRamaIzq){
					mensajeEnRamaDer = buscarPorRamaCatch(msgCatch, nodoActual->hijoDer);
					pthread_mutex_unlock(mutexMemoria);
					return mensajeEnRamaDer;
				}
				pthread_mutex_unlock(mutexMemoria);
				return mensajeEnRamaIzq;
		}
	}
	pthread_mutex_unlock(mutexMemoria);
	return true;
}

bool buscarPorRamaGet(mensajeGet* msgGet, struct nodoMemoria* partActual ){
	if (estaOcupado(partActual) && compararGet(deserializarGet(partActual->mensaje->stream), msgGet)){
		return partActual->mensaje;
	}else if (estaParticionado(partActual)){
		bool retorno = buscarPorRamaGet(msgGet,partActual->hijoIzq);
		if (!retorno)
			return buscarPorRamaGet(msgGet,partActual->hijoDer);
		return retorno;
	}else{
		return false;
	}
}

bool buscarPorRamaCatch(mensajeCatch* msgCatch, struct nodoMemoria* partActual ){
	if (estaOcupado(partActual) && compararCatch(deserializarCatch(partActual->mensaje->stream), msgCatch)){
		return partActual->mensaje;
	}else if (estaParticionado(partActual)){
		bool retorno = buscarPorRamaCatch(msgCatch,partActual->hijoIzq);
		if (!retorno)
			return buscarPorRamaCatch(msgCatch,partActual->hijoDer);
		return retorno;
	}else{
		return false;
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

void removerDeListaOcupados(struct nodoMemoria* nodo){
	struct nodoMemoria* aux;

	for(int i=0; i<sizeListaMutex(nodosOcupados);i++){
		aux = getListaMutex(nodosOcupados, i);
		if(aux == nodo){
			removeListaMutex(nodosOcupados,i);
		}
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

bool esParticionMinima(struct nodoMemoria* particion){

	return (particion->header.size == particionMinima);
}

bool estaLibre(struct nodoMemoria* particion){
	return particion->header.status == LIBRE;
}

uint32_t tamanioParticion(struct nodoMemoria* part){
	return part->header.size;
}

struct tm tiempoCarga(struct nodoMemoria* nodo){
	return nodo->header.tiempoDeCarga;
}

struct tm tiempoUso(struct nodoMemoria* nodo){
	return nodo->header.ultimoAcceso;
}

bool tiempoDeCargaMenor(struct nodoMemoria* nodo, struct nodoMemoria* otroNodo){
	return tiempoCarga(nodo).tm_hour < tiempoCarga(otroNodo).tm_hour
			&& tiempoCarga(nodo).tm_min < tiempoCarga(otroNodo).tm_min
			&& tiempoCarga(nodo).tm_sec < tiempoCarga(otroNodo).tm_sec;
}

bool tiempoDeUsoMenor(struct nodoMemoria* nodo, struct nodoMemoria* otroNodo){
	return tiempoUso(nodo).tm_hour < tiempoUso(otroNodo).tm_hour
			&& tiempoUso(nodo).tm_min < tiempoUso(otroNodo).tm_min
			&& tiempoUso(nodo).tm_sec < tiempoUso(otroNodo).tm_sec;
}
