#include <Arduino.h>
#include <Timer.h>

// https://www.exp-tech.de/blog/arduino-tutorial-timer
// https://wolles-elektronikkiste.de/timer-und-pwm-teil-1
// https://eleccelerator.com/avr-timer-calculator/ - Rechner

/*
8Bit Timer0 nicht nutzen da für millis(), micros(), delay() genutzt
16Bit Timer1
8Bit Timer2
*/

//  f = Systemtakt / ((OCRnx) * prescaler)
// 100000 Hz = (16000000Hz /((4+1)*32))

volatile unsigned long timer2_micros = 0;

ISR(TIMER1_COMPA_vect)
{ // Interrupt Service Routine
  timer2_micros++;
}

void Timer::Initialize()
{
  cli();
  /*
    //RESET ALL TIMER2 REGISTER
    TCCR2A = 0x00;
    TCCR2B = 0x00;
    TIMSK2 = 0x00;
    TCNT2 = 0x00;
    OCR2A = 0x00;
    OCR2B = 0x00;

    // SET TIMER2
    //CTC - Clear Timer on Compare Match
    TCCR2A = (1 << WGM21) + (0 << WGM20);
    TCCR2B = (0 << WGM22);

    //TCCR2B – Timer/Counter Control Register B
    TCCR2B = (0 << CS22) + (1 << CS21) + (1 << CS20);
    // 0 1 1  clk/32 (From prescaler)

    //OCR2A – Output Compare Register A
    OCR2A = 4;
    
    sei();
    */

  //RESET ALL TIMER2 REGISTER
  TCCR1A = 0x00;
  TCCR1B = 0x00;
  TIMSK1 = 0x00;
  TCNT1 = 0x00;
  OCR1A = 0x00;
  OCR1B = 0x00;

  OCR1A = 15;

  TCCR1A = (1 << COM1A0);                          // CTC mode, toggle OC1A on compare match
  TCCR1B |= (0 << CS12) + (0 << CS11) + (1 << CS10);
  TCCR1B |= (1 << WGM12);

  sei(); //allow interrupts
}

void Timer::AttatchInterrupt()
{
  //TIMSK2 – Timer/Counter2 Interrupt Mask Register
  TIMSK1 |= (1 << OCIE1A); //AttachInterrupt when Compare Match with OCR2A
}
void Timer::DetatchInterrupt()
{
  //TIMSK2 – Timer/Counter2 Interrupt Mask Register
  TIMSK1 &= ~(1 << OCIE1A); //DetatchInterrupt when Compare Match with OCR2A
}

unsigned long Timer::micros_10()
{

  return timer2_micros;
}