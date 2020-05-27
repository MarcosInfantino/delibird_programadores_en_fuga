/*
 * messages_lib.c
 *
 *  Created on: 12 abr. 2020
 *      Author: utnso
 */
#include "messages_lib.h"

//void enviarMensajeSuscripcion(mensajeSuscripcion* mensaje, int socket){
//	uint32_t bytes=sizeof(uint32_t)*4;
//	void* stream=serializarMensajeSuscripcion(mensaje, bytes);
//	send(socket,stream,bytes,0);
//}

mensajeAppearedBroker* llenarMensajeAppearedBroker(char* pokemon, uint32_t posX,uint32_t posY, uint32_t idCorrelativo){
	mensajeAppearedBroker* msg=malloc(sizeof(mensajeAppearedBroker));
	msg->sizePokemon=strlen(pokemon) + 1;
	msg->pokemon=malloc(msg->sizePokemon);
	strcpy(msg->pokemon,pokemon);
	msg->posX=posX;
	msg->posY=posY;
	msg->idCorrelativo=idCorrelativo;
	return msg;

}
void* serializarAppearedBroker(mensajeAppearedBroker* mensaje){
	void* stream    = malloc(sizeof(uint32_t)*4 + mensaje->sizePokemon);
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
	return mensaje;
}

mensajeAppearedTeam* llenarMensajeAppearedTeam(char* pokemon, uint32_t posX, uint32_t posY){
	mensajeAppearedTeam* msg=malloc(sizeof(mensajeAppearedTeam));
	msg->sizePokemon=strlen(pokemon)+1;
	msg->pokemon=malloc(msg->sizePokemon);
	strcpy(msg->pokemon,pokemon);
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

	return mensaje;
}

mensajeNewBroker* llenarMensajeNewBroker(char* pokemon, uint32_t posX, uint32_t posY, uint32_t cantidad){
	mensajeNewBroker* msg=malloc(sizeof(mensajeNewBroker));
		msg->sizePokemon=strlen(pokemon)+1;
		msg->pokemon=malloc(msg->sizePokemon);
		strcpy(msg->pokemon,pokemon);
		msg->posX=posX;
		msg->posY=posY;
		msg->cantidad=cantidad;
		return msg;

}
void* serializarNewBroker(mensajeNewBroker* mensaje){
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

	return mensaje;
}

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

	return mensaje;
}

mensajeCatchBroker* llenarMensajeCatchBroker(char* pokemon, uint32_t posX, uint32_t posY){

	mensajeCatchBroker* msg=malloc(sizeof(mensajeCatchBroker));
	msg->sizePokemon=strlen(pokemon)+1;
	msg->pokemon=malloc(msg->sizePokemon);
	strcpy(msg->pokemon,pokemon);
	msg->posX=posX;
	msg->posY=posY;
	return msg;
}

void* serializarCatchBroker(mensajeCatchBroker* mensaje){
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

	return mensaje;
}

mensajeCatchGamecard* llenarMensajeCatchGamecard(char* pokemon, uint32_t posX, uint32_t posY, uint32_t id){
	mensajeCatchGamecard* msg=malloc(sizeof(mensajeCatchGamecard));
	msg->sizePokemon=strlen(pokemon)+1;
	msg->pokemon=malloc(msg->sizePokemon);
	strcpy(msg->pokemon,pokemon);
	msg->posX=posX;
	msg->posY=posY;
	msg->id=id;
	return msg;

}
void* serializarCatchGamecard(mensajeCatchGamecard* mensaje){
	void* stream    = malloc(sizeof(uint32_t)*4 + mensaje->sizePokemon);
	uint32_t offset = 0;

	memcpy(stream+offset, &(mensaje->sizePokemon), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, mensaje->pokemon, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(stream+offset, &(mensaje->posX), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->posY), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->id), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	return mensaje;
}

