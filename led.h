#ifndef __led_h_
#define __led_h_

#define A PIND5 
#define B PIND3 
#define C PIND2 

extern uint8_t PWR;
extern uint8_t ALERT;
extern uint8_t LED0;
extern uint8_t LED1;
extern uint8_t LED2;
extern uint8_t LED3;

void setLED(uint8_t, uint8_t);
#endif // __led_h_
