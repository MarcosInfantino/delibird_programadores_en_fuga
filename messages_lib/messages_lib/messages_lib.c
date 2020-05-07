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

void* serializarSuscripcionTiempo(mensajeSuscripcionTiempo* mensaje){

		void* stream=malloc(sizeof(uint32_t)*2);
		uint32_t offset=0;

		memcpy(stream+offset, &(mensaje->cola), sizeof(uint32_t));
		offset+=sizeof(uint32_t);

		memcpy(stream+offset,&(mensaje->tiempo),sizeof(uint32_t));

		return stream;
}

void* serializarSuscripcion(mensajeSuscripcion* mensaje){

	void* stream=malloc(sizeof(uint32_t)*2);
	uint32_t offset=0;

	memcpy(stream+offset, &(mensaje->cola), sizeof(uint32_t));

	return stream;
}

paquete* llenarPaquete( uint32_t modulo,uint32_t tipoMensaje, uint32_t sizeStream,void* stream){
	paquete* paqueteASerializar = malloc(sizeof(paquete));
	paqueteASerializar->modulo=modulo;
	paqueteASerializar->tipoMensaje=tipoMensaje;
	paqueteASerializar->id=-1;
	paqueteASerializar->idCorrelativo=-1;
	paqueteASerializar->sizeStream=sizeStream;
	paqueteASerializar->stream=stream;

	return paqueteASerializar;
}

void* serializarPaquete(paquete* paqueteASerializar){
	void* paquete = malloc(sizeof(uint32_t)*5+paqueteASerializar->sizeStream);
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

mensajeAppearedBroker* deserializarAppearedBroker(void* streamRecibido){
	mensajeAppearedBroker* mensaje = malloc(sizeof(mensajeAppearedBroker));
	uint32_t offset=0;

	memcpy((void*)mensaje->sizePokemon, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy((void*)mensaje->posX, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->posY, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->id, streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

mensajeAppearedTeam* deserializarAppearedTeam (void* streamRecibido){
	mensajeAppearedTeam* mensaje = malloc(sizeof(mensajeAppearedTeam));
	uint32_t offset=0;

	memcpy((void*)mensaje->sizePokemon, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy((void*)mensaje->posX, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->posY, streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

mensajeNewBroker* deserializarNewBroker (void* streamRecibido){
	mensajeNewBroker* mensaje = malloc(sizeof(mensajeNewBroker));
	uint32_t offset = 0;

	memcpy((void*)mensaje->sizePokemon, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy((void*)mensaje->posX, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->posY, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->cantidad, streamRecibido+offset, sizeof(int32_t));

	return mensaje;
}

mensajeNewGamecard* deserializarNewGamecard (void* streamRecibido){
	mensajeNewGamecard* mensaje = malloc(sizeof(mensajeNewGamecard));
	uint32_t offset=0;

	memcpy((void*)mensaje->sizePokemon, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy((void*)mensaje->posX, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->posY, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->cantidad, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->id, streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

mensajeCatchBroker* deserializarCatchBroker (void* streamRecibido){
	mensajeCatchBroker* mensaje = malloc(sizeof(mensajeCatchBroker));
	uint32_t offset = 0;

	memcpy((void*)mensaje->sizePokemon, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy((void*)mensaje->posX, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->posY, streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

mensajeCatchGamecard* deserializarCatchGamecard (void* streamRecibido){
	mensajeCatchGamecard* mensaje = malloc(sizeof(mensajeCatchGamecard));
	uint32_t offset=0;

	memcpy((void*)mensaje->sizePokemon, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy((void*)mensaje->posX, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->posY, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->id, streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

mensajeCaught* deserializarCaught(void* streamRecibido){
	mensajeCaught* mensaje = malloc(sizeof(mensajeCaught));
	uint32_t offset=0;

	memcpy((void*)mensaje->id, streamRecibido+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->resultadoCaught, streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

mensajeGet* deserializarGet (void* streamRecibido){
	mensajeGet* mensaje = malloc(sizeof(mensajeGet));
	uint32_t offset=0;

	memcpy((void*)mensaje->sizePokemon, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);

	return mensaje;
}

mensajeSuscripcionTiempo* deserializarSuscripcionTiempo(void* streamRecibido){
	mensajeSuscripcionTiempo* mensaje = malloc(sizeof(mensajeSuscripcionTiempo));
	uint32_t offset = 0;

	memcpy((void*)mensaje->cola, streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)mensaje->tiempo, streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

mensajeSuscripcion* deserializarSuscripcion (void* streamRecibido){
	mensajeSuscripcion* mensaje = malloc(sizeof(mensajeSuscripcion));

	memcpy((void*)mensaje->cola, streamRecibido, sizeof(uint32_t));

	return mensaje;
}

paquete* deserializarPaquete(void* paqueteRecibido){
	paquete* paquete = malloc(sizeof(paquete));
	uint32_t offset=0;

	memcpy((void*)paquete->modulo, paqueteRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)paquete->tipoMensaje, paqueteRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)paquete->id, paqueteRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)paquete->idCorrelativo, paqueteRecibido+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy((void*)paquete->sizeStream, paqueteRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	paquete->stream = malloc(paquete->sizeStream);
	memcpy((void*)paquete->stream, paqueteRecibido+offset, paquete->sizeStream);

	free(paqueteRecibido);
	return paquete;
}













