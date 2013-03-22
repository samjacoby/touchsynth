#ifndef __led_h_
#define __led_h_

#define A PIND5 
#define B PIND3 
#define C PIND2 

extern int PWR;
extern int ALERT;
extern int LED1;
extern int LED2;
extern int LED3;
extern int LED4;

void setLED(int nLed);
#endif // __led_h_
