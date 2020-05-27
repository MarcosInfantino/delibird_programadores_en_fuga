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


dataTeam* team;



//int main(){
//	//mensajeLocalized* llenarMensajeLocalized(uint32_t sizePokemon, char* pokemon, uint32_t cantidad, posicion* posiciones)
//
//
//	char* pokemon="Pikachu";
//	uint32_t cantidad=2;
//	posicion* posiciones=malloc(sizeof(posicion)*2);
//	posicion pos1={1,0};
//	posicion pos2={0,0};
//	*(posiciones)=pos1;
//	*(posiciones+1)=pos2;
//
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

int main(int argc , char* argv[]){
	//char pathConfig  = argv;
	sem_init(&semaforoEjecucionCpu, 0,0);
	entrenadoresLibres=inicializarListaMutex();
	colaEjecucionFifo=inicializarColaMutex();
	pokemonesPendientes=inicializarColaMutex();
	listaIdsEntrenadorMensaje=inicializarListaMutex();
	entrenadores=inicializarListaMutex();
	especiesLocalizadas=inicializarListaMutex();
	entrenadoresExit=inicializarListaMutex();
	listaIdsRespuestasGet=inicializarListaMutex();
	char* pathConfig   = "Team2.config";
	t_config* config   = config_create(pathConfig);
	retardoCicloCpu    = config_get_int_value(config,"RETARDO_CICLO_CPU");
	puertoBroker       = config_get_int_value(config,"PUERTO_BROKER");
	ipBroker           = config_get_string_value(config,"IP_BROKER");
	obtenerAlgoritmoPlanificacion(config);
	//printf("hola\n");

	team     = inicializarTeam(config);
	//printf("hola2\n");
	printf("%s\n", ((objetivo*)getListaMutex(team->objetivoGlobal,2))->pokemon);
	entrenadores->lista       = team->entrenadores;
	//entrenadoresLibres=entrenadores;

	//mutexEntrenadores=inicializarMutexEntrenadores();
	uint32_t cantEntrenadores = list_size(team->entrenadores);

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

	posicion pos = {1,2};

	printf("id entrenador mas cercano: %i\n", obtenerIdEntrenadorMasCercano(pos));

	while(!objetivoCumplido());

	printf("Programa terminado: objetivo golbal cumplido\n");

	//HACER DESTROY DE TODAS LAS LISTAS Y ESTRUCTURAS AL FINAL



	return 0;
}



bool objetivoCumplido(){
	return sizeListaMutex(entrenadores)==sizeListaMutex(entrenadoresExit);
}


void atenderAppeared(mensajeAppearedTeam* msg){
	pokemonPosicion* pokePosicion=malloc(sizeof(pokemonPosicion));
	addListaMutex(especiesLocalizadas,(void*)(msg->pokemon));
	pokePosicion->pokemon=msg->pokemon;
	(pokePosicion->posicion).x=msg->posX;
	(pokePosicion->posicion).y=msg->posY;
	free(msg);
	if(pokemonEsObjetivo(pokePosicion->pokemon)){
		if(sizeListaMutex(entrenadoresLibres)>0){
			seleccionarEntrenador(pokePosicion);
		}else{
			pushColaMutex(pokemonesPendientes,(void*)pokePosicion);
		}
	}
}

