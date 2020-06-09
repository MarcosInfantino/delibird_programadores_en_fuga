/*
 * paquete.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "messages_lib.h"

paquete* llenarPaquete( uint32_t modulo,uint32_t tipoMensaje, uint32_t sizeStream,void* stream){
	paquete* paqueteASerializar       = malloc(sizeof(paquete));
	paqueteASerializar->modulo        = modulo;
	paqueteASerializar->tipoMensaje   = tipoMensaje;
	paqueteASerializar->id            = -1;
	paqueteASerializar->idCorrelativo = -1;
	paqueteASerializar->sizeStream    = sizeStream;
	if(paqueteASerializar->sizeStream>0){
		paqueteASerializar->stream = malloc(paqueteASerializar->sizeStream);
		memcpy(paqueteASerializar->stream,stream,sizeStream);
	}else{
		paqueteASerializar->stream=NULL;
	}
	return paqueteASerializar;
}

uint32_t sizePaquete(paquete* paq){
	return paq->sizeStream + sizeof(uint32_t)*5;
}

void destruirPaquete(paquete* paq){
	if(paq->stream!=NULL){
		free(paq->stream);
	}
	free(paq);
}

void* serializarPaquete(paquete* paqueteASerializar){
	void* paq   = malloc(sizeof(uint32_t)*5+paqueteASerializar->sizeStream);
	uint32_t offset = 0;

	memcpy(paq+offset, &(paqueteASerializar->modulo), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(paq+offset, &(paqueteASerializar->tipoMensaje), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(paq+offset, &(paqueteASerializar->id), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(paq+offset, &(paqueteASerializar->idCorrelativo), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(paq+offset, &(paqueteASerializar->sizeStream), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	if(paqueteASerializar->sizeStream>0){
		memcpy(paq+offset,paqueteASerializar->stream,paqueteASerializar->sizeStream);
	}
	return paq;
}

paquete* deserializarPaquete(void* paqueteRecibido){
	paquete* paq = malloc(sizeof(paquete));
	uint32_t offset  = 0;

	memcpy(&(paq->modulo), paqueteRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(paq->tipoMensaje), paqueteRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(paq->id), paqueteRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(paq->idCorrelativo), paqueteRecibido+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(paq->sizeStream), paqueteRecibido+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	if(paq->sizeStream>0){
	paq->stream = malloc(paq->sizeStream);

	memcpy(paq->stream, paqueteRecibido+offset, paq->sizeStream);
	}else{
		paq->stream=NULL;
	}

	free(paqueteRecibido);
	return paq;
}


paquete* recibirPaquete(uint32_t socket){
	paquete* paq = malloc(sizeof(paquete));

	if(recv(socket,&(paq->modulo),sizeof(uint32_t),0)==-1){
		return NULL;
	}
	if(recv(socket,&(paq->tipoMensaje),sizeof(uint32_t),0)==-1){
		return NULL;
	}
	if(recv(socket,&(paq->id),sizeof(uint32_t),0)==-1){
		return NULL;
	}
	if(recv(socket,&(paq->idCorrelativo),sizeof(uint32_t),0)==-1){
		return NULL;
	}
	if(recv(socket,&(paq->sizeStream),sizeof(uint32_t),0)==-1){
		return NULL;
	}

	if(paq->sizeStream>0){
	paq->stream = malloc(paq->sizeStream);

	if(recv(socket,paq->stream,(paq->sizeStream),0)==-1){
		return NULL;
	}}else{
		paq->stream=NULL;
	}

	return paq;
}

void insertarIdPaquete(paquete* paq, uint32_t id){
	paq->id=id;
}

void insertarIdCorrelativoPaquete(paquete* paq, uint32_t idCorrelativo){
	paq->idCorrelativo=idCorrelativo;
}
