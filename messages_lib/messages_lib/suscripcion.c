/*
 * suscripcion.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "messages_lib.h"

mensajeSuscripcion* llenarMensajeSuscripcion(uint32_t cola){
	mensajeSuscripcion* msg = malloc(sizeof(mensajeSuscripcion));
	msg->cola=cola;
	return msg;
}

void* serializarSuscripcion(mensajeSuscripcion* mensaje){

	void* stream    = malloc(sizeof(mensajeSuscripcion));
	uint32_t offset = 0;

	memcpy(stream+offset, &(mensaje->cola), sizeof(uint32_t));

	return stream;
}

void destruirSuscripcion(mensajeSuscripcion* msg){
	free(msg);
}

mensajeSuscripcion* deserializarSuscripcion (void* streamRecibido){
	mensajeSuscripcion* mensaje = malloc(sizeof(mensajeSuscripcion));

	memcpy(&(mensaje->cola), streamRecibido, sizeof(uint32_t));

	return mensaje;
}