void atenderLocalized(paquete* paquete){
	mensajeLocalized* msg=deserializarLocalized(paquete->stream);
	if(!especieFueLocalizada(msg->pokemon)&& localizedMeInteresa(paquete)){
		uint32_t i;
		for(i=0;i<msg->cantidad;i++){
			posicion posActual= *((msg->arrayPosiciones)+i);
			mensajeAppearedTeam* msgAppeared=llenarMensajeAppearedTeam(msg->pokemon,posActual.x,posActual.y);
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
			list_add(entrenadorEncontrado->pokemones,(void*)(entrenadorEncontrado->pokemonAAtrapar->pokemon));
			registrarPokemonAtrapado(entrenadorEncontrado->pokemonAAtrapar->pokemon);
			replanificarEntrenador(entrenadorEncontrado);
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
//llenarPaquete( uint32_t modulo,uint32_t tipoMensaje, uint32_t sizeStream,void* stream)
void* suscribirseColasBroker(void* conf){

	t_config* config=(t_config*) conf;
	tiempoReconexion =config_get_int_value(config, "TIEMPO_RECONEXION");




	//uint32_t sizeStream=sizeof(uint32_t);

	mensajeSuscripcion* mensajeAppeared=malloc(sizeof(mensajeSuscripcion));
	mensajeSuscripcion * mensajeCaught=malloc(sizeof(mensajeSuscripcion));
	mensajeSuscripcion* mensajeLocalized=malloc(sizeof(mensajeSuscripcion));

	mensajeAppeared->cola  = APPEARED_POKEMON;

	mensajeCaught ->cola   = CAUGHT_POKEMON;

	mensajeLocalized->cola = LOCALIZED_POKEMON;

//	void* streamAppeared=serializarSuscripcion(&mensajeAppeared);
//	void* streamCaught=serializarSuscripcion(&mensajeCaught);
//	void* streamLocalized=serializarSuscripcion(&mensajeLocalized);
//
//
//
//	paquete* suscripcionAppeared  = llenarPaquete(TEAM,SUSCRIPCION,sizeStream, streamAppeared);
//	paquete* suscripcionCaught    = llenarPaquete(TEAM,SUSCRIPCION,sizeStream, streamCaught);
//	paquete* suscripcionLocalized = llenarPaquete(TEAM,SUSCRIPCION,sizeStream, streamLocalized);
	//suscribirseCola(NULL);



	pthread_t threadSuscripcionAppeared;
	pthread_create(&threadSuscripcionAppeared, NULL, suscribirseCola, (void*)(mensajeAppeared));

	pthread_detach(threadSuscripcionAppeared);

	pthread_t threadSuscripcionAppeared1;
	//sleep(4);
	pthread_create(&threadSuscripcionAppeared1, NULL, suscribirseCola, (void*)(mensajeCaught));

	pthread_detach(threadSuscripcionAppeared1);
//	pthread_t threadSuscripcionLocalized;
//	pthread_create(&threadSuscripcionLocalized, NULL, suscribirseCola,(void*) (suscripcionLocalized));
//	pthread_detach(threadSuscripcionLocalized);
	//sleep(4);
	pthread_t threadSuscripcionCaught;
	pthread_create(&threadSuscripcionCaught, NULL, suscribirseCola, (void*)(mensajeCaught));
	pthread_detach(threadSuscripcionCaught);

	while(1);

//	free(suscripcionAppeared);
//	free(suscripcionLocalized);
//	free(suscripcionCaught);
	free(conf);
	return NULL;
}



void* suscribirseCola(void* msgSuscripcion){
	mensajeSuscripcion* msg=(mensajeSuscripcion*)msgSuscripcion;
	uint32_t sizeStream=sizeof(uint32_t);
	void* streamMsgSuscripcion=serializarSuscripcion(msg);
	//free(msg);
	paquete* paq=llenarPaquete(TEAM,SUSCRIPCION,sizeStream, streamMsgSuscripcion);

	struct sockaddr_in direccionServidor;
		direccionServidor.sin_family      = AF_INET;
		direccionServidor.sin_addr.s_addr = inet_addr(ipBroker);
		direccionServidor.sin_port        = htons(puertoBroker);

		uint32_t cliente=socket(AF_INET,SOCK_STREAM,0);
		printf("cliente: %d\n",cliente);
		while(connect(cliente,(void*) &direccionServidor,sizeof(direccionServidor))<0){
			printf("Conexión fallida con el Broker reintentando en %i segundos...\n",tiempoReconexion);
			sleep(tiempoReconexion);
		}



	printf("Comienzo suscripcion\n");
	uint32_t bytes = sizeof(uint32_t)*5+paq->sizeStream;

	void* stream   = serializarPaquete(paq);


	send(cliente,stream,bytes,0);

	//free(stream);

	//destruirPaquete(paq);

	uint32_t respuesta = -1;
	printf("Espero respuesta\n");
	recv(cliente,&respuesta,sizeof(uint32_t),0);

		if(respuesta == CORRECTO){
			printf("Mensaje recibido correctamente\n");

		}else{
			printf("Mensaje recibido incorrectamente\n");
			printf("mensaje: %i\n", respuesta);

		}
	while(1){
		paquete* paqueteRespuesta=recibirPaquete(cliente);
		switch(paqueteRespuesta->tipoMensaje){
			case APPEARED_POKEMON:;
				mensajeAppearedTeam* msgAppeared=deserializarAppearedTeam(paqueteRespuesta->stream);
				//destruirPaquete(paqueteRespuesta);
				atenderAppeared(msgAppeared);
				//free(msgAppeared);
				break;
			case LOCALIZED_POKEMON:
				//destruirPaquete(paqueteRespuesta);//recordar destruir el paquete
				break;
			case CAUGHT_POKEMON:
				atenderCaught(paqueteRespuesta);
				//destruirPaquete(paqueteRespuesta);
				break;
			default: break; //esto no puede pasar
		}
	}


		return NULL;
}




//TODO: hacer funciones de llenarMensajeX en la lib
//llenarPaquete( uint32_t modulo,uint32_t tipoMensaje, uint32_t sizeStream,void* stream)
void enviarCatch(dataEntrenador* infoEntrenador){
	uint32_t cliente=crearSocketCliente(ipBroker,puertoBroker);
	mensajeCatchBroker* msgCatch=malloc(sizeof(mensajeCatchBroker));
	msgCatch->pokemon=infoEntrenador->pokemonAAtrapar->pokemon;
	msgCatch->posX=(infoEntrenador->pokemonAAtrapar->posicion).x;
	msgCatch->posY=(infoEntrenador->pokemonAAtrapar->posicion).y;
	msgCatch->sizePokemon=strlen(msgCatch->pokemon)+1;
	void* streamMsg=serializarCatchBroker(msgCatch);
	paquete* paq=llenarPaquete(TEAM,CATCH_POKEMON,   sizeArgumentos(CATCH_POKEMON,msgCatch->pokemon,BROKER)  , streamMsg);
	void* paqueteSerializado=serializarPaquete(paq);
	free(msgCatch);
	//destruirPaquete(paq);
	send(cliente,paqueteSerializado, sizePaquete(paq), 0);
	free(paqueteSerializado);

	uint32_t idMensaje=0;

	if(recv(cliente, &idMensaje, sizeof(uint32_t),0)==-1){
		printf("Ocurrio un error al recibir la respuesta de un catch\n");
	}
	if(idMensaje>0){


	idsEntrenadorMensaje* parDeIds=malloc(sizeof(idsEntrenadorMensaje));
	parDeIds->idEntrenador=infoEntrenador->id;
	parDeIds->idMensaje=idMensaje;
	addListaMutex(listaIdsEntrenadorMensaje,(void*)parDeIds);

	}else{
		//se recibio erroneamente
	}
	close(cliente);
}

void* enviarGet(void* arg){
	char* pokemon=(char*) arg;
	uint32_t cliente=crearSocketCliente(ipBroker,puertoBroker);
	if(cliente!=-1){//se pudo conectar
	mensajeGetBroker* msg=llenarMensajeGetBroker(pokemon);
	void* stream=serializarGetBroker(msg);
	paquete* paq=llenarPaquete(TEAM, GET_POKEMON,sizeArgumentos(GET_POKEMON,msg->pokemon,BROKER),stream);
	void* paqueteSerializado=serializarPaquete(paq);
	// hacer destroy para el msg
	send(cliente,paqueteSerializado, sizePaquete(paq), 0);
	free(paqueteSerializado);
	uint32_t idMensaje=0;

	if(recv(cliente, &idMensaje, sizeof(uint32_t),0)==-1){
			printf("Ocurrio un error al recibir la respuesta de un get\n");
	}
	if(idMensaje>0){
		addListaMutex(listaIdsRespuestasGet,(void*)(&idMensaje));

	}else{
			//se recibio erroneamente
	}}

	close(cliente);
	return NULL;

}

dataTeam* inicializarTeam(t_config* config){

	dataTeam* dataTeam       = malloc(sizeof(dataTeam));
	dataTeam->entrenadores   = list_create();
	t_list* especiesObjetivo = list_create();
	dataTeam->objetivoGlobal=inicializarListaMutex();
	dataTeam->objetivosCumplidos = list_create();

	char** arrayPosicionesEntrenadores=config_get_array_value(config,"POSICIONES_ENTRENADORES");
	char** arrayPokemonesEntrenadores=config_get_array_value(config,"POKEMON_ENTRENADORES");
	char** arrayObjetivosEntrenadores=config_get_array_value(config,"OBJETIVOS_ENTRENADORES");

	t_list* posicionesEntrenadores = obtenerListaDeListas(arrayPosicionesEntrenadores);
	t_list* pokemonesEntrenadores  = obtenerListaDeListas(arrayPokemonesEntrenadores);
	t_list* objetivosEntrenadores  = obtenerListaDeListas(arrayObjetivosEntrenadores);
	uint32_t cantEntrenadores      = list_size(posicionesEntrenadores);

	uint32_t id;

	for(id=0;id<cantEntrenadores;id++){
		dataEntrenador* dataEntrenador=malloc(sizeof(dataEntrenador));
		char** pos		 = list_get(posicionesEntrenadores,id);
		char** pokemones = list_get(pokemonesEntrenadores,id);
		char** objetivos = list_get(objetivosEntrenadores,id);

		(dataEntrenador->posicion).x = atoi(pos[0]);
		(dataEntrenador->posicion).y = atoi(pos[1]);

		dataEntrenador->pokemones        = arrayStringALista(pokemones);
		dataEntrenador->objetivoPersonal = arrayStringALista(objetivos);

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

		dataEntrenador->estado			= NEW;
		dataEntrenador->id				= id;
		dataEntrenador->pokemonAAtrapar = NULL;
		sem_init(&(dataEntrenador->semaforo), 0,0);
		list_add(entrenadoresLibres->lista,(void*)dataEntrenador);
		list_add(dataTeam->entrenadores,dataEntrenador);


	}

	dataTeam->objetivoGlobal = obtenerObjetivos(especiesObjetivo);
	return dataTeam;

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
			objetivo* objetivo = malloc(sizeof(objetivo));
			objetivo->cantidad = 1;
			objetivo->pokemon  = (char*) especie;
			//printf("%s\n",(char*)(objetivo->pokemon));
			addListaMutex(objetivos,(void*)objetivo);
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
		}
	}
	return -1;
}


void registrarPokemonAtrapado(char* pokemon){
	uint32_t pos=buscarObjetivoPorEspecie(team->objetivoGlobal,pokemon);
	if(pos!=-1){
		(((objetivo*)getListaMutex(team->objetivoGlobal,pos))->cantidad)--;
	}else
		printf("No se pudo registrar el pokemon atrapado ya que no está en los objetivos del team\n");
}

bool pokemonEsObjetivo(char* pokemon){
	uint32_t pos=buscarObjetivoPorEspecie(team->objetivoGlobal,pokemon);
	if(pos!=-1){
			return((((objetivo*)getListaMutex(team->objetivoGlobal,pos))->cantidad)>0);
		}
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


