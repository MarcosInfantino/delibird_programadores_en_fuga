/*
 ============================================================================
 Name        : brokerSO.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "broker.h"
#include <messages_lib/messages_lib.h>

pthread_mutex_t mutex;

colaMensajes appearedPokemon, newPokemon, caughtPokemon, catchPokemon,
		getPokemon, localizedPokemon;

uint32_t idHiloAppearedPokemon, idHiloNewPokemon, idHiloCaughtPokemon,
		idHiloCatchPokemon, idHiloGetPokemon, idHiloLocalizedPokemon;

pthread_t hiloAppearedPokemon, hiloNewPokemon, hiloCaughtPokemon,
		hiloCatchPokemon, hiloGetPokemon, hiloLocalizedPokemon;

int main(void) {
//	printf("hola");
//	char* pathConfig="/home/utnso/Escritorio/repoSO/tp-2020-1c-Programadores-en-Fuga/Broker/src/broker.config";
//	t_config* config=config_create(pathConfig);
//	printf("hola");
//	puertoBroker=config_get_int_value(config, "PUERTO_BROKER​");
//	printf("hola");
//	ipBroker=config_get_string_value(config, "IP_BROKER");
//	printf("hola");
	iniciarHilos();

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(5002);

	int servidor = socket(AF_INET, SOCK_STREAM, 0);

	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if (bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor))
			!= 0) {
		perror("Falló el bind");
		return 1;
	}

	printf("Estoy escuchando\n");

	printf("Lib\n");
	t_paquete* prueba = armarPaquete("hola");
	printf("Lib despues\n");

	//listen(servidor, 100);

//		struct sockaddr_in direccionCliente;
//		unsigned int tamanioDireccion=sizeof(direccionCliente);
//		int cliente = accept(servidor, (void*) &direccionCliente, &tamanioDireccion);


	while (1)  								//para recibir n cantidad de conexiones
		esperar_cliente(servidor);

	return EXIT_SUCCESS;
}

void* iniciarCola(void* c) {
	colaMensajes* cc = (colaMensajes*) c;
	cc->cola 		 = malloc(sizeof(t_queue));
	cc->suscriptores = malloc(sizeof(t_list));
	return NULL;
}

void iniciarHilos() {

	idHiloAppearedPokemon  = pthread_create(&hiloAppearedPokemon, NULL, iniciarCola, (void*) &appearedPokemon);
	idHiloNewPokemon 	   = pthread_create(&hiloNewPokemon, NULL, iniciarCola, (void*) &newPokemon);
	idHiloCaughtPokemon    = pthread_create(&hiloCaughtPokemon, NULL, iniciarCola, (void*) &caughtPokemon);
	idHiloCatchPokemon     = pthread_create(&hiloCatchPokemon, NULL, iniciarCola, (void*) &catchPokemon);
	idHiloGetPokemon       = pthread_create(&hiloGetPokemon, NULL, iniciarCola, (void*) &getPokemon);
	idHiloLocalizedPokemon = pthread_create(&hiloLocalizedPokemon, NULL, iniciarCola, (void*) &localizedPokemon);

	pthread_join(hiloAppearedPokemon, NULL);
	pthread_join(hiloNewPokemon, NULL);
	pthread_join(hiloCaughtPokemon, NULL);
	pthread_join(hiloCatchPokemon, NULL);
	pthread_join(hiloGetPokemon, NULL);
	pthread_join(hiloLocalizedPokemon, NULL);
}

void esperar_cliente(uint32_t servidor) {

	listen(servidor, 100);
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion = sizeof(struct sockaddr_in);
	printf("Espero un nuevo cliente\n");
	uint32_t socket_cliente = accept(servidor, (void*) &dir_cliente, &tam_direccion);
	printf("Gestiono un nuevo cliente\n");

	pthread_create(&thread, NULL, atenderCliente, (void*) (&socket_cliente));
	pthread_detach(thread);
}

void* atenderCliente(void* sock) {
	uint32_t* socket = (uint32_t*) sock;
	uint32_t modulo;
	uint32_t tipoDeMensaje;

	if (recv(*socket, &modulo, sizeof(int), MSG_WAITALL) == -1) { //si hubo un error al recibir
		modulo = -1;
		printf("error recv\n");
	}
	printf("recibi un mensaje\n");
	recv(*socket, &tipoDeMensaje, sizeof(int), MSG_WAITALL);


	char * conexionDeProceso = "Se conectó un proceso ";
	strcat(conexionDeProceso, nombreDeProceso(modulo));
	log_info(loggerBroker,conexionDeProceso);


	if (tipoDeMensaje == SUSCRIPCION) { //se quiere suscribir a alguna cola
		suscribirSegunCola(modulo, *socket);
	} else {
		manejarTipoDeMensaje(modulo, tipoDeMensaje, *socket);
	}
	return NULL;
}


void manejarTipoDeMensaje(uint32_t modulo, uint32_t  cola, uint32_t cliente_fd) {

	char * mensajeNuevoDeProceso = "Llegó un nuevo mensaje a la cola ";
	strcat(mensajeNuevoDeProceso, nombreDeCola(cola));
	log_info(loggerBroker, mensajeNuevoDeProceso);

	/*switch(cola){
	 case APPEARED_POKEMON:
	 meterEnCola( appearedPokemon, deserializarAppeared(cliente_fd););
	 break;
	 case NEW_POKEMON:
	 meterEnCola( newPokemon, cliente_fd);
	 break;
	 case CAUGHT_POKEMON:
	 meterEnCola( caughtPokemon, cliente_fd);
	 break;
	 case CATCH_POKEMON:
	 meterEnCola( catchPokemon, cliente_fd);
	 break;
	 case GET_POKEMON:
	 meterEnCola( getPokemon, cliente_fd);
	 break;
	 case LOCALIZED_POKEMON:
	 meterEnCola( localizedPokemon, cliente_fd);
	 break;
	 case -1:
	 pthread_exit(NULL);
	 }*/
}

