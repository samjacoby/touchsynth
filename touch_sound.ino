#include <stdio.h>
#include <avr/io.h>
#include <avr/interript.h>
#include <CapSense.h>

#include "touch_sound.h"
#include "audio.h"
#include "sinetable.h"
#include "synth.h"

CapSense clip_one = CapSense(9, 10);
CapSense clip_two = CapSense(4, 12);
CapSense clip_three = CapSense(13, 5);

uint8_t next_sample = 0;

void setup() {

   DDRF |= (1 << LED1) | (1 << LED2);

   cli();
   audio_init();
   synth_init();
   sei();

   Serial.begin(115200);

}

unsigned long s_time;

void synth_play(uint16_t note, uint16_t duration)  {
    synth_clear();
    s_time = millis();
    for(;;) {
        synth_generate(note);
        if(millis() - s_time > duration) {
            break; 
        }
    }
}

void loop() {
    for(;;) {
        synth_clear();
        synth_play(1, 1000);
        synth_play(2, 750);
        synth_play(3, 500);
        synth_play(4, 1000);
        synth_play(2, 500);
        delay(100);
        synth_play(2, 500);
        delay(100);
        synth_play(2, 500);
        delay(100);
        synth_play(3, 100);
        delay(100);
        synth_play(2, 500);
        delay(100);
        synth_play(2, 750);
        synth_play(3, 100);
    }
}

