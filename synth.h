#ifndef __synth_h_
#define __synth_h_

typedef struct {
    uint16_t carrier_inc;
    uint16_t carrier_pos;
    uint8_t released;
} synth_channel_t;

extern synth_channel_t channels[NUMCLIPS];


void synth_init(void);
void synth_enable(void);
void synth_disable(void);
void synth_set_amplitude(uint8_t amplitude);
void synth_set_mod_ratio(uint16_t mod_ratio_numerator, uint16_t mod_ratio_denominator); 
void synth_stop_note(synth_channel_t *channel); 
void synth_play_note(synth_channel_t *channel, uint16_t note); 
void synth_generate();

#endif // __synth_h_
