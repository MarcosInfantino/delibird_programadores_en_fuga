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


//TODO: corregir recibirPaquete

//t_list* idsHilos=list_create();//son ints
//t_list* hilos=list_create();//son pthread_t
//uint32_t puertoTeam=5003;
//pthread_mutex_t mutexEntrenadorEnEjecucion=PTHREAD_MUTEX_INITIALIZER;
//listaMutex listaIdsEntrenadorMensaje; // del tipo idsEntrenadorMensaje , //ver a futuro si esta lista requiere mutex
//listaMutex entrenadoresLibres;
//colaMutex pokemonesPendientes;//lista de pokePosicion que contiene los pokemones que no pudieron ser asignados a ningun entrenador por no haber entrenadore libres
//listaMutex entrenadoresExit;
////t_list* mutexEntrenadores;
//listaMutex entrenadores;
//colaMutex colaEjecucionFifo;

//pokemonPosicion pokemonAAtrapar;
//
//pthread_mutex_t mutexEntrenadorEnEjecucion=PTHREAD_MUTEX_INITIALIZER;
//int socketGameboy;
//int socketGamecard;
//uint32_t puertoBroker;
//char* ipBroker;
//pthread_t* arrayIdHilosEntrenadores;
//uint32_t tiempoReconexion;
//uint32_t retardoCicloCpu;
//char* algoritmoPlanificacion;
//
//sem_t semaforoEjecucionCpu;
//
//int main(void) {
////	char* str="[Pikachu|Squirtle|Pidgey, Squirtle|Charmander, Bulbasaur]";
////
////	t_list* lst=obtenerListaDeListas(str);
////	t_list* primerLista=arrayStringALista(((char**)list_get(lst,0)));
////
////	printf("%s\n", (char*) (list_get(primerLista,2)));
//	char* pathConfig="Team2.config";
//	t_config* config=config_create(pathConfig);
//	dataTeam* t=inicializarTeam(config);
//
//	printf("No se colgo\n");
//
////	t_list* prueba=list_create();
//
//	int pos=1;
//	printf("Objetivo : %s\n",((objetivo*)list_get(t->objetivoGlobal,pos))->pokemon);
//	printf("Cantidad : %i\n",((objetivo*)list_get(t->objetivoGlobal,pos))->cantidad);
//
//	return EXIT_SUCCESS;
//}

//int main(void){
//	int i=0;
//	char* aux="Pikachu|Squirtle|Pidgey";
//	char** lst=string_split(aux, "|");
//	printf("%s",lst[i]);
//	return 0;
//}

//


//dataTeam* team;



//int main(){
//	//mensajeLocalized* llenarMensajeLocalized(uint32_t sizePokemon, char* pokemon, uint32_t cantidad, posicion* posiciones)
//
//	t_log* logPrueba=iniciar_logger("pruebaLocalized.log", "TEAM");
//	char* pokemon="Pikachu";
//	uint32_t cantidad=2;
//	posicion* posiciones=malloc(sizeof(posicion)*2);
//	posicion pos1={1,0};
//	posicion pos2={0,0};
//	*(posiciones)=pos1;
//	*(posiciones+1)=pos2;
//
//	mensajeLocalized* msg= llenarLocalized(pokemon,cantidad,posiciones);
//	void* stream= serializarLocalized(msg);
//	//mensajeLocalized* msgResultado=deserializarLocalized(stream);
//	paquete* paq=llenarPaquete(TEAM,LOCALIZED_POKEMON,sizeArgumentos(LOCALIZED_POKEMON, msg->pokemon,msg->cantidad),stream);
//	loggearMensaje( paq, logPrueba);
//	destruirLocalized(msg);
//
//	return 0;
//}


