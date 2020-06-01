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
#include "log.h"
#include "memoria.h"


int main(void) {


	char* pathConfig = "broker.config";
	t_config* config = config_create(pathConfig);

	puertoBroker     = config_get_int_value(config, "PUERTO_BROKER​");
	ipBroker         = config_get_string_value(config, "IP_BROKER");
	tamMemoria       = config_get_int_value(config, "TAMANO_MEMORIA");
	particionMimina  = config_get_int_value(config, "TAMANO_MINIMO_PARTICION");

	loggerBroker = iniciar_logger();
	definirAlgoritmoMemoria(config);
	definirAlgoritmoParticionLibre(config);
	definirAlgoritmoReemplazo(config);

	iniciarHilos();
	inicializarContador();
	iniciarServidor();

	return EXIT_SUCCESS;
}

void* iniciarServidor(){
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
	}

	log_info(loggerBroker, "Estoy escuchando!");
	printf("Estoy escuchando\n");

	while (1)  								//para recibir n conexiones
		esperar_cliente(servidor);

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

void* iniciarCola(void* c) {

	colaMensajes* cc = (colaMensajes*) c;

	cc->suscriptores = inicializarListaMutex();
	cc->cola = inicializarColaMutex();

	cc->mensajesEnCola = malloc(sizeof(sem_t));
	sem_init(cc->mensajesEnCola,0,0);
	return NULL;

}

void esperar_cliente(uint32_t servidor) {

	listen(servidor, SOMAXCONN);
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion = sizeof(struct sockaddr_in);
	printf("Espero un nuevo cliente\n");
	uint32_t* socketCliente=malloc(sizeof(uint32_t));

	*socketCliente = accept(servidor, (void*) &dir_cliente, &tam_direccion);

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


	log_info(loggerBroker,armarConexionNuevoProcesoLog(paquete->modulo));

	if( paquete == NULL){
		printf("RESPONDO MENSAJE ERRONEO\n");
		responderMensaje(*socket, INCORRECTO);
		}else{
			manejarTipoDeMensaje(*paquete, *socket);
		}
	return NULL;
}

void manejarTipoDeMensaje(paquete paq, uint32_t socket) {

	//log_info(loggerBroker, armarStringMsgNuevoLog(paq.tipoMensaje));

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
			 meterEnCola( &localizedPokemon, &paq, socket);
			 break;
		 case SUSCRIPCION:
			 suscribirSegunCola(paq, socket);
			 break;
		 case SUSCRIPCION_TIEMPO:
			 datosSuscribir = deserializarSuscripcionTiempo(paq.stream);
			 structTiempo.cola   = datosSuscribir->cola;
			 structTiempo.tiempo = datosSuscribir->tiempo;
			 structTiempo.paq    = paq;
			 structTiempo.socket = socket;
			 suscribirPorTiempo((void*) &structTiempo);
			 break;
		/* case ACK:
			 guardarSubEnMemoria(paq->idCorrelativo, socket, CONFIRMADO);
			 break;*/
		 default:
			 pthread_exit(NULL);
	 }
}

void meterEnCola( colaMensajes* structCola, paquete * paq, uint32_t  socket){

	pthread_mutex_lock(contador.mutexContador);
	asignarID(paq);

	send(socket,(void*)(&contador.contador),sizeof(uint32_t),0);
	printf("Lo mete en la cola");

	//registrarMensajeEnMemoria(contador.contador, &paq, );

	contador.contador++;
	pthread_mutex_unlock(contador.mutexContador);

	pushColaMutex(structCola->cola, (void *) paq);
	sem_post(structCola->mensajesEnCola);

}

