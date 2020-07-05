/*
 * gamecard.c
 *
 *  Created on: 12 may. 2020
 *      Author: juancito
 */



#include "gamecard.h"

uint32_t puertoBrokerGC;
char* ipBrokerGC;
uint32_t tiempoReconexionGC;
uint32_t tiempoRetardoGC;
uint32_t puertoGamecardGC = 5001;


int main(void) {
	gamecardLogger2=log_create("gamecardLoggerSecundario.log","gamecard", true, LOG_LEVEL_INFO);
	t_config * configGamecard = config_create("Gamecard.config");
	puertoBrokerGC = config_get_int_value(configGamecard, "PUERTO_BROKER");//5002;
	ipBrokerGC = config_get_string_value(configGamecard, "IP_BROKER");//"127.0.0.1"; //
	tiempoRetardoGC = config_get_int_value(configGamecard, "TIEMPO_DE_RETARDO_OPERACION"); //
	puntoMontaje = config_get_string_value(configGamecard, "PUNTO_MONTAJE_TALLGRASS");
	log_info(gamecardLogger2,"Puerto Broker:%i",puertoBrokerGC);

	//iniciarFileSystem(); //"/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/TALL_GRASS"; //
	iniciarMetadata();
	iniciarBitmap();
	//crearDirectorio("TALL_GRASS", "/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/", DIRECTORIO);
	//crearDirectorio("Metadata", "/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/TALL_GRASS/", DIRECTORIO);
	crearDirectorio("Files","/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/TALL_GRASS/", DIRECTORIO);
	crearDirectorio("Blocks","/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/TALL_GRASS/", DIRECTORIO);
	crearDirectorio("Pikachu",pathFiles,ARCHIVO);
	inicializarListaBloques();
	escribirBloque(1,0,strlen("Hola")+1,"Hola");
	escribirBloque(1,4,strlen("Hola")+1,"Hola");
	//escribirBloque2(1,"Hola");
	//printf("El resultado fue: %i\n",resul);
//	blockHeader* bloque= malloc(sizeof(blockHeader));
//	bloque->id=10;
//	agregarBloque(var,bloque);
//	blockHeader* bloque2= malloc(sizeof(blockHeader));
//		bloque2->id=1023214234;
//	agregarBloque(var,bloque2);
//	blockHeader* bloque3= malloc(sizeof(blockHeader));
//		bloque3->id=102384724;
//	agregarBloque(var,bloque3);
//	blockHeader* bloque4= malloc(sizeof(blockHeader));
//		bloque4->id=103345232;
//	agregarBloque(var,bloque4);
//
//	removerBloque(var,1038423);

	suscribirseColasBroker(configGamecard);
	pthread_t hiloServidorDeEscucha;
	crearHiloServidorGameboy(&hiloServidorDeEscucha);
	return EXIT_SUCCESS;
}


void* suscribirseColasBroker(void* config) {

	t_config* confi = (t_config*) config;
	tiempoReconexionGC = config_get_int_value(confi,"TIEMPO_DE_REINTENTO_CONEXION");

	mensajeSuscripcion* mensajeNew = malloc(sizeof(mensajeSuscripcion));
	mensajeSuscripcion * mensajeCatch = malloc(sizeof(mensajeSuscripcion));
	mensajeSuscripcion* mensajeGet = malloc(sizeof(mensajeSuscripcion));

	mensajeNew->cola = NEW_POKEMON;

	mensajeCatch->cola = CATCH_POKEMON;

	mensajeGet->cola = GET_POKEMON;

	pthread_t threadSuscripcionNew;
	pthread_create(&threadSuscripcionNew, NULL, suscribirseCola,(void*) mensajeNew);
	pthread_detach(threadSuscripcionNew);

	pthread_t threadSuscripcionCatch;
	pthread_create(&threadSuscripcionCatch, NULL, suscribirseCola,(void*) mensajeCatch);
	pthread_detach(threadSuscripcionCatch);

	pthread_t threadSuscripcionGet;
	pthread_create(&threadSuscripcionGet, NULL, suscribirseCola,(void*) mensajeGet);
	pthread_detach(threadSuscripcionGet);

	while (1);
	free(confi);
	return NULL;
}

