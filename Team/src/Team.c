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




//t_list* idsHilos=list_create();//son ints
//t_list* hilos=list_create();//son pthread_t
uint32_t puertoTeam=5003;
listaMutex listaIdsEntrenadorMensaje; // del tipo idsEntrenadorMensaje , //ver a futuro si esta lista requiere mutex
listaMutex entrenadoresLibres;
colaMutex pokemonesPendientes;//lista de pokePosicion que contiene los pokemones que no pudieron ser asignados a ningun entrenador por no haber entrenadore libres
listaMutex entrenadoresExit;
//t_list* mutexEntrenadores;
listaMutex entrenadores;
colaMutex colaEjecucionFifo;

pokemonPosicion pokemonAAtrapar;

pthread_mutex_t mutexEntrenadorEnEjecucion=PTHREAD_MUTEX_INITIALIZER;
int socketGameboy;
int socketGamecard;
uint32_t puertoBroker;
char* ipBroker;
pthread_t* arrayIdHilosEntrenadores;
uint32_t tiempoReconexion;
uint32_t retardoCicloCpu;
char* algoritmoPlanificacion;

sem_t semaforoEjecucionCpu;
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
int main(int argc , char* argv[]){
	//char pathConfig  = argv;
	sem_init(&semaforoEjecucionCpu, 0,0);
	entrenadoresLibres=inicializarListaMutex();
	colaEjecucionFifo=inicializarColaMutex();
	pokemonesPendientes=inicializarColaMutex();
	listaIdsEntrenadorMensaje=inicializarListaMutex();
	entrenadores=inicializarListaMutex();
	entrenadoresExit=inicializarListaMutex();

	char* pathConfig   = "Team2.config";
	t_config* config   = config_create(pathConfig);
	retardoCicloCpu    = config_get_int_value(config,"RETARDO_CICLO_CPU");
	puertoBroker       = config_get_int_value(config,"PUERTO_BROKER");
	ipBroker           = config_get_string_value(config,"IP_BROKER");
	algoritmoPlanificacion=config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	dataTeam* team     = inicializarTeam(config);

	entrenadores.lista       = team->entrenadores;
	//entrenadoresLibres=entrenadores;

	//mutexEntrenadores=inicializarMutexEntrenadores();
	uint32_t cantEntrenadores = list_size(team->entrenadores);

	arrayIdHilosEntrenadores  = malloc(cantEntrenadores*sizeof(pthread_t));
	inicializarEntrenadores(team->entrenadores);

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

uint32_t crearHiloPlanificador(pthread_t* hiloPlanificador){
	uint32_t err=pthread_create(hiloPlanificador,NULL,iniciarPlanificador,NULL);

						if(err!=0){
							printf("Hubo un problema en la creación del hilo para el planificador \n");
							return err;
						}

			pthread_detach(*hiloPlanificador);
			return 0;
}

void* iniciarPlanificador(void* arg){
	if(strcmp(algoritmoPlanificacion,"FIFO")==0)
		ejecucionPlanificadorFifo();
	return NULL;
}

void ejecucionPlanificadorFifo(){
	sem_post(&semaforoEjecucionCpu);
	while(1){
		if(sizeColaMutex(colaEjecucionFifo)>0){
			sem_wait(&semaforoEjecucionCpu);
			dataEntrenador* entrenadorAEjecutar=(dataEntrenador*)popColaMutex(colaEjecucionFifo);
			sem_post(&(entrenadorAEjecutar->semaforo));

		}
	}
}

bool objetivoCumplido(){
	return sizeListaMutex(entrenadores)==sizeListaMutex(entrenadoresExit);
}


uint32_t distanciaEntrePosiciones(posicion pos1, posicion pos2){

	return abs((pos1.x)-(pos2.x))+abs((pos1.y)-(pos2.y));
}

uint32_t distanciaEntrenadorPosicion(dataEntrenador* entrenador, posicion posicion){

	return distanciaEntrePosiciones(entrenador->posicion,posicion);
}

uint32_t obtenerIdEntrenadorMasCercano(posicion pos){ //el id es el index del entrenador enla lista de entrenadores
    dataEntrenador* entrenadorMasCercano;
    uint32_t distanciaASuperar;
    pthread_mutex_lock(entrenadoresLibres.mutex);
    for(uint32_t i=0;i<list_size(entrenadoresLibres.lista);i++){
        dataEntrenador* entrenadorActual = (dataEntrenador*) list_get(entrenadoresLibres.lista,i);
        if (i == 0){
            entrenadorMasCercano = entrenadorActual;
            distanciaASuperar    = distanciaEntrenadorPosicion(entrenadorActual,pos);
        } else {
        if(distanciaEntrenadorPosicion(entrenadorActual,pos) < distanciaASuperar){
            entrenadorMasCercano = entrenadorActual;
            distanciaASuperar    = distanciaEntrenadorPosicion(entrenadorActual,pos);
        }
    }

}
    pthread_mutex_unlock(entrenadoresLibres.mutex);
    return idEntrenadorEnLista(entrenadorMasCercano);}

dataEntrenador* obtenerEntrenadorPorId(uint32_t id){
	return (dataEntrenador*)getListaMutex(entrenadores,id);
}

uint32_t idEntrenadorEnLista(dataEntrenador* entrenadorMasCercano){
    pthread_mutex_lock(entrenadoresLibres.mutex);
	for(uint32_t j=0;j<sizeListaMutex(entrenadores);j++){
        if((dataEntrenador*) getListaMutex(entrenadores,j) == entrenadorMasCercano){
            pthread_mutex_unlock(entrenadoresLibres.mutex);
        	return j;
        }
    }
	pthread_mutex_unlock(entrenadoresLibres.mutex);
    return -1;
}

int crearHiloServidorGameboy(pthread_t* hilo){
	uint32_t err=pthread_create(hilo,NULL,iniciarServidorGameboy,NULL);
					if(err!=0){
						printf("Hubo un problema en la creación del hilo para iniciar el servidor para el Gameboy \n");
						return err;
					}

		pthread_detach(*hilo);
	return 0;
}

void* iniciarServidorGameboy(void* arg){
		struct sockaddr_in direccionServidor;
		direccionServidor.sin_family=AF_INET;
		direccionServidor.sin_addr.s_addr=INADDR_ANY;
		direccionServidor.sin_port=htons(puertoTeam);

		uint32_t servidor=socket(AF_INET,SOCK_STREAM,0);

//		int activado=1;
//		setsockopt(servidor,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));

		if(bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor))!=0){
			perror("Falló el bind");

		}else{
		printf("Estoy escuchando\n");
		while (1)  								//para recibir n cantidad de conexiones
				esperar_cliente(servidor);
		}



	return NULL;
}

