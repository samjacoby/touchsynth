#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>

#include "touch_sound.h"
#include "sinetable.h"
#include "audio.h"

// Variable to hold the next sample that will be output to the speaker
// static volatile uint8_t next_sample;
static volatile uint8_t next_sample;

// Flag indicating whether the next sample has been computed.
// static volatile uint8_t synth_ready;
static volatile uint8_t synth_ready;

// Schedule the synth
void synth_init(void) {
    PORTF = (1 << LED1);
    TCCR3A = (1 << WGM31); //CTC
    TCCR3B = (1 << CS31) | (1 << CS30);
    TIMSK3 = 1 << OCIE3A;
    OCR4A = 127;
}

ISR(TIMER3_COMPA_vect) {
    PORTF ^= (1 << LED2); 
    audio_output(next_sample);
    synth_ready = 0;
}

void synth_generate(uint16_t note) {

    uint16_t carrier_inc;
    uint16_t carrier_pos;

    uint16_t modulator_inc;
    uint16_t modulator_pos;

    uint16_t mod_ratio_numerator;
    uint16_t mod_ratio_denominator; 

    uint8_t cpos;
    uint8_t mpos;

    uint8_t modulation;

    if(synth_ready) return;

    carrier_inc = note;
    mod_ratio_numerator = 1;
    mod_ratio_denominator = 2;

    modulator_inc = carrier_inc * mod_ratio_numerator / mod_ratio_denominator;

    modulator_pos = 0;
    carrier_pos = 0;

    cpos = 0;
    mpos = 0;

    modulator_pos += modulator_inc;
    mpos += modulator_pos & SINETABLE_MASK;
    modulation = pgm_read_byte(&sinetable[mpos]);

    carrier_pos += carrier_inc;
    cpos += carrier_inc + modulation;

    next_sample = pgm_read_byte(&sinetable[cpos]);

    synth_ready = 1;
}
