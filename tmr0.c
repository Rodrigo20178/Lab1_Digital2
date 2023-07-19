#include "tmr0.h"

void tmr0(void){
     OPTION_REGbits.T0CS = 0;    // Reloj interno seleccionado
    OPTION_REGbits.T0SE = 0;    // Flancos positivos
    OPTION_REGbits.PSA = 0;     // Prescaler a Timer0
    OPTION_REGbits.PS2 = 1;     // Prescaler 1:256
    OPTION_REGbits.PS1 = 1;     
    OPTION_REGbits.PS0 = 1;     
    TMR0 = 200;
}
