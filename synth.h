#ifndef __synth_h_
#define __synth_h_

void synth_init(void);
void synth_clear(void);
void synth_set_mod_ratio(uint16_t mod_ratio_numerator, uint16_t mod_ratio_denominator); 
void synth_generate(uint16_t note);

#endif // __synth_h_
