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
#include <semaphore.h>
#include <arpa/inet.h>
#include <stdbool.h>

//void* especieAComparar;

listaMutex* entrenadores;
colaMutex* colaEjecucionFifo;
listaMutex* especiesLocalizadas;//lista de pokemones
listaMutex* listaIdsRespuestasGet;
listaMutex* listaIdsEntrenadorMensaje; // del tipo idsEntrenadorMensaje , //ver a futuro si esta lista requiere mutex
listaMutex* entrenadoresLibres;
colaMutex* pokemonesPendientes;//lista de pokePosicion que contiene los pokemones que no pudieron ser asignados a ningun entrenador por no haber entrenadore libres
listaMutex* entrenadoresExit;

//pokemonPosicion pokemonAAtrapar;


int socketGameboy;
int socketGamecard;
uint32_t puertoBroker;
char* ipBroker;
pthread_t* arrayIdHilosEntrenadores;
uint32_t tiempoReconexion;
uint32_t retardoCicloCpu;
uint32_t algoritmoPlanificacion;

sem_t semaforoEjecucionCpu;

t_log* teamLogger;

typedef enum {
	NEW=1500,
	READY=1501,
	BLOCKED=1502,
	EXEC=1503,
	EXIT=1504
}estado;

typedef enum{
	FIFO
}AlgoritmoPlanificacion;

//typedef struct {
//	t_list* lista;//lista
//	pthread_mutex_t* mutex;
//}listaMutex;
//
//typedef struct{
//	t_queue* cola;
//	pthread_mutex_t* mutex;
//}colaMutex;
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
	char* pokemon;
	posicion posicion;
} pokemonPosicion;

typedef struct {
	posicion posicion;
	t_list* pokemones; //lista de strings
	t_list* objetivoPersonal;//lista de strings
	estado estado;
	uint32_t id;
	pokemonPosicion* pokemonAAtrapar;
	sem_t semaforo;
	uint32_t cantidadCiclosCpu;
} dataEntrenador;

typedef struct {
	listaMutex* objetivoGlobal;//lista de objetivo
	t_list* entrenadores;
	t_list* objetivosCumplidos;
	//uint32_t** mapa;
	uint32_t cantidadCiclosCpuTotales;
}dataTeam;

dataTeam* team;

typedef struct{
	uint32_t idEntrenador;
	uint32_t idMensaje;
}idsEntrenadorMensaje;

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

listaMutex* obtenerObjetivos(t_list* especies);

//bool objetivoMismaEspecie(void* obj);

void* ejecucionHiloEntrenador(void* arg);

int inicializarEntrenadores(t_list* entrenadores);

int crearHiloConexionColasBroker(void* config, pthread_t* hilo);

void* suscribirseColasBroker(void* conf);

int crearHiloServidorGameboy(pthread_t* hilo);

void* iniciarServidorGameboy(void* arg);

uint32_t buscarMismoPokemon(t_list* lst, char* pokemon);

void* suscribirseCola(void* estructuraSuscripcion);

uint32_t buscarObjetivoPorEspecie(listaMutex* listaObjetivos, char* especie);

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

void atenderAppeared(mensajeAppeared* msg);

void enviarCatch(dataEntrenador* infoEntrenador);

void atenderCaught(paquete* paqueteRespuesta);

uint32_t buscarEntrenadorParaMensaje(listaMutex* listaIds, uint32_t idMensaje);

void replanificarEntrenador(dataEntrenador* entrenador);

void asignarPokemonAEntrenador(dataEntrenador* entrenador, pokemonPosicion* pokePosicion);

bool cumplioObjetivo(dataEntrenador* entrenador);

bool objetivoCumplido();

uint32_t crearHiloPlanificador(pthread_t* hiloPlanificador);

void* iniciarPlanificador(void* arg);

void ejecucionPlanificadorFifo();

void poneteEnReady(dataEntrenador* entrenador);

void obtenerAlgoritmoPlanificacion(t_config* config);

void registrarPokemonAtrapado(char* pokemon);

bool pokemonEsObjetivo(char* pokemon);

void* enviarGet(void* arg);

uint32_t crearHiloParaEnviarGets(pthread_t* hilo);

void* enviarGets(void* arg);

bool especieFueLocalizada(char* pokemon);

void atenderLocalized(paquete* paquete);

bool localizedMeInteresa(paquete* paquete);

bool mismaListaPokemones(t_list* listaPokemones1, t_list* listaPokemones2);
bool entrenadorEnDeadlock(dataEntrenador* entrenador);
void entrarEnEjecucionParaDeadlock(dataEntrenador* infoEntrenador);

void loggearPokemonAAtrapar(pokemonPosicion* pokePosicion, t_log* teamLogger);

uint32_t crearSocketClienteBroker (char* ip, uint32_t puerto);

uint32_t reconectarseAlBroker(uint32_t cliente,void* direccionServidor,socklen_t length);

#endif /* TEAM_H_ */
