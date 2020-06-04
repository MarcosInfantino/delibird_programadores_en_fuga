/*
 * getPokemonGamecard.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "messages_lib.h"
//
//mensajeGetGamecard* llenarMensajeGetGamecard(char* pokemon, uint32_t id){
//	mensajeGetGamecard* msg = malloc(sizeof(mensajeGetGamecard));
//	msg->sizePokemon=strlen(pokemon)+1;
//	msg->pokemon=malloc(msg->sizePokemon);
//	strcpy(msg->pokemon,pokemon);
//	msg->id=id;
//	return msg;
//
//}
//void* serializarGetGamecard (mensajeGetGamecard* mensaje){
//	void* stream   = malloc(sizeof(uint32_t)*2 + mensaje->sizePokemon);
//	uint32_t offset= 0;
//
//	memcpy(stream+offset,&(mensaje->sizePokemon),sizeof(uint32_t));
//	offset+= sizeof(uint32_t);
//	memcpy(stream+offset,mensaje->pokemon,mensaje->sizePokemon);
//	offset+= mensaje->sizePokemon;
//	memcpy(stream+offset, &(mensaje->id), sizeof(uint32_t));
//	return stream;
//}
//
//mensajeGetGamecard* deserializarGetGamecard (void* streamRecibido){
//	mensajeGetGamecard* mensaje = malloc(sizeof(mensajeGetGamecard));
//	uint32_t offset		= 0;
//
//	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
//	offset+=sizeof(uint32_t);
//	mensaje->pokemon=malloc(mensaje->sizePokemon);
//	memcpy(mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);
//	offset+=mensaje->sizePokemon;
//	memcpy(&(mensaje->id), streamRecibido+offset, sizeof(uint32_t));
//	return mensaje;
//}
//
//void destruirGetGamecard(mensajeGetGamecard* msg){
//	free(msg->pokemon);
//	free(msg);
//}