void meterEnCola( colaMensajes structCola, uint32_t  socket){

	/*
	 uint32_t id;
	 uint32_t idCorrelativo;
	 uint32_t sizeStream;
	 void* stream;

	recv(*socket, &id, sizeof(uint32_t), MSG_WAITALL);
	recv(*socket, &idCorrelativo, sizeof(uint32_t), MSG_WAITALL);
	recv(*socket, &sizeStream, sizeof(uint32_t), MSG_WAITALL);
	recv(*socket, &stream, sizeof(sizeStream), MSG_WAITALL);

	//tipoDEMensaje mensaje = acá hay que deserializar segun tipo de mensaje creo que haría un switch
	 *
	 *
	 * esto va fuera del switch
	list_add(structCola->cola, mensaje);*/
	//el broker tiene que contestar mandando id de mensaje


}

void suscribirSegunCola(uint32_t modulo, uint32_t socket) {
	//uint32_t  id_proceso;
	uint32_t cola;

	//recv(socket, &id_proceso, sizeof(int), MSG_WAITALL);
	recv(socket, &cola, sizeof(int), MSG_WAITALL);
	printf("Hice el recv de la cola\n");

	//Relleno parametros
	parametroValidacion parameter;
	parameter.modulo = modulo;
	parameter.socketCliente = socket;
	parameter.colaEnum = cola;

	switch (cola) {
		case APPEARED_POKEMON:
			parameter.structCola = appearedPokemon;
			suscribir(&parameter);
			break;
		case NEW_POKEMON:
			parameter.structCola = newPokemon;
			suscribir(&parameter);
			break;
		case CAUGHT_POKEMON:
			parameter.structCola = caughtPokemon;
			suscribir(&parameter);
			break;
		case CATCH_POKEMON:
			parameter.structCola = catchPokemon;
			suscribir(&parameter);
			break;
		case GET_POKEMON:
			parameter.structCola = getPokemon;
			suscribir(&parameter);
			break;
		case LOCALIZED_POKEMON:
			parameter.structCola = localizedPokemon;
			suscribir(&parameter);
	}
	//enviar todos los mensajes que hubiesen en la cola antes de suscribirse
	//cuando se envien mensajes que no sean suscripción asignarles un numero para posibbles respuestas en otra cola

}

