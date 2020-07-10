/*
 * newPokemonBroker.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "messages_lib.h"

mensajeNew* llenarNew(char* pokemon, uint32_t posX, uint32_t posY, uint32_t cantidad){
	mensajeNew* msg=malloc(sizeof(mensajeNew));
		msg->sizePokemon=strlen(pokemon);
		msg->pokemon=malloc(msg->sizePokemon + 1);
		strcpy(msg->pokemon,pokemon);
		msg->posX=posX;
		msg->posY=posY;
		msg->cantidad=cantidad;
		return msg;

}
void* serializarNew(mensajeNew* mensaje){
	void* stream 	= malloc(sizeof(uint32_t)*4 + mensaje->sizePokemon);
	uint32_t offset = 0;

	memcpy(stream+offset, &(mensaje->sizePokemon), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, mensaje->pokemon, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(stream+offset, &(mensaje->posX), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->posY), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->cantidad), sizeof(uint32_t));
	offset+= sizeof(uint32_t);

	return stream;
}

mensajeNew* deserializarNew (void* streamRecibido){
	mensajeNew* mensaje = malloc(sizeof(mensajeNew));
	uint32_t offset 		  = 0;

	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	mensaje->pokemon=malloc(mensaje->sizePokemon+1);
	memcpy(mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	*(mensaje->pokemon + mensaje->sizePokemon)='\0';

	memcpy(&(mensaje->posX), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->posY), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->cantidad), streamRecibido+offset, sizeof(int32_t));

	return mensaje;
}


void destruirNew(mensajeNew* msg){
	free(msg->pokemon);
	free(msg);
}
