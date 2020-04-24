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

void* especieAComparar;

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
	t_list* objetivoPersonal;//lista de strings
	estado estado;

} dataEntrenador;

typedef struct {
	t_list* objetivoGlobal;//lista de objetivo
	t_list* entrenadores;
	t_list* objetivosCumplidos;
	//uint32_t** mapa;

}dataTeam;

dataTeam* inicializarTeam(char* path);

t_list* obtenerListaDeListas(char* str);

t_list* arrayStringALista(char** arr);

t_list* obtenerObjetivos(t_list* especies);

bool objetivoMismaEspecie(void* obj);

#endif /* TEAM_H_ */
