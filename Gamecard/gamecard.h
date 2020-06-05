/*
 * gamecard.h
 *
 *  Created on: 12 may. 2020
 *      Author: juancito
 */
#include <commons/collections/queue.h>
#include <string.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <messages_lib/messages_lib.h>
#include <stdbool.h>

#ifndef GAMECARD_H_
#define GAMECARD_H_

typedef struct{
	char* pokemon;
	posicion posicion;
	uint32_t cantidad;
	uint32_t id;
} pokemonEnPosicion;

typedef struct{
	char* pokemon;
	uint32_t id;
	uint32_t cantPosiciones;
	posicion* posicion;
} pokemonADevolver;

typedef struct{
	char* pokemon;
	uint32_t id;
	posicion* posicion;
	bool resultado;
} pokemonAAtrapar;

void* suscribirseCola(void* msgSuscripcion);
void* suscribirseColasBroker(void* config);
void* iniciarServidorGameboy(void* arg);
int crearHiloServidorGameboy(pthread_t* hilo);
void esperar_cliente(uint32_t servidor);
void* atenderCliente(void* sock);
void atenderNew(mensajeNew* msg, uint32_t idCorrelativo);
void atenderGet(mensajeGet* msg, uint32_t idCorrelativo);
void atenderCatch(mensajeCatch* msg, uint32_t idCorrelativo );
void enviarLocalized(pokemonADevolver* pokeADevolver);
void enviarAppeared (pokemonEnPosicion* pokeEnPosicion);
void enviarCaught (pokemonAAtrapar* pokeAAtrapar);



#endif /* GAMECARD_H_ */
