#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>
#define F_CPU 9600000UL

volatile uint8_t beep_disable = 0;
volatile uint8_t beep_enable  = 0;

static void set_wait_timer (void);
void set_beep_timer (uint8_t outport);


// main function
int main (void)
{

  DDRB = (1<<DDB1) | (1<<DDB0);  // set PB0 & PB1 to output

  set_sleep_mode(SLEEP_MODE_IDLE);  // set sleep mode

  set_beep_timer(0);  // generate signal with beeper frequency

  while (beep_disable == 0) {} // wait for expired beep counter

  set_wait_timer();  // start timer to wait 600 secs

  // wait for expired wait counter
  while (beep_enable == 0) {
    sleep_mode();  // goto sleep until next interrupt
  }

  // endless loop until switch-off
  while(1) {
    set_beep_timer(0);  // generate signal with beeper frequency
    while (beep_disable == 0) {}
    set_beep_timer(1);  // generate signal with beeper frequency
    while (beep_disable == 0) {}
  }

  return 0;
}


// routine to set timer0 for waiting until beep
static void set_wait_timer (void)
{
  cli();
  TCCR0A = 0x00;  // set timer0 to normal mode 
  TCCR0B = (1<<CS02) | (1<<CS00);  // set timer0 prescaler to 1024
  TIMSK0 = (1<<TOIE0);  // interrupt enable for timer0 overflow
  sei();
  return;
}


// routine to set timer0 for to generate frequency for beeper
void set_beep_timer (uint8_t outport)
{
  cli();
  if (outport == 0) {
    TCCR0A = (1<<COM0A0) | (1<<WGM01);  // toggle OC0A on compare match , ctc mode
  } else {
    TCCR0A = (1<<COM0B0) | (1<<WGM01);  // toggle OC0B on compare match , ctc mode
  }
  TCCR0B = (1<<CS00);    // set timer0 prescaler to 1 (no prescaling)
  OCR0A  = 0xE7;         // set output compare register a
  TIMSK0 = (1<<OCIE0A);  // interrupt enable for timer0 compare match a
  sei();
  return;
}


// interrupt routine for timer0 output compare match a
ISR(TIM0_COMPA_vect)
{
  static uint16_t overflow = 0;
  // after count up to 1 second
  // deactivate beeper & disable timer0 output compare match a interrupt
  if (overflow <= 10920) {
    beep_disable = 0;
    overflow ++;
  } else {
    TIMSK0       &= ~(1<<OCIE0A);  // interrupt disable for timer0 compare match a
    beep_disable = 1;
    overflow     = 0;
  }
}


// interrupt routine for timer0 overflow
ISR(TIM0_OVF_vect)
{
  static uint16_t overflow = 0;
  // after count up to 1200 seconds
  // activate beeper & disable timer0 overflow interrupt
  if (overflow <= 6000) {  // for test: 30 ca. 6 sek.
    overflow ++;
  } else {
    beep_enable = 1;
    TIMSK0      &= ~(1<<TOIE0);  // interrupt disable for timer0 overflow
  }
}