//TODO repensar como sabe el otro modulo el largo, habria que armar header
void responderMensaje(uint32_t socketCliente, uint32_t respuesta) {
	send(socketCliente, (void*) (&respuesta), sizeof(uint32_t), 0);
}

//TODO chequear que esten bien tema punteros con parameter (Creo que si)
void suscribir(parametroValidacion* parameter) {
	if (validarParaSuscripcion(parameter)) { //si se puede suscribir y aun no esta en la cola
		list_add((parameter->structCola).suscriptores, (void*) &(parameter->socketCliente));
		responderMensaje(parameter->socketCliente, CORRECTO);
		printf("respondi mensaje correcto\n");

		char * frase = armarStringSuscripLog(parameter->modulo,parameter->colaEnum);
		log_info(loggerBroker, frase);

	} else {
		responderMensaje(parameter->socketCliente, INCORRECTO);
		printf("respondi mensaje incorrecto\n");
	}

}

bool validarParaSuscripcion(parametroValidacion* parameter){
	return (validarSuscripcionSegunModulo(parameter->modulo, parameter->colaEnum) &&
			!validarPertenencia(parameter->structCola, parameter->socketCliente));
}

bool validarSuscripcionSegunModulo(uint32_t modulo, uint32_t cola) {

	switch (modulo) {
	case TEAM:
		if (cola == APPEARED_POKEMON || cola == CAUGHT_POKEMON
				|| cola == LOCALIZED_POKEMON) {
			printf("Soy team\n");
			return true;
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

	return false; //tiraba error de sin retorno, no sé si está bien
}

bool validarPertenencia(colaMensajes cola, uint32_t socket) {
	uint32_t i;
	void* socketLista;
	for (i = 0; i < list_size(cola.suscriptores); i++) {
		socketLista = list_get(cola.suscriptores, i);
		if (*((uint32_t*) socketLista) == socket) {
			printf("Me aprobo validar pertenencia\n");
			return true;
		}
	}
	printf("Me rechazo validar pertenencia\n");
	return false;

}



char* nombreDeProceso(uint32_t modulo){

	switch(modulo){
	case BROKER:
		return "Broker";
		break;
	case TEAM:
		return "Team";
		break;
	case GAMECARD:
		return "GameCard";
		break;
	case GAMEBOY:
		return "GameBoy";
		break;
	case -1:
		return "ERROR";
		break;
	}
	return "0";

}

char* nombreDeCola(uint32_t cola){

	switch(cola){
	case APPEARED_POKEMON:
		return "APPEARED_POKEMON";
		break;
	case NEW_POKEMON:
		return "NEW_POKEMON";
		break;
	case CAUGHT_POKEMON:
		return "CAUGHT_POKEMON";
		break;
	case CATCH_POKEMON:
		return "CATCH_POKEMON";
		break;
	case GET_POKEMON:
		return "GET_POKEMON";
		break;
	case LOCALIZED_POKEMON:
		return "LOCALIZED_POKEMON";
		break;
	case -1:
		return "ERROR";
		break;
	}

	return "0";
}

char* armarStringSuscripLog(uint32_t modulo, uint32_t cola){
	char* suscripcionDeUnProceso = "Se suscribrió el proceso ";
	char* suscripcionAcola = " a la cola ";
	strcat(suscripcionDeUnProceso, nombreDeProceso(modulo));
	strcat(suscripcionDeUnProceso,suscripcionAcola );
	strcat(suscripcionDeUnProceso ,nombreDeCola(cola));

	return suscripcionDeUnProceso;
}
