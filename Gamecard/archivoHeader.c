/*
 * archivoHeader.c
 *
 *  Created on: 26 jun. 2020
 *      Author: utnso
 */

#include "gamecard.h"

bool estaAbierto(archivoHeader* metadata){
	return metadata->estaAbierto;
}

void cerrarArchivo(archivoHeader* metadata, FILE* archivo){
	if(metadata->tipo==ARCHIVO){
		fclose(archivo);
		t_config* config=config_create(metadata->pathArchivo);
		config_set_value(config,"OPEN","N");
		config_destroy(config);
		metadata->estaAbierto=false;
	}
}

FILE* abrirArchivo(archivoHeader* metadata){
	if(metadata->tipo==ARCHIVO){
		metadata->estaAbierto=true;
		t_config* config=config_create(metadata->pathArchivo);
		config_set_value(config,"OPEN","Y");
		config_destroy(config);
		return fopen(metadata->pathArchivo,"r+");

	}
	return NULL;
}
