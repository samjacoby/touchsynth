#include <stdio.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <CapSense.h>

#include "touchsynth.h"
#include "audio.h"
#include "sinetable.h"
#include "synth.h"
#include "led.h"
#include "button.h"

#define SERIALON

// percentage above which to change calibration 
#define CALIBRATION_OFFSET .35 
// number of ticks to wait to debounce signal
#define SENSOR_DEBOUNCE_TICK 5

typedef struct {
    CapSense *clip;
    synth_channel_t *channel;
    uint8_t active;
    uint16_t calibration;
    uint8_t led;
    uint8_t shift;
    uint16_t last;
    uint16_t trigger;
} sense_t; 

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
    WDT_off();
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
        clips[i].channel = &channels[i]; 
        clips[i].clip->set_CS_AutocaL_Millis(500);
        clips[i].active = 0;
        clips[i].shift = i << 3; // multiply by eight 
        clips[i].trigger = 0; 
        clips[i].led = i + 3;
    }

    calibrate();
    calibrate();
    calibrate();
    calibrate();
}


void calibrate() {
    unsigned long c;

    for(int i=0; i < NUMCLIPS; i++) {
        c = clips[i].clip->capSenseRaw(SAMPLES) ;
        clips[i].calibration = c + c * CALIBRATION_OFFSET;
    }
}

unsigned long last_press = 0;
#define LOCKOUT 1000

uint8_t active = 1;

uint8_t notes_i = 0;
uint8_t notes_mask = 7;

void loop() {

    long sense;

    uint8_t b_status = button_status();

    if((b_status == TAP) && ((millis() - last_press) > LOCKOUT)) {
        setLED(ALERT, HIGH);

        #ifdef SERIALON
        Serial.println("GOING FOR SLEEP");
        delayMicroseconds(1000);
        #endif

        enable_external_interrupt();
        setLED(ALERT, LOW);
        power_down();

        last_press = millis();
    }

    if((b_status == HOLD) && ((millis() - last_press) > LOCKOUT)) {
        #ifdef SERIALON
        Serial.println("GOING FOR RESET");
        delayMicroseconds(1000);
        #endif

        last_press = millis();
        WDT_on();
    }

    setLED(PWR, HIGH);

    for(int i=0; i < NUMCLIPS; i++) {

        sense = clips[i].clip->capSenseRaw(SAMPLES);  
        clips[i].last = sense;

        if(sense > clips[i].calibration) {
          
            setLED(clips[i].led, HIGH);

            clips[i].trigger -= 1;

            if(!clips[i].active) {
                clips[i].active = 1;
                clips[i].trigger = 0;
                synth_play_note(clips[i].channel, notes[notes_i + clips[i].shift]); 
            }
            
        } else if(clips[i].active) {
            
            clips[i].trigger += 1;
            
            if(clips[i].trigger > SENSOR_DEBOUNCE_TICK) {

                setLED(clips[i].led, LOW);
                clips[i].clip->reset_CS_AutoCal(); 

                clips[i].active = 0; 
                notes_i = (notes_i + 1) & notes_mask;
                synth_set_mod_ratio(notes_i, 1);
                synth_stop_note(clips[i].channel);

            }
            
        }
    }   

    #ifdef SERIALON
    char buffer [50];
    sprintf(buffer, "%d, %d, %d, %d", clips[0].last, clips[1].last, clips[2].last, clips[3].last);
    //sprintf(buffer, "%d, %d, %d, %d", clips[0].channel->carrier_inc, clips[1].channel->carrier_inc, clips[2].channel->carrier_inc, clips[3].channel->carrier_inc);
    sprintf(buffer, "%u, %u, %u, %u", clips[0].channel->carrier_pos, clips[1].channel->carrier_pos, clips[2].channel->carrier_pos, clips[3].channel->carrier_pos);
    Serial.println(buffer);
    //sprintf(buffer, "%d, %d, %d, %d", clips[0].channel->released, clips[1].channel->released, clips[2].channel->released, clips[3].channel->released);
    //Serial.println(buffer);
    /*sprintf(buffer, "%d, %d, %d, %d", clips[0].calibration, clips[1].calibration, clips[2].calibration, clips[3].calibration);
    Serial.println(buffer);*/
    Serial.flush();
    #endif

}

ISR(INT6_vect) {
    cli();
    /*
    PCICR = ~(1 << PCIE0);
    PCMSK0 = ~(1 << PCINT3);
    PCIFR = (1 << PCIF0);
    */
    disable_external_interrupt();
    sleep_disable();
}

void enable_external_interrupt() {
    cli();
    EIMSK &= ~(1 << INT6);
    EICRB = !((1 << ISC61) | (1 << ISC60));
    EIMSK = 1 << INT6;
    EIFR = 1 << INTF6; 
}

void disable_external_interrupt() {
    EIMSK &= ~(1 << INT6);
    EIFR = 1 << INTF6; 
}
void power_down() {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sei(); // enable external interrupts for wake-up
    sleep_cpu();
}

void WDT_on(void) {
    cli();
    WDTCSR = 1 << WDE; 
    wdt_enable(WDTO_15MS);
    delay(15); // reset
    sei();
}

void WDT_off(void) {
    cli(); 
    wdt_reset();
    /* Clear WDRF in MCUSR */
    MCUSR &= ~(1<<WDRF);
    /* Write logical one to WDCE and WDE */
    /* Keep old prescaler setting to prevent unintentional time-out */
    //WDTCSR |= (1<<WDCE) | (1<<WDE); 
    /* Turn off WDT */ 
    wdt_disable();
    WDTCSR = 0x00; 
    sei();
}

