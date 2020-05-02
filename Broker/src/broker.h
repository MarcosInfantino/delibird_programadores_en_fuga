/*
 * broker.h
 *
 *  Created on: 19 abr. 2020
 *      Author: utnso
 */

#ifndef BROKER_H_
#define BROKER_H_

#include <commons/collections/queue.h>
#include<string.h>
#include<pthread.h>
#include <stdbool.h>
#include <commons/config.h>

uint32_t puertoBroker;
char* ipBroker;

typedef enum {
	INCORRECTO, CORRECTO
} respuesta_broker;

typedef struct {
	int size;
	void* stream;
} t_buffer;

typedef struct {
	respuesta_broker respuesta;
	t_buffer* buffer;
} t_paquete;

typedef struct {
	uint32_t modulo;
	colaMensajes structCola;
	uint32_t socketCliente;
	uint32_t colaEnum;
} parametroValidacion;

typedef struct {
	t_queue* cola;
	t_list* suscriptores;
} colaMensajes;

enum modulosTP {
	BROKER, TEAM, GAMECARD, GAMEBOY
};
enum colas {
	APPEARED_POKEMON,
	NEW_POKEMON,
	CAUGHT_POKEMON,
	CATCH_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON
};

enum tipoMensaje {
	NORMAL, SUSCRIPCION
};

pthread_t thread;

uint32_t contadorMensajes = 0;

void iniciarHilos();
void* iniciarCola(void*);

void esperar_cliente(uint32_t);
void* atenderCliente(void* sock);
void manejarTipoDeMensaje(uint32_t modulo, uint32_t cliente_fd);
void suscribirSegunCola(uint32_t modulo, uint32_t socket);
void suscribir(parametroValidacion* parameter);
void responderMensaje(uint32_t socketCliente, uint32_t respuesta);
bool validarParaSuscripcion(parametroValidacion* parameter);
bool validarSuscripcionSegunModulo(uint32_t modulo, uint32_t cola);
bool validarPertenencia(colaMensajes cola, uint32_t socket);

#endif /* BROKER_H_ */
