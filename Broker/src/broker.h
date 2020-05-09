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
#include<commons/log.h>
#include <messages_lib/messages_lib.h>

uint32_t puertoBroker;
char* ipBroker;
t_log* loggerBroker;

typedef struct {
	t_queue* cola;
	t_list* suscriptores;
} colaMensajes;

typedef struct{
	uint32_t cola;
	paquete paq;
	uint32_t socket;
	uint32_t tiempo;
}suscripcionTiempo;


//typedef struct {
//	paquete paquete;
//	colaMensajes structCola;
//	uint32_t socketCliente;
//} parametroValidacion;

pthread_t thread;

uint32_t contadorMensajes = 1;

void iniciarHilos();
void* iniciarCola(void*);

void esperar_cliente(uint32_t);
void* atenderCliente(void* sock);
void manejarTipoDeMensaje(paquete paq, uint32_t socket);
void suscribirSegunCola(paquete paq, uint32_t socket);
void suscribir(colaMensajes cola, paquete paq, uint32_t socket);
void responderMensaje(uint32_t socketCliente, uint32_t respuesta);
bool validarParaSuscripcion(colaMensajes cola, paquete paq, uint32_t socket);
bool validarSuscripcionSegunModulo(uint32_t modulo, uint32_t cola);
bool validarPertenencia(colaMensajes cola, uint32_t socket);
char* nombreDeProceso(uint32_t modulo);
char* nombreDeCola(uint32_t cola);
char* armarStringSuscripLog(uint32_t modulo, uint32_t cola);
void meterEnCola( colaMensajes* structCola, void* mensaje, uint32_t  socket);
colaMensajes* obtenerCola(uint32_t colaInt);
void desuscribir(uint32_t socket, uint32_t cola );
void suscribirPorTiempo(void* estructura);

#endif /* BROKER_H_ */
