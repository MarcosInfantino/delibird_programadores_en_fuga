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
#include<commons/log.h>

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
	posicion posicion;
	uint32_t cantidad;
}posicionCantidad;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	uint32_t posX;
	uint32_t posY;
}mensajeAppeared;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	uint32_t posX;
	uint32_t posY;
	uint32_t cantidad;
}mensajeNew;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	uint32_t posX;
	uint32_t posY;
}mensajeCatch;

typedef struct{
	uint32_t resultadoCaught;
}mensajeCaught;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
}mensajeGet;

typedef struct{
	uint32_t sizePokemon;
	char* pokemon;
	//uint32_t cantidad;
	//posicion* arrayPosiciones;
	t_list* listaPosicionCantidad;
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



mensajeAppeared* llenarAppeared(char* pokemon, uint32_t posX, uint32_t posY);
mensajeAppeared* llenarAppearedMemoria(char* pokemon, uint32_t posX, uint32_t posY);
void* serializarAppeared(mensajeAppeared* mensaje);
mensajeAppeared* deserializarAppeared (void* streamRecibido);
void destruirAppeared(mensajeAppeared* msg);

mensajeNew* llenarNew(char* pokemon, uint32_t posX, uint32_t posY, uint32_t cantidad);
void* serializarNew(mensajeNew* mensaje);
mensajeNew* deserializarNew (void* streamRecibido);
void destruirNew(mensajeNew* msg);

mensajeCatch* llenarCatch(char* pokemon, uint32_t posX, uint32_t posY);
void* serializarCatch(mensajeCatch* mensaje);
mensajeCatch* deserializarCatch (void* streamRecibido);
void destruirCatch(mensajeCatch* msg);

mensajeCaught* llenarCaught(uint32_t resultadoCaught);
void* serializarCaught (mensajeCaught* mensaje);
mensajeCaught* deserializarCaught(void* streamRecibido);
void destruirCaught(mensajeCaught* msg);

mensajeGet* llenarGet(char* pokemon);
void* serializarGet (mensajeGet* mensaje);
mensajeGet* deserializarGet (void* streamRecibido);
void destruirGet(mensajeGet* msg);

//mensajeLocalized* llenarLocalized(char* pokemon, uint32_t cantidad, posicion* posiciones);
mensajeLocalized* llenarLocalized(char* pokemon, t_list* listaPosicionCantidad);
void* serializarLocalized(mensajeLocalized* mensaje);
void* serializarArrayPosiciones(posicion* pos, uint32_t cantidad);
void* serializarPosicion(posicion* pos);
mensajeLocalized* deserializarLocalized(void* streamRecibido);
posicion* deserializarArrayPosiciones(void* stream,uint32_t cantidad);
posicion* deserializarPosicion(void* stream);
void destruirLocalized(mensajeLocalized* msg);
void* serializarListaPosicionCantidad(t_list* lista);
void* serializarPosicionCantidad(posicionCantidad* posCant);
posicionCantidad* deserializarPosicionCantidad(void* stream);
t_list* deserializarListaPosicionCantidad(void* stream);

mensajeSuscripcionTiempo* llenarSuscripcionTiempo(uint32_t cola, uint32_t tiempo);
void* serializarSuscripcionTiempo(mensajeSuscripcionTiempo* mensaje);
mensajeSuscripcionTiempo* deserializarSuscripcionTiempo(void* streamRecibido);
void destruirSuscripcionTiempo(mensajeSuscripcionTiempo* msg);

mensajeSuscripcion* llenarSuscripcion(uint32_t cola);
void* serializarSuscripcion(mensajeSuscripcion* mensaje);
mensajeSuscripcion* deserializarSuscripcion (void* streamRecibido);
void destruirSuscripcion(mensajeSuscripcion* msg);

paquete* llenarPaquete( uint32_t modulo,uint32_t tipoMensaje, uint32_t sizeStream,void* stream);
void* serializarPaquete(paquete* paqueteASerializar);
paquete* deserializarPaquete(void* paqueteRecibido);
void destruirPaquete(paquete* paq);
paquete* recibirPaquete(uint32_t socket);
uint32_t sizePaquete(paquete* paq);
void insertarIdPaquete(paquete* paq, uint32_t id);
void insertarIdCorrelativoPaquete(paquete* paq, uint32_t idCorrelativo);

//void* serializarACK(paquete* ackAserializar);
//mensajeACK* deserializarACK(void* stream);

uint32_t crearSocketCliente (char* ip, uint32_t puerto);
uint32_t sizeArgumentos (uint32_t colaMensaje, char* nombrePokemon, uint32_t cantidadPokemon);

uint32_t enviarACK(uint32_t socket, uint32_t modulo, uint32_t id);

listaMutex* inicializarListaMutex();
void destruirListaEntrenadoresLibres();
void addListaMutex(listaMutex* list,void* elemento);
void* getListaMutex(listaMutex* list, uint32_t index);
uint32_t sizeListaMutex(listaMutex* list);
void destruirListaMutex(listaMutex* lista);
void destruirListaMutexYElementos(listaMutex* lista,void(*element_destroyer)(void*));
void* removeListaMutex(listaMutex* list,uint32_t pos);
void removeAndDestroyElementListaMutex(listaMutex* list,uint32_t pos,void(*element_destroyer)(void*));
listaMutex* convertirAListaMutex(t_list* lista);
void list_sort_Mutex(listaMutex* list, bool (*comparator)(void *, void *));
void* list_remove_by_condition_Mutex(listaMutex* list, bool(*condition)(void*));
void listAddAllMutex(t_list* list, listaMutex* list1);

colaMutex* inicializarColaMutex();
void pushColaMutex(colaMutex* cola, void* infoEntrenador);
void* popColaMutex(colaMutex* cola);
uint32_t sizeColaMutex(colaMutex* cola);
void destruirColaMutexYElementos(colaMutex* cola, void(*element_destroyer)(void*));
void destruirColaMutex(colaMutex* cola);

t_log* iniciar_logger(char* file, char* program_name);
void terminar_programa(t_log* logger, t_config* config);
void loggearMensaje (paquete* paqueteRespuesta, t_log* logger);
char* nombreDeProceso(uint32_t modulo);
char* nombreDeCola(uint32_t cola);
char* armarStringSuscripLog(uint32_t modulo, uint32_t cola);
char* armarStringMsgNuevoLog(uint32_t cola);
char* armarConexionNuevoProcesoLog(uint32_t modulo);
char* armarStringACK(uint32_t cola, uint32_t idMensaje, uint32_t socket);

#endif /* MESSAGES_LIB_H_ */
