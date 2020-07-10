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

uint32_t puertoGamecardGC = 5001;

//int main (void){
//	gamecardLogger2=log_create("gamecardLoggerSecundario.log","gamecard", true, LOG_LEVEL_INFO);
//
//	posicionCantidad* pos1= malloc(sizeof(posicionCantidad));
//	posicionCantidad* pos2= malloc(sizeof(posicionCantidad));
//	posicionCantidad* pos3= malloc(sizeof(posicionCantidad));
//
//	(pos1->posicion).x = 1;
//	(pos1->posicion).y = 1;
//	 pos1->cantidad = 10;
//
//	 (pos2->posicion).x = 2;
//	 (pos2->posicion).y = 2;
//	  pos2->cantidad = 10;
//
//	 (pos3->posicion).x = 3;
//	 (pos3->posicion).y = 3;
//	  pos3->cantidad = 10;
//
//	  t_list* listaLoca = list_create();
//
//	  list_add(listaLoca,(void*) pos1);
//	  list_add(listaLoca,(void*) pos2);
//	  list_add(listaLoca,(void*) pos3);
//
//	  mensajeLocalized* msgLocalized = llenarLocalized("Pikachu",listaLoca);
//	  void* stream = serializarLocalized(msgLocalized);
//	  paquete* paq = llenarPaquete(GAMECARD,LOCALIZED_POKEMON,sizeArgumentos(LOCALIZED_POKEMON,"Pikachu",3),stream);
//
//
//	  log_info(gamecardLogger2,"Size del stream:%i ",paq->sizeStream );
//	  void* paqSerializado = serializarPaquete(paq);
//
//	  paquete* paqDeserializado = deserializarPaquete(paqSerializado);
//	  mensajeLocalized* msgLocalizedDeserializado = deserializarLocalized(paqDeserializado->stream);
//
//	  posicionCantidad* p1 = (posicionCantidad*) list_get(msgLocalizedDeserializado->listaPosicionCantidad,0);
//	  posicionCantidad* p2 = (posicionCantidad*) list_get(msgLocalizedDeserializado->listaPosicionCantidad,1);
//	  posicionCantidad* p3 = (posicionCantidad*) list_get(msgLocalizedDeserializado->listaPosicionCantidad,2);
//
//	  log_info(gamecardLogger2, "Pokemon:%s",msgLocalizedDeserializado->pokemon);
//	  log_info(gamecardLogger2, "Posicion1: %i - %i = % i ",(p1->posicion).x,(p1->posicion).y,p1->cantidad);
//	  log_info(gamecardLogger2, "Posicion2: %i - %i = % i ",(p2->posicion).x,(p2->posicion).y,p2->cantidad);
//	  log_info(gamecardLogger2, "Posicion3: %i - %i = % i ",(p3->posicion).x,(p3->posicion).y,p3->cantidad);
//
//
//	return 0;
//}