void esperar_cliente(uint32_t servidor) {

	listen(servidor, 100);
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion = sizeof(struct sockaddr_in);
	printf("Espero un nuevo cliente\n");
	uint32_t* socket_cliente=malloc(sizeof(uint32_t));
	*socket_cliente = accept(servidor, (void*) &dir_cliente, &tam_direccion);
	printf("Gestiono un nuevo cliente\n");
	pthread_t threadAtencionGameboy;
	pthread_create(&threadAtencionGameboy, NULL, atenderCliente, (void*) (socket_cliente));
	pthread_detach(threadAtencionGameboy);
}

void* atenderCliente(void* sock){
	printf("atiendo cliente\n");
	uint32_t* socket = (uint32_t*) sock;
	paquete* paquete=recibirPaquete(*socket);
	uint32_t respuesta=0;
	if(paquete==NULL){
		respuesta=INCORRECTO;
	}else{
		respuesta=CORRECTO;
	}

	send(*socket,(void*)(&respuesta),sizeof(uint32_t),0);

	printf("hice el send: %i\n",respuesta);

	switch(paquete->tipoMensaje){
		case APPEARED_POKEMON:;
			mensajeAppearedTeam* msg=deserializarAppearedTeam(paquete->stream);
			//destruirPaquete(paquete);
			printf("leyo bien\n");atenderAppeared(msg); ;break;
		default: printf("leyo cualquiera\n"); break;
	}

	free(socket);

	return NULL;
}

void atenderAppeared(mensajeAppearedTeam* msg){
	pokemonPosicion* pokePosicion=malloc(sizeof(pokemonPosicion));
	pokePosicion->pokemon=msg->pokemon;
	(pokePosicion->posicion).x=msg->posX;
	(pokePosicion->posicion).y=msg->posY;
	free(msg);
	if(sizeListaMutex(entrenadoresLibres)>0){
			seleccionarEntrenador(pokePosicion);
		}else{
			pushColaMutex(pokemonesPendientes,(void*)pokePosicion);
		}

}

void atenderCaught(paquete* paqueteCaught){
	mensajeCaught* msgCaught=deserializarCaught(paqueteCaught->stream);
	uint32_t id=paqueteCaught->idCorrelativo;
	uint32_t idEncontrado=buscarEntrenadorParaMensaje(listaIdsEntrenadorMensaje,id)!=1;
	if(idEncontrado!=-1){
		dataEntrenador* entrenadorEncontrado=(dataEntrenador*)getListaMutex(entrenadores, idEncontrado);
		if(msgCaught->resultadoCaught==CORRECTO){
			list_add(entrenadorEncontrado->pokemones,entrenadorEncontrado->pokemonAAtrapar);
			//falta disminuir el objetivo global del team
			replanificarEntrenador(entrenadorEncontrado);
		}else{
//			entrenadorEncontrado->estado=BLOCKED;
//			habilitarHiloEntrenador(idEncontrado);
			replanificarEntrenador(entrenadorEncontrado);
		}
	}


}

