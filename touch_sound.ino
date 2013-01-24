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

    long s1, s1o;
    s1 = clip_one.capSense(5);
    s1o = s1;

    long s2 = clip_two.capSense(5);
    long s3 = clip_three.capSense(5);
    long s4 = clip_four.capSense(5);

    sprintf(data_str, "%lu, %lu, %lu, %lu", s1, s2, s3, s4);
    Serial.println(data_str);

    uint8_t i = 1;
    while(s1 > 40) {
            synth_enable();
            uint16_t s_t = ((s1o * 7) + s1) >> 3;
            Serial.println(s_t);
            uint16_t s_tm = map(s_t, 40, 500, 5 ,127);
            Serial.println(s_tm);
            //play_note(s_t);
            synth_play(s_tm, 350);
            s1o = s1;
            s1 = clip_one.capSense(5);
            delay(20);
    }
    synth_disable();
    PORTF &= ~(1 << LED2);
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



