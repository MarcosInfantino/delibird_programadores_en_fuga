/*
 * suscripcion.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "messages_lib.h"

mensajeSuscripcion* llenarSuscripcion(uint32_t cola, uint32_t idProceso){
	mensajeSuscripcion* msg = malloc(sizeof(mensajeSuscripcion));
	msg->cola=cola;
	msg->idProceso=idProceso;
	return msg;
}

void* serializarSuscripcion(mensajeSuscripcion* mensaje){

	void* stream    = malloc(2*sizeof(uint32_t));
	uint32_t offset = 0;

	memcpy(stream+offset, &(mensaje->cola), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->idProceso), sizeof(uint32_t));

	return stream;
}

void destruirSuscripcion(mensajeSuscripcion* msg){
	free(msg);
}

mensajeSuscripcion* deserializarSuscripcion (void* streamRecibido){
	mensajeSuscripcion* mensaje = malloc(sizeof(mensajeSuscripcion));

	uint32_t offset=0;
	memcpy(&(mensaje->cola), streamRecibido, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->idProceso), streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}
