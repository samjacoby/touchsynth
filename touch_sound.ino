#include <stdio.h>
#include <avr/io.h>
#include <CapSense.h>

#include "touch_sound.h"
#include "audio.h"
#include "sinetable.h"
#include "synth.h"

#define SERIALON

typedef struct {
    CapSense *clip;
    uint8_t active;
    uint16_t calibration;
    uint8_t shift;
    uint16_t last;

} sense_t; 

#define NUMCLIPS 4
#define SAMPLES 10

sense_t clips[NUMCLIPS];

//byte notes[] = { 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36};
byte notes[] = { 34, 38, 42, 46, 50, 54, 58, 62, 66, 70, 74, 78, 82, 86, 90, 94, 98, 102, 106, 110, 114, 118, 122, 126, 130 };


CapSense clip0 = CapSense(13, 5);
CapSense clip1 = CapSense(9, 10);
CapSense clip2 = CapSense(6, 8);
CapSense clip3 = CapSense(4, 12);

CapSense *sensors[] = {
    &clip0,
    &clip1,
    &clip2,
    &clip3,
};

uint8_t next_sample = 0;

void setup() {

    DDRF |= (1 << LED1) | (1 << LED2);

    cli();
    audio_init();
    synth_init();
    synth_set_amplitude(255);
    sei();
    
    #ifdef SERIALON
    Serial.begin(115200);
    #endif

    uint8_t pinIndex = 0;
    for(byte i=0; i < NUMCLIPS; i++) {
        pinIndex = i * 2; 
        clips[i].clip = sensors[i]; 
        clips[i].active = 0;
        clips[i].shift = i << 3; // multiply by eight 
        clips[i].calibration = 0; // multiply by eight 
    }

    calibrate();
}

void calibrate() {

    for(int i=0; i < NUMCLIPS; i++) {
        clips[i].calibration = (clips[i].clip->capSense(SAMPLES) + 10) << 3;
        delay(10);
    }

}

void synth_play(uint16_t note, uint16_t duration)  {
    synth_clear();
    unsigned long s_time = millis();
    for(;;) {
        synth_play_note(note);
        if(millis() - s_time > duration) {
            synth_stop_note();
        }
    }
}

uint8_t active = 1;

uint8_t notes_i = 0;
uint8_t notes_mask = 7;

void loop() {

    long sense;
    
    for(int i=0; i < NUMCLIPS; i++) {

        sense = clips[i].clip->capSense(SAMPLES);  
        clips[i].last = sense;

        if(sense > clips[i].calibration) {
            PORTF |= 1 << LED2;
            synth_play_note(notes[notes_i + clips[i].shift]); 
            clips[i].active = 1;
        } else if(clips[i].active) {
            PORTF &= ~(1 << LED2);
            clips[i].active = 0; 
            notes_i = (notes_i + 1) & notes_mask;
            synth_stop_note();
        }

    }   

    delay(10);
    #ifdef SERIALON
    char buffer [50];
    sprintf(buffer, "%d, %d, %d, %d", clips[0].last, clips[1].last, clips[2].last, clips[3].last);
    Serial.println(buffer);
    #endif

}