int main(void) {
	listaArchivos=inicializarListaMutex();
	mutexPrueba=malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutexPrueba,NULL);

	gamecardLogger2=log_create("gamecardLoggerSecundario.log","gamecard", true, LOG_LEVEL_INFO);
	t_config * configGamecard = config_create("Gamecard.config");
	puertoBrokerGC = config_get_int_value(configGamecard, "PUERTO_BROKER");//5002;
	ipBrokerGC = config_get_string_value(configGamecard, "IP_BROKER");//"127.0.0.1"; //
	tiempoRetardoGC = config_get_int_value(configGamecard, "TIEMPO_DE_RETARDO_OPERACION"); //
	tiempoReintentoOperacion=config_get_int_value(configGamecard, "TIEMPO_DE_REINTENTO_OPERACION"); //
	puntoMontaje = config_get_string_value(configGamecard, "PUNTO_MONTAJE_TALLGRASS");
	log_info(gamecardLogger2,"Puerto Broker:%i",puertoBrokerGC);

	crearDirectorio("Files","/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/TALL_GRASS/", DIRECTORIO);
	crearDirectorio("Blocks","/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/TALL_GRASS/", DIRECTORIO);

	iniciarMetadata();
	iniciarBitmap();
	inicializarListaBloques();


	//suscribirseColasBroker(configGamecard);
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
	log_info(gamecardLogger2,"Voy a llenar el paquete");
	paquete* paq = llenarPaquete(GAMECARD, SUSCRIPCION, sizeStream,streamMsgSuscripcion);

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ipBrokerGC);
	direccionServidor.sin_port = htons(puertoBrokerGC);

	uint32_t cliente = socket(AF_INET, SOCK_STREAM, 0);
	log_info(gamecardLogger2,"cliente: %d", cliente);
	while (connect(cliente, (void*) &direccionServidor,sizeof(direccionServidor)) < 0) {
		log_info(gamecardLogger2,"Conexión fallida con el Broker reintentando en %i segundos...",tiempoReconexionGC);
		sleep(tiempoReconexionGC);
	}
	log_info(gamecardLogger2,"Comienzo suscripcion a %i", paq->tipoMensaje);
	uint32_t bytes = sizeof(uint32_t) * 5 + paq->sizeStream;

	void* stream = serializarPaquete(paq);

	//free(streamMsgSuscripcion);

	send(cliente, stream, bytes, 0);

	//free(stream);

	uint32_t respuesta;
	log_info(gamecardLogger2,"Espero respuesta");
	recv(cliente, &respuesta, sizeof(uint32_t), 0);

	if (respuesta == CORRECTO) {
		log_info(gamecardLogger2,"Se suscribio correctamente");
		while (1) {

			paquete* paqueteRespuesta = recibirPaquete(cliente);

			while (enviarACK(cliente, GAMECARD, paqueteRespuesta->id) < 0) {

				log_info(gamecardLogger2,"Conexión fallida con el Broker reintentando en %i segundos",tiempoReconexionGC);
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
		log_info(gamecardLogger2,"Mensaje recibido incorrectamente\n");
		log_info(gamecardLogger2,"mensaje: %i", respuesta);

	}
	return NULL;
}

int crearHiloServidorGameboy(pthread_t* hilo) {
	uint32_t err = pthread_create(hilo, NULL, iniciarServidorGameboy, NULL);
	if (err != 0) {
		log_info(gamecardLogger2,"Hubo un problema en la creación del hilo para iniciar el servidor para el Gameboy \n");
		return err;
	}

	pthread_detach(*hilo);
	while(1);//despues sacarlo
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
		log_info(gamecardLogger2,"Estoy escuchando");
		while (1)  						//para recibir n cantidad de conexiones
			esperar_cliente(servidor);
	}
	return NULL;
}

void esperar_cliente(uint32_t servidor) {

	listen(servidor, 100);
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion = sizeof(struct sockaddr_in);
	log_info(gamecardLogger2,"Espero un nuevo cliente");
	uint32_t* socket_cliente = malloc(sizeof(uint32_t));
	*socket_cliente = accept(servidor, (void*) &dir_cliente, &tam_direccion);
	log_info(gamecardLogger2,"Gestiono un nuevo cliente");
	pthread_t threadAtencionGameboy;
	pthread_create(&threadAtencionGameboy, NULL, atenderCliente,(void*) (socket_cliente));
	pthread_detach(threadAtencionGameboy);
}

void* atenderCliente(void* sock) {
	log_info(gamecardLogger2,"atiendo cliente");
	uint32_t* socket = (uint32_t*) sock;
	log_info(gamecardLogger2,"hola llegue");
	paquete* paquete = recibirPaquete(*socket);
	uint32_t respuesta = 0;
	if (paquete == NULL) {
		respuesta = INCORRECTO;
	} else {
		respuesta = CORRECTO;
	}

	send(*socket, (void*) (&respuesta), sizeof(uint32_t), 0);
	free(socket);

	log_info(gamecardLogger2,"hice el send: %i", respuesta);
	log_info(gamecardLogger2,"recibi: %i", paquete->sizeStream);
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
		log_info(gamecardLogger2,"leyo cualquiera");
		break;
	}

	return NULL;
}

