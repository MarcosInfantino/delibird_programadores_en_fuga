/*
 * memoria.h
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_
#include <commons/collections/queue.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <commons/config.h>
#include <commons/log.h>
#include <messages_lib/messages_lib.h>
#include "broker.h"

typedef struct {
	uint32_t idMensaje;
	uint32_t cola;
	listaMutex* subsYaEnviado;
	listaMutex* subsACK;
	paquete* paq;
}msgMemoriaBroker;

typedef enum{
	LIBRE,
	OCUPADO,
	PARTICIONADO
}nodeStatus;

typedef struct{
	nodeStatus status;
	uint32_t size;
}nodeData;

struct nodoMemoria {
	nodeData header;
	msgMemoriaBroker* mensaje;
	struct nodoMemoria* hijoIzq;
	struct nodoMemoria* hijoDer;
	struct nodoMemoria* padre;  //agregué esto no estoy segura
};

typedef enum{
	PARTICIONES_DINAMICAS,
	BUDDY_SYSTEM
}algoritmoMem;

typedef enum{
	FIRST_FIT,
	BEST_FIT
}algoritmoParticiones;

typedef enum{
	FIFO,
	LRU
}algoritmoReemp;

typedef enum {
	CONFIRMADO,
	SUBSYAENVIADOS
}ListasMemoria;

typedef struct{
	t_config* config;
	char* configAtributo;
	char* OPCION1;
	char* OPCION2;
	uint32_t OP1;
	uint32_t OP2;
	char* error;
} algoritmoParameter;

struct nodoMemoria* nodoRaizMemoria;
listaMutex* memoriaPARTICIONES;

//void enviarMensajesPreviosEnMemoria(uint32_t socket,uint32_t identificadorCola);

struct nodoMemoria* inicializarNodo();
void liberarNodo(struct nodoMemoria* nodo);
struct nodoMemoria* crearRaizArbol(void);

void registrarMensajeEnMemoria(uint32_t idMensaje, paquete* paq, algoritmoMem metodo);
void registrarEnMemoriaPARTICIONES(msgMemoriaBroker*);
void registrarEnMemoriaBUDDYSYSTEM(msgMemoriaBroker* mensajeNuevo, struct nodoMemoria* partActual);

void particionarMemoriaBUDDY(struct nodoMemoria*);


listaMutex* iniciarMemoriaPARTICIONES();

void evaluarTamanioParticion(struct nodoMemoria* partActual, msgMemoriaBroker* msg);

void guardarSubEnMemoria(uint32_t idmensaje, uint32_t socket, uint32_t lista);
msgMemoriaBroker* buscarMensajeEnMemoria(uint32_t idMensajeBuscado);
msgMemoriaBroker* buscarMensajeEnMemoriaBuddy(uint32_t id);
msgMemoriaBroker* buscarPorRama(uint32_t id, struct nodoMemoria* nodoActual );

bool noEsParticionMinima(struct nodoMemoria* particion);
uint32_t tamanioParticion(struct nodoMemoria* part);
bool estaLibre(struct nodoMemoria* particion);
bool estaEnLista(uint32_t socket, ListasMemoria lista, msgMemoriaBroker* mensaje);

uint32_t evaluarTamanioParticionYasignar(struct nodoMemoria* partActual, msgMemoriaBroker* msg);

uint32_t intentarRamaIzquierda(msgMemoriaBroker* mensajeNuevo, struct nodoMemoria* partActual);

bool entraEnLaMitad(struct nodoMemoria* partActual, msgMemoriaBroker* mensajeNuevo);

#endif /* MEMORIA_H_ */
