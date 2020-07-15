/*
 * Suscripciones.c
 *
 *  Created on: 29 may. 2020
 *      Author: utnso
 */

#include "broker.h"

bool validarSuscripcionSegunModulo(uint32_t modulo, uint32_t cola) {

	switch (modulo) {
	case TEAM:
		if (cola == APPEARED_POKEMON || cola == CAUGHT_POKEMON || cola == LOCALIZED_POKEMON) {
			return true;
			printf("me acepto validarSuscripcionSegunModulo\n");
		}
		break;
	case GAMECARD:
		if (cola == NEW_POKEMON || cola == GET_POKEMON || cola == CATCH_POKEMON)
			return true;
		break;
	case GAMEBOY: //acepta cualquier cola
		return true;
	default:
		return false;
	}

	return false;
}

bool validarParaSuscripcion(colaMensajes * cola, paquete paq, uint32_t socket, uint32_t identificadorCola){
	return validarSuscripcionSegunModulo(paq.modulo, identificadorCola);
}

int suscribirACola(uint32_t* socket, uint32_t idProceso, colaMensajes * cola){


	for(uint32_t i=0; i<sizeListaMutex(cola->suscriptores);i++){
		socketIdProceso* actual = getListaMutex(cola->suscriptores, i);
		if(actual->idProceso == idProceso){
			actual->socket=*socket;
			return 0;
		}
	}

	socketIdProceso* socket_id=malloc(sizeof(socketIdProceso));
	socket_id->socket = *socket;
	socket_id->idProceso = idProceso;
	addListaMutex(cola->suscriptores, (void*) socket_id);
	return 0;

	log_info(brokerLogger2,"Realizo suscripción a una cola. Socket suscripto: %i" ,*socket);
}

void suscribirSegunCola(paquete paq, uint32_t* socket) {
		mensajeSuscripcion* msgSuscripcion=deserializarSuscripcion(paq.stream);
	switch (msgSuscripcion->cola) {
		case APPEARED_POKEMON:
			suscribir(&appearedPokemon, paq, socket,APPEARED_POKEMON, msgSuscripcion->idProceso);
			break;
		case NEW_POKEMON:
			suscribir(&newPokemon, paq, socket,NEW_POKEMON, msgSuscripcion->idProceso);
			break;
		case CAUGHT_POKEMON:
			suscribir(&caughtPokemon, paq, socket, CAUGHT_POKEMON, msgSuscripcion->idProceso);
			break;
		case CATCH_POKEMON:
			suscribir(&catchPokemon, paq, socket, CATCH_POKEMON, msgSuscripcion->idProceso);
			break;
		case GET_POKEMON:
			suscribir(&getPokemon, paq, socket, GET_POKEMON, msgSuscripcion->idProceso);
			break;
		case LOCALIZED_POKEMON:
			suscribir(&localizedPokemon, paq, socket, LOCALIZED_POKEMON, msgSuscripcion->idProceso);
			break;
	}
}

void suscribir(colaMensajes * cola, paquete paq, uint32_t* socket,uint32_t identificadorCola, uint32_t idProceso) {
	log_info(loggerBroker,"Un proceso %s se sucribió a la cola %s.", intToModulo(paq.modulo), colaToString(cola));

	log_info(brokerLogger2, "---------------------------- Suscribo a %i", idProceso);

	if (validarParaSuscripcion(cola, paq, *socket,identificadorCola)) {
		suscribirACola(socket, idProceso,cola);
		responderMensaje(*socket, CORRECTO);

		enviarMensajesPreviosEnMemoria(*socket, idProceso, identificadorCola);

	} else {
		responderMensaje(*socket, INCORRECTO);
		free(socket);
	}

}

char* colaToString(colaMensajes* cola){
	if(cola==&appearedPokemon){
		return "APPEARED_POKEMON";
	}else if(cola==&newPokemon){
		return "NEW_POKEMON";
	}else if(cola==&caughtPokemon){
		return "CAUGHT_POKEMON";
	}else if(cola==&catchPokemon){
		return "CATCH_POKEMON";
	}else if(cola==&getPokemon){
		return "GET_POKEMON";
	}else if(cola==&localizedPokemon){
		return "LOCALIZED_POKEMON";
	}else{
		return "";
	}
}

void suscribirPorTiempo(void* estructura){

	suscripcionTiempo* structPorTiempo = (suscripcionTiempo*) estructura;
	mensajeSuscripcionTiempo* msgSuscripcion= deserializarSuscripcionTiempo((structPorTiempo->paq).stream);

	log_info(brokerLogger2, "---------------------Suscribo por tiempo al proceso %i", msgSuscripcion->idProceso);

	suscribir(obtenerCola(structPorTiempo->cola), structPorTiempo->paq, structPorTiempo->socket, structPorTiempo->cola, msgSuscripcion->idProceso);
	sleep(structPorTiempo->tiempo);

	desuscribir(msgSuscripcion->idProceso, structPorTiempo->cola);

}

int desuscribir(uint32_t idProceso, uint32_t cola ){
	uint32_t i;
	colaMensajes* punteroACola = obtenerCola(cola);

	for(i = 0; i < sizeListaMutex(punteroACola->suscriptores); i++){
		socketIdProceso* actual = (socketIdProceso*)getListaMutex(punteroACola->suscriptores, i);

		if (idProceso==actual->idProceso) {

			uint32_t respuestaDesuscripcion = SUSCRIPCION_FINALIZADA;
			paquete* paqueteDesuscripcion = llenarPaquete(GAMEBOY,SUSCRIPCION_FINALIZADA,4,(void*)(&respuestaDesuscripcion));
			void* paqueteSerializado = serializarPaquete(paqueteDesuscripcion);

			send(actual->socket,paqueteSerializado,sizePaquete(paqueteDesuscripcion),0);
			destruirPaquete(paqueteDesuscripcion);
			free(paqueteSerializado);

			removeAndDestroyElementListaMutex(punteroACola->suscriptores, i, free);

			return 0;
		}
	}
	return 0;
}
