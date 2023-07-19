/*
 * File:   Lab01.c
 * Author: Rodrigo García
 *
 * Created on 13 de julio de 2023, 05:19 PM
 */

// CONFIG1
#pragma config FOSC = INTRC_CLKOUT// Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// Librerías
#include <xc.h>
#include <stdint.h>
#include "adc.h"
#include "interruption.h"
#include "Display.h"
#include "tmr0.h"

// Constantes
#define _XTAL_FREQ 4000000

//Variables
uint8_t estado;                 // Determina el estado de los transistores
uint8_t valor;            // Almacena el último valor de la conversión ADC

uint8_t dec;                    // Variables que almacenan dígitos del display
uint8_t unit;

uint8_t dec_disp;                  // Variables que almacenan los dígitos después
uint8_t unit_disp;                  // de haber sido modificados por la tabla

//Prototipo de Funciones
void setup(void);

//Función de Interrupciones
void __interrupt() isr (void){
    if(PIR1bits.ADIF)           // Si la bandera está encendida, entrar
    {
        valor = ADRESH;      // la variable asignada al a lo que valga adresh  
        adc_read();         // leer el valor del adresh
    }
    
    if (INTCONbits.RBIF){
        if (!PORTBbits.RB0)     
        {
            PORTC++;            //cuando se utiliza el primer botón, aumenta el contador de referencia
        }
        else if (!PORTBbits.RB1) 
        {
            PORTC--;           //cuando se utiliza el segundo botón, disminuye el contador de referencia
        }
        INTCONbits.RBIF = 0;
    }
    
     if(INTCONbits.T0IF)         // Interrupción de TMR0
    {
        INTCONbits.T0IF = 0;    // Limpiar bandera
 
        switch(estado)
        {
        case 0:
            estado = 1;             // Cambiar de estado
            PORTE = 0b01;           // Encender transistor 1
            PORTD = unit_disp;      // Display recibe valor de unidades          
            break;
        case 1:
            estado = 0;            // Cambiar de estado
            PORTE = 0b10;          // Encender transistor 2
            PORTD = dec_disp;      // Display recibe valor de decenas
            break;
        default:
            estado = 0;
            break;
        }
        TMR0 = 200;
    }
}  
// MAIN
void main(void) {
    setup();
    while(1){
         if(!ADCON0bits.GO)              
        {
            __delay_us(50);             // Delay para no interrumpir conversión
            ADCON0bits.GO = 1;          // Iniciar nueva conversión
        }
        
        divisor(valor, &dec, &unit);    // Usar función para convertir valores a hexadecimales
        dec_disp = tabla(dec);          // Llamar tabla para el display de las decenas
        unit_disp = tabla(unit);        // Llamar tabla para el display de las unidades
        
        if(valor > PORTC)
        {
            PORTBbits.RB7 = 1;          // Si el valor del display es mayor al de la referencia, prender led de alarma
        }
        else
        {
            PORTBbits.RB7 = 0;         // Si el valor del display es menor o igual al de la referencia, apagar led de alarma
        }
      
       
    }
    return;
}

// Función de Configuraciones
void setup(void){
    ANSEL = 0b00001111;          //AN0, AN1, AN2, AN3 como entradas analógicas
    ANSELH = 0;
    
    TRISA = 0b00001111;         // RA0, RA1, RA2 & RA3 como entradas 
    PORTA = 0;                  // Se limpia el PORTA 
    
    TRISB = 0b00000011;        // RB0 & RB1 como entradas
    PORTB = 0;                 // Se limpia el PORTB
    
    TRISC = 0;                 // PORTC como salida (contador de referencia)
    PORTC = 0;                 // Se limpia el PORTC
    
    TRISD = 0;                // PORTD como salida (displays) 
    PORTD = 0;                // Se limpia el PORTD
    
    TRISE = 0;               // PORTE como salida (cambio de multiplex)
    PORTE = 0;               // Se limpia el PORTE
    
    // Configuración de los IOCB
    OPTION_REGbits.nRBPU = 0;
    ioc_int(0);
    ioc_int(1);
    
    // Configuración del TMR0
    tmr0();
   
    // Configuración del Oscilador
    OSCCONbits.IRCF = 0b0110;    // IRCF <2:0> -> 4 MHz
    OSCCONbits.SCS = 1;         // Oscilador interno
    
    adc_init(1);
    ADCON0bits.CHS = 0b0000;    // Seleccionamos el AN0
    
                    
    
    // Configuración de las Interrupciones
    INTCONbits.GIE = 1;         // Habilitamos interrupciones globales
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones de los puertos
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;        // Limpiamos bandera de interrupción de PORTB
    INTCONbits.T0IE = 1;        // Activar interrupciones de TMR0
    INTCONbits.T0IF = 0;        // Apagar bandera de overflow de TMR0
    
    
}
