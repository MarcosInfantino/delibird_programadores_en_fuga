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


/*uint32_t intentarRamaIzquierda(msgMemoriaBroker* mensajeNuevo, struct nodoMemoria* partActual){
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

}*/

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

/* msgMemoriaBroker* buscarPorRama(uint32_t id, struct nodoMemoria* partActual ){
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
} */


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

bool esParticionMinima(struct nodoMemoria* particion){

	return (particion->header.size == particionMinima);
}

bool estaLibre(struct nodoMemoria* particion){
	return particion->header.status == LIBRE;
}

uint32_t tamanioParticion(struct nodoMemoria* part){
	return part->header.size;
}