bool leFaltaCantidadDePokemones(dataEntrenador* entrenador){
	return !(list_size(entrenador->pokemones)==list_size(entrenador->objetivoPersonal));
}

void replanificarEntrenador(dataEntrenador* entrenador){
	if(leFaltaCantidadDePokemones(entrenador)){
		if(sizeColaMutex(pokemonesPendientes)>0){
			habilitarHiloEntrenador(entrenador->id);
			pokemonPosicion* pokePosicion=(pokemonPosicion*)popColaMutex(pokemonesPendientes);
			asignarPokemonAEntrenador(entrenador, pokePosicion);
			free(pokePosicion);
		}else{
			entrenador->estado=BLOCKED;
			habilitarHiloEntrenador(entrenador->id);
			//addListaMutex(entrenadoresLibres,entrenador);

		}
	}else{
			if(cumplioObjetivo(entrenador)){
							entrenador->estado=EXIT;
							habilitarHiloEntrenador(entrenador->id);//preguntar si aca se mata el hilo
							addListaMutex(entrenadoresExit, (void*)entrenador);

						}else{
							//DEADLOCK
						}
	}
}

bool cumplioObjetivo(dataEntrenador* entrenador){
	uint32_t i;
	for(i=0;i<list_size(entrenador->objetivoPersonal);i++){
		char* pokemon=list_get(entrenador->objetivoPersonal,i);
		if(buscarMismoPokemon(entrenador->pokemones,pokemon)==-1)
			return false;
	}
	return true;
}

void asignarPokemonAEntrenador(dataEntrenador* entrenador, pokemonPosicion* pokePosicion){
	if(entrenador->pokemonAAtrapar!=NULL){
		free(entrenador->pokemonAAtrapar);
	}
	entrenador->pokemonAAtrapar=pokePosicion;
	entrenador->estado=READY;
	habilitarHiloEntrenador(entrenador->id);
}

