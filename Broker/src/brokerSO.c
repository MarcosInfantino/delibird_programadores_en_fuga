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


pthread_mutex_t mutex;

contadorMensajes contador;

colaMensajes appearedPokemon, newPokemon, caughtPokemon, catchPokemon,
		getPokemon, localizedPokemon;

colaMensajes colaNula;

uint32_t idHiloAppearedPokemon, idHiloNewPokemon, idHiloCaughtPokemon,
		idHiloCatchPokemon, idHiloGetPokemon, idHiloLocalizedPokemon;

uint32_t manejoMensajesAppearedPokemon, manejoMensajesNewPokemon, manejoMensajesCaughtPokemon,
		manejoMensajesCatchPokemon, manejoMensajesGetPokemon, manejoMensajesLocalizedPokemon;

pthread_t hiloAppearedPokemon, hiloNewPokemon, hiloCaughtPokemon,
		hiloCatchPokemon, hiloGetPokemon, hiloLocalizedPokemon;

pthread_t devolverMensajeAppeared, devolverMensajeNew, devolverMensajeCaught,
		devolverMensajeCatch, devolverMensajeGet, devolverMensajeLocalized;

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
	inicializarContador();

	loggerBroker = log_create( "log-broker.log", "Broker", true , 0);


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


//		struct sockaddr_in direccionCliente;
//		unsigned int tamanioDireccion=sizeof(direccionCliente);
//		int cliente = accept(servidor, (void*) &direccionCliente, &tamanioDireccion);

	abrirHiloParaEnviarMensajes();

	while (1)  								//para recibir n cantidad de conexiones
		esperar_cliente(servidor);

	return EXIT_SUCCESS;
}

void* iniciarCola(void* c) {
//	colaMensajes* cc = (colaMensajes*) c;
//	cc->cola 		 = malloc(sizeof(t_queue));
//	cc->suscriptores = malloc(sizeof(t_list));
//	cc->mutexCola=malloc(sizeof(pthread_mutex_t));
//	pthread_mutex_init((cc->mutexCola),NULL);
//	return NULL;
	colaMensajes* cc = (colaMensajes*) c;
	cc->suscriptores = inicializarListaMutex();
	cc->cola = inicializarColaMutex();
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

	listen(servidor, SOMAXCONN);
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion = sizeof(struct sockaddr_in);
	//printf("Espero un nuevo cliente\n");
	uint32_t* socketCliente=malloc(sizeof(uint32_t));

	socketCliente = accept(servidor, (void) &dir_cliente, &tam_direccion);

	//printf("Gestiono un nuevo cliente\n");
	pthread_t thread;
	pthread_create(&thread, NULL, atenderCliente, (void*) (socketCliente));

	pthread_detach(thread);
	//printf("cree el hilo\n");
}

void* atenderCliente(void* sock) {
//	printf("Atiendo cliente\n");
	uint32_t* socket = (uint32_t*) sock;
	paquete* paquete = recibirPaquete(*socket);

	char * conexionDeProceso = "Se conectó un proceso ";
	strcat(conexionDeProceso, nombreDeProceso((*paquete).modulo));
	log_info(loggerBroker,conexionDeProceso);

	if( paquete == NULL){
		printf("RESPONDO MENSAJE ERRONEO\n");
		responderMensaje(*socket, INCORRECTO);
		}else{
			manejarTipoDeMensaje(*paquete, *socket);
		}
	return NULL;
}


