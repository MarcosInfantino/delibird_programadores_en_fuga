/*
 * Team.h
 *
 *  Created on: 20 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_H_
#define TEAM_H_

#include <commons/collections/queue.h>
#include <commons/string.h>
#include <stdint.h>
#include <inttypes.h>

typedef enum {
	NEW=1500,
	READY=1501,
	BLOCKED=1502,
	EXEC=1503,
	EXIT=1504
}estado;
typedef struct{
	uint32_t cantidad;
	char* pokemon;
} objetivo;

typedef struct{
	uint32_t x;
	uint32_t y;
} posicion;

typedef struct {
	posicion posicion;
	t_list* pokemones;
	t_list* objetivoPersonal;
	estado estado;

} dataEntrenador;

typedef struct {
	t_list* objetivoGlobal;
	t_list* entrenadores;
	//uint32_t** mapa;

}dataTeam;

dataTeam* inicializarTeam(char* path);

t_list* obtenerListaDeListas(char* str);

t_list* arrayStringALista(char** arr);

#endif /* TEAM_H_ */
