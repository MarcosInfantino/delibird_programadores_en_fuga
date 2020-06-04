/*
 * getPokemonBroker.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "messages_lib.h"

mensajeGet* llenarGet(char* pokemon){
	mensajeGet* msg = malloc(sizeof(mensajeGet));
	msg->sizePokemon = strlen(pokemon)+1;
	msg->pokemon=malloc(msg->sizePokemon);
	strcpy(msg->pokemon,pokemon);
	return msg;
}

void* serializarGet (mensajeGet* mensaje){
	void* stream   = malloc(sizeof(uint32_t) + mensaje->sizePokemon);
	uint32_t offset= 0;

	memcpy(stream+offset,&(mensaje->sizePokemon),sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset,mensaje->pokemon,mensaje->sizePokemon);
	return stream;
}

mensajeGet* deserializarGet (void* streamRecibido){
	mensajeGet* mensaje = malloc(sizeof(mensajeGet));
	uint32_t offset		= 0;

	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensaje->pokemon=malloc(mensaje->sizePokemon);
	memcpy(mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);

	return mensaje;
}

void destruirGet(mensajeGet* msg){
	free(msg->pokemon);
	free(msg);
}