void manejarTipoDeMensaje(paquete paq, uint32_t socket) {

	char * mensajeNuevoDeProceso = "Llegó un nuevo mensaje a la cola ";
	strcat(mensajeNuevoDeProceso, nombreDeCola(paq.tipoMensaje));
	log_info(loggerBroker, mensajeNuevoDeProceso);
	suscripcionTiempo structTiempo;
	mensajeSuscripcionTiempo* datosSuscribir;

	switch(paq.tipoMensaje){
		 case APPEARED_POKEMON:
			 meterEnCola(&appearedPokemon, &paq, socket );
			 break;
		 case NEW_POKEMON:
			 meterEnCola( &newPokemon, &paq, socket);
			 break;
		 case CAUGHT_POKEMON:
			 meterEnCola( &caughtPokemon, &paq, socket);
			 break;
		 case CATCH_POKEMON:
			 meterEnCola( &catchPokemon, &paq, socket);
			 break;
		 case GET_POKEMON:
			 meterEnCola( &getPokemon, &paq, socket);
		 break;
		 case LOCALIZED_POKEMON:
			 //meterEnCola( &localizedPokemon, (void*)deserializarLocalized(paq.stream), socket); //aun no está
			 break;
		 case SUSCRIPCION:
			 suscribirSegunCola(paq, socket);
			 break;
		 case SUSCRIPCION_TIEMPO:
			 datosSuscribir = deserializarSuscripcionTiempo(paq.stream);
			 structTiempo.cola = datosSuscribir->cola;
			 structTiempo.tiempo = datosSuscribir->tiempo;
			 structTiempo.paq = paq;
			 structTiempo.socket = socket;
			 suscribirPorTiempo((void*) &structTiempo);
			 break;
		 default:
			 pthread_exit(NULL);
	 }

}

void suscribirPorTiempo(void* estructura){

	suscripcionTiempo* structPorTiempo = (suscripcionTiempo*) estructura;
	suscribir(obtenerCola(structPorTiempo->cola), structPorTiempo->paq, structPorTiempo->socket, structPorTiempo->cola);

	sleep(structPorTiempo->tiempo);

	desuscribir(structPorTiempo->socket, structPorTiempo->cola);

}

void desuscribir(uint32_t socket, uint32_t cola ){
	uint32_t i;

	void* socketLista;
	colaMensajes* punteroACola = obtenerCola(cola);

	for(i = 0; i < sizeListaMutex(punteroACola->suscriptores); i++){
		socketLista = getListaMutex(punteroACola->suscriptores, i);
		if (((uint32_t) socketLista) == socket) {
			removeListaMutex(punteroACola->suscriptores, i);
		}
	}
}

void abrirHiloParaEnviarMensajes(){
	manejoMensajesAppearedPokemon = pthread_create(&devolverMensajeAppeared, NULL, chequearMensajesEnCola, (void*) &appearedPokemon);
	manejoMensajesNewPokemon = pthread_create(&devolverMensajeNew, NULL, chequearMensajesEnCola, (void*) &newPokemon);
	manejoMensajesCaughtPokemon = pthread_create(&devolverMensajeCaught, NULL, chequearMensajesEnCola, (void*) &caughtPokemon);
	manejoMensajesCatchPokemon = pthread_create(&devolverMensajeCatch, NULL, chequearMensajesEnCola, (void*) &catchPokemon);
	manejoMensajesGetPokemon = pthread_create(&devolverMensajeGet, NULL, chequearMensajesEnCola, (void*) &getPokemon);
	manejoMensajesLocalizedPokemon = pthread_create(&devolverMensajeLocalized, NULL, chequearMensajesEnCola, (void*) &localizedPokemon);
	pthread_detach(devolverMensajeAppeared);
	pthread_detach(devolverMensajeNew);
	pthread_detach(devolverMensajeCaught);
	pthread_detach(devolverMensajeCatch);
	pthread_detach(devolverMensajeGet);
	pthread_detach(devolverMensajeLocalized);

}

void * chequearMensajesEnCola(void * colaVoid){
	colaMensajes* cola = (colaMensajes*) colaVoid;
	uint32_t i;
	while (1){
		while(sizeColaMutex(cola->cola) == 0);
		paquete* paq = (paquete*) popColaMutex(cola->cola);
		void * paqSerializado = serializarPaquete(paq);
		for(i = 0; i < sizeListaMutex(cola->suscriptores) ;i ++){
			uint32_t * socketActual = (uint32_t *) getListaMutex(cola->suscriptores, i);
			send(*socketActual, paqSerializado , sizePaquete(paq), 0);
		}
	}
	return NULL;
}

colaMensajes* obtenerCola(uint32_t colaInt){
	switch(colaInt){
		case APPEARED_POKEMON:
			return &appearedPokemon;
		case NEW_POKEMON:
			return &newPokemon;
		case CAUGHT_POKEMON:
			return &caughtPokemon;
		case CATCH_POKEMON:
			return &catchPokemon;
		case GET_POKEMON:
			return &getPokemon;
		case LOCALIZED_POKEMON:
			return &localizedPokemon;
		default :
			return &colaNula;
	}
}

