/*
 ============================================================================
 Name        : Team.c
 Author      : Programadores en fuga
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Team.h"
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

//TEAM APPEARED_POKEMON Pikachu 0 0
//BROKER CAUGHT_POKEMON 4 OK

//./gameboy TEAM APPEARED_POKEMON Pikachu 1 1
//./gameboy TEAM APPEARED_POKEMON Squirtle 9 7
//./gameboy TEAM APPEARED_POKEMON Onix 2 2
//./gameboy TEAM APPEARED_POKEMON Squirtle 3 5
//./gameboy TEAM APPEARED_POKEMON Gengar 7 5

int main(int argc , char* argv[]){

	char* pathConfig   = argv[1];
	t_config* config = crearYLeerConfig(pathConfig);
	estimacionInicial=5;
	esperaPedido=0;
	teamLogger = iniciar_logger(logFilePrincipal, "TEAM");

	teamLogger2=log_create("teamLoggerSecundario.log","team", true, LOG_LEVEL_INFO);

	inicializarSemaforos();
	inicializarColasYListas();

	log_info(teamLogger2,"--------------------------------------------------------------------------");
	log_info(teamLogger2,"NUEVA EJECUCION");
	log_info(teamLogger2,"--------------------------------------------------------------------------");

	obtenerAlgoritmoPlanificacion(config);

	team     = inicializarTeam(config);

	entrenadores->lista       = team->entrenadores;


	cantEntrenadores = list_size(team->entrenadores);
	loggearObjetivoDelTeam();
	arrayIdHilosEntrenadores  = malloc(cantEntrenadores*sizeof(pthread_t));
	inicializarEntrenadores(team->entrenadores);

	crearHilos(config);

	sem_wait(semaforoObjetivoCumplido);
	loggearResultado();


	liberarMemoria();
	terminar_programa(teamLogger, config);
	return 0;
}

void inicializarSemaforos(){
	sem_init(&semaforoEjecucionCpu, 0,0);
	sem_init(&intercambioFinalizado, 0,0);

	iniciarResolucionDeadlock=malloc(sizeof(sem_t));
	sem_init((iniciarResolucionDeadlock), 0,0);

	semaforoObjetivoCumplido=malloc(sizeof(sem_t));
	sem_init((semaforoObjetivoCumplido), 0,0);

	finalizacionCicloCpu=malloc(sizeof(sem_t));
	sem_init((finalizacionCicloCpu), 0,0);

	mutexPlanificador = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutexPlanificador, NULL);
}

void crearHilos(t_config* config){
	crearHiloParaEnviarGets(&hiloEnviarGets);
	crearHiloResolucionDeadlock(&resolucionDeadlock);
	crearHiloConexionColasBroker((void*)config,&hiloConexionInicialBroker);
	crearHiloServidorGameboy(&hiloServidorGameboy);
	crearHiloPlanificador(&hiloPlanificador);
}

t_config* crearYLeerConfig(char* pathConfig){
	t_config* config   = config_create(pathConfig);
	estimacionInicial=config_get_int_value(config, "ESTIMACION_INICIAL");
	//log_info(teamLogger2, "ESTIMACION INICIAL= %d", estimacionInicial);
	retardoCicloCpu    = config_get_int_value(config,"RETARDO_CICLO_CPU");
	tiempoReconexion= config_get_int_value(config,"TIEMPO_RECONEXION");
	puertoBroker       = config_get_int_value(config,"PUERTO_BROKER");
	ipBroker           = config_get_string_value(config,"IP_BROKER");
	logFilePrincipal=config_get_string_value(config, "LOG_FILE");
	puertoTeam=config_get_int_value(config,"PUERTO_TEAM");
	idProcesoTeam=config_get_int_value(config, "ID_PROCESO");

	return config;
}

void inicializarColasYListas(){
	entrenadoresLibres=inicializarListaMutex();
	pokemonesPendientes=inicializarColaMutex();
	listaIdsEntrenadorMensaje=inicializarListaMutex();
	entrenadores=inicializarListaMutex();
	especiesLocalizadas=inicializarListaMutex();
	entrenadoresExit=inicializarListaMutex();
	entrenadoresDeadlock=inicializarListaMutex();
	listaIdsRespuestasGet=inicializarListaMutex();
	pokemonesConCatchPendiente=inicializarListaMutex();
	pokemonesPosicionDeReserva=inicializarListaMutex();
}

void loggearObjetivoDelTeam(){
	log_info(teamLogger2, "--------------------------------------------------------------");
	log_info(teamLogger2, "OBJETIVO DEL TEAM");
	for(uint32_t i=0; i<sizeListaMutex(team->objetivoGlobal);i++){
		objetivo* objetivoActual= (objetivo*) (getListaMutex(team->objetivoGlobal,i));
		log_info(teamLogger2, "Objetivo %i. Pokemon: %s. Cantidad: %i.", i, objetivoActual->pokemon, objetivoActual->cantidad);
	}
	log_info(teamLogger2, "--------------------------------------------------");
}
void loggearResultado(){
	log_info(teamLogger,"------------------------------------------------" );
	log_info(teamLogger,"Programa terminado: objetivo global cumplido.");
	log_info(teamLogger,"Cantidad de ciclos de CPU totales: %i.", team->cantidadCiclosCpuTotales );
	log_info(teamLogger,"Cantidad de cambios de contexto realizados: %i.", team->cantidadCambiosContexto );
	log_info(teamLogger,"------------------------------------------------" );
	log_info(teamLogger,"Información de los entrenadores: " );
	log_info(teamLogger,"Deadlocks producidos: %i.", team->cantidadDeadlocksEncontrados);
	log_info(teamLogger,"Deadlocks resueltos: %i.", team->cantidadDeadlocksResueltos);

	for(uint32_t i=0;i<sizeListaMutex(entrenadores);i++){
			dataEntrenador* entrenadorActual=(dataEntrenador*) getListaMutex(entrenadores,i);
			log_info(teamLogger, "Cantidad de ciclos de CPU realizados por el entrenador %i: %i.",entrenadorActual->id,entrenadorActual->cantidadCiclosCpu);

		}

}
bool objetivoCumplido(){
	return sizeListaMutex(entrenadores)==sizeListaMutex(entrenadoresExit);
}


void* atenderAppeared(void* paq){
	paquete* paqueteAppeared=(paquete*) paq;
	mensajeAppeared* msg=deserializarAppeared(paqueteAppeared->stream);
	log_info(teamLogger2, "Atiendo appeared. Pokemon: %s.", msg->pokemon);
	pokemonPosicion* pokePosicion=malloc(sizeof(pokemonPosicion));
	if(!especieFueLocalizada(msg->pokemon)){
		char* poke=malloc(strlen(msg->pokemon)+1);
		strcpy(poke, msg->pokemon);
		addListaMutex(especiesLocalizadas,(void*)poke);
	}

	pokePosicion->pokemon=malloc(strlen(msg->pokemon)+1);
	strcpy(pokePosicion->pokemon,msg->pokemon);
	(pokePosicion->posicion).x=msg->posX;
	(pokePosicion->posicion).y=msg->posY;
	destruirAppeared(msg);
	gestionarBusquedaPokemon(pokePosicion);
//	if(pokemonEsObjetivo(pokePosicion->pokemon)){
//		log_info(teamLogger2, "El pokemon es objetivo");
//		if(sizeListaMutex(entrenadoresLibres)>0){
//			log_info(teamLogger2, "Hay entrenadores disponibles para atrapar a %s.",pokePosicion->pokemon );
//			seleccionarEntrenador(pokePosicion);
//		}else{
//			log_info(teamLogger2, "No hay entrenadores disponibles para atrapar a %s. ",pokePosicion->pokemon);
//			pushColaMutex(pokemonesPendientes,(void*)pokePosicion);
//		}
//	}else
////		if(seEstaGestionandoCatch(pokePosicion->pokemon)){
////
////	}
//	{
//		log_info(teamLogger2, "El pokemon no es objetivo");
//	}
	destruirPaquete(paqueteAppeared);
	return NULL;
}

void gestionarBusquedaPokemon(pokemonPosicion* pokePosicion){
	if(pokemonEsObjetivo(pokePosicion->pokemon)){
			log_info(teamLogger2, "El pokemon es objetivo");
			if(sizeListaMutex(entrenadoresLibres)>0){
				log_info(teamLogger2, "Hay entrenadores disponibles para atrapar a %s.",pokePosicion->pokemon );
				seleccionarEntrenador(pokePosicion);
			}else{
				log_info(teamLogger2, "No hay entrenadores disponibles para atrapar a %s. ",pokePosicion->pokemon);
				pushColaMutex(pokemonesPendientes,(void*)pokePosicion);
				log_info(teamLogger2, "--------------------SIZE COLA MUTEX: %i", sizeColaMutex(pokemonesPendientes));
			}
		}else if(seEstaGestionandoCatch(pokePosicion->pokemon)){
			log_info(teamLogger2, "se esta gestionando el catch.");
				agregarPokemonPosicionAReserva(pokePosicion);

		}else {
			log_info(teamLogger2, "El pokemon no es objetivo");
		}
}

bool seEstaGestionandoCatch(char* pokemon){
	return buscarMismoPokemonListaMutex(pokemonesConCatchPendiente, pokemon)!=-1;
}

void* atenderLocalized(void* paq){
	paquete* paqueteLocalized= (paquete*) paq;
	mensajeLocalized* msg=deserializarLocalized(paqueteLocalized->stream);
	char* pokemonAAgregar=malloc(strlen(msg->pokemon)+1);
	strcpy(pokemonAAgregar,msg->pokemon);
	if(!especieFueLocalizada(msg->pokemon)&& localizedMeInteresa(paqueteLocalized)){
		uint32_t i;
		for(i=0;i<msg->cantidad;i++){
			posicion posActual= *((msg->arrayPosiciones)+i);
			mensajeAppeared* msgAppeared=llenarAppeared(pokemonAAgregar,posActual.x,posActual.y);
			void* streamAppeared=serializarAppeared(msgAppeared);
			paquete* paqueteAppeared=llenarPaquete(TEAM,APPEARED_POKEMON, sizeArgumentos(APPEARED_POKEMON, msgAppeared->pokemon,0), streamAppeared);
			atenderAppeared(paqueteAppeared);
		}
	}

	destruirPaquete(paqueteLocalized);
	return NULL;
}

bool localizedMeInteresa(paquete* paquete){
	uint32_t id=paquete->idCorrelativo;
	uint32_t i;
	for(i=0;i<sizeListaMutex(listaIdsRespuestasGet);i++){
		uint32_t* idActual=(uint32_t*)getListaMutex(listaIdsRespuestasGet,i);
		if(id==(*idActual)){
			return true;
		}
	}
	return false;
}

bool especieFueLocalizada(char* pokemon){
	uint32_t i=0;
	for(i=0;i<sizeListaMutex(especiesLocalizadas);i++){
		char* pokeActual=(char*)(getListaMutex(especiesLocalizadas,i));

		if(strcmp(pokeActual,pokemon)==0){
			return true;
		}
	}
	return false;
}

void* atenderCaught(void* paq){
	paquete* paqueteCaught=(paquete*) paq;
	log_info(teamLogger2,"Atiendo caught.");
	mensajeCaught* msgCaught=deserializarCaught(paqueteCaught->stream);
	uint32_t id=paqueteCaught->idCorrelativo;
	uint32_t idEncontrado=buscarEntrenadorParaMensaje(listaIdsEntrenadorMensaje,id);
	log_info(teamLogger2,"Resultado del caught: %i.",msgCaught->resultadoCaught);
	if(idEncontrado!=-1){
		dataEntrenador* entrenadorEncontrado=(dataEntrenador*)getListaMutex(entrenadores, idEncontrado);

		if(msgCaught->resultadoCaught==CORRECTO ){
//			list_add(entrenadorEncontrado->pokemones,(void*)(entrenadorEncontrado->pokemonAAtrapar->pokemon));
//			registrarPokemonAtrapado(entrenadorEncontrado->pokemonAAtrapar->pokemon);
//			replanificarEntrenador(entrenadorEncontrado);
			log_info(teamLogger2,"El caught me interesa.");
			atraparPokemonYReplanificar (entrenadorEncontrado);
		}else{
//			entrenadorEncontrado->estado=BLOCKED;
//			habilitarHiloEntrenador(idEncontrado);
			agregarObjetivo(entrenadorEncontrado->pokemonAAtrapar->pokemon);//lo vuelve a agregar a los objetivos porque vuelver a ser un objetivo necesario
			removerPokemonConCatchPendiente(entrenadorEncontrado->pokemonAAtrapar->pokemon);
			pokemonPosicion* pokeReserva=obtenerPokemonPosicionEnReserva(entrenadorEncontrado->pokemonAAtrapar->pokemon);

			if(pokeReserva!=NULL){
				gestionarBusquedaPokemon(pokeReserva);
			}

			destruirPokemonPosicion(entrenadorEncontrado->pokemonAAtrapar);
				log_info(teamLogger2,"El caught no fue exitoso.");

//			if(hayAppearedParaEsteCaughtFallido(entrenadorEncontrado->pokemonAAtrapar->pokemon)){
//				pokemonPosicion* poke=malloc(sizeof(pokemonPosicion));
//				poke->pokemon=malloc(strlen(entrenadorEncontrado->pokemonAAtrapar->pokemon)+1);
//				strcpy(poke->pokemon,entrenadorEncontrado->pokemonAAtrapar->pokemon);
//				poke->posicion=entrenadorEncontrado->pokemonAAtrapar->posicion;
//				pushColaMutex(pokemonesPendientes,(void*)pokePosicion);
//			}
			replanificarEntrenador(entrenadorEncontrado);
		}
	}else{
		log_info(teamLogger2,"El caught no me interesa.");
	}

	destruirCaught(msgCaught);
	destruirPaquete(paqueteCaught);
	return NULL;
}

bool mismoPokemonPosicion(pokemonPosicion* poke1, pokemonPosicion* poke2){
	return (strcmp(poke1->pokemon, poke2->pokemon)==0) && (poke1->posicion).x==(poke2->posicion).x && (poke1->posicion).y==(poke2->posicion).x;
}

bool yaEstaEnReserva(pokemonPosicion* poke){
	for(uint32_t i=0; i< sizeListaMutex(pokemonesPosicionDeReserva);i++){
		pokemonPosicion* pokeActual= (pokemonPosicion*) getListaMutex(pokemonesPosicionDeReserva,i);
		if(mismoPokemonPosicion(pokeActual, poke)){
			return true;
		}
	}

	return false;
}

void agregarPokemonPosicionAReserva(pokemonPosicion* poke){
	if(!yaEstaEnReserva(poke)){
		addListaMutex(pokemonesPosicionDeReserva, (void*) poke);
	}
}

uint32_t buscarEntrenadorParaMensaje(listaMutex* listaIds, uint32_t idMensaje){//devuelve el id del entrenador
	uint32_t i;

	for(i=0;i<sizeListaMutex(listaIds);i++){
		idsEntrenadorMensaje* actual=(void*)getListaMutex(listaIds,i);
		if(actual->idMensaje==idMensaje){
			uint32_t idEntrenador=actual->idEntrenador;
			free(actual);
			removeListaMutex(listaIds,i);
			return idEntrenador;
		}
	}
	return -1;
}

int32_t crearHiloResolucionDeadlock(pthread_t* hilo){
	int32_t err=pthread_create(hilo,NULL,resolverDeadlock,NULL);
					if(err!=0){
						printf("Hubo un problema en la creación del hilo para conectarse al broker \n");
						return err;
					}

		pthread_detach(*hilo);
		return 0;
}

int crearHiloConexionColasBroker(void* config, pthread_t* hilo){
	uint32_t err=pthread_create(hilo,NULL,suscribirseColasBroker,(void*)config);
				if(err!=0){
					printf("Hubo un problema en la creación del hilo para conectarse al broker \n");
					return err;
				}

	pthread_detach(*hilo);
	return 0;
}

uint32_t crearHiloParaEnviarGets(pthread_t* hilo){
	uint32_t err=pthread_create(hilo,NULL,enviarGets,NULL);
					if(err!=0){
						printf("Hubo un problema en la creación del hilo para conectarse al broker \n");
						return err;
					}

		pthread_detach(*hilo);
		return 0;
}

void* enviarGets(void* arg){
	listaMutex* objetivoGlobal=team->objetivoGlobal;
	uint32_t i=0;
	for(i=0;i<sizeListaMutex(objetivoGlobal);i++){
		objetivo* objetivoActual=(objetivo*)getListaMutex(objetivoGlobal,i);
		pthread_t hilo;
		uint32_t err=pthread_create(&hilo,NULL,enviarGet,(void*)(objetivoActual->pokemon));
							if(err!=0){
								printf("Hubo un problema en la creación del hilo para conectarse al broker \n");

							}

		pthread_detach(hilo);
	}
	return NULL;
}

//uint32_t reconectarseAlBroker(uint32_t cliente,void* direccionServidor,socklen_t length){
//	log_info(teamLogger, "Conexión fallida con el Broker\n");
//	log_info(teamLogger2, "Conexión fallida con el Broker\n");
//	log_info(teamLogger, "Reintentando conexión en %i segundos...\n",tiempoReconexion);
//	sleep(tiempoReconexion);
//	while(connect(cliente, direccionServidor,length)<0){
//		log_info(teamLogger,"El reintento de conexión no fue exitoso\n");
//		log_info(teamLogger2,"El reintento de conexión no fue exitoso\n");
//		log_info(teamLogger, "Reintentando conexión en %i segundos...\n",tiempoReconexion);
//		sleep(tiempoReconexion);
//
//
//	}
//	log_info(teamLogger, "El reintento de conexión fue exitoso\n");
//	log_info(teamLogger2, "El reintento de conexión fue exitoso\n");
//	return 0;
//}

uint32_t reconectarseAlBroker(){
	log_info(teamLogger, "Conexión fallida con el Broker\n");
	log_info(teamLogger2, "Conexión fallida con el Broker\n");
	log_info(teamLogger, "Reintentando conexión en %i segundos...\n",tiempoReconexion);
	sleep(tiempoReconexion);
	uint32_t cliente;
	struct sockaddr_in direccionServidor;
	uint32_t i=0;
	do{


		direccionServidor.sin_family      = AF_INET;
		direccionServidor.sin_addr.s_addr = inet_addr(ipBroker);
		direccionServidor.sin_port        = htons(puertoBroker);

		cliente=socket(AF_INET,SOCK_STREAM,0);
		if(i>0){
			log_info(teamLogger,"El reintento de conexión no fue exitoso\n");
			log_info(teamLogger2,"El reintento de conexión no fue exitoso\n");
		}
		i++;
		log_info(teamLogger, "Reintentando conexión en %i segundos...\n",tiempoReconexion);
		sleep(tiempoReconexion);

	}while(connect(cliente,(void*) &direccionServidor,sizeof(direccionServidor))<0);

	log_info(teamLogger, "El reintento de conexión fue exitoso\n");
	log_info(teamLogger2, "El reintento de conexión fue exitoso\n");

	return cliente;
}

void* suscribirseColasBroker(void* conf){

	mensajeSuscripcion* mensajeSuscripcionAppeared=llenarSuscripcion(APPEARED_POKEMON, idProcesoTeam);
	mensajeSuscripcion * mensajeSuscripcionCaught=llenarSuscripcion(CAUGHT_POKEMON, idProcesoTeam);
	mensajeSuscripcion* mensajeSuscripcionLocalized=llenarSuscripcion(LOCALIZED_POKEMON, idProcesoTeam);


	pthread_create(&threadSuscripcionAppeared, NULL, suscribirseCola, (void*)(mensajeSuscripcionAppeared));
	pthread_detach(threadSuscripcionAppeared);

	pthread_create(&threadSuscripcionLocalized, NULL, suscribirseCola,(void*) (mensajeSuscripcionLocalized));
	pthread_detach(threadSuscripcionLocalized);

	pthread_create(&threadSuscripcionCaught, NULL, suscribirseCola, (void*)(mensajeSuscripcionCaught));
	pthread_detach(threadSuscripcionCaught);

	while(1);


	return NULL;
}

uint32_t enviarSuscripcion(uint32_t socket, mensajeSuscripcion* msg){
	uint32_t cliente=socket;

	void* streamMsgSuscripcion=serializarSuscripcion(msg);

	paquete* paq=llenarPaquete(TEAM,SUSCRIPCION,sizeArgumentos(SUSCRIPCION, "",0), streamMsgSuscripcion);

	uint32_t bytes = sizePaquete(paq);

	void* stream   = serializarPaquete(paq);

	while(send(cliente,stream,bytes,0)<0){
		cliente=reconectarseAlBroker();
	}

	uint32_t respuesta = -1;

	recv(cliente,&respuesta,sizeof(uint32_t),0);
	printf("Socket: %i, cola: %i\n", cliente, msg->cola);

	if(respuesta!=CORRECTO){
		log_info(teamLogger2, "Hubo un problema con la suscripción a una cola.");
	}

	destruirPaquete(paq);
	free(stream);
	return cliente;
}

void* suscribirseCola(void* msgSuscripcion){
	mensajeSuscripcion* msg=(mensajeSuscripcion*)msgSuscripcion;
	//uint32_t sizeStream=sizeof(uint32_t);

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family      = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ipBroker);
	direccionServidor.sin_port        = htons(puertoBroker);

	uint32_t cliente=socket(AF_INET,SOCK_STREAM,0);

	if(connect(cliente,(void*) &direccionServidor,sizeof(direccionServidor))<0){
		cliente=reconectarseAlBroker();
	}

	cliente=enviarSuscripcion(cliente, msg);
	printf("socket: %i\n", cliente);
	log_info(teamLogger2,"Suscripción realizada correctamente\n");

	while(1){
		paquete* paqueteRespuesta=recibirPaquete(cliente);

		while(paqueteRespuesta==NULL){
			cliente=reconectarseAlBroker();
			cliente=enviarSuscripcion(cliente, msg);
			paqueteRespuesta=recibirPaquete(cliente);
		}

		loggearMensaje(paqueteRespuesta, teamLogger);

		int32_t resultadoAck=enviarACK(puertoBroker,ipBroker,  TEAM, paqueteRespuesta->id, idProcesoTeam);
		while(resultadoAck<0){
			cliente=reconectarseAlBroker();
			cliente=enviarSuscripcion(cliente, msg);
			resultadoAck=enviarACK(puertoBroker,ipBroker,  TEAM, paqueteRespuesta->id, idProcesoTeam);
		}

		switch(paqueteRespuesta->tipoMensaje){
			case APPEARED_POKEMON:;
				pthread_t threadAppeared;
				pthread_create(&threadAppeared, NULL, atenderAppeared,(void*) (paqueteRespuesta));
				pthread_detach(threadAppeared);
				break;
			case LOCALIZED_POKEMON:;
				pthread_t threadLocalized;
				pthread_create(&threadLocalized, NULL, atenderLocalized,(void*) (paqueteRespuesta));
				pthread_detach(threadLocalized);//recordar destruir el paquete
				break;
			case CAUGHT_POKEMON:;
				pthread_t threadCaught;
				pthread_create(&threadCaught, NULL, atenderCaught,(void*) (paqueteRespuesta));
				pthread_detach(threadCaught);
				break;
			default: break; //esto no puede pasar
			}

//				while(send(cliente,(void*)(&respuesta),sizeof(uint32_t),0)<0){
//					cliente=reconectarseAlBroker();
//					cliente=enviarSuscripcion(cliente, msg);
//
//				}
			}





		return NULL;
}





void enviarCatch(dataEntrenador* infoEntrenador){

	int32_t cliente=crearSocketCliente(ipBroker,puertoBroker);
	log_info(teamLogger2, "El entrenador %i envia el catch para el pokemon %s.", infoEntrenador->id, infoEntrenador->pokemonAAtrapar->pokemon);
	simularCicloCpu(1,infoEntrenador);
	if(cliente!=-1){


		mensajeCatch* msgCatch=llenarCatch(infoEntrenador->pokemonAAtrapar->pokemon, (infoEntrenador->pokemonAAtrapar->posicion).x,(infoEntrenador->pokemonAAtrapar->posicion).y);
		void* streamMsg=serializarCatch(msgCatch);
		paquete* paq=llenarPaquete(TEAM,CATCH_POKEMON,   sizeArgumentos(CATCH_POKEMON,msgCatch->pokemon,1)  , streamMsg);
		void* paqueteSerializado=serializarPaquete(paq);
		destruirCatch(msgCatch);

		if(send(cliente,paqueteSerializado, sizePaquete(paq), 0)!=-1){


			log_info(teamLogger2,"El entrenador %i hace el send del catch.", infoEntrenador->id);

			uint32_t idMensaje=0;

			if(recv(cliente, &idMensaje, sizeof(uint32_t),0)==-1){
				log_info(teamLogger2,"Ocurrio un error al recibir la respuesta de un catch\n");
			}

			if(idMensaje>0){

				log_info(teamLogger2,"El entrenador % i recibió el id del catch: %i.", infoEntrenador->id, idMensaje);
				idsEntrenadorMensaje* parDeIds=malloc(sizeof(idsEntrenadorMensaje));
				parDeIds->idEntrenador=infoEntrenador->id;
				parDeIds->idMensaje=idMensaje;
				addListaMutex(listaIdsEntrenadorMensaje,(void*)parDeIds);

			}else{
				atraparPokemonYReplanificar (infoEntrenador);
			//se recibio erroneamente
			}
		}else{
			log_info(teamLogger, "Fallo de comunicación con el Broker al enviar un catch. Se realizará la operación por default.");
			atraparPokemonYReplanificar (infoEntrenador);
			log_info(teamLogger2,"El entrenador % i TERMINÓ DE SIMULAR EL CATCH.", infoEntrenador->id);
			//atraparPokemonYReplanificar (infoEntrenador);
		}
		free(paqueteSerializado);
	}else{
		atraparPokemonYReplanificar (infoEntrenador);
		log_info(teamLogger2,"El entrenador % i TERMINÓ DE SIMULAR EL CATCH.", infoEntrenador->id);
		log_info(teamLogger, "Fallo de comunicación con el Broker al enviar un catch. Se realizará la operación por default.");
		//atraparPokemonYReplanificar (infoEntrenador);
	}

	close(cliente);
}

int32_t buscarMismoPokemonListaMutex(listaMutex* lst, char* pokemon){
	for(uint32_t i=0; i<sizeListaMutex(lst);i++){
		char* pokemonActual=(char*)getListaMutex(lst,i);
		if(strcmp(pokemon,pokemonActual)==0){
			return i;
		}
	}
	return -1;
}

void removerPokemonConCatchPendiente(char* pokemon){
	int32_t posPokemonPendiente=buscarMismoPokemonListaMutex(pokemonesConCatchPendiente, pokemon);
	if(posPokemonPendiente!=-1)
		removeListaMutex(pokemonesConCatchPendiente,posPokemonPendiente);
}

pokemonPosicion* obtenerPokemonPosicionEnReserva(char* pokemon){
	for(uint32_t i=0; i<sizeListaMutex(pokemonesPosicionDeReserva); i++){
		pokemonPosicion* pokeActual= (pokemonPosicion*) getListaMutex(pokemonesPosicionDeReserva,i);
		if(strcpy(pokeActual->pokemon,pokemon)){
			return (pokemonPosicion*)removeListaMutex(pokemonesPosicionDeReserva,i);
		}
	}
	return NULL;
}

void* enviarGet(void* arg){
	char* pokemon=(char*) arg;
	uint32_t cliente=crearSocketCliente(ipBroker,puertoBroker);
	if(cliente!=-1){//se pudo conectar
		mensajeGet* msg=llenarGet(pokemon);
		void* stream=serializarGet(msg);
		paquete* paq=llenarPaquete(TEAM, GET_POKEMON,sizeArgumentos(GET_POKEMON,msg->pokemon,1),stream);
		void* paqueteSerializado=serializarPaquete(paq);
		// hacer destroy para el msg
		if(send(cliente,paqueteSerializado, sizePaquete(paq), 0)!=-1){
			free(paqueteSerializado);
			uint32_t* idMensaje=malloc(sizeof(uint32_t));
			*idMensaje=0;

			if(recv(cliente, idMensaje, sizeof(uint32_t),0)==-1){
				printf("Ocurrio un error al recibir la respuesta de un get\n");
			}
			if(*idMensaje>0){
				addListaMutex(listaIdsRespuestasGet,(void*)(idMensaje));

			}else{
				//se recibio erroneamente
			}

		}else{
			log_info(teamLogger, "Fallo de comunicación con el Broker al enviar un get. Se realizará la operación por default.");
		}
		destruirGet(msg);
	}else{
		log_info(teamLogger, "Fallo de comunicación con el Broker al enviar un get. Se realizará la operación por default.");
	}
	close(cliente);
	return NULL;

}

uint32_t crearSocketClienteBroker (char* ip, uint32_t puerto){
		struct sockaddr_in direccionServidor;
		direccionServidor.sin_family      = AF_INET;
		direccionServidor.sin_addr.s_addr = inet_addr(ipBroker);
		direccionServidor.sin_port        = htons(puertoBroker);

		uint32_t cliente=socket(AF_INET,SOCK_STREAM,0);
		uint32_t resultadoConnect = connect(cliente,(void*) &direccionServidor,sizeof(direccionServidor));

		while(resultadoConnect<0){
			log_info(teamLogger, "Conexión fallida con el Broker\n");
			log_info(teamLogger, "Reintentando conexión en %i segundos...\n",tiempoReconexion);
			sleep(tiempoReconexion);
			resultadoConnect=connect(cliente,(void*) &direccionServidor,sizeof(direccionServidor));
			if(resultadoConnect<0){
				log_info(teamLogger,"El reintento de conexión no fue exitoso\n");
			}else{
				log_info(teamLogger, "El reintento de conexión fue exitoso\n");
			}
		}
	return cliente;
}


dataTeam* inicializarTeam(t_config* config){
	dataTeam* infoTeam       = malloc(sizeof(dataTeam));
	infoTeam->entrenadores   = list_create();
	t_list* especiesObjetivo = list_create();
	infoTeam->objetivoGlobal=inicializarListaMutex();

	infoTeam->cantidadCiclosCpuTotales=0;
	infoTeam->cantidadCambiosContexto=0;
	infoTeam->cantidadDeadlocksEncontrados=0;
	infoTeam->cantidadDeadlocksResueltos=0;

	t_list* pokemonesDelTeam=list_create();
	char** arrayPosicionesEntrenadores=config_get_array_value(config,"POSICIONES_ENTRENADORES");
	char** arrayPokemonesEntrenadores=config_get_array_value(config,"POKEMON_ENTRENADORES");

	char** arrayObjetivosEntrenadores=config_get_array_value(config,"OBJETIVOS_ENTRENADORES");

	t_list* posicionesEntrenadores = obtenerListaDeListas(arrayPosicionesEntrenadores);
	t_list* pokemonesEntrenadores  = obtenerListaDeListas(arrayPokemonesEntrenadores);

	t_list* objetivosEntrenadores  = obtenerListaDeListas(arrayObjetivosEntrenadores);
	uint32_t cantEntrenadores      = list_size(posicionesEntrenadores);

	uint32_t id;

	for(id=0;id<cantEntrenadores;id++){
		dataEntrenador* infoEntrenador=malloc(sizeof(dataEntrenador));
		char** pos		 = list_get(posicionesEntrenadores,id);
		char** pokemones = list_get(pokemonesEntrenadores,id);
		char** objetivos = list_get(objetivosEntrenadores,id);

		(infoEntrenador->posicion).x = atoi(pos[0]);
		(infoEntrenador->posicion).y = atoi(pos[1]);

		infoEntrenador->pokemones        = arrayStringALista(pokemones);
		infoEntrenador->objetivoPersonal = arrayStringALista(objetivos);

		t_list* pokemonesEntrenadorAux        = arrayStringALista(pokemones);
		t_list* objetivoPersonalEntrenadorAux = arrayStringALista(objetivos);
		uint32_t i;


		for(i=0;i<list_size(pokemonesEntrenadorAux);i++){
			char *pokemonAComparar = (char*) list_get(pokemonesEntrenadorAux,i);
			uint32_t encontrado    = buscarMismoPokemon(objetivoPersonalEntrenadorAux,pokemonAComparar);

			if(encontrado != -1){
				list_remove(objetivoPersonalEntrenadorAux,encontrado);
			}
		}

		for(i=0;i<list_size(infoEntrenador->objetivoPersonal);i++){

						list_add(especiesObjetivo,list_get(infoEntrenador->objetivoPersonal,i));

				}
		infoEntrenador->ejecucionEnPausa=false;
		infoEntrenador->estado			= NEW;
		infoEntrenador->id				= id;
		infoEntrenador->pokemonAAtrapar = NULL;
		infoEntrenador->cantidadCiclosCpu = 0;
		infoEntrenador->semaforo=malloc(sizeof(sem_t));
		infoEntrenador->semaforoContinuarEjecucion=malloc(sizeof(sem_t));
		infoEntrenador->semaforoPedidoCiclo=malloc(sizeof(sem_t));
		infoEntrenador->semaforoResultadoInterrupcion=malloc(sizeof(sem_t));
		sem_init((infoEntrenador->semaforo), 0,0);
		sem_init((infoEntrenador->semaforoContinuarEjecucion), 0,0);
		sem_init((infoEntrenador->semaforoPedidoCiclo), 0,0);
		sem_init((infoEntrenador->semaforoResultadoInterrupcion), 0,0);
		infoEntrenador->rafagaCpuAnterior=0;
		infoEntrenador->estimacionAnterior=estimacionInicial;
		//infoEntrenador->estimacionAnterior=5;
		infoEntrenador->contadorCpu=inicializarContadorRafagas();
		list_add(entrenadoresLibres->lista,(void*)infoEntrenador);
		list_add(infoTeam->entrenadores,infoEntrenador);

		list_add_all(pokemonesDelTeam, infoEntrenador->pokemones);

		list_destroy(pokemonesEntrenadorAux);
		list_destroy(objetivoPersonalEntrenadorAux);

		//-----------------------------------
		free(pos);
		free(pokemones);
		free(objetivos);
	}

	uint32_t z;
	for(z=0;z<list_size(pokemonesDelTeam);z++){
				char *pokemonAComparar = (char*) list_get(pokemonesDelTeam,z);
				uint32_t encontrado    = buscarMismoPokemon(especiesObjetivo,pokemonAComparar);

				if(encontrado != -1){

					list_remove(especiesObjetivo,encontrado);

				}
			}

	infoTeam->objetivoGlobal = obtenerObjetivos(especiesObjetivo);
	list_destroy(especiesObjetivo);
	list_destroy(pokemonesDelTeam);

	list_destroy(posicionesEntrenadores);
	list_destroy(pokemonesEntrenadores);
	list_destroy(objetivosEntrenadores);

	//---------------------------------------------------
	free(arrayPosicionesEntrenadores);
	free(arrayPokemonesEntrenadores);
	free(arrayObjetivosEntrenadores);
	return infoTeam;

}

void destruirElementoListaDeListas(void** elemento){
	free(*elemento);
	free(elemento);
}

uint32_t buscarMismoPokemon(t_list* lst, char* pokemon){//devuelve la posicion en la que encontro el pokemon

	uint32_t i;
	for(i=0;i<list_size(lst);i++){
		char* pokemonAComparar=(char*)list_get(lst,i);
		if(strcmp(pokemonAComparar,pokemon)==0){
			return i;
		}
	}

	return -1;
}


t_list* arrayStringALista(char** arr){
	uint32_t i;
	t_list* lst = list_create();
	if(arr==NULL){
		return lst;
	}
	for(i=0;arr[i]!=NULL;i++){
		list_add(lst,(void*) arr[i]);
	}
	return lst;
}




listaMutex* obtenerObjetivos(t_list* especies){
	listaMutex* objetivos = inicializarListaMutex();


	for(uint32_t i = 0;i<list_size(especies);i++){
		char* especie = (char*)list_get(especies,i);

		//especieAComparar=especie;

		uint32_t encontrado = buscarObjetivoPorEspecie(objetivos,especie);
		if(encontrado == -1){
			objetivo* obj = malloc(sizeof(objetivo));
			obj->cantidad = 1;
			obj->pokemon=malloc(strlen(especie)+1);
			strcpy(obj->pokemon,especie);
			//printf("%s\n",(char*)(objetivo->pokemon));
			addListaMutex(objetivos,(void*)obj);
		}else{
			(((objetivo*)getListaMutex(objetivos,encontrado))->cantidad)++;
		}
	}
	return objetivos;

}

uint32_t buscarObjetivoPorEspecie(listaMutex* listaObjetivos, char* especie){
	uint32_t i;
	for(i=0;i<sizeListaMutex(listaObjetivos);i++){
		objetivo* obj = (objetivo*)getListaMutex(listaObjetivos,i);
		if(strcmp(obj->pokemon,especie) == 0){
			return i;
		}else{
//			log_info(teamLogger2, "--------------------------------------------------------");
//			log_info(teamLogger2, "Objetivo en lista: %s. Objetivo recibido: %s.", obj->pokemon,especie);
		}
	}
	return -1;
}


void registrarPokemonAtrapado(char* pokemon){

		log_info(teamLogger2,"Se registró el pokemon atrapado. %s", pokemon);

}

void agregarObjetivo(char* pokemon){
	uint32_t encontrado = buscarObjetivoPorEspecie(team->objetivoGlobal,pokemon);
	if(encontrado == -1){
		objetivo* obj = malloc(sizeof(objetivo));
		obj->cantidad = 1;
		obj->pokemon=malloc(strlen(pokemon)+1);
		strcpy(obj->pokemon,pokemon);
		addListaMutex(team->objetivoGlobal,(void*)obj);
	}else{
		(((objetivo*)getListaMutex(team->objetivoGlobal,encontrado))->cantidad)++;
	}
}

void removerObjetivo(char* pokemon){
	uint32_t pos=buscarObjetivoPorEspecie(team->objetivoGlobal,pokemon);
		if(pos!=-1){
			objetivo* objetivoEncontrado=((objetivo*)getListaMutex(team->objetivoGlobal,pos));
			(objetivoEncontrado->cantidad)--;
			if(objetivoEncontrado->cantidad==0){
				removeListaMutex(team->objetivoGlobal,pos);
				destruirObjetivo((void*)objetivoEncontrado);
			}
		}
}

bool pokemonEsObjetivo(char* pokemon){
	uint32_t pos=buscarObjetivoPorEspecie(team->objetivoGlobal,pokemon);
	if(pos!=-1){
			//log_info(teamLogger2, "Encontre al pokemon %s en mis objetivos", pokemon);
			return((((objetivo*)getListaMutex(team->objetivoGlobal,pos))->cantidad)>0);
		}
	//log_info(teamLogger2, "No encontre al pokemon %s en mis objetivos", pokemon);
	return false;
}


t_list* obtenerListaDeListas(char** lst){
	t_list* lstDeLst = list_create();
	uint32_t i       = 0;
	char* aux=lst[i];
	while(aux!=NULL){
		char** str = string_split(aux, "|");
		list_add(lstDeLst,(void**)str);
		i++;
		aux = lst[i];
	}
	return lstDeLst;
}

void loggearPokemonAAtrapar(dataEntrenador* entrenador, t_log* teamLogger){
	log_info(teamLogger, "El entrenador % i se mueve a atrapar a %s en posición (%i, %i)\n",entrenador->id, entrenador->pokemonAAtrapar->pokemon, (entrenador->pokemonAAtrapar->posicion).x, (entrenador->pokemonAAtrapar->posicion).y);
}



//void resetearSemaforo(sem_t* semaforo){
//	int32_t valor;
//	sem_getvalue(semaforo,&valor);
//	while(valor<0){
//		sem_post(semaforo);
//		valor++;
//	}
//}





