#include <stdio.h>
#include <avr/io.h>
#include <CapSense.h>

#include "touch_sound.h"
#include "audio.h"
#include "sinetable.h"
#include "synth.h"

CapSense clip_one = CapSense(13, 5);
CapSense clip_two = CapSense(9, 10);
CapSense clip_three = CapSense(6, 8);
CapSense clip_four = CapSense(4, 12);

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

char * data_str = (char*) malloc(18 * sizeof(char));
void loop() {

    long s1 = clip_one.capSense(5);
    long s2 = clip_two.capSense(5);
    long s3 = clip_three.capSense(5);
    long s4 = clip_four.capSense(5);
    sprintf(data_str, "%lu, %lu, %lu, %lu", s1, s2, s3, s4);

    Serial.println(data_str);

    if(s1 > 20) {
        synth_set_mod_ratio(s2, s3);
        long s_map = map(s1, 0, 700, 1, 63);
        synth_play(s_map, 5);
    }
    delay(5);
}