int main(int argc , char* argv[]){
	teamLogger = iniciar_logger("team.log", "TEAM");
	teamLogger2=log_create("teamLoggerSecundario.log","team", true, LOG_LEVEL_INFO);

	log_info(teamLogger2,"--------------------------------------------------------------------------");
	log_info(teamLogger2,"NUEVA EJECUCION");
	log_info(teamLogger2,"--------------------------------------------------------------------------");
	//char pathConfig  = argv;
	sem_init(&semaforoEjecucionCpu, 0,0);
	sem_init(&intercambioFinalizado, 0,0);
	entrenadoresLibres=inicializarListaMutex();
	colaEjecucionFifo=inicializarColaMutex();
	pokemonesPendientes=inicializarColaMutex();
	listaIdsEntrenadorMensaje=inicializarListaMutex();
	entrenadores=inicializarListaMutex();
	especiesLocalizadas=inicializarListaMutex();
	entrenadoresExit=inicializarListaMutex();
	entrenadoresDeadlock=inicializarListaMutex();
	listaIdsRespuestasGet=inicializarListaMutex();

	char* pathConfig   = "Team2.config";
	t_config* config   = config_create(pathConfig);
	retardoCicloCpu    = config_get_int_value(config,"RETARDO_CICLO_CPU");
	tiempoReconexion= config_get_int_value(config,"TIEMPO_RECONEXION");
	puertoBroker       = config_get_int_value(config,"PUERTO_BROKER");
	ipBroker           = config_get_string_value(config,"IP_BROKER");
	obtenerAlgoritmoPlanificacion(config);
	//printf("hola\n");

	team     = inicializarTeam(config);
	//printf("hola2\n");
	//printf("%s\n", ((objetivo*)getListaMutex(team->objetivoGlobal,2))->pokemon);
	entrenadores->lista       = team->entrenadores;
	//entrenadoresLibres=entrenadores;

	//mutexEntrenadores=inicializarMutexEntrenadores();
	uint32_t cantEntrenadores = list_size(team->entrenadores);
	loggearObjetivoDelTeam();
	arrayIdHilosEntrenadores  = malloc(cantEntrenadores*sizeof(pthread_t));
	inicializarEntrenadores(team->entrenadores);

	pthread_t hiloEnviarGets;
	crearHiloParaEnviarGets(&hiloEnviarGets);

	pthread_t hiloConexionInicialBroker;

	crearHiloConexionColasBroker((void*)config,&hiloConexionInicialBroker);

	pthread_t hiloServidorGameboy;
	crearHiloServidorGameboy(&hiloServidorGameboy);

	pthread_t hiloPlanificador;
	crearHiloPlanificador(&hiloPlanificador);

//	posicion pos = {1,2};
//
//	printf("id entrenador mas cercano: %i\n", obtenerIdEntrenadorMasCercano(pos));

	while(!objetivoCumplido());

	loggearResultado();


	//HACER DESTROY DE TODAS LAS LISTAS Y ESTRUCTURAS AL FINAL
	terminar_programa(teamLogger, config);

	return 0;
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
	log_info(teamLogger,"Programa terminado: objetivo global cumplido.");
	log_info(teamLogger,"Cantidad de ciclos de CPU totales: %i.", team->cantidadCiclosCpuTotales );
	log_info(teamLogger,"Cantidad de cambios de contexto realizados: %i.", team->cantidadCambiosContexto );
	log_info(teamLogger,"------------------------------------------------" );
	log_info(teamLogger,"Información de los entrenadores: " );

	for(uint32_t i=0;i<sizeListaMutex(entrenadores);i++){
		dataEntrenador* entrenadorActual=(dataEntrenador*) getListaMutex(entrenadores,i);
		log_info(teamLogger, "Cantidad de ciclos de CPU realizados por el entrenador %i: %i.",entrenadorActual->id,entrenadorActual->cantidadCiclosCpu);

	}

	log_info(teamLogger,"Deadlocks producidos y resueltos: %i.", team->cantidadDeadlocks);

}
bool objetivoCumplido(){
	return sizeListaMutex(entrenadores)==sizeListaMutex(entrenadoresExit);
}


void atenderAppeared(mensajeAppeared* msg){
	log_info(teamLogger2, "Atiendo appeared. Pokemon: %s.", msg->pokemon);
	pokemonPosicion* pokePosicion=malloc(sizeof(pokemonPosicion));
	if(!especieFueLocalizada(msg->pokemon))
		addListaMutex(especiesLocalizadas,(void*)(msg->pokemon));

	pokePosicion->pokemon=malloc(strlen(msg->pokemon)+1);
	strcpy(pokePosicion->pokemon,msg->pokemon);
	(pokePosicion->posicion).x=msg->posX;
	(pokePosicion->posicion).y=msg->posY;
	destruirAppeared(msg);
	if(pokemonEsObjetivo(pokePosicion->pokemon)){
		log_info(teamLogger2, "El pokemon es objetivo");
		if(sizeListaMutex(entrenadoresLibres)>0){
			log_info(teamLogger2, "Hay entrenadores disponibles para atrapar a %s.",pokePosicion->pokemon );
			seleccionarEntrenador(pokePosicion);
		}else{
			log_info(teamLogger2, "No hay entrenadores disponibles para atrapar a %s. ",pokePosicion->pokemon);
			pushColaMutex(pokemonesPendientes,(void*)pokePosicion);
		}
	}else{
		log_info(teamLogger2, "El pokemon no es objetivo");
	}
}

