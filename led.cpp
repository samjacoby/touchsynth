#include <avr/io.h>
#include "Arduino.h"
#include "led.h"

uint8_t PWR = 1;
uint8_t ALERT = 2;
uint8_t LED0 = 3;
uint8_t LED1 = 4;
uint8_t LED2 = 5;
uint8_t LED3 = 6; 

void setLED(uint8_t nLed, uint8_t val) {

    DDRD &= ~((1 << A) | (1 << B) | (1 << C)); // zero everything out
    PORTD &= ~((1 << A) | (1 << B) | (1 << C)); 


    //PORTD &= 0xE9;  // zero everything out 

    if(val == HIGH) {
        switch(nLed) {  

            case 1: // PWR
                DDRD |= (1 << A) | (1 << C);
                PORTD |= (1 << A); 
                break;
            case 2: // ALERT
                DDRD |= (1 << A) | (1 << C);
                PORTD |= (1 << C); 
                break;
            case 3: // LED0
                DDRD |= (1 << A) | (1 << B); 
                PORTD |= (1 << A); 
                break;
            case 4: // LED1
                DDRD |= (1 << B) | (1 << C);
                PORTD |= (1 << B); 
                break;
            case 5: // LED2
                DDRD |= (1 << A) | (1 << B); 
                PORTD |= (1 << B); 
                break;
            case 6: // LED3
                DDRD |= (1 << B) | (1 << C);
                PORTD |= (1 << C); 
                break;

        }
    }

}

