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

void registrarEnMemoriaPARTICIONES(msgMemoriaBroker* mensajeNuevo){

}

void registrarEnMemoriaBUDDYSYSTEM(msgMemoriaBroker* mensajeNuevo, struct nodoMemoria* partActual){
	struct nodoMemoria* backUp = partActual;
	if(intentarRamaIzquierda(mensajeNuevo, partActual) < 0){
		partActual = backUp->hijoDer;
		registrarEnMemoriaBUDDYSYSTEM(mensajeNuevo, partActual);
	}
}

//uint32_t intentarRamaIzquierda(msgMemoriaBroker* mensajeNuevo, struct nodoMemoria* partActual){
//	if(noEsParticionMinima(partActual) && !estaLibre(partActual)){
//		if(!entraEnLaMitad(partActual, mensajeNuevo))
//			return -1;
//		if( partActual->hijoIzq->header.status == OCUPADO){
//			return intentarRamaIzquierda(mensajeNuevo, partActual->hijoDer);
//		}else{
//			return intentarRamaIzquierda(mensajeNuevo, partActual->hijoIzq);
//		}
//	}else{
//		return evaluarTamanioParticionYasignar(partActual, mensajeNuevo);
//	}
//
//}
bool ambosHijosOcupados(struct nodoMemoria* padre){
	return estaOcupado(padre->hijoDer) && estaOcupado(padre->hijoIzq);
}
bool esHijoDerecho(struct nodoMemoria* particion){
	return particion==(particion->padre->hijoDer);
}
uint32_t intentarRamaIzquierda(msgMemoriaBroker* mensajeNuevo, struct nodoMemoria* partActual){
//	if(estaParticionado(partActual) && ambosHijosOcupados(partActual->padre)){
//		return intentarRamaIzquierda(mensajeNuevo, partActual->padre->padre->hijoDer);
//	}else
		if(estaOcupado(partActual)){
		log_info(brokerLogger2,"Encontre un nodo ocupado");
		if(esHijoDerecho(partActual)){
			return intentarRamaIzquierda(mensajeNuevo, partActual->padre->padre->hijoDer);
		}else{
			return intentarRamaIzquierda(mensajeNuevo, partActual->padre->hijoDer);
		}

		}else if(!esParticionMinima(partActual) && estaParticionado(partActual)){

		if(intentarRamaIzquierda(mensajeNuevo, partActual->hijoIzq)>0){

			log_info(brokerLogger2,"Rama exitosa encontrada. Tamaño particion actual: %i", (partActual->header).size);
			return 1;//entramos en la rama izquierda
		}else{
			log_info(brokerLogger2,"Paso a la derecha");
			return intentarRamaIzquierda(mensajeNuevo, partActual->hijoDer); //fallo la rama izquierda => intento por la derecha
		}

	}else if (estaLibre(partActual)){// tanto si es minima como si no
		return evaluarTamanioParticionYasignar(partActual, mensajeNuevo); //la particion actual esta libre
	}else{
		log_info(brokerLogger2,"Hola como andas.");
		return intentarRamaIzquierda(mensajeNuevo, partActual->padre->hijoDer);
	}

}

bool estaOcupado(struct nodoMemoria* partActual){
	return (partActual->header).status==OCUPADO;
}

bool estaParticionado(struct nodoMemoria* partActual){
	return (partActual->header).status==PARTICIONADO;
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

uint32_t tamanioMinimo(struct nodoMemoria* partActual){
	return (partActual->header).size;
}

uint32_t evaluarTamanioParticionYasignar(struct nodoMemoria* partActual, msgMemoriaBroker* msg){
	log_info(brokerLogger2,"Entre a validar.");
	uint32_t tamanioMsg = sizeof(paquete);
	log_info(brokerLogger2, "Comienzo a evaluar el tamaño de la particion y asignar.");
	if(tamanioParticion(partActual) >= tamanioMsg){
		while(tamanioParticion(partActual)/2 >= tamanioMsg && tamanioParticion(partActual)/2>=particionMinima){
			particionarMemoriaBUDDY(partActual);
			partActual->header.status = PARTICIONADO;
			partActual = partActual->hijoIzq;
		}
		partActual->header.status = OCUPADO;
		partActual->mensaje = msg;
		log_info(brokerLogger2,"Size: %i. Id mensaje: %i. Size del mensaje: %i.", (partActual->header).size, partActual->mensaje->idMensaje,sizePaquete(partActual->mensaje->paq));
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

msgMemoriaBroker* buscarMensajeEnMemoriaBuddy(uint32_t id){

	pthread_mutex_lock(mutexMemoria);
	struct nodoMemoria* nodoActual = nodoRaizMemoria;
	//struct nodoMemoria* backUp = nodoActual;

	if(estaLibre(nodoActual)) {
		pthread_mutex_unlock(mutexMemoria);
		return NULL;}

	if(nodoActual->mensaje->idMensaje != id){
		msgMemoriaBroker* mensajeEnRamaIzq=buscarPorRama(id, nodoActual->hijoIzq);
		msgMemoriaBroker* mensajeEnRamaDer;

		if(mensajeEnRamaIzq == NULL){

			mensajeEnRamaDer=buscarPorRama(id, nodoActual->hijoDer);
			pthread_mutex_unlock(mutexMemoria);
			return mensajeEnRamaDer;

		}

		pthread_mutex_unlock(mutexMemoria);
		return mensajeEnRamaIzq;

	}

	pthread_mutex_unlock(mutexMemoria);
	return nodoActual->mensaje;

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

msgMemoriaBroker* buscarPorRama(uint32_t id, struct nodoMemoria* nodoActual ){
	if(estaParticionado(nodoActual)){
		msgMemoriaBroker* mensajeEnRamaIzq=buscarPorRama(id, nodoActual->hijoIzq);

				if(mensajeEnRamaIzq == NULL){

					msgMemoriaBroker* mensajeEnRamaDer=buscarPorRama(id, nodoActual->hijoDer);
					return mensajeEnRamaDer;

				}

				return mensajeEnRamaIzq;

	}else if(estaOcupado(nodoActual) && (nodoActual->mensaje->idMensaje)==id ){
		return nodoActual->mensaje;
	}else {
		return NULL;
	}

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

bool esParticionMinima(struct nodoMemoria* particion){

	return (particion->header.size == particionMinima);
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
