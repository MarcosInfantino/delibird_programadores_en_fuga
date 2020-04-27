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
	OK,
	FAIL
}resultadoCaught;

typedef enum{
	BROKER,
	TEAM,
	GAMECARD,
}proceso;

typedef enum{
	NEW,
	APPEARED,
	CATCH,
	CAUGHT,
	GET,
	LOCALIZED
}tipo_mensaje;

typedef struct{
	uint32_t tipoMensaje;
	uint32_t bytes;
	void* stream;
}mensaje;

uint32_t obtenerPuertoProceso (uint32_t proceso, t_config* config);

char* obtenerIpProceso (uint32_t proceso, t_config* config);

uint32_t obtenerNombreProceso (char* proceso);

uint32_t obtenerTipoMensaje (char* tipo);

uint32_t socketCliente(char* ip, uint32_t puerto);

uint32_t sizeStream (uint32_t tipoMensaje, char* argv[], uint32_t proceso);

void enviarMensaje (uint32_t socket, uint32_t sizeStream);

#endif /* GAMEBOY_H_ */