void atenderLocalized(paquete* paquete){
	mensajeLocalized* msg=deserializarLocalized(paquete->stream);
	char* pokemonAAgregar=malloc(strlen(msg->pokemon)+1);
	strcpy(pokemonAAgregar,msg->pokemon);
	if(!especieFueLocalizada(msg->pokemon)&& localizedMeInteresa(paquete)){
		uint32_t i;
		for(i=0;i<msg->cantidad;i++){
			posicion posActual= *((msg->arrayPosiciones)+i);
			mensajeAppeared* msgAppeared=llenarAppeared(pokemonAAgregar,posActual.x,posActual.y);
			atenderAppeared(msgAppeared);
		}
	}



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
		if(strcpy(pokeActual,pokemon)){
			return true;
		}
	}
	return false;
}
void atenderCaught(paquete* paqueteCaught){
	mensajeCaught* msgCaught=deserializarCaught(paqueteCaught->stream);
	uint32_t id=paqueteCaught->idCorrelativo;
	uint32_t idEncontrado=buscarEntrenadorParaMensaje(listaIdsEntrenadorMensaje,id)!=1;
	if(idEncontrado!=-1){
		dataEntrenador* entrenadorEncontrado=(dataEntrenador*)getListaMutex(entrenadores, idEncontrado);
		if(msgCaught->resultadoCaught==CORRECTO){
//			list_add(entrenadorEncontrado->pokemones,(void*)(entrenadorEncontrado->pokemonAAtrapar->pokemon));
//			registrarPokemonAtrapado(entrenadorEncontrado->pokemonAAtrapar->pokemon);
//			replanificarEntrenador(entrenadorEncontrado);
			atraparPokemonYReplanificar (entrenadorEncontrado);
		}else{
//			entrenadorEncontrado->estado=BLOCKED;
//			habilitarHiloEntrenador(idEncontrado);
			replanificarEntrenador(entrenadorEncontrado);
		}
	}


}



