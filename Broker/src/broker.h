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
#include "memoria.h"
#include "files.h"


uint32_t puerto_broker;
char* ip_broker;
t_log* loggerBroker;
t_log* brokerLogger2;
uint32_t tamMemoria;
uint32_t particionMinima;
algoritmoMem algoritmoMemoria;
algoritmoParticiones algoritmoParticionLibre;
algoritmoReemp algoritmoReemplazo;
pthread_mutex_t* mutexMemoria;

typedef struct{
	uint32_t cola;
	paquete paq;
	uint32_t* socket;
	uint32_t tiempo;
}suscripcionTiempo;

typedef struct{
	uint32_t contador;
	pthread_mutex_t* mutexContador;
}contadorMensajes;


//typedef struct {
//	paquete paquete;
//	colaMensajes structCola;
//	uint32_t socketCliente;
//} parametroValidacion;

//pthread_t thread;

pthread_mutex_t mutex;

archivoMutex* archivoSem;

contadorMensajes contador;

colaMensajes appearedPokemon, newPokemon, caughtPokemon, catchPokemon,
		getPokemon, localizedPokemon;

colaMensajes colaNula;

uint32_t idHiloAppearedPokemon, idHiloNewPokemon, idHiloCaughtPokemon,
		idHiloCatchPokemon, idHiloGetPokemon, idHiloLocalizedPokemon;

uint32_t manejoMensajesAppearedPokemon, manejoMensajesNewPokemon, manejoMensajesCaughtPokemon,
		manejoMensajesCatchPokemon, manejoMensajesGetPokemon, manejoMensajesLocalizedPokemon;

pthread_t hiloAppearedPokemon, hiloNewPokemon, hiloCaughtPokemon,
		hiloCatchPokemon, hiloGetPokemon, hiloLocalizedPokemon;

pthread_t devolverMensajeAppeared, devolverMensajeNew, devolverMensajeCaught,
		devolverMensajeCatch, devolverMensajeGet, devolverMensajeLocalized;

void iniciarHilos();
void* iniciarCola(void*);
void* iniciarServidor();
void definirAlgoritmoMemoria(t_config*);
void definirAlgoritmoParticionLibre(t_config*);
void definirAlgoritmoReemplazo(t_config*);
void definirAlgoritmo(algoritmoParameter , uint32_t);
void definirComienzoDeMemoria();

void esperar_cliente(uint32_t);
void asignarID(paquete * paq);
uint32_t obtenerIDultimoMensaje();
void* atenderCliente(void* sock);
void manejarTipoDeMensaje(paquete* paq, uint32_t* socket);
void suscribirSegunCola(paquete paq, uint32_t* socket);
void suscribir(colaMensajes * cola, paquete paq, uint32_t* socket,uint32_t identificadorCola);
void responderMensaje(uint32_t socketCliente, uint32_t respuesta);

bool validarParaSuscripcion(colaMensajes *cola, paquete paq, uint32_t socket, uint32_t identificadorCola);
bool validarSuscripcionSegunModulo(uint32_t modulo, uint32_t cola);
bool validarPertenencia(colaMensajes * cola, uint32_t socket);


//char* nombreDeProceso(uint32_t modulo);
//char* nombreDeCola(uint32_t cola);
//char* armarStringSuscripLog(uint32_t modulo, uint32_t cola);
//char* armarStringMsgNuevoLog(uint32_t modulo);

void meterEnCola( colaMensajes* structCola, paquete* paq, uint32_t  socket);
colaMensajes* obtenerCola(uint32_t colaInt);

void desuscribir(uint32_t socket, uint32_t cola );
void suscribirPorTiempo(void* estructura);
void suscribirACola(uint32_t* socket, colaMensajes * cola);

void inicializarContador();
void abrirHiloParaEnviarMensajes();
void* chequearMensajesEnCola(void * par);
uint32_t incrementarContador();
uint32_t obtenerContador();

t_config* leer_config(void);

#endif /* BROKER_H_ */