void* suscribirseCola(void* msgSuscripcion) {
	mensajeSuscripcion* msg = (mensajeSuscripcion*) msgSuscripcion;
	uint32_t sizeStream = sizeof(uint32_t);
	void* streamMsgSuscripcion = serializarSuscripcion(msg);
	printf("Voy a llenar el paquete\n");
	paquete* paq = llenarPaquete(GAMECARD, SUSCRIPCION, sizeStream,streamMsgSuscripcion);

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ipBrokerGC);
	direccionServidor.sin_port = htons(puertoBrokerGC);

	uint32_t cliente = socket(AF_INET, SOCK_STREAM, 0);
	printf("cliente: %d\n", cliente);
	while (connect(cliente, (void*) &direccionServidor,sizeof(direccionServidor)) < 0) {
		printf("Conexión fallida con el Broker reintentando en %i segundos...\n",tiempoReconexionGC);
		sleep(tiempoReconexionGC);
	}

	printf("Comienzo suscripcion a %i \n", paq->tipoMensaje);
	uint32_t bytes = sizeof(uint32_t) * 5 + paq->sizeStream;

	void* stream = serializarPaquete(paq);

	//free(streamMsgSuscripcion);

	send(cliente, stream, bytes, 0);

	//free(stream);

	uint32_t respuesta;
	printf("Espero respuesta\n");
	recv(cliente, &respuesta, sizeof(uint32_t), 0);

	if (respuesta == CORRECTO) {
		printf("Se suscribio correctamente\n");
		while (1) {

			paquete* paqueteRespuesta = recibirPaquete(cliente);

			while (enviarACK(cliente, GAMECARD, paqueteRespuesta->id) < 0) {

				printf("Conexión fallida con el Broker reintentando en %i segundos...\n",tiempoReconexionGC);
				sleep(tiempoReconexionGC);

			}

			switch (paqueteRespuesta->tipoMensaje) {
			case NEW_POKEMON:;
				pthread_t threadNew;
				pthread_create(&threadNew, NULL, atenderNew, (void*)(paqueteRespuesta));
				pthread_detach(threadNew);

				break;
			case GET_POKEMON:;
				pthread_t threadGet;
				pthread_create(&threadGet, NULL, atenderGet, (void*)(paqueteRespuesta));
				pthread_detach(threadGet);
				break;
			case CATCH_POKEMON:;
				pthread_t threadCatch;
				pthread_create(&threadCatch, NULL, atenderCatch, (void*)(paqueteRespuesta));
				pthread_detach(threadCatch);
				break;
			default:
				break;

			}

			while (send(cliente, (void*) (&respuesta), sizeof(uint32_t), 0) < 0) {
				sleep(tiempoReconexionGC);
				//reconectarseAlBroker(cliente, (void*) &direccionServidor,sizeof(direccionServidor));
			}
		}
	} else {
		printf("Mensaje recibido incorrectamente\n");
		printf("mensaje: %i\n", respuesta);

	}
	return NULL;
}

int crearHiloServidorGameboy(pthread_t* hilo) {
	uint32_t err = pthread_create(hilo, NULL, iniciarServidorGameboy, NULL);
	if (err != 0) {
		printf(
				"Hubo un problema en la creación del hilo para iniciar el servidor para el Gameboy \n");
		return err;
	}

	pthread_detach(*hilo);
	return 0;
}

void* iniciarServidorGameboy(void* arg) {
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(puertoGamecardGC);
	uint32_t servidor = socket(AF_INET, SOCK_STREAM, 0);

	if (bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor))
			!= 0) {
		perror("Falló el bind");

	} else {
		printf("Estoy escuchando\n");
		while (1)  						//para recibir n cantidad de conexiones
			esperar_cliente(servidor);
	}
	return NULL;
}

