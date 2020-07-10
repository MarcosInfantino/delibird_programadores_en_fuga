/*
 * localizedPokemon.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "messages_lib.h"

mensajeLocalized* llenarLocalized(char* pokemon, uint32_t cantidad, posicion* posiciones){
	mensajeLocalized* msg = malloc(sizeof(mensajeLocalized));
	msg->sizePokemon=strlen(pokemon);
	msg->pokemon=malloc(msg->sizePokemon);
	strcpy(msg->pokemon,pokemon);
	msg->cantidad=cantidad;
	msg->arrayPosiciones=malloc(cantidad*sizeof(posicion));
	memcpy(msg->arrayPosiciones,posiciones,cantidad*sizeof(posicion));
	return msg;
}

void* serializarLocalized (mensajeLocalized* mensaje){
	void* stream = malloc(2*sizeof(uint32_t)+mensaje->sizePokemon+(mensaje->cantidad)*2*sizeof(uint32_t));
	uint32_t offset = 0;
	memcpy(stream+offset,&(mensaje->sizePokemon),sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset,mensaje->pokemon,mensaje->sizePokemon);
	offset+= mensaje->sizePokemon;
	memcpy(stream+offset,&(mensaje->cantidad),sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	void* arraySerializado=serializarArrayPosiciones(mensaje->arrayPosiciones,mensaje->cantidad);
	memcpy(stream+offset,arraySerializado,(mensaje->cantidad)*2*sizeof(uint32_t));
	return stream;
}

void destruirLocalized(mensajeLocalized* msg){
	free(msg->arrayPosiciones);
	free(msg->pokemon);
	free(msg);
}

void* serializarPosicion(posicion* pos){
	void* stream=malloc(2*sizeof(uint32_t));
	uint32_t offset=0;
	memcpy(stream+offset,&(pos->x),sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset,&(pos->y),sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	return stream;
}

void* serializarArrayPosiciones(posicion* pos, uint32_t cantidad){
	uint32_t offset=0;
	void* stream=malloc(cantidad*2*sizeof(uint32_t));
	uint32_t i=0;

	for(i=0;i<cantidad;i++){
		posicion* posActual=malloc(sizeof(posicion));
		*posActual=*(pos + i);
		void* posicionSerializada=serializarPosicion(posActual);
		memcpy(stream+offset,posicionSerializada,2*sizeof(uint32_t));
		offset+=2*sizeof(uint32_t);
		free(posActual);
	}
	return stream;
}

mensajeLocalized* deserializarLocalized(void* stream){
	uint32_t sizePokemon,cantidad;
	uint32_t offset=0;
	memcpy(&sizePokemon, stream+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	char* pokemon=malloc(sizePokemon+1);
	memcpy(pokemon, stream+offset,sizePokemon);
	offset+=(sizePokemon);
	*(pokemon + sizePokemon)='\0';

	memcpy(&cantidad,stream+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensajeLocalized* msg=malloc(sizeof(mensajeLocalized));
	msg->sizePokemon=sizePokemon;
	msg->pokemon=pokemon;
	msg->cantidad=cantidad;
	msg->arrayPosiciones=deserializarArrayPosiciones(stream+offset,cantidad);
	return msg;

}

posicion* deserializarArrayPosiciones(void* stream,uint32_t cantidad){
	uint32_t offsetStream=0;
	uint32_t i=0;
	posicion* pos=malloc(cantidad*sizeof(posicion));
	for(i=0;i<cantidad;i++){
			posicion* posActual=malloc(sizeof(posicion));
			posActual=deserializarPosicion(stream+offsetStream);
			offsetStream+=2*sizeof(uint32_t);
			memcpy(pos+i,posActual,sizeof(posicion));
			free(posActual);

		}
	return pos;
}

posicion* deserializarPosicion(void* stream){
	posicion* pos=malloc(sizeof(posicion));
	uint offset=0;
	memcpy(&(pos->x),stream+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(pos->y),stream+offset,sizeof(uint32_t));
	return pos;
}
