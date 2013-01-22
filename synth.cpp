#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>

#include "config.h"
#include "touch_sound.h"
#include "sinetable.h"
#include "audio.h"

// Variable to hold the next sample that will be output to the speaker
static volatile uint8_t next_sample;

// Flag indicating whether the next sample has been computed.
// static volatile uint8_t synth_ready;
static volatile uint8_t synth_ready;

// Schedule the synth
void synth_init(void) {
    PORTF = (1 << LED1);
    TCCR3A = (1 << WGM31) | (1 << WGM30);
    TCCR3B = (1 << WGM33) |  (1 << CS30);
    TIMSK3 = 1 << OCIE3A;
    OCR3A = 0x022C; // N = 14400

    synth_ready = 0;
}

static volatile uint8_t t_inc = 1;
static volatile uint8_t t_pos = 0;
ISR(TIMER3_COMPA_vect) {
    PORTF ^= (1 << LED2); 
    next_sample = pgm_read_byte(&sinetable[t_pos & SINETABLE_MASK]);
    audio_output(next_sample);
    t_pos += t_inc;
    //synth_ready = 0;
}



static volatile uint16_t carrier_inc;
static volatile uint16_t carrier_pos = 0;
static volatile uint16_t modulator_inc;
static volatile uint16_t modulator_pos = 0;

void synth_generate(uint16_t note) {
    uint16_t next_sample;
    uint8_t cpos = 0;

    if(synth_ready) return;

    carrier_inc = note;
    carrier_pos += carrier_inc;
    cpos = carrier_pos & SINETABLE_MASK;
    next_sample = pgm_read_byte(&sinetable[cpos]);

    synth_ready = 1;
}

void synth_generates(uint16_t note) {

    uint16_t mod_ratio_numerator;
    uint16_t mod_ratio_denominator; 

    uint8_t cpos;
    uint8_t mpos;

    uint8_t modulation;

    if(synth_ready) return;

    carrier_inc = note;
    mod_ratio_numerator = 20;
    mod_ratio_denominator = 1;

    modulator_inc = carrier_inc * mod_ratio_numerator / mod_ratio_denominator;

    cpos = 0;
    mpos = 0;

    modulator_pos += modulator_inc;
    mpos = modulator_pos & SINETABLE_MASK;
    modulation = pgm_read_byte(&sinetable[mpos]);

    carrier_pos += carrier_inc; // should this effect the frequency?
    cpos = (carrier_inc + modulation) & SINETABLE_MASK;

    next_sample = pgm_read_byte(&sinetable[cpos]);

    synth_ready = 1;
}
