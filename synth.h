#ifndef __synth_h_
#define __synth_h_

void synth_init(void);
void synth_clear(void);
void synth_enable(void);
void synth_disable(void);
void synth_set_amplitude(uint8_t amplitude);
void synth_set_mod_ratio(uint16_t mod_ratio_numerator, uint16_t mod_ratio_denominator); 
void synth_stop_note(void); 
void synth_play_note(uint16_t note); 
void synth_generate(uint16_t note);

#endif // __synth_h_
