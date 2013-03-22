#include <avr/io.h>
#include "led.h"

int PWR = 1;
int ALERT = 2;
int LED1 = 3;
int LED2 = 4;
int LED3 = 5;
int LED4 = 6; 

void setLED(int nLed) {

    DDRD |= 0x16;   // initalize port to output 
    PORTD &= 0xE9;  // zero everything out 

    switch(nLed) {  

        case 1: // PWR
            DDRD &= ~(1 << B);  
            PORTD |= (1 << A); 
            break;
        case 2: // ALERT
            DDRD &= ~(1 << B); 
            PORTD |= (1 << C); 
            break;
        case 3: // LED1
            DDRD &= ~(1 << A); 
            PORTD |= (1 << C); 
            break;
        case 4: // LED2
            DDRD &= ~(1 << C); 
            PORTD |= (1 << B); 
            break;
        case 5: // LED3
            DDRD &= ~(1 << A); 
            PORTD |= (1 << B); 
            break;
        case 6: // LED4
            DDRD &= ~(1 << C); 
            PORTD |= (1 << A); 
            break;

    }

}

