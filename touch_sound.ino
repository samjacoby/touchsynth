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

uint8_t active = 1;
char * data_str = (char*) malloc(18 * sizeof(char));
void loop() {

    long s1 = clip_one.capSense(5);
    long s2 = clip_two.capSense(5);
    long s3 = clip_three.capSense(5);
    long s4 = clip_four.capSense(5);
    sprintf(data_str, "%lu, %lu, %lu, %lu", s1, s2, s3, s4);

    Serial.println(data_str);

    while(s1 > 20) {
        active = 0;
        play_note(s1);
        s1 = clip_one.capSense(5);
        s1 = map(s1, 20, 700, 1, 63);
        delay(100);
    }
    active = 1;
}

void play_song() {
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



