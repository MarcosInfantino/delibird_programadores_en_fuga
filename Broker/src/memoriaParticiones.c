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
#include <stdbool.h>
#include "broker.h"
#include "memoria.h"

void registrarEnMemoriaPARTICIONES(msgMemoriaBroker* mensajeNuevo){

}


//void particionarMemoriaPARTICIONES(listaMutex* partición, msgMemoriaBroker* msg ){ //todo
	//uint32_t tamanio = msg->paq->sizeStream;

//}


listaMutex* iniciarMemoriaPARTICIONES(){
	return inicializarListaMutex();
}
