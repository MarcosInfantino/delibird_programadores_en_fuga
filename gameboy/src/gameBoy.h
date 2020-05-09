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

typedef struct{
	void * paqueteAEnviar;
	uint32_t socketCliente;
	uint32_t sizeDelStream;
}paqueteYSocket;

uint32_t obtenerPuertoProceso (uint32_t proceso, t_config* config);

char* obtenerIpProceso (uint32_t proceso, t_config* config);

uint32_t obtenerNombreProceso (char* proceso);

uint32_t obtenerColaMensaje (char* cola);

uint32_t socketCliente(char* ip, uint32_t puerto);

uint32_t sizeArgumentos (uint32_t colaMensaje, char* argv[], uint32_t proceso);

void* generarStreamArgumentos (uint32_t colaMensaje, char* argv[]);

void* enviarMensaje(void* paqueteConSocket);

void iniciarHiloEnvio(paqueteYSocket* paqueteySocket);

#endif /* GAMEBOY_H_ */