void abrirHiloParaEnviarMensajes(){
	manejoMensajesAppearedPokemon  = pthread_create(&devolverMensajeAppeared, NULL, chequearMensajesEnCola, (void*) &appearedPokemon);
	manejoMensajesNewPokemon       = pthread_create(&devolverMensajeNew, NULL, chequearMensajesEnCola, (void*) &newPokemon);
	manejoMensajesCaughtPokemon    = pthread_create(&devolverMensajeCaught, NULL, chequearMensajesEnCola, (void*) &caughtPokemon);
	manejoMensajesCatchPokemon     = pthread_create(&devolverMensajeCatch, NULL, chequearMensajesEnCola, (void*) &catchPokemon);
	manejoMensajesGetPokemon       = pthread_create(&devolverMensajeGet, NULL, chequearMensajesEnCola, (void*) &getPokemon);
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
		sem_wait(cola->mensajesEnCola); //hasta q no aparezca 1 mensaje no sigue
		while(sizeColaMutex(cola->cola) == 0);
		paquete* paq = (paquete*) popColaMutex(cola->cola);
		void * paqSerializado = serializarPaquete(paq);

		for(i = 0; i < sizeListaMutex(cola->suscriptores) ;i ++){
			uint32_t * socketActual = (uint32_t *) getListaMutex(cola->suscriptores, i);
			send(*socketActual, paqSerializado , sizePaquete(paq), 0);
			//guardarSubEnMemoria(paq->id, socketActual, SUBSYAENVIADOS);
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

void asignarID(paquete * paq){
	paq->id = contador.contador;
}

void responderMensaje(uint32_t socketCliente, uint32_t respuesta) {
	send(socketCliente, (void*) (&respuesta), sizeof(uint32_t), 0);
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

uint32_t obtenerIDultimoMensaje(){

	pthread_mutex_lock(contador.mutexContador);
	return contador.contador;
	pthread_mutex_unlock(contador.mutexContador);

}

void definirAlgoritmoMemoria(t_config* config){

	//definirAlgoritmo(config, "ALGORITMO_MEMORIA", "PARTICIONES", "BS", PARTICIONES_DINAMICAS, BUDDY_SYSTEM, "Hubo un error al definir el algoritmo de memoria");


	char* algoritmo = config_get_string_value(config, "ALGORITMO_MEMORIA");

	if( strcmp(algoritmo,"PARTICIONES") == 0){
			algoritmoMemoria = PARTICIONES_DINAMICAS;
		}else if(strcmp(algoritmo,"BS") == 0){
			algoritmoMemoria = BUDDY_SYSTEM;
			nodoRaizMemoria = crearRaizArbol(); //partición libre inicial
		}else{
			printf("Hubo un error al definir el algoritmo de memoria");
		}

	/*switch(config_get_string_value(config, "ALGORITMO_MEMORIA")){ LA CHOTA DE C NO ME DEJÓ IMPLEMENTAR ESTO
	case "PARTICIONES":
		algoritmoMemoria = PARTICIONES_DINAMICAS;
		break;
	case "BS":
		algoritmoMemoria = BUDDY_SYSTEM;
		break;
	default:
		printf("Hubo un error al definir el algoritmo de memoria");
		return;
	}*/
}

void definirAlgoritmoParticionLibre(t_config* config){
	//definirAlgoritmo(config, "ALGORITMO_PARTICION_LIBRE", "FF", "BF", FIRST_FIT, BEST_FIT, "Hubo un error al definir el algoritmo de particiones de memoria libres");


	char* algoritmo = config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE");

	if( strcmp(algoritmo,"FF") == 0){
		algoritmoParticionLibre = FIRST_FIT;
	}else if(strcmp(algoritmo,"BF") == 0){
		algoritmoParticionLibre = BEST_FIT;
	}else{
		printf("Hubo un error al definir el algoritmo de particiones de memoria libres");
	}
}

void definirAlgoritmoReemplazo(t_config* config){

	//definirAlgoritmo(config, "ALGORITMO_REEMPLAZO", "FIFO", "LRU", FIFO, LRU, "Hubo un error al definir el algoritmo de particiones de memoria libres");


	char* algoritmo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");

	if( strcmp(algoritmo,"FIFO") == 0){
		algoritmoReemplazo = FIFO;
	}else if(strcmp(algoritmo,"LRU") == 0){
		algoritmoReemplazo = LRU;
	}else{
		printf("Hubo un error al definir el algoritmo de particiones de memoria libres");
	}
}

/*void definirAlgoritmo(t_config* config, char* configAtributo, char* OPCION1, char* OPCION2, uint32_t OP1, uint32_t OP2, char* error){
	char* algoritmo = config_get_string_value(config, configAtributo);

		if( strcmp(algoritmo, OPCION1) == 0){
			VARIABLE = OP1;
		}else if(strcmp(algoritmo, OPCION2) == 0){
			VARIJABLE = OP1;
		}else{
			printf("%s", error);
		}

}*/


