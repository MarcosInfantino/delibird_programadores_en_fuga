/*
 * appearedPokemonBroker.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "messages_lib.h"

mensajeAppearedBroker* llenarMensajeAppearedBroker(char* pokemon, uint32_t posX,uint32_t posY, uint32_t idCorrelativo){
	mensajeAppearedBroker* msg = malloc(sizeof(mensajeAppearedBroker));
	msg->sizePokemon=strlen(pokemon) + 1;
	msg->pokemon=malloc(msg->sizePokemon);
	strcpy(msg->pokemon,pokemon);
	msg->posX=posX;
	msg->posY=posY;
	msg->idCorrelativo=idCorrelativo;
	return msg;

}
void* serializarAppearedBroker(mensajeAppearedBroker* mensaje){
	void* stream = malloc(sizeof(uint32_t)*4 + mensaje->sizePokemon);
	uint32_t offset = 0;

	memcpy(stream+offset, &(mensaje->sizePokemon), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, mensaje->pokemon, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(stream+offset, &(mensaje->posX), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->posY), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->idCorrelativo), sizeof(uint32_t));
	offset+= sizeof(uint32_t);

	return stream;
}

mensajeAppearedBroker* deserializarAppearedBroker(void* streamRecibido){
	mensajeAppearedBroker* mensaje = malloc(sizeof(mensajeAppearedBroker));
	uint32_t offset                = 0;

	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensaje->pokemon=malloc(mensaje->sizePokemon);
	memcpy(mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(&(mensaje->posX), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->posY), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->idCorrelativo), streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

void destruirAppearedBroker(mensajeAppearedBroker* msg){
	free(msg->pokemon);
	free(msg);
}
