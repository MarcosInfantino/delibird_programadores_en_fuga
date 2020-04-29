/*
 * Team.h
 *
 *  Created on: 20 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_H_
#define TEAM_H_

#include <commons/collections/queue.h>
#include <commons/string.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>
#include <commons/config.h>
#include <unistd.h>
//void* especieAComparar;

typedef enum {
	NEW=1500,
	READY=1501,
	BLOCKED=1502,
	EXEC=1503,
	EXIT=1504
}estado;
typedef struct{
	uint32_t cantidad;
	char* pokemon;
} objetivo;

typedef struct{
	uint32_t x;
	uint32_t y;
} posicion;

typedef struct {
	posicion posicion;
	t_list* pokemones;
	t_list* objetivoPersonal;//lista de strings
	estado estado;

} dataEntrenador;

typedef struct {
	t_list* objetivoGlobal;//lista de objetivo
	t_list* entrenadores;
	t_list* objetivosCumplidos;
	//uint32_t** mapa;

}dataTeam;

typedef enum {
	BROKER,
	TEAM,
	GAMECARD,
	GAMEBOY
}modulo;

typedef enum{
	APPEARED_POKEMON,
	NEW_POKEMON,
	CAUGHT_POKEMON,
	CATCH_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON
}cola;

typedef enum{
	MENSAJE_NORMAL,
	MENSAJE_SUSCRIPCION
}tipoMensaje;

typedef struct{
	uint32_t modulo;
	uint32_t tipoMensaje;
	uint32_t idProceso;
	uint32_t cola;
	uint32_t socket;
}mensajeSuscripcion;

dataTeam* inicializarTeam(t_config* config);

t_list* obtenerListaDeListas(char** lst);

t_list* arrayStringALista(char** arr);

t_list* obtenerObjetivos(t_list* especies);

//bool objetivoMismaEspecie(void* obj);

void* ejecucionHiloEntrenador(void* arg);

int inicializarEntrenadores(t_list* entrenadores, pthread_t arrayIdHilos[]);

int crearHiloConexionColasBroker(void* config, pthread_t* hilo);

void* suscribirseColasBroker(void* conf);

int crearHiloServidorGameboy(pthread_t* hilo);

void* iniciarServidorGameboy(void* arg);

uint32_t buscarMismoPokemon(t_list* lst, char* pokemon);

void suscribirseCola(uint32_t modulo,uint32_t tipoMensaje,uint32_t idProceso, uint32_t cola, uint32_t socket);

uint32_t buscarObjetivoPorEspecie(t_list* listaObjetivos, char* especie);

void* serializarMensajeSuscripcion(mensajeSuscripcion* mensaje, uint32_t bytes);
#endif /* TEAM_H_ */
