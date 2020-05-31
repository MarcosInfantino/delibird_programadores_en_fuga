/*
 * appearedPokemonTeam.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "messages_lib.h"

mensajeAppearedTeam* llenarMensajeAppearedTeam(char* pokemon, uint32_t posX, uint32_t posY){
	mensajeAppearedTeam* msg = malloc(sizeof(mensajeAppearedTeam));
	msg->sizePokemon=strlen(pokemon)+1;
	msg->pokemon=malloc(msg->sizePokemon);
	memcpy(msg->pokemon,pokemon,msg->sizePokemon);
	msg->posX=posX;
	msg->posY=posY;

	return msg;
}
void* serializarAppearedTeam(mensajeAppearedTeam* mensaje){
	void* stream    = malloc(sizeof(uint32_t)*3 + mensaje->sizePokemon);
	uint32_t offset = 0;

	memcpy(stream+offset, &(mensaje->sizePokemon), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, mensaje->pokemon, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(stream+offset, &(mensaje->posX), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->posY), sizeof(uint32_t));
	offset+= sizeof(uint32_t);

	return stream;
}

mensajeAppearedTeam* deserializarAppearedTeam (void* streamRecibido){
	uint32_t offset              = 0;
	mensajeAppearedTeam* mensaje = malloc(sizeof(mensajeAppearedTeam));

	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensaje->pokemon=malloc(mensaje->sizePokemon);
	memcpy(mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(&(mensaje->posX), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->posY), streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

void destruirAppearedTeam(mensajeAppearedTeam* msg){
	free(msg->pokemon);
	free(msg);
}
