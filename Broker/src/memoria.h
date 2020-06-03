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

//si no toma esto, poner SRC_ en el define

typedef struct {
	uint32_t idMensaje;
	uint32_t cola;
	listaMutex* subsYaEnviado;
	listaMutex* subsACK;
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

typedef struct{
	t_config* config;
	char* configAtributo;
	char* OPCION1;
	char* OPCION2;
	uint32_t OP1;
	uint32_t OP2;
	char* error;
} algoritmoParameter;

struct nodoMemoria* nodoRaizMemoria; //no sé si va como puntero

//void enviarMensajesPreviosEnMemoria(uint32_t socket,uint32_t identificadorCola);

void registrarMensajeEnMemoria(uint32_t idMensaje, paquete* paq, algoritmoMem metodo);
void registrarEnMemoriaPARTICIONES(msgMemoriaBroker*);
void registrarEnMemoriaBUDDYSYSTEM(msgMemoriaBroker*);

struct nodoMemoria* inicializarNodo();

void guardarSubEnMemoria(uint32_t idmensaje, uint32_t socket, uint32_t lista);

struct nodoMemoria* crearRaizArbol(void);
void particionarMemoriaBUDDY(struct nodoMemoria*);



#endif /* MEMORIA_H_ */