void meterEnCola( colaMensajes* structCola, paquete * paq, uint32_t  socket){
	pthread_mutex_lock(contador.mutexContador);
	asignarID(paq);
	send(socket,(void*)(&contador.contador),sizeof(uint32_t),0);
	printf("Lo mete en la cola");
	contador.contador++;
	pthread_mutex_unlock(contador.mutexContador);
	pushColaMutex(structCola->cola, (void *) paq);
	//registrarMensajeEnMemoria(mensaje)
}

void asignarID(paquete * paq){
	paq->id = contador.contador;
}

void suscribirSegunCola(paquete paq, uint32_t socket) {

//	parametroValidacion parameter;
//	parameter.socketCliente = socket;
//	parameter.paquete       = paq;


	switch (deserializarSuscripcion(paq.stream)->cola) {
		case APPEARED_POKEMON:
			//parameter.structCola = appearedPokemon;
			suscribir(&appearedPokemon, paq, socket,APPEARED_POKEMON);
			break;
		case NEW_POKEMON:
			//parameter.structCola = newPokemon;
			suscribir(&newPokemon, paq, socket,NEW_POKEMON);
			break;
		case CAUGHT_POKEMON:
			//parameter.structCola = caughtPokemon;
			suscribir(&caughtPokemon, paq, socket, CAUGHT_POKEMON);
			break;
		case CATCH_POKEMON:
			//parameter.structCola = catchPokemon;
			suscribir(&catchPokemon, paq, socket, CATCH_POKEMON);
			break;
		case GET_POKEMON:
			//parameter.structCola = getPokemon;
			suscribir(&getPokemon, paq, socket, GET_POKEMON);
			break;
		case LOCALIZED_POKEMON:
			//parameter.structCola = localizedPokemon;
			suscribir(&localizedPokemon, paq, socket, LOCALIZED_POKEMON);
	}
	//enviar todos los mensajes que hubiesen en la cola antes de suscribirse

}


void responderMensaje(uint32_t socketCliente, uint32_t respuesta) {
	send(socketCliente, (void*) (&respuesta), sizeof(uint32_t), 0);
}


void suscribir(colaMensajes * cola, paquete paq, uint32_t socket,uint32_t identificadorCola) {
	if (validarParaSuscripcion(cola, paq, socket,identificadorCola)) { //si se puede suscribir y aun no esta en la cola
		suscribirACola(&socket, cola);
		responderMensaje(socket, CORRECTO);
		//printf("suscripcion correcta\n");

		char * frase = armarStringSuscripLog(paq.modulo, paq.tipoMensaje);
		log_info(loggerBroker, frase);

	} else {
		responderMensaje(socket, INCORRECTO);
		//printf("suscripcion incorrecta\n");
	}

}

void suscribirACola(uint32_t* socket, colaMensajes * cola){
	addListaMutex(cola->suscriptores, (void*) socket);
}


bool validarParaSuscripcion(colaMensajes * cola, paquete paq, uint32_t socket, uint32_t identificadorCola){
	return (validarSuscripcionSegunModulo(paq.modulo, identificadorCola)
			&& !validarPertenencia(cola, socket));
}

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
		if (((uint32_t) socketLista) == socket) {
			//printf("Me aprobo validar pertenencia\n");
			return true;
		}
	}
	//printf("Me rechazo validar pertenencia\n");
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
	char* suscripcionAcola       = " a la cola ";
	strcat(suscripcionDeUnProceso, nombreDeProceso(modulo));
	strcat(suscripcionDeUnProceso,suscripcionAcola );
	strcat(suscripcionDeUnProceso ,nombreDeCola(cola));

	return suscripcionDeUnProceso;
}

void inicializarContador(){
	contador.contador=1;
	contador.mutexContador=malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(contador.mutexContador,NULL);
}

void incrementarContador(){
	pthread_mutex_lock(contador.mutexContador);
	contador.contador++;
	pthread_mutex_unlock(contador.mutexContador);
}
