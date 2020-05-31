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
	SUSCRIPCION_TIEMPO,
	ACK,
	SUSCRIPCION_FINALIZADA
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
	uint32_t idCorrelativo;
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
	uint32_t idCorrelativo;
	uint32_t resultadoCaught;
}mensajeCaught;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
}mensajeGetBroker;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	uint32_t id;
}mensajeGetGamecard;

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

typedef struct {
	colaMutex* cola;
	listaMutex* suscriptores;
	sem_t* mensajesEnCola;
}colaMensajes;


void* serializarAppearedBroker(mensajeAppearedBroker* mensaje);
mensajeAppearedBroker* deserializarAppearedBroker(void* streamRecibido);
mensajeAppearedBroker* llenarMensajeAppearedBroker(char* pokemon, uint32_t posX,uint32_t posY, uint32_t idCorrelativo);
void destruirAppearedBroker(mensajeAppearedBroker* msg);

mensajeAppearedTeam* llenarMensajeAppearedTeam(char* pokemon, uint32_t posX, uint32_t posY);
void* serializarAppearedTeam(mensajeAppearedTeam* mensaje);
mensajeAppearedTeam* deserializarAppearedTeam (void* streamRecibido);
void destruirAppearedTeam(mensajeAppearedTeam* msg);

mensajeNewBroker* llenarMensajeNewBroker(char* pokemon, uint32_t posX, uint32_t posY, uint32_t cantidad);
void* serializarNewBroker(mensajeNewBroker* mensaje);
mensajeNewBroker* deserializarNewBroker (void* streamRecibido);
void destruirNewBroker(mensajeNewBroker* msg);

mensajeNewGamecard* llenarMensajeNewGameCard(char* pokemon, uint32_t posX, uint32_t posY, uint32_t cantidad, uint32_t id);
void* serializarNewGamecard(mensajeNewGamecard* mensaje);
mensajeNewGamecard* deserializarNewGamecard (void* streamRecibido);
void destruirNewGamecard(mensajeNewGamecard* msg);

mensajeCatchBroker* llenarMensajeCatchBroker(char* pokemon, uint32_t posX, uint32_t posY);
void* serializarCatchBroker(mensajeCatchBroker* mensaje);
mensajeCatchBroker* deserializarCatchBroker (void* streamRecibido);
void destruirCatchBroker(mensajeCatchBroker* msg);

mensajeCatchGamecard* llenarMensajeCatchGamecard(char* pokemon, uint32_t posX, uint32_t posY, uint32_t id);
void* serializarCatchGamecard(mensajeCatchGamecard* mensaje);
mensajeCatchGamecard* deserializarCatchGamecard (void* streamRecibido);
void destruirCatchGamecard(mensajeCatchGamecard* msg);

mensajeCaught* llenarMensajeCaught(uint32_t idCorrelativo, uint32_t resultadoCaught);
void* serializarCaught (mensajeCaught* mensaje);
mensajeCaught* deserializarCaught(void* streamRecibido);
void destruirCaught(mensajeCaught* msg);

mensajeGetBroker* llenarMensajeGetBroker(char* pokemon);
void* serializarGetBroker (mensajeGetBroker* mensaje);
mensajeGetBroker* deserializarGetBroker (void* streamRecibido);
void destruirGetBroker(mensajeGetBroker* msg);

mensajeGetGamecard* llenarMensajeGetGamecard(char* pokemon, uint32_t id);
void* serializarGetGamecard (mensajeGetGamecard* mensaje);
mensajeGetGamecard* deserializarGetGamecard (void* streamRecibido);
void destruirGetGamecard(mensajeGetGamecard* msg);

mensajeLocalized* llenarMensajeLocalized(char* pokemon, uint32_t cantidad, posicion* posiciones);
void* serializarLocalized(mensajeLocalized* mensaje);
void* serializarArrayPosiciones(posicion* pos, uint32_t cantidad);
void* serializarPosicion(posicion* pos);
mensajeLocalized* deserializarLocalized(void* streamRecibido);
posicion* deserializarArrayPosiciones(void* stream,uint32_t cantidad);
posicion* deserializarPosicion(void* stream);

mensajeSuscripcionTiempo* llenarMensajeSuscripcionTiempo(uint32_t cola, uint32_t tiempo);
void* serializarSuscripcionTiempo(mensajeSuscripcionTiempo* mensaje);
mensajeSuscripcionTiempo* deserializarSuscripcionTiempo(void* streamRecibido);
void destruirSuscripcionTiempo(mensajeSuscripcionTiempo* msg);

mensajeSuscripcion* llenarMensajeSuscripcion(uint32_t cola);
void* serializarSuscripcion(mensajeSuscripcion* mensaje);
mensajeSuscripcion* deserializarSuscripcion (void* streamRecibido);
void destruirSuscripcion(mensajeSuscripcion* msg);

paquete* llenarPaquete( uint32_t modulo,uint32_t tipoMensaje, uint32_t sizeStream,void* stream);
void* serializarPaquete(paquete* paqueteASerializar);
paquete* deserializarPaquete(void* paqueteRecibido);
void destruirPaquete(paquete* paq);
paquete* recibirPaquete(uint32_t socket);
uint32_t sizePaquete(paquete* paq);

//void* serializarACK(paquete* ackAserializar);
//mensajeACK* deserializarACK(void* stream);

uint32_t crearSocketCliente (char* ip, uint32_t puerto);
uint32_t sizeArgumentos (uint32_t colaMensaje, char* nombrePokemon, uint32_t procesoDestinatario);

listaMutex* inicializarListaMutex();
void destruirListaEntrenadoresLibres();
void addListaMutex(listaMutex* list,void* elemento);
void* getListaMutex(listaMutex* list, uint32_t index);
uint32_t sizeListaMutex(listaMutex* list);
void destruirListaMutex(listaMutex* lista,void(*element_destroyer)(void*));
void removeListaMutex(listaMutex* list,uint32_t pos);

colaMutex* inicializarColaMutex();
void pushColaMutex(colaMutex* cola, void* infoEntrenador);
void* popColaMutex(colaMutex* cola);
uint32_t sizeColaMutex(colaMutex* cola);
void destruirColaMutex(colaMutex* cola, void(*element_destroyer)(void*));

#endif /* MESSAGES_LIB_H_ */