void esperar_cliente(uint32_t servidor) {

	listen(servidor, 100);
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion = sizeof(struct sockaddr_in);
	printf("Espero un nuevo cliente\n");
	uint32_t* socket_cliente = malloc(sizeof(uint32_t));
	*socket_cliente = accept(servidor, (void*) &dir_cliente, &tam_direccion);
	printf("Gestiono un nuevo cliente\n");
	pthread_t threadAtencionGameboy;
	pthread_create(&threadAtencionGameboy, NULL, atenderCliente,(void*) (socket_cliente));
	pthread_detach(threadAtencionGameboy);
}

void* atenderCliente(void* sock) {
	printf("atiendo cliente\n");
	uint32_t* socket = (uint32_t*) sock;
	printf("hola llegue\n");
	paquete* paquete = recibirPaquete(*socket);
	uint32_t respuesta = 0;
	if (paquete == NULL) {
		respuesta = INCORRECTO;
	} else {
		respuesta = CORRECTO;
	}

	send(*socket, (void*) (&respuesta), sizeof(uint32_t), 0);
	free(socket);

	printf("hice el send: %i\n", respuesta);
	printf("recibi: %i\n", paquete->sizeStream);
	switch (paquete->tipoMensaje) {
	case NEW_POKEMON:;
		atenderNew((void*)paquete);
		break;
	case GET_POKEMON:;

		atenderGet((void*)paquete);

		break;
	case CATCH_POKEMON:;
		atenderCatch((void*)paquete);
		break;
	default:
		printf("leyo cualquiera\n");
		break;
	}

	return NULL;
}

void* atenderNew(void* paq) {
	paquete* paqueteNew = (paquete*) paq;
	uint32_t idNew = paqueteNew->id;
	mensajeNew* msgNew = deserializarNew(paqueteNew->stream);
	printf("deserializado\n");

	pokemonEnPosicion* pokeEnPosicion = malloc(sizeof(pokemonEnPosicion));
	//addListaMutex(especiesLocalizadas,(void*)(msg->pokemon));
	pokeEnPosicion->pokemon = msgNew->pokemon;
	pokeEnPosicion->cantidad = msgNew->cantidad;
	pokeEnPosicion->id = idNew;
	(pokeEnPosicion->posicion).x = msgNew->posX;
	(pokeEnPosicion->posicion).y = msgNew->posY;

	//free(msg);
	//To do :
	//Verificar que el pokemon este en nuestro FileSystem
	//Una vez encontrado (o creado) verificar si puedo abrirlo
	//Verificar si las posiciones existen en el archivo
	//IF SUCCESS
	sleep(tiempoRetardoGC);
	//CERRAR ARCHIVO
	enviarAppeared(pokeEnPosicion);
	return NULL;

}

void enviarAppeared(pokemonEnPosicion* pokeEnPosicion) {
	uint32_t cliente = crearSocketCliente(ipBrokerGC, puertoBrokerGC);
	mensajeAppeared* msgAppeared = malloc(sizeof(mensajeAppeared));
	msgAppeared->pokemon = pokeEnPosicion->pokemon;
	msgAppeared->posX = (pokeEnPosicion->posicion).x;
	msgAppeared->posY = (pokeEnPosicion->posicion).y;
	//msgAppeared->idCorrelativo =pokeEnPosicion->id;
	msgAppeared->sizePokemon = strlen(msgAppeared->pokemon) + 1;
	void* streamMsg = serializarAppeared(msgAppeared);
	paquete* paq = llenarPaquete(GAMECARD, APPEARED_POKEMON,sizeArgumentos(APPEARED_POKEMON, msgAppeared->pokemon, BROKER),streamMsg);
	insertarIdCorrelativoPaquete(paq, (pokeEnPosicion->id));
	void* paqueteSerializado = serializarPaquete(paq);
	free(msgAppeared);
	//destruirPaquete(paq);
	send(cliente, paqueteSerializado, sizePaquete(paq), 0);
	free(paqueteSerializado);

}