mensajeCaught* llenarMensajeCaught(uint32_t idCorrelativo, uint32_t resultadoCaught){
	mensajeCaught* msg=malloc(sizeof(mensajeCaught));
	msg->idCorrelativo=idCorrelativo;
	msg->resultadoCaught=resultadoCaught;
	return msg;

}

void* serializarCaught (mensajeCaught* mensaje){
	void* stream    = malloc(sizeof(uint32_t)*2);
	uint32_t offset = 0;

	memcpy(stream+offset, &(mensaje->idCorrelativo), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->resultadoCaught), sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	return stream;
}
mensajeGetBroker* llenarMensajeGetBroker(char* pokemon){
	mensajeGetBroker* msg=malloc(sizeof(mensajeGetBroker));
	msg->sizePokemon=strlen(pokemon)+1;
	msg->pokemon=malloc(msg->sizePokemon);
	strcpy(msg->pokemon,pokemon);
	return msg;
}

void* serializarGetBroker (mensajeGetBroker* mensaje){
	void* stream   = malloc(sizeof(uint32_t) + mensaje->sizePokemon);
	uint32_t offset= 0;

	memcpy(stream+offset,&(mensaje->sizePokemon),sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset,mensaje->pokemon,mensaje->sizePokemon);
	return stream;
}

mensajeGetGamecard* llenarMensajeGetGamecard(char* pokemon, uint32_t id){
	mensajeGetGamecard* msg=malloc(sizeof(mensajeGetGamecard));
	msg->sizePokemon=strlen(pokemon)+1;
	msg->pokemon=malloc(msg->sizePokemon);
	strcpy(msg->pokemon,pokemon);
	msg->id=id;
	return msg;

}
void* serializarGetGamecard (mensajeGetGamecard* mensaje){
	void* stream   = malloc(sizeof(uint32_t)*2 + mensaje->sizePokemon);
	uint32_t offset= 0;

	memcpy(stream+offset,&(mensaje->sizePokemon),sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset,mensaje->pokemon,mensaje->sizePokemon);
	offset+= mensaje->sizePokemon;
	memcpy(stream+offset, &(mensaje->id), sizeof(uint32_t));
	return stream;
}

mensajeLocalized* llenarMensajeLocalized(char* pokemon, uint32_t cantidad, posicion* posiciones){
	mensajeLocalized* msg=malloc(sizeof(mensajeLocalized));
	msg->sizePokemon=strlen(pokemon)+1;
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

}//hay que checkear funcionamiento

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
	char* pokemon=malloc(sizePokemon);
	memcpy(pokemon,stream+offset,sizePokemon);
	offset+=sizePokemon;
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

mensajeSuscripcionTiempo* llenarMensajeSuscripcionTiempo(uint32_t cola, uint32_t tiempo){
	mensajeSuscripcionTiempo* msg= malloc(sizeof(mensajeSuscripcionTiempo));
	msg->cola=cola;
	msg->tiempo=tiempo;
	return msg;

}
void* serializarSuscripcionTiempo(mensajeSuscripcionTiempo* mensaje){

		void* stream    = malloc(sizeof(mensajeSuscripcionTiempo));
		uint32_t offset = 0;

		memcpy(stream+offset, &(mensaje->cola), sizeof(uint32_t));
		offset+= sizeof(uint32_t);

		memcpy(stream+offset,&(mensaje->tiempo),sizeof(uint32_t));

		return stream;
}

mensajeSuscripcion* llenarMensajeSuscripcion(uint32_t cola){
	mensajeSuscripcion* msg=malloc(sizeof(mensajeSuscripcion));
	msg->cola=cola;
	return msg;
}
void* serializarSuscripcion(mensajeSuscripcion* mensaje){

	void* stream    = malloc(sizeof(mensajeSuscripcion));
	uint32_t offset = 0;

	memcpy(stream+offset, &(mensaje->cola), sizeof(uint32_t));

	return stream;
}

