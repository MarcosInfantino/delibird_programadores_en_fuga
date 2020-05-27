/*
 * log.h
 *
 *  Created on: 27 may. 2020
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

#include "gameBoy.h"

t_log* iniciar_logger();
void terminar_programa(t_log*);

#endif /* SRC_LOG_H_ */
