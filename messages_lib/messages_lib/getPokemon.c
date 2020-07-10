/*
 * getPokemonBroker.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "messages_lib.h"

mensajeGet* llenarGet(char* pokemon){
	mensajeGet* msg = malloc(sizeof(mensajeGet));
	msg->sizePokemon = strlen(pokemon);
	msg->pokemon=malloc(msg->sizePokemon + 1);
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

	mensaje->pokemon=malloc(mensaje->sizePokemon+1);
	memcpy(mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	*(mensaje->pokemon + mensaje->sizePokemon)='\0';

	return mensaje;
}

void destruirGet(mensajeGet* msg){
	free(msg->pokemon);
	free(msg);
}
