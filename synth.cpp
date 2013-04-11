#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>

#include "config.h"
#include "touchsynth.h"
#include "synth.h"
#include "sinetable.h"
#include "audio.h"

// store all of the channels we'll be playing with
synth_channel_t channels[NUMCLIPS];

// next note that will be generated by the synth 
static volatile uint8_t next_note;

// next sample that will be output to the speaker
static volatile uint8_t next_sample;

// flag indicating whether the next sample has been computed.
static volatile uint8_t synth_ready;

// flag indicating whether or not the synth is active
static volatile uint8_t synth_active;

// Schedule the synth
void synth_init(void) {

    TCCR3A = (1 << WGM31) | (1 << WGM30);
    TCCR3B = (1 << WGM33) |  (1 << CS30);
    TIMSK3 = 1 << OCIE3A;
    OCR3A = 0x022C; // N = 14400

    // initialize channels
    for(int i=0; i < NUMCLIPS; i++) {
        channels[i].carrier_inc = 0;
        channels[i].carrier_pos = 0;
        channels[i].released = 1;
    }

    synth_ready = 1; //get the ball rolling

}

void synth_enable(void) {
    synth_active = 0;
}

void synth_disable(void) {
    synth_active = 1;

}

static volatile uint8_t t_inc = 1;
static volatile uint8_t t_pos = 0;
volatile static uint16_t last_note;

static volatile uint16_t carrier_inc;
static volatile uint16_t carrier_pos = 0;
static volatile uint16_t modulator_inc;
static volatile uint16_t modulator_pos = 0;
static volatile uint8_t next_amplitude = 20;

static volatile uint16_t mod_ratio_numerator = 0;
static volatile uint16_t mod_ratio_denominator = 1; 

void synth_set_amplitude(uint8_t amplitude) {
    next_amplitude = amplitude;
}

void synth_set_mod_ratio(uint16_t mod_ratio_numerator_v, uint16_t mod_ratio_denominator_v) {
    mod_ratio_numerator = mod_ratio_numerator_v;
    mod_ratio_denominator = mod_ratio_denominator_v;
}

void synth_play_note(synth_channel_t *channel, uint16_t note) {
    Serial.println("###### PLAY ##");
    Serial.println(note);
    channel->carrier_inc = note;
    channel->carrier_pos = 0;
    channel->released = 0;
}

void synth_stop_note(synth_channel_t *channel) {
    channel->released = 1;
}

void synth_generate(void) {

    synth_ready = 0;

    uint16_t cpos = 0;
    uint16_t sample = 0;
    uint8_t mixed_samples = 0;
    synth_channel_t *chan;

    for(int i=0; i < NUMCLIPS; i++) {
        chan = &channels[i];
        if(chan->released) continue;
        chan->carrier_pos += chan->carrier_inc;
        cpos = chan->carrier_pos & SINETABLE_MASK; 
        //sample = (pgm_read_byte(&sinetable[cpos]) * next_amplitude) >> 8;
        //mixed_samples += (sample >> 8);
        sample = (uint16_t)pgm_read_byte(&sinetable[cpos]);
        mixed_samples += (sample >> 2);
    }

    if(mixed_samples == 0) {
        audio_disable();
    } else {
        audio_enable();
    }

    next_sample = (mixed_samples  & 0xFF);
    synth_ready = 1;
}



void synth_generate_x(uint16_t note) {

    uint16_t cpos = 0;
    uint16_t mpos = 0;

    uint16_t modulation;

    carrier_inc = note;

    modulator_inc = carrier_inc * mod_ratio_numerator / mod_ratio_denominator;


    modulator_pos += modulator_inc;
    mpos = modulator_pos & SINETABLE_MASK;
    modulation = pgm_read_byte(&sinetable[mpos]);

    carrier_pos += carrier_inc; // should this effect the frequency?
    cpos = (carrier_pos + modulation) & SINETABLE_MASK;

    next_sample = (pgm_read_byte(&sinetable[cpos]) * next_amplitude) >> 8;

    last_note = note;
}

ISR(TIMER3_COMPA_vect) {
    if(synth_ready) synth_generate();
    audio_output(next_sample);
}
