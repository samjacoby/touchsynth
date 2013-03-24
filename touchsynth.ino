#include <stdio.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <CapSense.h>


#include "touchsynth.h"
#include "audio.h"
#include "sinetable.h"
#include "synth.h"
#include "led.h"
#include "button.h"

#define SERIALON

// arbitrary constant to add to baseline sensing
#define CALIBRATION_OFFSET 2

typedef struct {
    CapSense *clip;
    uint8_t active;
    uint8_t led;
    uint16_t calibration;
    uint8_t shift;
    uint16_t last;
    uint16_t trigger;


} sense_t; 

#define NUMCLIPS 4
#define SAMPLES 10

sense_t clips[NUMCLIPS];

byte notes[] = { 34, 38, 42, 46, 50, 54, 58, 62, 66, 70, 74, 78, 82, 86, 90, 94, 98, 102, 106, 110, 114, 116, 118, 120, 24, 26, 28, 30, 32, 34, 36, 38 };

// Instantiate clips
CapSense clip3 = CapSense(13, 5);
CapSense clip2 = CapSense(9, 10);
CapSense clip1 = CapSense(6, 8);
CapSense clip0 = CapSense(4, 12);

CapSense *sensors[] = {
    &clip0,
    &clip1,
    &clip2,
    &clip3,
};

uint8_t next_sample = 0;

void setup() {


    cli();
    audio_init();
    synth_init();
    button_init();
    synth_set_amplitude(255);
    sei();
    
    #ifdef SERIALON
    Serial.begin(115200);
    #endif

    for(byte i=0; i < NUMCLIPS; i++) {
        clips[i].clip = sensors[i]; 
        clips[i].active = 0;
        clips[i].shift = i << 3; // multiply by eight 
        clips[i].calibration = 0;  
        clips[i].trigger = 0; 
        clips[i].led = i + 3; 
    }

    /* wtf
    PCICR = ~(1 << PCIE0);
    PCMSK0 = (1 << PCINT3);
    PCICR = (1 << PCIE0);
    PCIFR = 1 << PCIF0;
    */
    calibrate();
    calibrate();
    calibrate();
}


void calibrate() {
    for(int i=0; i < NUMCLIPS; i++) {
        clips[i].calibration = (clips[i].clip->capSense(SAMPLES) * CALIBRATION_OFFSET) ;
    }
}

unsigned long last_power = 0;
#define LOCKOUT 1000

uint8_t active = 1;

uint8_t notes_i = 0;
uint8_t notes_mask = 7;

void loop() {

    long sense;


    Serial.println(millis());
    Serial.println(last_power);
    if((button_status() == HOLD) && ((millis() - last_power) > LOCKOUT)) {
        setLED(ALERT, HIGH);
        #ifdef SERIALON
        Serial.println("GOING FOR SLEEP");
        delayMicroseconds(1000);
        #endif

        cli();

        // configure interrupt for wake-up
        PCICR = ~(1 << PCIE0);
        PCMSK0 = (1 << PCINT3);
        PCICR = (1 << PCIE0);
        PCIFR = 1 << PCIF0;

        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        setLED(ALERT, LOW);
        sleep_enable();
        sei();
        sleep_cpu();
        sei();
        last_power = millis();
    }

    setLED(PWR, HIGH);
    
    for(int i=0; i < NUMCLIPS; i++) {

        sense = clips[i].clip->capSense(SAMPLES);  
        //sense = random(0, 102); 
        clips[i].last = sense;

        //if(sense > clips[i].calibration) {
        if(sense > 100) {
          
            setLED(clips[i].led, HIGH);
            clips[i].trigger = 0;
            synth_play_note(notes[notes_i + clips[i].shift]); 
            clips[i].active = 1;
            
        } else if(clips[i].active) {
            
            clips[i].trigger += 1;
            
            if(clips[i].trigger > 5) {

                setLED(clips[i].led, LOW);

                clips[i].active = 0; 
                notes_i = (notes_i + 1) & notes_mask;
                synth_set_mod_ratio(notes_i, 1);
                synth_stop_note();

            }
            
        }
    }   

    #ifdef SERIALON
    char buffer [50];
//    sprintf(buffer, "%d, %d, %d, %d", clips[0].last, clips[1].last, clips[2].last, clips[3].last);
//    Serial.println(buffer);
    Serial.flush();
    #endif

}

ISR(PCINT0_vect) {
     
    // disable 
    cli();
    PCICR = ~(1 << PCIE0);
    PCMSK0 = ~(1 << PCINT3);
    PCIFR = (1 << PCIF0);
    sleep_disable();
    sei();

}
