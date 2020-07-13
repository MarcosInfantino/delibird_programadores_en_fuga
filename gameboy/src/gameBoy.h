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
#include <messages_lib/messages_lib.h>

t_log* gameboyLogger;
uint32_t idProcesoGameboy;
typedef struct{
	void * paqueteAEnviar;
	uint32_t socketCliente;
}paqueteYSocket;

uint32_t obtenerPuertoProceso (uint32_t proceso, t_config* config);

char* obtenerIpProceso (uint32_t proceso, t_config* config);

uint32_t obtenerNombreProceso (char* proceso);

uint32_t obtenerColaMensaje (char* cola);

void* generarStreamArgumentos (uint32_t colaMensaje, char* argv[]);

void* enviarMensaje(void* paqueteConSocket);

void* enviarMensajeSuscripcion(void* paqueteySocket);

void iniciarHiloEnvio(paqueteYSocket* paqueteySocket);

void asignarIds(char * argv[], uint32_t proceso, paquete* paq);

#endif /* GAMEBOY_H_ */
