/*
 * getPokemonBroker.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "messages_lib.h"

mensajeGetBroker* llenarMensajeGetBroker(char* pokemon){
	mensajeGetBroker* msg = malloc(sizeof(mensajeGetBroker));
	msg->sizePokemon = strlen(pokemon)+1;
	msg->pokemon=malloc(msg->sizePokemon);
	strcpy(msg->pokemon,pokemon);
	return msg;
}

void* serializarGetBroker (mensajeGetBroker* mensaje){
	void* stream   = malloc(sizeof(uint32_t) + mensaje->sizePokemon);
	uint32_t offset= 0;

	memcpy(stream+offset,&(mensaje->sizePokemon),sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset,mensaje->pokemon,mensaje->sizePokemon);
	return stream;
}

mensajeGetBroker* deserializarGetBroker (void* streamRecibido){
	mensajeGetBroker* mensaje = malloc(sizeof(mensajeGetBroker));
	uint32_t offset		= 0;

	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensaje->pokemon=malloc(mensaje->sizePokemon);
	memcpy(mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);

	return mensaje;
}

void destruirGetBroker(mensajeGetBroker* msg){
	free(msg->pokemon);
	free(msg);
}