void* atenderGet(void* paq) {
	paquete* paqueteGet = (paquete*) paq;
	mensajeGet* msgGet = deserializarGet(paqueteGet->stream);
	uint32_t idGet = paqueteGet->id;
	printf("deserializado\n");
	//destruirPaquete(paquete);

	pokemonADevolver* pokeADevolver = malloc(sizeof(pokemonADevolver));
	pokeADevolver->pokemon = msgGet->pokemon;
	pokeADevolver->id = idGet;
	//Todo :
	//Verificar que el pokemon este en nuestro FileSystem (si no encuentra mando posiciones vacias)
	//Una vez encontrado verificar si puedo abrirlo
	//Verificar si las posiciones existen en el archivo
	//Conseguir Posiciones
	//pokeADevolver->cantPosiciones = DEL FILESYSTEM
	//pokeADevolver->posicion= DEL FILESYSTEM
	//free(msg);
	//IF SUCCESS
	sleep(tiempoRetardoGC);
	//CERRAR ARCHIVO
	enviarLocalized(pokeADevolver);
	return NULL;
}

void enviarLocalized(pokemonADevolver* pokeADevolver) {
	uint32_t cliente = crearSocketCliente(ipBrokerGC, puertoBrokerGC);
	mensajeLocalized* msgLocalized = malloc(sizeof(mensajeLocalized));
	msgLocalized->pokemon = pokeADevolver->pokemon;
	msgLocalized->cantidad = pokeADevolver->cantPosiciones;
	msgLocalized->arrayPosiciones = pokeADevolver->posicion;
	msgLocalized->sizePokemon = strlen(msgLocalized->pokemon) + 1;
	void* streamMsg = serializarLocalized(msgLocalized);
	paquete* paq = llenarPaquete(GAMECARD, LOCALIZED_POKEMON,
			sizeArgumentos(LOCALIZED_POKEMON, msgLocalized->pokemon, BROKER),
			streamMsg);
	insertarIdCorrelativoPaquete(paq, (pokeADevolver->id));
	void* paqueteSerializado = serializarPaquete(paq);
	//free(msgLocalized);
	//destruirPaquete(paq);
	send(cliente, paqueteSerializado, sizePaquete(paq), 0);
	free(paqueteSerializado);
}

void* atenderCatch(void* paq) {
	paquete* paqueteCatch = (paquete*) paq;
	uint32_t idCatch = paqueteCatch->id;
	mensajeCatch* msgCatch = deserializarCatch(paqueteCatch->stream);
	printf("deserializado\n");
	pokemonAAtrapar* pokeAAtrapar = malloc(sizeof(pokemonAAtrapar));
	pokeAAtrapar->id = idCatch;
	pokeAAtrapar->pokemon = msgCatch->pokemon;
	pokeAAtrapar->posicion->x = msgCatch->posX;
	pokeAAtrapar->posicion->y = msgCatch->posY;

	//Todo :
	//Verificar que el pokemon este en nuestro FileSystem (si no encuentra mando ERROR)
	//Una vez encontrado verificar si puedo abrirlo
	//Verificar si las posiciones existen en el archivo (SI NO MANDO ERROR)
	//Si la cantidad del pokemon es 1 elimino la linea, si no la reduzco en 1
	//free(msg);
	//IF SUCCESS
	pokeAAtrapar->resultado = 1;
	//else pokeAAtrapar->resultado = 0;
	sleep(tiempoRetardoGC);
	//Cerramos

	enviarCaught(pokeAAtrapar); //Momentaneo hasta saber bien que hacer con fileSystem
	return NULL;
}

void enviarCaught(pokemonAAtrapar* pokeAAtrapar) {
	uint32_t cliente = crearSocketCliente(ipBrokerGC, puertoBrokerGC);
	mensajeCaught* msgCaught = malloc(sizeof(mensajeCaught));
	msgCaught->resultadoCaught = pokeAAtrapar->resultado;
	void* streamMsg = serializarCaught(msgCaught);
	paquete* paq = llenarPaquete(GAMECARD, CAUGHT_POKEMON,
	sizeArgumentos(CAUGHT_POKEMON, NULL, BROKER), streamMsg);
	insertarIdCorrelativoPaquete(paq, (pokeAAtrapar->id));
	void* paqueteSerializado = serializarPaquete(paq);
	//free(msgLocalized);
	//destruirPaquete(paq);
	send(cliente, paqueteSerializado, sizePaquete(paq), 0);
	free(paqueteSerializado);

}