uint32_t buscarEntrenadorParaMensaje(listaMutex* listaIds, uint32_t idMensaje){//devuelve el id del entrenador
	uint32_t i;
	pthread_mutex_lock(listaIds->mutex);
	for(i=0;i<list_size(listaIds->lista);i++){
		idsEntrenadorMensaje* actual=(void*)list_get(listaIds->lista,i);
		if(actual->idMensaje==idMensaje){
			uint32_t idEntrenador=actual->idEntrenador;
			free(actual);
			list_remove(listaIds->lista,i);
			return idEntrenador;
		}
	}
	pthread_mutex_unlock(listaIds->mutex);
	return -1;
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

uint32_t reconectarseAlBroker(uint32_t cliente,void* direccionServidor,socklen_t length){
	log_info(teamLogger, "Conexión fallida con el Broker\n");
	log_info(teamLogger, "Reintentando conexión en %i segundos...\n",tiempoReconexion);
	sleep(tiempoReconexion);
	while(connect(cliente, direccionServidor,length)<0){
		log_info(teamLogger,"El reintento de conexión no fue exitoso\n");
		log_info(teamLogger, "Reintentando conexión en %i segundos...\n",tiempoReconexion);
		sleep(tiempoReconexion);


	}
	log_info(teamLogger, "El reintento de conexión fue exitoso\n");
	return 0;
}

void* suscribirseColasBroker(void* conf){




	mensajeSuscripcion* mensajeSuscripcionAppeared=llenarSuscripcion(APPEARED_POKEMON);
	mensajeSuscripcion * mensajeSuscripcionCaught=llenarSuscripcion(CAUGHT_POKEMON);
	mensajeSuscripcion* mensajeSuscripcionLocalized=llenarSuscripcion(LOCALIZED_POKEMON);

	pthread_t threadSuscripcionAppeared;
	pthread_create(&threadSuscripcionAppeared, NULL, suscribirseCola, (void*)(mensajeSuscripcionAppeared));
	pthread_detach(threadSuscripcionAppeared);

	pthread_t threadSuscripcionLocalized;
	pthread_create(&threadSuscripcionLocalized, NULL, suscribirseCola,(void*) (mensajeSuscripcionLocalized));
	pthread_detach(threadSuscripcionLocalized);

	pthread_t threadSuscripcionCaught;
	pthread_create(&threadSuscripcionCaught, NULL, suscribirseCola, (void*)(mensajeSuscripcionCaught));
	pthread_detach(threadSuscripcionCaught);

	while(1);


	return NULL;
}



void* suscribirseCola(void* msgSuscripcion){
	mensajeSuscripcion* msg=(mensajeSuscripcion*)msgSuscripcion;
	uint32_t sizeStream=sizeof(uint32_t);
	void* streamMsgSuscripcion=serializarSuscripcion(msg);
	destruirSuscripcion(msg);
	paquete* paq=llenarPaquete(TEAM,SUSCRIPCION,sizeStream, streamMsgSuscripcion);

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family      = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ipBroker);
	direccionServidor.sin_port        = htons(puertoBroker);

	uint32_t cliente=socket(AF_INET,SOCK_STREAM,0);

	if(connect(cliente,(void*) &direccionServidor,sizeof(direccionServidor))<0){


		reconectarseAlBroker(cliente,(void*) &direccionServidor,sizeof(direccionServidor));

	}

	uint32_t bytes = sizeof(uint32_t)*5+paq->sizeStream;

	void* stream   = serializarPaquete(paq);

	send(cliente,stream,bytes,0);



	destruirPaquete(paq);
	free(stream);

	uint32_t respuesta = -1;

	recv(cliente,&respuesta,sizeof(uint32_t),0);

		if(respuesta == CORRECTO){
			printf("Mensaje recibido correctamente\n");
			while(1){


				paquete* paqueteRespuesta=recibirPaquete(cliente);
				loggearMensaje(paqueteRespuesta, teamLogger);
				enviarACK(cliente, TEAM, paqueteRespuesta->id);
				switch(paqueteRespuesta->tipoMensaje){
					case APPEARED_POKEMON:;
						mensajeAppeared* msgAppeared=deserializarAppeared(paqueteRespuesta->stream);
						destruirPaquete(paqueteRespuesta);
						atenderAppeared(msgAppeared);

						break;
					case LOCALIZED_POKEMON:
						atenderLocalized(paqueteRespuesta);
						destruirPaquete(paqueteRespuesta);//recordar destruir el paquete
						break;
					case CAUGHT_POKEMON:
						atenderCaught(paqueteRespuesta);
						destruirPaquete(paqueteRespuesta);
						break;
					default: break; //esto no puede pasar


				}

				while(send(cliente,(void*)(&respuesta),sizeof(uint32_t),0)<0){
					reconectarseAlBroker(cliente,(void*) &direccionServidor,sizeof(direccionServidor));

				}
			}

		}else{
			//printf("Mensaje recibido incorrectamente\n");


		}



		return NULL;
}




