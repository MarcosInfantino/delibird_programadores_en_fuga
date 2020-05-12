/*
 * messages_lib.h
 *
 *  Created on: 12 abr. 2020
 *      Author: utnso
 */

#ifndef MESSAGES_LIB_H_
#define MESSAGES_LIB_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include <commons/collections/queue.h>
#include <commons/string.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>
#include <commons/config.h>
#include <unistd.h>
#include <semaphore.h>
#include <arpa/inet.h>

typedef struct{
	int size;
	void* stream;
} t_buffer;

typedef enum
{
	CADENA = 1,
}op_code;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef enum{
	APPEARED_POKEMON,
	NEW_POKEMON,
	CAUGHT_POKEMON,
	CATCH_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON,
	SUSCRIPCION,
	SUSCRIPCION_TIEMPO
}tipoMensaje;

typedef enum {
	BROKER,
	TEAM,
	GAMECARD,
	GAMEBOY
}modulo;

typedef enum{
	FAIL,
	OK
}resultadoCaught;

typedef enum{
	INCORRECTO,
	CORRECTO
}respuestas;

typedef struct{
	uint32_t x;
	uint32_t y;
} posicion;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	uint32_t posX;
	uint32_t posY;
	uint32_t id;
}mensajeAppearedBroker;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	uint32_t posX;
	uint32_t posY;
}mensajeAppearedTeam;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	uint32_t posX;
	uint32_t posY;
	uint32_t cantidad;
}mensajeNewBroker;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	uint32_t posX;
	uint32_t posY;
	uint32_t cantidad;
	uint32_t id;
}mensajeNewGamecard;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	uint32_t posX;
	uint32_t posY;
}mensajeCatchBroker;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	uint32_t posX;
	uint32_t posY;
	uint32_t id;
}mensajeCatchGamecard;

typedef struct{
	uint32_t id;
	uint32_t resultadoCaught;
}mensajeCaught;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
}mensajeGet;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	uint32_t cantidad;
	posicion* arrayPosiciones;
}mensajeLocalized;

typedef struct{
	uint32_t cola;
	uint32_t tiempo;
}mensajeSuscripcionTiempo;

typedef struct{
	uint32_t cola;
}mensajeSuscripcion;

typedef struct{
	uint32_t modulo;
	uint32_t tipoMensaje;
	uint32_t id;
	uint32_t idCorrelativo;
	uint32_t sizeStream;
	void* stream;
}paquete;

typedef struct{
	uint32_t id;
}mensajeRespuestaBroker;

typedef struct {
	t_list* lista;//lista
	pthread_mutex_t* mutex;
}listaMutex;

typedef struct{
	t_queue* cola;
	pthread_mutex_t* mutex;
}colaMutex;

t_paquete* armarPaquete(char* cadena);
mensajeSuscripcion* deserializarMensajeSuscripcion(void* stream, uint32_t bytes);

void* serializarAppearedBroker(mensajeAppearedBroker* mensaje);
void* serializarAppearedTeam(mensajeAppearedTeam* mensaje);
void* serializarNewBroker(mensajeNewBroker* mensaje);
void* serializarNewGamecard(mensajeNewGamecard* mensaje);
void* serializarCatchBroker(mensajeCatchBroker* mensaje);
void* serializarCatchGamecard(mensajeCatchGamecard* mensaje);
void* serializarCaught (mensajeCaught* mensaje);
void* serializarGet (mensajeGet* mensaje);
void* serializarLocalized(mensajeLocalized* mensaje);
void* serializarSuscripcionTiempo(mensajeSuscripcionTiempo* mensaje);
void* serializarSuscripcion(mensajeSuscripcion* mensaje);
paquete* llenarPaquete( uint32_t modulo,uint32_t tipoMensaje, uint32_t sizeStream,void* stream);
void* serializarPaquete(paquete* paqueteASerializar);

mensajeAppearedBroker* deserializarAppearedBroker(void* streamRecibido);
mensajeAppearedTeam* deserializarAppearedTeam (void* streamRecibido);
mensajeNewBroker* deserializarNewBroker (void* streamRecibido);
mensajeNewGamecard* deserializarNewGamecard (void* streamRecibido);
mensajeCatchBroker* deserializarCatchBroker (void* streamRecibido);
mensajeCatchGamecard* deserializarCatchGamecard (void* streamRecibido);
mensajeCaught* deserializarCaught(void* streamRecibido);
mensajeGet* deserializarGet (void* streamRecibido);
mensajeLocalized* deserializarLocalized(void* streamRecibido);
mensajeSuscripcionTiempo* deserializarSuscripcionTiempo(void* streamRecibido);
mensajeSuscripcion* deserializarSuscripcion (void* streamRecibido);
paquete* deserializarPaquete(void* paqueteRecibido);
paquete* recibirPaquete(uint32_t socket);

uint32_t crearSocketCliente (char* ip, uint32_t puerto);
uint32_t sizeArgumentos (uint32_t colaMensaje, char* nombrePokemon, uint32_t procesoDestinatario);
uint32_t sizePaquete(paquete* paq);


listaMutex inicializarListaMutex();

void destruirListaEntrenadoresLibres();

void addListaMutex(listaMutex list,void* elemento);

void* getListaMutex(listaMutex list, uint32_t index);

uint32_t sizeListaMutex(listaMutex list);

void destruirListaMutex(listaMutex* lista);

void removeListaMutex(listaMutex list,uint32_t pos);

colaMutex inicializarColaMutex();

void pushColaMutex(colaMutex cola, void* infoEntrenador);

void* popColaMutex(colaMutex cola);


#endif /* MESSAGES_LIB_H_ */
