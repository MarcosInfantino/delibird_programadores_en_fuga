#include "Team.h"

uint32_t puertoTeam=5003;

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
		//printf("Estoy escuchando\n");
		while (1)  								//para recibir n cantidad de conexiones
				esperar_cliente(servidor);
		}

	return NULL;
}

void esperar_cliente(uint32_t servidor) {

	listen(servidor, 100);
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion = sizeof(struct sockaddr_in);
	//printf("Espero un nuevo cliente\n");
	uint32_t* socket_cliente=malloc(sizeof(uint32_t));
	*socket_cliente = accept(servidor, (void*) &dir_cliente, &tam_direccion);
	//printf("Gestiono un nuevo cliente\n");
	pthread_t threadAtencionGameboy;
	pthread_create(&threadAtencionGameboy, NULL, atenderCliente, (void*) (socket_cliente));
	pthread_detach(threadAtencionGameboy);
}

void* atenderCliente(void* sock){

	uint32_t* socket = (uint32_t*) sock;

	paquete* paquete=recibirPaquete(*socket);
	uint32_t respuesta=0;
	if(paquete==NULL){
		respuesta=INCORRECTO;
	}else{
		respuesta=CORRECTO;
	}

	send(*socket,(void*)(&respuesta),sizeof(uint32_t),0);
	free(socket);

	loggearMensaje(paquete, teamLogger);
	switch(paquete->tipoMensaje){
		case APPEARED_POKEMON:;
			mensajeAppeared* msg=deserializarAppeared(paquete->stream);
			destruirPaquete(paquete);
			atenderAppeared(msg); ;break;
		default: break;
	}


	return NULL;
}
