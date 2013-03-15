#include <avr/io.h>
#include "audio.h"
#include "touch_sound.h"

/* At a system clock of 16Mhz and a signal bit depth
 * of 8, the PWM frequency will be running at 
 * 62.5 KHz. The duty cycle is varied by the value of
 * OCR1C.
 */
void audio_init(void) {
    DDRB |= (1 << PINB7);
    TCCR1A = (1 << COM1C1) | (1 << WGM10);
    TCCR1B = (1 << WGM12) | (1 << CS10);   
}

void audio_disable(void) {
    DDRB &= ~(1 << PINB7); 
    TCCR1B &= ~(0x07); // disable clock source

}

void audio_enable(void) {
    DDRB |= (1 << PINB7);  
    TCCR1B |= (1 << CS10); // enable clock source 

}

void audio_output(uint8_t next_sample) {
    OCR1C = next_sample; 
}