//TODO: hacer funciones de llenarMensajeX en la lib
//llenarPaquete( uint32_t modulo,uint32_t tipoMensaje, uint32_t sizeStream,void* stream)
void enviarCatch(dataEntrenador* infoEntrenador){
	log_info(teamLogger2,"El entrenador %i inició el proceso de envío del catch.", infoEntrenador->id);
	uint32_t cliente=crearSocketCliente(ipBroker,puertoBroker);
	if(cliente!=-1){


		mensajeCatch* msgCatch=llenarCatch(infoEntrenador->pokemonAAtrapar->pokemon, (infoEntrenador->pokemonAAtrapar->posicion).x,(infoEntrenador->pokemonAAtrapar->posicion).y);
		void* streamMsg=serializarCatch(msgCatch);
		paquete* paq=llenarPaquete(TEAM,CATCH_POKEMON,   sizeArgumentos(CATCH_POKEMON,msgCatch->pokemon,1)  , streamMsg);
		void* paqueteSerializado=serializarPaquete(paq);
		destruirCatch(msgCatch);
		log_info(teamLogger2,"El entrenador %i intenta hacer el send del catch.", infoEntrenador->id);
		//destruirPaquete(paq);
		if(send(cliente,paqueteSerializado, sizePaquete(paq), 0)!=-1){
			simularCicloCpu(1,infoEntrenador);

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
			//se recibio erroneamente
			}
		}else{
			log_info(teamLogger, "Fallo de comunicación con el Broker al enviar un catch. Se realizará la operación por default.");
			atraparPokemonYReplanificar (infoEntrenador);
		}
		free(paqueteSerializado);
	}else{
		log_info(teamLogger, "Fallo de comunicación con el Broker al enviar un catch. Se realizará la operación por default.");
		atraparPokemonYReplanificar (infoEntrenador);
	}
	close(cliente);
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
			uint32_t idMensaje=0;

			if(recv(cliente, &idMensaje, sizeof(uint32_t),0)==-1){
				printf("Ocurrio un error al recibir la respuesta de un get\n");
			}
			if(idMensaje>0){
				addListaMutex(listaIdsRespuestasGet,(void*)(&idMensaje));

			}else{
				//se recibio erroneamente
			}

		}else{
			log_info(teamLogger, "Fallo de comunicación con el Broker al enviar un get. Se realizará la operación por default.");
		}
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
	infoTeam->objetivosCumplidos = list_create();
	infoTeam->cantidadCiclosCpuTotales=0;
	infoTeam->cantidadCambiosContexto=0;
	infoTeam->cantidadDeadlocks=0;

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
		//for(i=0;i<list_size(pokemonesEntrenadorAux);i++){

		for(i=0;i<list_size(pokemonesEntrenadorAux);i++){
			char *pokemonAComparar = (char*) list_get(pokemonesEntrenadorAux,i);
			uint32_t encontrado    = buscarMismoPokemon(objetivoPersonalEntrenadorAux,pokemonAComparar);

			if(encontrado != -1){
				list_remove(objetivoPersonalEntrenadorAux,encontrado);
			}
		}

		for(i=0;i<list_size(objetivoPersonalEntrenadorAux);i++){

						list_add(especiesObjetivo,list_get(objetivoPersonalEntrenadorAux,i));

				}

		infoEntrenador->estado			= NEW;
		infoEntrenador->id				= id;
		infoEntrenador->pokemonAAtrapar = NULL;
		infoEntrenador->cantidadCiclosCpu = 0;
		sem_init(&(infoEntrenador->semaforo), 0,0);
		list_add(entrenadoresLibres->lista,(void*)infoEntrenador);
		list_add(infoTeam->entrenadores,infoEntrenador);


	}

	infoTeam->objetivoGlobal = obtenerObjetivos(especiesObjetivo);
	return infoTeam;

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
	for(i=0;arr[i]!=NULL;i++){
		list_add(lst,(void*) arr[i]);
	}
	return lst;
}


//t_list* obtenerObjetivos(t_list* especies){
//	t_list* objetivos=list_create();
//
//
//	for(uint32_t i=0;i<list_size(especies);i++){
//		void* especie=list_get(especies,i);
//
//		especieAComparar=especie;
//
//		void* encontrado=list_find(objetivos,objetivoMismaEspecie);
//		if(encontrado==NULL){
//			objetivo* objetivo=malloc(sizeof(objetivo));
//			objetivo->cantidad=1;
//			objetivo->pokemon= (char*) especie;
//			//printf("%s\n",(char*)(objetivo->pokemon));
//			list_add(objetivos,(void*)objetivo);
//		}else{
//			(((objetivo*)encontrado)->cantidad)++;
//		}
//	}
//	return objetivos;
//
//}

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
			log_info(teamLogger2, "--------------------------------------------------------");
			log_info(teamLogger2, "Objetivo en lista: %s. Objetivo recibido: %s.", obj->pokemon,especie);
		}
	}
	return -1;
}


void registrarPokemonAtrapado(char* pokemon){
	uint32_t pos=buscarObjetivoPorEspecie(team->objetivoGlobal,pokemon);
	if(pos!=-1){
		objetivo* objetivoEncontrado=((objetivo*)getListaMutex(team->objetivoGlobal,pos));
		(objetivoEncontrado->cantidad)--;
		if(objetivoEncontrado->cantidad==0){
			removeListaMutex(team->objetivoGlobal,pos);
		}

	}else
		printf("No se pudo registrar el pokemon atrapado ya que no está en los objetivos del team\n");
}

bool pokemonEsObjetivo(char* pokemon){
	uint32_t pos=buscarObjetivoPorEspecie(team->objetivoGlobal,pokemon);
	if(pos!=-1){
			log_info(teamLogger2, "Encontre al pokemon %s en mis objetivos", pokemon);
			return((((objetivo*)getListaMutex(team->objetivoGlobal,pos))->cantidad)>0);
		}
	log_info(teamLogger2, "No encontre al pokemon %s en mis objetivos", pokemon);
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

void loggearPokemonAAtrapar(pokemonPosicion* pokePosicion, t_log* teamLogger){
	log_info(teamLogger, "El pokemon a atrapar es: %s, y la posición es: (%i, %i)\n", pokePosicion->pokemon, (pokePosicion->posicion).x, (pokePosicion->posicion).y);
}









