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
#include <time.h>


typedef struct {
	uint32_t idMensaje;
	uint32_t cola;
	listaMutex* subsYaEnviado;
	listaMutex* subsACK;
	uint32_t sizeStream;
	void* stream;
}msgMemoriaBroker;

typedef enum{
	LIBRE,
	OCUPADO,
	PARTICIONADO
}nodeStatus;

typedef struct{
	nodeStatus status;
	uint32_t size;
	struct tm tiempo;
}nodeData;

struct nodoMemoria {
	nodeData header;
	msgMemoriaBroker* mensaje;
	uint32_t offset;
	struct nodoMemoria* hijoIzq;
	struct nodoMemoria* hijoDer;
	struct nodoMemoria* padre;
};

typedef struct{
	uint32_t offset;
	uint32_t sizeParticion;
}particionLibre;

typedef struct{
	uint32_t offset;
	struct tm lru;
	msgMemoriaBroker* mensaje;
}particionOcupada;

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
listaMutex* particionesLibres;

uint32_t auxTamanioStreamGlobal;

//void enviarMensajesPreviosEnMemoria(uint32_t socket,uint32_t identificadorCola);

struct nodoMemoria* crearRaizArbol(void);
listaMutex* iniciarMemoriaPARTICIONES();
struct nodoMemoria* inicializarNodo();
void liberarNodo(struct nodoMemoria* nodo);


void registrarMensajeEnMemoria(uint32_t idMensaje, paquete* paq, algoritmoMem metodo);
void registrarEnMemoriaPARTICIONES(msgMemoriaBroker*);
void registrarEnMemoriaBUDDYSYSTEM(msgMemoriaBroker* mensajeNuevo, struct nodoMemoria* partActual);

void particionarMemoriaBUDDY(struct nodoMemoria*);
void evaluarTamanioParticion(struct nodoMemoria* partActual, msgMemoriaBroker* msg);
uint32_t evaluarTamanioParticionYasignar(struct nodoMemoria* partActual, msgMemoriaBroker* msg);

void guardarEnListaMemoria(uint32_t idmensaje, uint32_t socket, uint32_t lista);
msgMemoriaBroker* buscarMensajeEnMemoria(uint32_t idMensajeBuscado);
msgMemoriaBroker* buscarMensajeEnMemoriaBuddy(uint32_t id);
msgMemoriaBroker* buscarMensajeEnMemoriaParticiones(uint32_t idMensajeBuscado);
msgMemoriaBroker* buscarPorRama(uint32_t id, struct nodoMemoria* partActual );
bool buscarPorRamaGet(mensajeGet* msgGet, struct nodoMemoria* nodoActual );
bool buscarPorRamaCatch(mensajeCatch*  msgCatch, struct nodoMemoria* nodoActual );


uint32_t intentarRamaIzquierda(msgMemoriaBroker* mensajeNuevo, struct nodoMemoria* partActual);

uint32_t tamanioParticion(struct nodoMemoria* part);
uint32_t tamanioMinimo(struct nodoMemoria* partActual);
bool noEsParticionMinima(struct nodoMemoria* particion);
bool estaLibre(struct nodoMemoria* particion);
bool estaEnLista(uint32_t socket, ListasMemoria lista, msgMemoriaBroker* mensaje);
bool entraEnLaMitad(struct nodoMemoria* partActual, msgMemoriaBroker* mensajeNuevo);
bool esParticionMinima(struct nodoMemoria* particion);

bool estaParticionado(struct nodoMemoria* partActual);
bool estaOcupado(struct nodoMemoria* partActual);
bool ambosHijosOcupados(struct nodoMemoria* padre);

particionLibre* obtenerParticionLibrePARTICIONES(uint32_t tamStream);
bool menorAmayorSegunSize(void* primero, void* segundo);
bool menorAmayorSegunOffset(void* primero, void* segundo);
bool esSuficientementeGrandeParaElMSG(void* elemento);
void generarParticionLibre(uint32_t base);

void crearDumpDeCache();
void asignarPuntero(uint32_t offset, void* stream, uint32_t sizeStream);
void destroyParticion(void* particion);

bool yaEstaEnMemoria(paquete* paq);
bool yaSeGuardoEnMemoria(mensajeCatch* msgCatch, mensajeGet* msgGet);
bool existeMensajeEnMemoriaBuddy(mensajeGet* msgGet, mensajeCatch*  msgCatch);
uint32_t compararCatch(mensajeCatch*  elemLista, mensajeCatch*  msgCatch);
uint32_t compararGet(mensajeGet* elemLista, mensajeGet* msgGet);

#endif /* MEMORIA_H_ */
