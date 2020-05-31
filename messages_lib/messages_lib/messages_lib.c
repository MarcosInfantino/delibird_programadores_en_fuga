/*
 * messages_lib.c
 *
 *  Created on: 12 abr. 2020
 *      Author: utnso
 */

#include "messages_lib.h"

uint32_t crearSocketCliente (char* ip, uint32_t puerto){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family      = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port        = htons(puerto);

	uint32_t cliente=socket(AF_INET,SOCK_STREAM,0);
	if(connect(cliente,(void*) &direccionServidor,sizeof(direccionServidor)) != 0){

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
	case SUSCRIPCION_TIEMPO:
		size=2*sizeof(uint32_t);  break;
	default:
		printf("Error: el caso ingresado no esta contemplado \n");
		break;
	}
	return size;
}

//int main(){
//	//mensajeLocalized* llenarMensajeLocalized(uint32_t sizePokemon, char* pokemon, uint32_t cantidad, posicion* posiciones)
//
//	char* pokemon="Pikachu";
//	uint32_t cantidad=2;
//	posicion* posiciones=malloc(sizeof(posiciones)*2);
//	posicion pos1={1,0};
//	posicion pos2={0,0};
//	*(posiciones)=pos1;
//	*(posiciones+1)=pos2;
//	mensajeLocalized* msg= llenarMensajeLocalized(pokemon,cantidad,posiciones);
//	void* stream= serializarLocalized(msg);
//	mensajeLocalized* msgResultado=deserializarLocalized(stream);
//	printf("sizePokemon: %i\n", msgResultado->sizePokemon);
//	printf("pokemon: %s\n", msgResultado->pokemon);
//	printf("cantidad: %i\n", msgResultado->cantidad);
//	printf("posx1: %i\n", (msgResultado->arrayPosiciones)->x);
//	printf("posy1: %i\n", (msgResultado->arrayPosiciones)->y);
//	printf("posx2: %i\n", ((msgResultado->arrayPosiciones) +1)->x);
//	printf("posy2: %i\n", ((msgResultado->arrayPosiciones) +1)->y);
//
//	return 0;
//}