uint32_t buscarEntrenadorParaMensaje(listaMutex listaIds, uint32_t idMensaje){//devuelve el id del entrenador
	uint32_t i;
	pthread_mutex_lock(listaIds.mutex);
	for(i=0;i<list_size(listaIds.lista);i++){
		idsEntrenadorMensaje* actual=(void*)list_get(listaIds.lista,i);
		if(actual->idMensaje==idMensaje){
			uint32_t idEntrenador=actual->idEntrenador;
			free(actual);
			list_remove(listaIds.lista,i);
			return idEntrenador;
		}
	}
	pthread_mutex_unlock(listaIds.mutex);
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



int inicializarEntrenadores(t_list* entrenadores){
	uint32_t i;

	for(i=0;i<list_size(entrenadores);i++){
		void* entrenadorActual = list_get(entrenadores,i);
		uint32_t err		   = pthread_create(&(arrayIdHilosEntrenadores[i]),NULL,ejecucionHiloEntrenador,entrenadorActual);
		if(err!=0){
			printf("Hubo un problema en la creación del hilo del entrenador \n");
			return err;
		}

//		}else{
//			printf("Todo bien\n");
//		}
		pthread_detach(arrayIdHilosEntrenadores[i]);
	}
	return 0;
}


void* ejecucionHiloEntrenador(void* argEntrenador){
	dataEntrenador* infoEntrenador=(dataEntrenador*) argEntrenador;
	sem_t* semaforoEntrenador=&(infoEntrenador->semaforo);
	while(1){
		sem_wait(semaforoEntrenador);
		//infoEntrenador->estado=READY;
		pushColaMutex(colaEjecucionFifo,(void*)infoEntrenador);
		removeListaMutex(entrenadoresLibres,encontrarPosicionEntrenadorLibre(infoEntrenador));
		entrarEnEjecucion(infoEntrenador);
		//despues de esto enviaria el catch, recibe id y se pone en BLOCKED

		infoEntrenador->estado=BLOCKED;//IMPORTANTE: CUANDO LLEGUE LA RESPUESTA DEL CATCH SE TIENE QUE HACER UN UNLOCK AL ENTRENADOR CORRESPONDIENTE
		sem_wait(semaforoEntrenador);// ESPERA A QUE EL TEAM LE AVISE QUE LLEGO LA RESPUESTA DEL POKEMON QUE QUISO ATRAPAR
		//meter un if() para verificar estado y ver que hacer despues
		if(infoEntrenador->estado==BLOCKED){
			addListaMutex(entrenadoresLibres, (void*)infoEntrenador);//vuelve a agregar al entrenador a la lista de entrenadores libres
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
	msgCatch->sizePokemon=strlen(msgCatch->pokemon);
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


uint32_t encontrarPosicionEntrenadorLibre(dataEntrenador* entrenador){
	pthread_mutex_lock(entrenadoresLibres.mutex);
	for(uint32_t i=0;i<list_size(entrenadoresLibres.lista);i++){
	        dataEntrenador* entrenadorActual = (dataEntrenador*) list_get(entrenadoresLibres.lista,i);
	        if(entrenadorActual->id == entrenador->id){
	        	pthread_mutex_unlock(entrenadoresLibres.mutex);
	        	return i;
	        }
	        }
	 pthread_mutex_unlock(entrenadoresLibres.mutex);
	 return -1;
}

void entrarEnEjecucion(dataEntrenador* infoEntrenador){

	sem_wait(&(infoEntrenador->semaforo));
	infoEntrenador->estado = EXEC;
	moverEntrenadorAPosicion(infoEntrenador, (pokemonAAtrapar.posicion));
	enviarCatch(infoEntrenador);
	sem_post(&semaforoEjecucionCpu);
}

void seleccionarEntrenador(pokemonPosicion* pokemon){
	uint32_t idEntrenadorMasCercano      = obtenerIdEntrenadorMasCercano(pokemon->posicion);
	dataEntrenador* entrenadorMasCercano = getListaMutex(entrenadores,idEntrenadorMasCercano);
	asignarPokemonAEntrenador(entrenadorMasCercano,pokemon);
	//habilitarHiloEntrenador(idEntrenadorMasCercano);
}

void habilitarHiloEntrenador(uint32_t idEntrenador){
	sem_post(&(((dataEntrenador*)(getListaMutex(entrenadores,idEntrenador)))->semaforo));
}


void moverEntrenadorAPosicion(dataEntrenador* entrenador, posicion pos){
	uint32_t restaX=pos.x-(entrenador->posicion).x;
	uint32_t restaY=pos.y-(entrenador->posicion).y;
	moverEntrenadorX(entrenador, restaX);
	moverEntrenadorY(entrenador, restaY);
}

void moverEntrenadorX(dataEntrenador* entrenador, uint32_t movimientoX){

	if(movimientoX!=0){
			uint32_t unidad=movimientoX/abs(movimientoX);
			for(uint32_t i=0;i< abs(movimientoX);i++){
			sleep(retardoCicloCpu);
				(entrenador->posicion).x+=unidad;
			}
		}
}

void moverEntrenadorY(dataEntrenador* entrenador, uint32_t movimientoY){
	if(movimientoY!=0){
		uint32_t unidad=movimientoY/abs(movimientoY);
		for(uint32_t i=0;i< abs(movimientoY);i++){
		sleep(retardoCicloCpu);
			(entrenador->posicion).y+=unidad;
		}
	}
}

void moverEntrenador(dataEntrenador* entrenador, uint32_t movimientoX, uint32_t movimientoY){
	posicion posAnterior     = entrenador->posicion;
	(entrenador->posicion).x = posAnterior.x+movimientoX;
	(entrenador->posicion).y = posAnterior.y+movimientoY;
}

dataTeam* inicializarTeam(t_config* config){

	dataTeam* dataTeam       = malloc(sizeof(dataTeam));
	dataTeam->entrenadores   = list_create();
	t_list* especiesObjetivo = list_create();

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
		list_add(entrenadoresLibres.lista,(void*)dataEntrenador);
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

t_list* obtenerObjetivos(t_list* especies){
	t_list* objetivos = list_create();


	for(uint32_t i = 0;i<list_size(especies);i++){
		char* especie = (char*)list_get(especies,i);

		//especieAComparar=especie;

		uint32_t encontrado = buscarObjetivoPorEspecie(objetivos,especie);
		if(encontrado == -1){
			objetivo* objetivo = malloc(sizeof(objetivo));
			objetivo->cantidad = 1;
			objetivo->pokemon  = (char*) especie;
			//printf("%s\n",(char*)(objetivo->pokemon));
			list_add(objetivos,(void*)objetivo);
		}else{
			(((objetivo*)list_get(objetivos,encontrado))->cantidad)++;
		}
	}
	return objetivos;

}

uint32_t buscarObjetivoPorEspecie(t_list* listaObjetivos, char* especie){
	uint32_t i;
	for(i=0;i<list_size(listaObjetivos);i++){
		objetivo* obj = (objetivo*)list_get(listaObjetivos,i);
		if(strcmp(obj->pokemon,especie) == 0){
			return i;
		}
	}
	return -1;
}

//bool objetivoMismaEspecie(void* obj){
//	objetivo* objAux=(objetivo*) obj;
//	return strcmp(objAux->pokemon,(char*)especieAComparar)==0;
//}


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