paquete* llenarPaquete( uint32_t modulo,uint32_t tipoMensaje, uint32_t sizeStream,void* stream){
	paquete* paqueteASerializar       = malloc(sizeof(paquete));
	paqueteASerializar->modulo        = modulo;
	paqueteASerializar->tipoMensaje   = tipoMensaje;
	paqueteASerializar->id            = -1;
	paqueteASerializar->idCorrelativo = -1;
	paqueteASerializar->sizeStream    = sizeStream;
	paqueteASerializar->stream        = stream;

	return paqueteASerializar;
}

void destruirPaquete(paquete* paq){
	free(paq->stream);
	//free(paq);
}

void* serializarPaquete(paquete* paqueteASerializar){
	void* paquete   = malloc(sizeof(uint32_t)*5+paqueteASerializar->sizeStream);
	uint32_t offset = 0;

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

mensajeAppearedTeam* deserializarAppearedTeam (void* streamRecibido){
	mensajeAppearedTeam* mensaje = malloc(sizeof(mensajeAppearedTeam));
	uint32_t offset              = 0;
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
mensajeNewBroker* deserializarNewBroker (void* streamRecibido){
	mensajeNewBroker* mensaje = malloc(sizeof(mensajeNewBroker));
	uint32_t offset 		  = 0;

	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensaje->pokemon=malloc(mensaje->sizePokemon);
	memcpy(mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(&(mensaje->posX), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->posY), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->cantidad), streamRecibido+offset, sizeof(int32_t));

	return mensaje;
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

mensajeCatchBroker* deserializarCatchBroker (void* streamRecibido){
	mensajeCatchBroker* mensaje = malloc(sizeof(mensajeCatchBroker));
	uint32_t offset 			= 0;

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

mensajeCatchGamecard* deserializarCatchGamecard (void* streamRecibido){
	mensajeCatchGamecard* mensaje = malloc(sizeof(mensajeCatchGamecard));
	uint32_t offset				  = 0;

	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensaje->pokemon=malloc(mensaje->sizePokemon);
	memcpy(mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);
	offset+=(mensaje->sizePokemon);
	memcpy(&(mensaje->posX), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->posY), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->id), streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

mensajeCaught* deserializarCaught(void* streamRecibido){
	mensajeCaught* mensaje = malloc(sizeof(mensajeCaught));
	uint32_t offset        = 0;

	memcpy(&(mensaje->idCorrelativo), streamRecibido+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->resultadoCaught), streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

mensajeGetBroker* deserializarGetBroker (void* streamRecibido){
	mensajeGetBroker* mensaje = malloc(sizeof(mensajeGetBroker));
	uint32_t offset		= 0;

	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensaje->pokemon=malloc(mensaje->sizePokemon);
	memcpy(mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);

	return mensaje;
}

mensajeGetGamecard* deserializarGetGamecard (void* streamRecibido){
	mensajeGetGamecard* mensaje = malloc(sizeof(mensajeGetGamecard));
	uint32_t offset		= 0;

	memcpy(&(mensaje->sizePokemon), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	mensaje->pokemon=malloc(mensaje->sizePokemon);
	memcpy(mensaje->pokemon, streamRecibido+offset, mensaje->sizePokemon);
	offset+=mensaje->sizePokemon;
	memcpy(&(mensaje->id), streamRecibido+offset, sizeof(uint32_t));
	return mensaje;
}

//mensajeLocalized* deserializarLocalized (void* streamRecibido){
//
//}

mensajeSuscripcionTiempo* deserializarSuscripcionTiempo(void* streamRecibido){
	mensajeSuscripcionTiempo* mensaje = malloc(sizeof(mensajeSuscripcionTiempo));
	uint32_t offset 				  = 0;

	memcpy(&(mensaje->cola), streamRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(mensaje->tiempo), streamRecibido+offset, sizeof(uint32_t));

	return mensaje;
}

mensajeSuscripcion* deserializarSuscripcion (void* streamRecibido){
	mensajeSuscripcion* mensaje = malloc(sizeof(mensajeSuscripcion));

	memcpy(&(mensaje->cola), streamRecibido, sizeof(uint32_t));

	return mensaje;
}

paquete* deserializarPaquete(void* paqueteRecibido){
	paquete* paquete = malloc(sizeof(paquete));
	uint32_t offset  = 0;

	memcpy(&(paquete->modulo), paqueteRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(paquete->tipoMensaje), paqueteRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(paquete->id), paqueteRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(paquete->idCorrelativo), paqueteRecibido+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(paquete->sizeStream), paqueteRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	paquete->stream = malloc(paquete->sizeStream);
	memcpy(paquete->stream, paqueteRecibido+offset, paquete->sizeStream);

	free(paqueteRecibido);
	return paquete;
}


paquete* recibirPaquete(uint32_t socket){
	paquete* paquete = malloc(sizeof(paquete));
	uint32_t offset  = 0;

	//recv(cliente,&respuesta1,sizeof(uint32_t),0);

	if(recv(socket,&(paquete->modulo),sizeof(uint32_t),0)==-1)
		return NULL;
	offset+=sizeof(uint32_t);
	if(recv(socket,&(paquete->tipoMensaje),sizeof(uint32_t),0)==-1)
		return NULL;
	offset+=sizeof(uint32_t);
	if(recv(socket,&(paquete->id),sizeof(uint32_t),0)==-1)
		return NULL;
	offset+=sizeof(uint32_t);
	if(recv(socket,&(paquete->idCorrelativo),sizeof(uint32_t),0)==-1)
		return NULL;
	offset+=sizeof(uint32_t);
	if(recv(socket,&(paquete->sizeStream),sizeof(uint32_t),0)==-1)
		return NULL;
	offset+=sizeof(uint32_t);
	paquete->stream = malloc(paquete->sizeStream);
	if(recv(socket,(paquete->stream),(paquete->sizeStream),0)==-1)
		return NULL;

	return paquete;
}

uint32_t crearSocketCliente (char* ip, uint32_t puerto){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family      = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port        = htons(puerto);

	uint32_t cliente=socket(AF_INET,SOCK_STREAM,0);
	if(connect(cliente,(void*) &direccionServidor,sizeof(direccionServidor)) != 0){
		perror("No se pudo conectar");
		return -1;
	}

	return cliente;
}

uint32_t sizeArgumentos (uint32_t colaMensaje, char* nombrePokemon, uint32_t proceso){
	uint32_t size;
	switch(colaMensaje){
	case NEW_POKEMON:
		if(proceso == BROKER){ //size pokemon, pokemon,posx, posy, cantidad
			size = strlen(nombrePokemon) + 1 + sizeof(uint32_t)*4;
		} else if (proceso == GAMECARD){ //size pokemon, pokemon, posX, posY, cantidad, ID
			size = strlen(nombrePokemon) + 1 + sizeof(uint32_t)*5;
		}
		break;

	case APPEARED_POKEMON:
		if(proceso == BROKER){ // sizePokemon, pokemon, posX, posY, ID
			size = strlen(nombrePokemon) +1 + sizeof(uint32_t)*4;
		}else if(proceso == TEAM){ //sizePokemon, pokemon, posX, posY
			size = strlen(nombrePokemon) + 1 +sizeof(uint32_t)*3;
		}
		break;

	case CATCH_POKEMON:
		if(proceso == BROKER){ //sizePokemon, pokemon, posX, posY
			size = strlen(nombrePokemon) + 1 + sizeof(uint32_t)*3;
		}else if (proceso == GAMECARD){ //sizePokemon, pokemon, posX, posY, ID
			size = strlen(nombrePokemon) + 1 + sizeof(uint32_t)*4;
		}
		break;

	case CAUGHT_POKEMON://ID, ok/fail
		size = sizeof(uint32_t)*2;
		break;

	case GET_POKEMON://sizePokemon, pokemon
		if(proceso == BROKER){
			size = strlen(nombrePokemon) + 1 + sizeof(uint32_t);
		}else if(proceso == GAMECARD){
			size=strlen(nombrePokemon) + 1 + sizeof(uint32_t)*2;
		}
		break;

	default:
		printf("Error: el caso ingresado no esta contemplado \n");
		break;
	}
	return size;
}

uint32_t sizePaquete(paquete* paq){
	return paq->sizeStream+ sizeof(uint32_t)*5;
}

listaMutex inicializarListaMutex(){
	listaMutex list;
	list.lista=list_create();
	list.mutex=malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(list.mutex,NULL);
	return list;
}

void addListaMutex(listaMutex list,void* elemento){
	pthread_mutex_lock(list.mutex);
	list_add(list.lista,elemento);
	pthread_mutex_unlock(list.mutex);
}

void* getListaMutex(listaMutex list, uint32_t index){
	pthread_mutex_lock(list.mutex);
	void* value=list_get(list.lista,index);
	pthread_mutex_unlock(list.mutex);
	return value;
}

uint32_t sizeListaMutex(listaMutex list){
	pthread_mutex_lock(list.mutex);
	uint32_t size=list_size(list.lista);
	pthread_mutex_unlock(list.mutex);
	return size;
}

void removeListaMutex(listaMutex list,uint32_t pos){
	pthread_mutex_lock(list.mutex);
	list_remove(list.lista,pos);
	pthread_mutex_unlock(list.mutex);
}

void destruirListaMutex(listaMutex lista,void(*element_destroyer)(void*)){
	free(lista.mutex);
	list_clean_and_destroy_elements(lista.lista, element_destroyer);
}

colaMutex inicializarColaMutex(){
	colaMutex cola;
	cola.cola=queue_create();
	cola.mutex=malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(cola.mutex,NULL);
	return cola;
}

void pushColaMutex(colaMutex cola, void* infoEntrenador){
	pthread_mutex_lock(cola.mutex);
	queue_push(cola.cola,infoEntrenador);
	pthread_mutex_unlock(cola.mutex);

}

void* popColaMutex(colaMutex cola){
	pthread_mutex_lock(cola.mutex);
	void* elemento=queue_pop(cola.cola);
	pthread_mutex_unlock(cola.mutex);
	return elemento;

}

uint32_t sizeColaMutex(colaMutex cola){
	pthread_mutex_lock(cola.mutex);
	uint32_t size=queue_size(cola.cola);
	pthread_mutex_unlock(cola.mutex);
	return size;
}

void destruirColaMutex(colaMutex cola, void(*element_destroyer)(void*)){
	free(cola.mutex);
	queue_destroy_and_destroy_elements(cola.cola,element_destroyer);
}

int main(){
	//mensajeLocalized* llenarMensajeLocalized(uint32_t sizePokemon, char* pokemon, uint32_t cantidad, posicion* posiciones)

	uint32_t sizePokemon=8;
	char* pokemon="Pikachu";
	uint32_t cantidad=2;
	posicion* posiciones=malloc(sizeof(posiciones)*2);
	posicion pos1={1,0};
	posicion pos2={0,0};
	*(posiciones)=pos1;
	*(posiciones+1)=pos2;
	mensajeLocalized* msg= llenarMensajeLocalized(sizePokemon,pokemon,cantidad,posiciones);
	void* stream= serializarLocalized(msg);
	mensajeLocalized* msgResultado=deserializarLocalized(stream);
	printf("sizePokemon: %i\n", msgResultado->sizePokemon);
	printf("pokemon: %s\n", msgResultado->pokemon);
	printf("cantidad: %i\n", msgResultado->cantidad);
	printf("posx1: %i\n", (msgResultado->arrayPosiciones)->x);
	printf("posy1: %i\n", (msgResultado->arrayPosiciones)->y);
	printf("posx2: %i\n", ((msgResultado->arrayPosiciones) +1)->x);
	printf("posy2: %i\n", ((msgResultado->arrayPosiciones) +1)->y);

	return 0;
}

