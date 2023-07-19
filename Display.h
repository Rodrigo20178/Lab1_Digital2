/* 
 * File:   Display.h
 * Author: Rodrigo García
 *
 *
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

uint8_t tabla(uint8_t valor);    // Traduce números de 0-9 a valores de display
void divisor(uint8_t num, uint8_t *decena, uint8_t *unidad);

#endif	/* DISPLAY_H */

