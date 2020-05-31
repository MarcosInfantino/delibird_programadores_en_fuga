/*
 * suscripcionTiempo.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "messages_lib.h"

mensajeSuscripcionTiempo* llenarMensajeSuscripcionTiempo(uint32_t cola, uint32_t tiempo){
	mensajeSuscripcionTiempo* msg= malloc(sizeof(mensajeSuscripcionTiempo));
	msg->cola=cola;
	msg->tiempo=tiempo;
	return msg;
}

void* serializarSuscripcionTiempo(mensajeSuscripcionTiempo* mensaje){

		void* stream    = malloc(sizeof(uint32_t)*2);
		uint32_t offset = 0;

		memcpy(stream+offset, &(mensaje->cola), sizeof(uint32_t));
		offset+= sizeof(uint32_t);

		memcpy(stream+offset,&(mensaje->tiempo),sizeof(uint32_t));

		return stream;
}

mensajeSuscripcionTiempo* deserializarSuscripcionTiempo(void* streamRecibido){
	mensajeSuscripcionTiempo* mensaje = malloc(sizeof(mensajeSuscripcionTiempo));
	uint32_t offset 				  = 0;

	memcpy(&(mensaje->cola), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->tiempo), streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

void destruirSuscripcionTiempo(mensajeSuscripcionTiempo* msg){
	free(msg);
}
