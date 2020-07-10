/*
 * localizedPokemon.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "messages_lib.h"

//mensajeLocalized* llenarLocalized(char* pokemon, uint32_t cantidad, posicion* posiciones){
//	mensajeLocalized* msg = malloc(sizeof(mensajeLocalized));
//	msg->sizePokemon=strlen(pokemon);
//	msg->pokemon=malloc(msg->sizePokemon + 1);
//	strcpy(msg->pokemon,pokemon);
//	msg->cantidad=cantidad;
//	msg->arrayPosiciones=malloc(cantidad*sizeof(posicion));
//	memcpy(msg->arrayPosiciones,posiciones,cantidad*sizeof(posicion));
//	return msg;
//}

mensajeLocalized* llenarLocalized(char* pokemon, t_list* listaPosicionCantidad){
	mensajeLocalized* msg = malloc(sizeof(mensajeLocalized));
	msg->sizePokemon=strlen(pokemon);
	msg->pokemon=malloc(msg->sizePokemon + 1);
	strcpy(msg->pokemon,pokemon);
	//msg->cantidad=cantidad;
	msg-> listaPosicionCantidad=listaPosicionCantidad;
	//memcpy(msg->arrayPosiciones,posiciones,cantidad*sizeof(posicion));
	return msg;
}

//void* serializarLocalized (mensajeLocalized* mensaje){
//	void* stream = malloc(2*sizeof(uint32_t)+mensaje->sizePokemon+(mensaje->cantidad)*2*sizeof(uint32_t));
//	uint32_t offset = 0;
//	memcpy(stream+offset,&(mensaje->sizePokemon),sizeof(uint32_t));
//	offset+= sizeof(uint32_t);
//	memcpy(stream+offset,mensaje->pokemon,mensaje->sizePokemon);
//	offset+= mensaje->sizePokemon;
//	memcpy(stream+offset,&(mensaje->cantidad),sizeof(uint32_t));
//	offset+=sizeof(uint32_t);
//	void* arraySerializado=serializarArrayPosiciones(mensaje->arrayPosiciones,mensaje->cantidad);
//	memcpy(stream+offset,arraySerializado,(mensaje->cantidad)*2*sizeof(uint32_t));
//	return stream;
//}

void* serializarLocalized (mensajeLocalized* mensaje){
	uint32_t sizeListaSerializada=3*sizeof(uint32_t)* list_size(mensaje->listaPosicionCantidad) + sizeof(uint32_t);

	void* stream = malloc(sizeof(uint32_t)+mensaje->sizePokemon+ sizeListaSerializada);
	uint32_t offset = 0;
	memcpy(stream+offset,&(mensaje->sizePokemon),sizeof(uint32_t));
	offset+= sizeof(uint32_t);
	memcpy(stream+offset,mensaje->pokemon,mensaje->sizePokemon);
	offset+= mensaje->sizePokemon;
//	memcpy(stream+offset,&(mensaje->cantidad),sizeof(uint32_t));
//	offset+=sizeof(uint32_t);
	void* listaSerializada= serializarListaPosicionCantidad(mensaje->listaPosicionCantidad);
	//void* arraySerializado=serializarArrayPosiciones(mensaje->arrayPosiciones,mensaje->cantidad);
	memcpy(stream+offset,listaSerializada,sizeListaSerializada );
	return stream;
}

void destruirLocalized(mensajeLocalized* msg){
	list_destroy_and_destroy_elements(msg->listaPosicionCantidad, free);
	free(msg->pokemon);
	free(msg);
}
//
//void* serializarPosicion(posicion* pos){
//	void* stream=malloc(2*sizeof(uint32_t));
//	uint32_t offset=0;
//	memcpy(stream+offset,&(pos->x),sizeof(uint32_t));
//	offset+=sizeof(uint32_t);
//	memcpy(stream+offset,&(pos->y),sizeof(uint32_t));
//	offset+=sizeof(uint32_t);
//	return stream;
//}

//void* serializarArrayPosiciones(posicion* pos, uint32_t cantidad){
//	uint32_t offset=0;
//	void* stream=malloc(cantidad*2*sizeof(uint32_t));
//	uint32_t i=0;
//
//	for(i=0;i<cantidad;i++){
//		posicion* posActual=malloc(sizeof(posicion));
//		*posActual=*(pos + i);
//		void* posicionSerializada=serializarPosicion(posActual);
//		memcpy(stream+offset,posicionSerializada,2*sizeof(uint32_t));
//		offset+=2*sizeof(uint32_t);
//		free(posActual);
//	}
//	return stream;
//}

void* serializarListaPosicionCantidad(t_list* lista){
	uint32_t offset=0;
	void* stream=malloc(sizeof (uint32_t) + 3*sizeof(uint32_t)*list_size(lista));

	uint32_t sizeLista=list_size(lista);

	memcpy(stream+offset,&sizeLista,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	uint32_t i=0;
	for(i=0;i<list_size(lista);i++){
			posicionCantidad* posCantActual=(posicionCantidad*)list_get(lista,i);
			void* posicionCantidadSerializada=serializarPosicionCantidad(posCantActual);
			memcpy(stream+offset,posicionCantidadSerializada,3*sizeof(uint32_t));
			offset+=3*sizeof(uint32_t);
		}
	return stream;
}

void* serializarPosicionCantidad(posicionCantidad* posCant){
	void* stream=malloc(3*sizeof(uint32_t));
		uint32_t offset=0;
		memcpy(stream+offset,&((posCant->posicion).x),sizeof(uint32_t));
		offset+=sizeof(uint32_t);
		memcpy(stream+offset,&((posCant->posicion).y),sizeof(uint32_t));
		offset+=sizeof(uint32_t);
		memcpy(stream+offset,&(posCant->cantidad),sizeof(uint32_t));
		offset+=sizeof(uint32_t);
		return stream;
}


//mensajeLocalized* deserializarLocalized(void* stream){
//	uint32_t sizePokemon,cantidad;
//	uint32_t offset=0;
//	memcpy(&sizePokemon, stream+offset,sizeof(uint32_t));
//	offset+=sizeof(uint32_t);
//
//	char* pokemon=malloc(sizePokemon+1);
//	memcpy(pokemon, stream+offset,sizePokemon);
//	offset+=(sizePokemon);
//	*(pokemon + sizePokemon)='\0';
//
//	memcpy(&cantidad,stream+offset,sizeof(uint32_t));
//	offset+=sizeof(uint32_t);
//	mensajeLocalized* msg=malloc(sizeof(mensajeLocalized));
//	msg->sizePokemon=sizePokemon;
//	msg->pokemon=pokemon;
//	msg->cantidad=cantidad;
//	msg->arrayPosiciones=deserializarArrayPosiciones(stream+offset,cantidad);
//	return msg;
//
//}

mensajeLocalized* deserializarLocalized(void* stream){
	uint32_t sizePokemon;
	uint32_t offset=0;
	memcpy(&sizePokemon, stream+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	char* pokemon=malloc(sizePokemon+1);
	memcpy(pokemon, stream+offset,sizePokemon);
	offset+=(sizePokemon);
	*(pokemon + sizePokemon)='\0';

//	memcpy(&cantidad,stream+offset,sizeof(uint32_t));
//	offset+=sizeof(uint32_t);

	mensajeLocalized* msg=malloc(sizeof(mensajeLocalized));
	msg->sizePokemon=sizePokemon;
	msg->pokemon=pokemon;
	//msg->cantidad=cantidad;
	msg->listaPosicionCantidad=deserializarListaPosicionCantidad(stream+offset);
	return msg;

}

//posicion* deserializarArrayPosiciones(void* stream,uint32_t cantidad){
//	uint32_t offsetStream=0;
//	uint32_t i=0;
//	posicion* pos=malloc(cantidad*sizeof(posicion));
//	for(i=0;i<cantidad;i++){
//			posicion* posActual=malloc(sizeof(posicion));
//			posActual=deserializarPosicion(stream+offsetStream);
//			offsetStream+=2*sizeof(uint32_t);
//			memcpy(pos+i,posActual,sizeof(posicion));
//			free(posActual);
//
//		}
//	return pos;
//}

t_list* deserializarListaPosicionCantidad(void* stream){
	uint32_t offsetStream=0;
	uint32_t i=0;
	t_list* lista=list_create();
	uint32_t cantidad;
	memcpy(&cantidad, stream, sizeof(uint32_t));
	offsetStream+=sizeof(uint32_t);


	for(i=0;i<cantidad;i++){
			posicionCantidad* posActual=deserializarPosicionCantidad(stream+offsetStream);
			list_add(lista, (void*) posActual);
			offsetStream+=3*sizeof(uint32_t);

		}
	return lista;
}
//posicion* deserializarPosicion(void* stream){
//	posicion* pos=malloc(sizeof(posicion));
//	uint offset=0;
//	memcpy(&(pos->x),stream+offset,sizeof(uint32_t));
//	offset+=sizeof(uint32_t);
//	memcpy(&(pos->y),stream+offset,sizeof(uint32_t));
//	return pos;
//}

posicionCantidad* deserializarPosicionCantidad(void* stream){
	posicionCantidad* pos=malloc(sizeof(posicionCantidad));
	uint offset=0;
	memcpy(&((pos->posicion).x),stream+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&((pos->posicion).y),stream+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&(pos->cantidad),stream+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	return pos;
}
