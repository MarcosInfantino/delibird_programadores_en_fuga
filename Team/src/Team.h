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
#include <messages_lib/messages_lib.h>
//void* especieAComparar;

pthread_t threadAtencionGameboy;

enum respuestasBroker{INCORRECTO, CORRECTO};

typedef enum {
	NEW=1500,
	READY=1501,
	BLOCKED=1502,
	EXEC=1503,
	EXIT=1504
}estado;


typedef struct{
	uint32_t modulo;
	uint32_t tipoMensaje;
	uint32_t cola;

}structSuscripcion;

typedef struct{
	uint32_t cantidad;
	char* pokemon;
} objetivo;

typedef struct{
	uint32_t x;
	uint32_t y;
} posicion;

typedef struct{
	char* pokemon;
	posicion posicion;
} pokemonPosicion;

typedef struct {
	posicion posicion;
	t_list* pokemones;
	t_list* objetivoPersonal;//lista de strings
	estado estado;
	uint32_t id;
	pokemonPosicion* pokemonAAtrapar;
	pthread_mutex_t mutex;
} dataEntrenador;

typedef struct {
	t_list* objetivoGlobal;//lista de objetivo
	t_list* entrenadores;
	t_list* objetivosCumplidos;
	//uint32_t** mapa;

}dataTeam;

//typedef enum {
//	BROKER,
//	TEAM,
//	GAMECARD,
//	GAMEBOY
//}modulo;

//typedef enum{
//	APPEARED_POKEMON,
//	NEW_POKEMON,
//	CAUGHT_POKEMON,
//	CATCH_POKEMON,
//	GET_POKEMON,
//	LOCALIZED_POKEMON
//}cola;



//typedef struct{
//	uint32_t modulo;
//	uint32_t tipoMensaje;
//	uint32_t cola;
//}mensajeSuscripcion;

typedef struct{
	uint32_t modulo;
	uint32_t tipoMensaje;
	uint32_t id;
	uint32_t idCorrelativo;
	uint32_t sizeStream;
	void* stream;
}paqueteMensaje;

dataTeam* inicializarTeam(t_config* config);

t_list* obtenerListaDeListas(char** lst);

t_list* arrayStringALista(char** arr);

t_list* obtenerObjetivos(t_list* especies);

//bool objetivoMismaEspecie(void* obj);

void* ejecucionHiloEntrenador(void* arg);

int inicializarEntrenadores(t_list* entrenadores);

int crearHiloConexionColasBroker(void* config, pthread_t* hilo);

void* suscribirseColasBroker(void* conf);

int crearHiloServidorGameboy(pthread_t* hilo);

void* iniciarServidorGameboy(void* arg);

uint32_t buscarMismoPokemon(t_list* lst, char* pokemon);

void* suscribirseCola(void* estructuraSuscripcion);

uint32_t buscarObjetivoPorEspecie(t_list* listaObjetivos, char* especie);

void* serializarMensajeSuscripcion(mensajeSuscripcion* mensaje, uint32_t bytes);

uint32_t distanciaEntrePosiciones(posicion pos1, posicion pos2);

mensajeSuscripcion* inicializarMensajeSuscripcion(uint32_t modulo,uint32_t mensaje,uint32_t cola);

uint32_t distanciaEntrenadorPosicion(dataEntrenador* entrenador, posicion posicion);

uint32_t obtenerIdEntrenadorMasCercano(posicion pos);

uint32_t idEntrenadorEnLista(dataEntrenador* entrenadorMasCercano);

t_list* inicializarMutexEntrenadores();

void moverEntrenador(dataEntrenador* entrenador, uint32_t movimientoX, uint32_t movimientoY);

void moverEntrenadorAPosicion(dataEntrenador* entrenador, posicion pos);

void moverEntrenadorY(dataEntrenador* entrenador, uint32_t movimientoY);

void moverEntrenadorX(dataEntrenador* entrenador, uint32_t movimientoX);

void seleccionarEntrenador(pokemonPosicion* pokemon);

void habilitarHiloEntrenador(uint32_t idEntrenador);

void entrarEnEjecucion(dataEntrenador* infoEntrenador);

uint32_t encontrarPosicionEntrenadorLibre(dataEntrenador* entrenador);

void esperar_cliente(uint32_t servidor);

void* atenderCliente(void* sock);



#endif /* TEAM_H_ */
