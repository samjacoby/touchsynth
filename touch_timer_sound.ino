#include <stdio.h>
#include <avr/io.h>
#include <avr/interript.h>
#include <CapSense.h>

#include "audio.h"
#include "sinetable.h"
#include "synth.h"

#define LED1 PINF1 
#define LED2 PINF0 

CapSense clip_one = CapSense(9, 10);
CapSense clip_two = CapSense(4, 12);
CapSense clip_three = CapSense(13, 5);

uint8_t next_sample = 0;

void setup() 
{
   cli();
   audio_init();
//   synth_init();
   sei();

   DDRF |= (1 << LED1) | (1 << LED2);
   Serial.begin(115200);
}

long total1, total2, total3;
char * data_str = (char*) malloc(18 * sizeof(char));

void loop() {
/*
    total1 =  clip_one.capSense(5);
    total2 =  clip_two.capSense(5);
    total3 =  clip_three.capSense(5);

    sprintf(data_str, "%lu, %lu, %lu", total1, total2, total3);
    
    delay(10);
    uint16_t note = 127;
    */

    PORTF = (1 << LED1) | (1 << LED2);
   Serial.println("hello");
   delay(100);

/*    for(;;) {
        synth_generate(127); 
    }
    */

}
