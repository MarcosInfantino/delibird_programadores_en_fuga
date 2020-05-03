/*
 * messages_lib.c
 *
 *  Created on: 12 abr. 2020
 *      Author: utnso
 */
#include "messages_lib.h"

t_paquete* armarPaquete(char* cadena){
	t_buffer* buffer=malloc(sizeof(t_buffer));
	buffer->size=strlen(cadena)+1;
	buffer->stream=malloc(buffer->size);
	memcpy(buffer->stream,cadena,buffer->size);
	t_paquete* paquete=malloc(sizeof(t_paquete));
	paquete->codigo_operacion=CADENA;
	paquete->buffer=buffer;

	return paquete;

}

void* serializarPaquete1 (t_paquete* paquete, int bytes){
	void* a_enviar=malloc(bytes);
		int offset=0;
		memcpy(a_enviar+offset,&(paquete->codigo_operacion),sizeof(op_code));
		offset+=sizeof(op_code);

		memcpy(a_enviar+offset,&(paquete->buffer->size),sizeof(int));
		offset+=sizeof(int);

		memcpy(a_enviar+offset,(paquete->buffer->stream),paquete->buffer->size);


		return a_enviar;
}

void enviarMensaje(char* cadena,int socket){
	t_paquete* paquete= armarPaquete(cadena);

	int bytes=sizeof(op_code)+sizeof(paquete->buffer->size)+paquete->buffer->size;

	void * a_enviar=serializarPaquete1(paquete, bytes);
	send(socket,a_enviar,bytes,0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	free(a_enviar);
}

//void enviarMensajeSuscripcion(mensajeSuscripcion* mensaje, int socket){
//	uint32_t bytes=sizeof(uint32_t)*4;
//	void* stream=serializarMensajeSuscripcion(mensaje, bytes);
//	send(socket,stream,bytes,0);
//}

void* serializarAppearedBroker(mensajeAppearedBroker* mensaje){
	void* stream = malloc(sizeof(uint32_t)*4 + mensaje->sizePokemon);
	uint32_t offset = 0;
	memcpy(stream+offset, &(mensaje->sizePokemon), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, mensaje->pokemon, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(stream+offset, &(mensaje->posX), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->posY), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->id), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	return mensaje;
}

void* serializarAppearedTeam(mensajeAppearedTeam* mensaje){
	void* stream = malloc(sizeof(uint32_t)*3+mensaje->sizePokemon);
	uint32_t offset = 0;
	memcpy(stream+offset, &(mensaje->sizePokemon), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, mensaje->pokemon, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(stream+offset, &(mensaje->posX), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->posY), sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	return mensaje;
}

void* serializarNewBroker(mensajeNewBroker* mensaje){
	void* stream = malloc(sizeof(uint32_t)*4+mensaje->sizePokemon);
	uint32_t offset = 0;
	memcpy(stream+offset, &(mensaje->sizePokemon), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, mensaje->pokemon, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(stream+offset, &(mensaje->posX), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->posY), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->cantidad), sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	return mensaje;
}

void* serializarNewGamecard(mensajeNewGamecard* mensaje){
	void* stream = malloc(sizeof(uint32_t)*5+mensaje->sizePokemon);
	uint32_t offset = 0;
	memcpy(stream+offset, &(mensaje->sizePokemon), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, mensaje->pokemon, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(stream+offset, &(mensaje->posX), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->posY), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->cantidad), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->id), sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	return mensaje;
}

void* serializarCatchBroker(mensajeCatchBroker* mensaje){
	void* stream = malloc(sizeof(uint32_t)*3+mensaje->sizePokemon);
	uint32_t offset = 0;
	memcpy(stream+offset, &(mensaje->sizePokemon), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, mensaje->pokemon, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(stream+offset, &(mensaje->posX), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->posY), sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	return mensaje;
}

void* serializarCatchGamecard(mensajeCatchGamecard* mensaje){
	void* stream = malloc(sizeof(uint32_t)*4 + mensaje->sizePokemon);
	uint32_t offset = 0;
	memcpy(stream+offset, &(mensaje->sizePokemon), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, mensaje->pokemon, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(stream+offset, &(mensaje->posX), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->posY), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->id), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	return mensaje;
}

void* serializarCaught (mensajeCaught* mensaje){
	void* stream = malloc(sizeof(uint32_t)*4);
	uint32_t offset = 0;
	memcpy(stream+offset, &(mensaje->id), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->resultadoCaught), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	return stream;
}

void* serializarGet (mensajeGet* mensaje){
	void* stream = malloc(sizeof(uint32_t)*3+mensaje->sizePokemon);
	uint32_t offset=0;
	memcpy(stream+offset,&(mensaje->sizePokemon),sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset,mensaje->pokemon,mensaje->sizePokemon);
	return stream;
}

void* serializarMensajeSuscripcionTiempo(mensajeSuscripcionTiempo* mensaje){

		void* stream=malloc(sizeof(uint32_t)*2);
		uint32_t offset=0;

		memcpy(stream+offset, &(mensaje->cola), sizeof(uint32_t));
		offset+=sizeof(uint32_t);

		memcpy(stream+offset,&(mensaje->tiempo),sizeof(uint32_t));

		return stream;
}

void* serializarMensajeSuscripcion(mensajeSuscripcion* mensaje){

	void* stream=malloc(sizeof(uint32_t)*2);
	uint32_t offset=0;

	memcpy(stream+offset, &(mensaje->cola), sizeof(uint32_t));

	return stream;
}

void* serializarPaquete(paquete* paqueteASerializar){
	void* paquete = malloc(sizeof(uint32_t)*3+paqueteASerializar->sizeStream);
	uint32_t offset=0;

	memcpy(paquete+offset, &(paqueteASerializar->modulo), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(paquete+offset, &(paqueteASerializar->tipoMensaje), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(paquete+offset, &(paqueteASerializar->id), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(paquete+offset, &(paqueteASerializar->idCorrelativo), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(paquete+offset, &(paqueteASerializar->sizeStream), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(paquete+offset,paqueteASerializar->stream,paqueteASerializar->sizeStream);

	return paquete;
}











