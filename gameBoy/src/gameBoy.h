/*
 * gameBoy.h
 *
 *  Created on: 26 abr. 2020
 *      Author: utnso
 */

#ifndef GAMEBOY_H_
#define GAMEBOY_H_
#include <stdint.h>
#include<commons/config.h>

typedef enum{
	FAIL,
	OK
}resultadoCaught;

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
	uint32_t proceso;
	uint32_t tipoDeMensaje;
	uint32_t cola;
	uint32_t bytes;
	void* argumentos;
}mensajeNormal;

typedef struct{
	uint32_t tipoMensaje;
	uint32_t proceso;
	uint32_t cola;
	uint32_t socket;
	uint32_t tiempoSuscripcion;
}mensajeSuscripcion;

uint32_t obtenerPuertoProceso (uint32_t proceso, t_config* config);

char* obtenerIpProceso (uint32_t proceso, t_config* config);

uint32_t obtenerNombreProceso (char* proceso);

uint32_t obtenerColaMensaje (char* cola);

uint32_t socketCliente(char* ip, uint32_t puerto);

uint32_t sizeArgumentos (uint32_t colaMensaje, char* argv[], uint32_t proceso);

void* llenarArgumentos (uint32_t colaMensaje, uint32_t bytesArgumentos, char* argv[],uint32_t proceso);

void* armarMensajeNormal (void* argumentos, uint32_t proceso,uint32_t colaMensaje, uint32_t bytesArgumentos);

void* armarMensajeSuscripcion(uint32_t colaSuscripcion, uint32_t tiempoSuscripcion);

void enviarMensaje(void* mensajeAEnviar, uint32_t socketCliente, uint32_t bytesArgumentos);

#endif /* GAMEBOY_H_ */
