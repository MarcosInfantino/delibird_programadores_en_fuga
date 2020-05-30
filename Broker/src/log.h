/*
 * log2.h
 *
 *  Created on: 30 may. 2020
 *      Author: utnso
 */

#ifndef SRC_LOG_H_
#define SRC_LOG_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "broker.h"

t_log* iniciar_logger();
void terminar_programa(t_log*);

char* nombreDeProceso(uint32_t modulo);
char* nombreDeCola(uint32_t cola);
char* armarStringSuscripLog(uint32_t modulo, uint32_t cola);
char* armarStringMsgNuevoLog(uint32_t modulo);
char* armarConexionNuevoProcesoLog(uint32_t modulo);


#endif /* SRC_LOG_H_ */
