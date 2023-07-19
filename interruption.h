/* 
 * File:   interruption.h
 * Author: rodri
 *
 * Created on 13 de julio de 2023, 04:57 PM
 */

#ifndef INTERRUPTION_H
#define	INTERRUPTION_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

void ioc_int(char pin);

#endif	/* INTERRUPTION_H */

