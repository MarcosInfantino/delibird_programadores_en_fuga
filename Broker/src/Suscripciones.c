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

bool validarPertenencia(colaMensajes * cola, uint32_t socket) {
	uint32_t i;
	void* socketLista;
	for (i = 0; i < sizeListaMutex(cola->suscriptores); i++) {
		socketLista = getListaMutex(cola->suscriptores, i);
		if (*((uint32_t*) socketLista) == socket) {
			//printf("Me aprobo validar pertenencia\n");
			return true;
		}
	}
	//printf("Me rechazo validar pertenencia\n");
	return false;

}

bool validarParaSuscripcion(colaMensajes * cola, paquete paq, uint32_t socket, uint32_t identificadorCola){
	return (validarSuscripcionSegunModulo(paq.modulo, identificadorCola)
			&& !validarPertenencia(cola, socket));
}

void suscribirACola(uint32_t* socket, colaMensajes * cola){
	addListaMutex(cola->suscriptores, (void*) socket);
	log_info(brokerLogger2,"Realizo suscripción a una cola. Socket suscripto: %i" ,*socket);
}

void suscribirSegunCola(paquete paq, uint32_t* socket) {
	switch (deserializarSuscripcion(paq.stream)->cola) {
		case APPEARED_POKEMON:
			suscribir(&appearedPokemon, paq, socket,APPEARED_POKEMON);
			break;
		case NEW_POKEMON:
			suscribir(&newPokemon, paq, socket,NEW_POKEMON);
			break;
		case CAUGHT_POKEMON:
			suscribir(&caughtPokemon, paq, socket, CAUGHT_POKEMON);
			break;
		case CATCH_POKEMON:
			suscribir(&catchPokemon, paq, socket, CATCH_POKEMON);
			break;
		case GET_POKEMON:
			suscribir(&getPokemon, paq, socket, GET_POKEMON);
			break;
		case LOCALIZED_POKEMON:
			suscribir(&localizedPokemon, paq, socket, LOCALIZED_POKEMON);
	}
	//enviar todos los mensajes que hubiesen en la cola antes de suscribirse
	//cuando se envien mensajes que no sean suscripción asignarles un numero para posibles respuestas en otra cola
}

void suscribir(colaMensajes * cola, paquete paq, uint32_t* socket,uint32_t identificadorCola) {
	if (validarParaSuscripcion(cola, paq, *socket,identificadorCola)) { //si se puede suscribir y aun no esta en la cola
		suscribirACola(socket, cola);
		responderMensaje(*socket, CORRECTO);

		//enviarMensajesPreviosEnMemoria(socket, identificadorCola);
		//char * frase = armarStringSuscripLog(paq.modulo, paq.tipoMensaje);
		//log_info(loggerBroker, frase);
	} else {
		responderMensaje(*socket, INCORRECTO);
		free(socket);
		//printf("suscripcion incorrecta\n");
	}
}

void suscribirPorTiempo(void* estructura){

	//suscribir(colaMensajes cola, paquete paq, uint32_t socket);

	suscripcionTiempo* structPorTiempo = (suscripcionTiempo*) estructura;
	suscribir(obtenerCola(structPorTiempo->cola), structPorTiempo->paq, structPorTiempo->socket, structPorTiempo->cola);

	sleep(structPorTiempo->tiempo);

	desuscribir(*(structPorTiempo->socket), structPorTiempo->cola);

}

void desuscribir(uint32_t socket, uint32_t cola ){
	uint32_t i;

	void* socketLista;
	colaMensajes* punteroACola = obtenerCola(cola);

	for(i = 0; i < sizeListaMutex(punteroACola->suscriptores); i++){
		socketLista = (uint32_t*)getListaMutex(punteroACola->suscriptores, i);
		if (*((uint32_t*) socketLista) == socket) {
			removeListaMutex(punteroACola->suscriptores, i);
			free(socketLista);
		}
	}
	uint32_t respuestaDesuscripcion=SUSCRIPCION_FINALIZADA;
	paquete* paqueteDesuscripcion=llenarPaquete(GAMEBOY,SUSCRIPCION_FINALIZADA,4,(void*)(&respuestaDesuscripcion));
	void* paqueteSerializado=serializarPaquete(paqueteDesuscripcion);

	send(socket,paqueteSerializado,sizePaquete(paqueteDesuscripcion),0);
	destruirPaquete(paqueteDesuscripcion);
	free(paqueteSerializado);

}
