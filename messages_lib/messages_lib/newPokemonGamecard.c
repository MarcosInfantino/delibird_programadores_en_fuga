/*
 * newPokemonGamecard.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "messages_lib.h"

mensajeNewGamecard* llenarMensajeNewGameCard(char* pokemon, uint32_t posX, uint32_t posY, uint32_t cantidad, uint32_t id){
	mensajeNewGamecard* msg=malloc(sizeof(mensajeNewGamecard));
	msg->sizePokemon=strlen(pokemon)+1;
	msg->pokemon=malloc(msg->sizePokemon);
	strcpy(msg->pokemon,pokemon);
	msg->posX=posX;
	msg->posY=posY;
	msg->cantidad=cantidad;
	msg->id=id;
	return msg;

}
void* serializarNewGamecard(mensajeNewGamecard* mensaje){
	void* stream    = malloc(sizeof(uint32_t)*5 + mensaje->sizePokemon);
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
	memcpy(stream+offset, &(mensaje->id), sizeof(uint32_t));
	offset+= sizeof(uint32_t);

	return stream;
}

mensajeNewGamecard* deserializarNewGamecard (void* streamRecibido){
	mensajeNewGamecard* mensaje = malloc(sizeof(mensajeNewGamecard));
	uint32_t offset				= 0;

	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensaje->pokemon=malloc(mensaje->sizePokemon);
	memcpy(mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(&(mensaje->posX), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->posY), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->cantidad), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->id), streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

void destruirNewGamecard(mensajeNewGamecard* msg){
	free(msg->pokemon);
	free(msg);
}
