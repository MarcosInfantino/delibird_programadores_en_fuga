/*
 ============================================================================
 Name        : brokerSO.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "broker.h"
#include "memoria.h"
#include "files.h"

int main(void) {

    brokerLogger2 = log_create("brokerLoggerSecundario.log", "Broker", true, LOG_LEVEL_INFO);

    log_info(brokerLogger2, "pid : %i", getpid());
    //log_info(brokerLogger2, armarStringEnvioXsub(2));

	signal(SIGUSR1, crearDumpDeCache);

	//levantarDatosDeConfig("Broker.config", 1); 			//1 para datos de config, otro para hardcode
	//levantarDatosDeConfig("pruebaBaseBroker.config", 1);
	levantarDatosDeConfig("pruebaBS.config", 1);

	char* nombreLog   = "logBroker.log";
	char* programName = "BROKER";
	loggerBroker = iniciar_logger(nombreLog, programName);

	mutexMemoria = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutexMemoria,NULL);

	definirComienzoDeMemoria();

	//archivoSem = iniciarArchivoMutex();

	iniciarHilos();
	inicializarContador();
	abrirHiloParaEnviarMensajes();
	iniciarServidor();

	return EXIT_SUCCESS;
}

void levantarDatosDeConfig(char * pathConfig, uint32_t intMock){
	if (intMock == 1){
		t_config* configBroker = config_create(pathConfig);

		tamMemoria        = config_get_int_value(configBroker, "TAMANO_MEMORIA");
		particionMinima   = config_get_int_value(configBroker, "TAMANO_MINIMO_PARTICION");
		ip_broker         = config_get_string_value(configBroker, "IP_BROKER");
		puerto_broker     = config_get_int_value(configBroker, "PUERTO_BROKER");
		frecuenciaCompactacion = config_get_int_value (configBroker, "FRECUENCIA_COMPACTACION");

		definirAlgoritmoMemoria(configBroker);
		definirAlgoritmoParticionLibre(configBroker);
		definirAlgoritmoReemplazo(configBroker);
	}else{
		puerto_broker   = 5002;
		ip_broker       = "127.0.0.1";
		tamMemoria      = 2048;
		particionMinima = 32;
		algoritmoMemoria   = BUDDY_SYSTEM;
		algoritmoReemplazo = FIFO;
	}
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


	while (1)
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

	uint32_t* socketCliente = malloc(sizeof(uint32_t));

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
		free(socket);
		}else{
			manejarTipoDeMensaje(paquete, socket);
		}
	return NULL;
}

void manejarTipoDeMensaje(paquete* paq, uint32_t* socket) {

	suscripcionTiempo structTiempo;
	mensajeSuscripcionTiempo* datosSuscribir;

	switch(paq->tipoMensaje){
		 case APPEARED_POKEMON:
			 meterEnCola(&appearedPokemon, paq, *socket );
			 break;
		 case NEW_POKEMON:
			 meterEnCola( &newPokemon, paq, *socket);
			 break;
		 case CAUGHT_POKEMON:;
			 //mensajeCaught* caught = deserializarCaught(paq->stream);
			// log_info(brokerLogger2, "Id del caught: %i", caught->resultadoCaught);
			 meterEnCola( &caughtPokemon, paq, *socket);
			 break;
		 case CATCH_POKEMON:
			 //log_info(brokerLogger2,"Me llegó un catch. Código de mensaje: %i. ",paq->tipoMensaje);
			 meterEnCola( &catchPokemon, paq, *socket);
			 break;
		 case GET_POKEMON:
			 meterEnCola( &getPokemon, paq, *socket);
			 break;
		 case LOCALIZED_POKEMON:
			 meterEnCola( &localizedPokemon, paq, *socket);
			 break;
		 case SUSCRIPCION:
			 suscribirSegunCola(*paq, socket);
			 break;
		 case SUSCRIPCION_TIEMPO:
			 datosSuscribir = deserializarSuscripcionTiempo(paq->stream);
			 structTiempo.cola   = datosSuscribir->cola;
			 structTiempo.tiempo = datosSuscribir->tiempo;
			 structTiempo.paq    = *paq;
			 structTiempo.socket = socket;
			 suscribirPorTiempo((void*) &structTiempo);
			 break;
		 case ACK:
			 guardarEnListaMemoria(paq->idCorrelativo, *socket, CONFIRMADO);
			 break;
		 default:
			 pthread_exit(NULL);
	 }
}

void meterEnCola( colaMensajes* structCola, paquete * paq, uint32_t  socket){
	//log_info(loggerBroker, armarStringMsgNuevoLog(paq->tipoMensaje));
	//log_info(brokerLogger2, armarStringMsgNuevoLog(paq->tipoMensaje));

	incrementarContador();
	pthread_mutex_lock(contador.mutexContador);
	insertarIdPaquete(paq,contador.contador);
	send(socket,(void*)(&contador.contador),sizeof(uint32_t),0);
	pthread_mutex_unlock(contador.mutexContador);

	registrarMensajeEnMemoria(contador.contador, paq, algoritmoMemoria);

	pushColaMutex(structCola->cola, (void *) paq);

	sem_post(structCola->mensajesEnCola);
	log_info(brokerLogger2,"Aviso que hay mensajes en cola.");
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
		sem_wait(cola->mensajesEnCola);
		log_info(brokerLogger2,"Comienza el proceso de envío del mensaje a todos los suscriptores.");
		paquete* paq = (paquete*) popColaMutex(cola->cola);

		//if(paq->tipoMensaje == CAUGHT_POKEMON){
		//mensajeCaught* msg = deserializarCaught(paq->stream);}

		void * paqSerializado = serializarPaquete(paq);

		for(i = 0; i < sizeListaMutex(cola->suscriptores) ;i ++){
			uint32_t * socketActual = (uint32_t *) getListaMutex(cola->suscriptores, i);

			send(*socketActual, paqSerializado , sizePaquete(paq), 0);

			log_info(brokerLogger2, "Envié mensaje a suscriptor: %d -.-", *socketActual);
			log_info(loggerBroker, "Envié mensaje a suscriptor: %d -.-", *socketActual);

			guardarEnListaMemoria(paq->id, *socketActual, SUBSYAENVIADOS);
		}

		destruirPaquete(paq);
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
	contador.contador = 0;
	contador.mutexContador = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(contador.mutexContador,NULL);
}

void incrementarContador(){
	pthread_mutex_lock(contador.mutexContador);
	contador.contador = contador.contador + 1;
	pthread_mutex_unlock(contador.mutexContador);
}

uint32_t obtenerContador(){
	pthread_mutex_lock(contador.mutexContador);
	uint32_t i = contador.contador;
	pthread_mutex_unlock(contador.mutexContador);
	return i;
}

void definirAlgoritmoMemoria(t_config* config){
	algoritmoParameter parAlgoritmo;
	parAlgoritmo.config  = config;
	parAlgoritmo.configAtributo = "ALGORITMO_MEMORIA";
	parAlgoritmo.OPCION1 = "PARTICIONES";
	parAlgoritmo.OPCION2 = "BS";
	parAlgoritmo.OP1     = PARTICIONES_DINAMICAS;
	parAlgoritmo.OP2     = BUDDY_SYSTEM;
	parAlgoritmo.error   = "Hubo un error al definir el algoritmo de memoria";

	definirAlgoritmo(parAlgoritmo, &algoritmoMemoria);
}

void definirAlgoritmoParticionLibre(t_config* config){
	algoritmoParameter parAlgoritmo;
	parAlgoritmo.config = config;
	parAlgoritmo.configAtributo = "ALGORITMO_PARTICION_LIBRE";
	parAlgoritmo.OPCION1 = "FF";
	parAlgoritmo.OPCION2 = "BF";
	parAlgoritmo.OP1 = FIRST_FIT;
	parAlgoritmo.OP2 = BEST_FIT;
	parAlgoritmo.error = "Hubo un error al definir el algoritmo de particiones de memoria libres";

	definirAlgoritmo(parAlgoritmo, &algoritmoParticionLibre);
}

void definirAlgoritmoReemplazo(t_config* config){
	algoritmoParameter parAlgoritmo;
	parAlgoritmo.config = config;
	parAlgoritmo.configAtributo = "ALGORITMO_REEMPLAZO";
	parAlgoritmo.OPCION1 = "FIFO";
	parAlgoritmo.OPCION2 = "LRU";
	parAlgoritmo.OP1 = FIFO;
	parAlgoritmo.OP2 = LRU;
	parAlgoritmo.error = "Hubo un error al definir el algoritmo de particiones de memoria libres";

	definirAlgoritmo(parAlgoritmo, &algoritmoReemplazo);
}
void definirAlgoritmo(algoritmoParameter parAlgoritmo, uint32_t * varInt){
	char* algoritmo = config_get_string_value(parAlgoritmo.config, parAlgoritmo.configAtributo);
		if( strcmp(algoritmo, parAlgoritmo.OPCION1) == 0){
			*varInt = parAlgoritmo.OP1;
		}else if(strcmp(algoritmo, parAlgoritmo.OPCION2) == 0){
			*varInt = parAlgoritmo.OP2;
		}else{
			printf("%s", parAlgoritmo.error);
		}
}

void definirComienzoDeMemoria(){
	memoria = malloc(tamMemoria);
	switch(algoritmoMemoria){
	case BUDDY_SYSTEM:
		nodoRaizMemoria = crearRaizArbol();
		nodoRaizMemoria->offset = 0;
		nodosOcupados = inicializarListaMutex();
		break;

	case PARTICIONES_DINAMICAS:
		particionesOcupadas = inicializarListaMutex();
		particionesLibres = inicializarListaMutex();
		break;
	}
}