void* atenderNew(void* paq) {
	paquete* paqueteNew = (paquete*) paq;
	uint32_t idNew = paqueteNew->id;
	mensajeNew* msgNew = deserializarNew(paqueteNew->stream);
	log_info(gamecardLogger2,"deserializado");

	pokemonEnPosicion* pokeEnPosicion = malloc(sizeof(pokemonEnPosicion));
	pokeEnPosicion->pokemon = msgNew->pokemon;
	pokeEnPosicion->cantidad = msgNew->cantidad;
	pokeEnPosicion->id = idNew;
	(pokeEnPosicion->posicion).x = msgNew->posX;
	(pokeEnPosicion->posicion).y = msgNew->posY;

	//free(msg);
	//To do :
	log_info(gamecardLogger2,"Atiendo new del pokemon: %s. Posicion: (%i, %i).", msgNew->pokemon, msgNew->posX, msgNew->posY);
	archivoHeader* archivoPoke= obtenerArchivoPokemon(pokeEnPosicion->pokemon);


	FILE* archivoMetadata=abrirArchivo(archivoPoke);


	log_info(gamecardLogger2, "comienzo a obtener lista de %s", msgNew->pokemon);
	t_list* listaPosCantidad=obtenerListaPosicionCantidadDeArchivo(archivoPoke);

	log_info(gamecardLogger2, "termino de obtener lista de %s", msgNew->pokemon);
	posicionCantidad* encontrado= buscarPosicionCantidad(listaPosCantidad, pokeEnPosicion->posicion);


	if(encontrado!=NULL){
		(encontrado->cantidad)+=pokeEnPosicion->cantidad;
	}else{
		posicionCantidad* posAgregar=malloc(sizeof(posicionCantidad));
		posAgregar->cantidad=pokeEnPosicion->cantidad;
		(posAgregar->posicion).x=(pokeEnPosicion->posicion).x;
		(posAgregar->posicion).y=(pokeEnPosicion->posicion).y;
		list_add(listaPosCantidad, (void*) posAgregar);
	}

	log_info(gamecardLogger2, "comienzo a actualizar posiciones de %s", msgNew->pokemon);
	actualizarPosicionesArchivo(archivoPoke,listaPosCantidad);
	log_info(gamecardLogger2, "termino de actualizar posiciones de %s", msgNew->pokemon);
	list_destroy_and_destroy_elements(listaPosCantidad, free);
	//Verificar que el pokemon este en nuestro FileSystem
	//Una vez encontrado (o creado) verificar si puedo abrirlo
	//Verificar si las posiciones existen en el archivo
	//IF SUCCESS
	sleep(tiempoRetardoGC);
	//CERRAR ARCHIVO
	cerrarArchivo(archivoPoke,archivoMetadata);

	log_info(gamecardLogger2,"Cierro el archivo del pokemon: %s. Posicion: (%i, %i).", msgNew->pokemon, msgNew->posX, msgNew->posY);
	enviarAppeared(pokeEnPosicion);
	return NULL;

}

void enviarAppeared(pokemonEnPosicion* pokeEnPosicion) {
	uint32_t cliente = crearSocketCliente(ipBrokerGC, puertoBrokerGC);
	mensajeAppeared* msgAppeared = malloc(sizeof(mensajeAppeared));
	llenarAppeared(pokeEnPosicion->pokemon,(pokeEnPosicion->posicion).x,(pokeEnPosicion->posicion).y);
//	msgAppeared->pokemon = pokeEnPosicion->pokemon;
//	msgAppeared->posX = (pokeEnPosicion->posicion).x;
//	msgAppeared->posY = (pokeEnPosicion->posicion).y;
//	msgAppeared->idCorrelativo =pokeEnPosicion->id;
//	msgAppeared->sizePokemon = strlen(msgAppeared->pokemon) + 1;
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
	log_info(gamecardLogger2,"deserializado");
	destruirPaquete(paqueteGet);

	pokemonADevolver* pokeADevolver = malloc(sizeof(pokemonADevolver));
	pokeADevolver->pokemon = msgGet->pokemon;
	pokeADevolver->id = idGet;
	log_info(gamecardLogger2,"Atiendo Get del pokemon: %s", msgGet->pokemon);

	if(archivoExiste(string_from_format(pathFiles,pokeADevolver->pokemon))){
		archivoHeader* archivoPoke = obtenerArchivoPokemon(pokeADevolver->pokemon);
		FILE* archivoMetadata = abrirArchivo(archivoPoke);
		pokeADevolver->posicionCantidad = obtenerListaPosicionCantidadDeArchivo(archivoPoke);
		sleep(tiempoRetardoGC);
		cerrarArchivo(archivoPoke,archivoMetadata);
	}else{
		//pokeADevolver->posicionCantidad = NULL;
	}

	//Verificar que el pokemon este en nuestro FileSystem (si no encuentra mando posiciones vacias)
	//Una vez encontrado verificar si puedo abrirlo
	//Verificar si las posiciones existen en el archivo
	//Conseguir Posiciones
	//pokeADevolver->cantPosiciones = DEL FILESYSTEM
	//pokeADevolver->posicion= DEL FILESYSTEM
	//free(msg);
	//IF SUCCESS

	enviarLocalized(pokeADevolver);
	return NULL;
}

void enviarLocalized(pokemonADevolver* pokeADevolver) {
	uint32_t cliente = crearSocketCliente(ipBrokerGC, puertoBrokerGC);
	mensajeLocalized* msgLocalized = malloc(sizeof(mensajeLocalized));
	msgLocalized = llenarLocalized(pokeADevolver->pokemon,pokeADevolver->posicionCantidad);
	//msgLocalized->pokemon = pokeADevolver->pokemon;
	//msgLocalized->cantidad = pokeADevolver->cantPosiciones;
	//msgLocalized->arrayPosiciones = pokeADevolver->posicion;
	//msgLocalized->sizePokemon = strlen(msgLocalized->pokemon) + 1;
	void* streamMsg = serializarLocalized(msgLocalized);
	paquete* paq = llenarPaquete(GAMECARD, LOCALIZED_POKEMON,sizeArgumentos(LOCALIZED_POKEMON, msgLocalized->pokemon, BROKER),streamMsg);
	insertarIdCorrelativoPaquete(paq, (pokeADevolver->id));
	void* paqueteSerializado = serializarPaquete(paq);
	//free(msgLocalized);
	//destruirPaquete(paq);
	send(cliente, paqueteSerializado, sizePaquete(paq), 0);
	free(paqueteSerializado);
}

void* atenderCatch(void* paq) {
	log_info(gamecardLogger2,"Entro al catch");
	paquete* paqueteCatch = (paquete*) paq;
	uint32_t idCatch = paqueteCatch->id;
	mensajeCatch* msgCatch = deserializarCatch(paqueteCatch->stream);
	log_info(gamecardLogger2,"deserializado");
	pokemonAAtrapar* pokeAAtrapar = malloc(sizeof(pokemonAAtrapar));
	pokeAAtrapar->id = idCatch;
	pokeAAtrapar->pokemon = msgCatch->pokemon;
	(pokeAAtrapar->posicion).x = msgCatch->posX;
	(pokeAAtrapar->posicion).y = msgCatch->posY;
	log_info(gamecardLogger2,"Pokemon:%s, posicion:%i-%i",pokeAAtrapar->pokemon,(pokeAAtrapar->posicion).x,(pokeAAtrapar->posicion).y);

	archivoHeader* archivoPokeCatch= obtenerArchivoPokemon(pokeAAtrapar->pokemon);
	FILE* archivoMetadata=abrirArchivo(archivoPokeCatch);
	t_list* listaPosCantidadCatch=obtenerListaPosicionCantidadDeArchivo(archivoPokeCatch);
	posicionCantidad* encontrado= buscarPosicionCantidad(listaPosCantidadCatch, pokeAAtrapar->posicion);
	log_info(gamecardLogger2,"Size lista antes de comprobar:%i",list_size(listaPosCantidadCatch));
	if(encontrado == NULL){
		log_info(gamecardLogger2,"NULLACIO");
		pokeAAtrapar->resultado = FAIL;
	}else{
		uint32_t idPosicion = buscarIdCantidad(listaPosCantidadCatch, pokeAAtrapar->posicion);
		if(encontrado->cantidad>1){
			log_info(gamecardLogger2,"Reemplazo");

			encontrado->cantidad = encontrado->cantidad - 1;
			//list_replace(listaPosCantidadCatch, idPosicion, encontrado);
		}else{
			log_info(gamecardLogger2,"Elimino linea");
			list_remove(listaPosCantidadCatch, idPosicion);
			//list_remove_and_destroy_element(listaPosCantidadCatch, idPosicion, free);
		}
		actualizarPosicionesArchivo(archivoPokeCatch,listaPosCantidadCatch);
		pokeAAtrapar->resultado = OK;
	}
	log_info(gamecardLogger2,"Size lista despues de comprobar:%i",list_size(listaPosCantidadCatch));

	sleep(tiempoRetardoGC);
	cerrarArchivo(archivoPokeCatch,archivoMetadata);
	enviarCaught(pokeAAtrapar); //Momentaneo hasta saber bien que hacer con fileSystem
	return NULL;
}

void enviarCaught(pokemonAAtrapar* pokeAAtrapar) {
	uint32_t cliente = crearSocketCliente(ipBrokerGC, puertoBrokerGC);
	mensajeCaught* msgCaught = malloc(sizeof(mensajeCaught));
	llenarCaught(pokeAAtrapar->resultado);
	//msgCaught->resultadoCaught = pokeAAtrapar->resultado;
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

FILE* verificarApertura(archivoHeader* archivo){
	while(estaAbierto(archivo)){
		sleep(tiempoReintentoOperacion);
		log_info(gamecardLogger2, "Reintento abrir el archivo en %i sengundos. ", tiempoReintentoOperacion);
	}
	return abrirArchivo(archivo);
}



