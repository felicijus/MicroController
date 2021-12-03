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

void Timer::Initialize()
{
  // cli();
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

  // RESET ALL TIMER1 REGISTER
  TCCR1A = 0x00;
  TCCR1B = 0x00;
  TIMSK1 = 0x00;
  TCNT1 = 0x00;
  OCR1A = 0x00;
  OCR1B = 0x00;

  TCCR1A = (1 << COM1A1) + (1 << COM1B1) + (1 << WGM11);
  TCCR1B = (1 << WGM13) + (1 << WGM12);
  TCCR1B |= (0 << CS12) + (0 << CS11) + (1 << CS10);
  ICR1 = 420;  //-> 38kHz
  OCR1A = 0;   // PIN 9
  OCR1B = 210; // PIN 10 -> 50% Duty-Cycle

  DDRB |= (1 << PB2);

  // sei(); // allow interrupts
}

void Timer::AttatchPWM()
{
  // PWM Aktivate [necessary after digitalWrite]
  // TCCR1A |= (1<<COM1A1) + (1<<COM1B1);
  TCCR1A |= 0b10100000;
}

void Timer::DetatchPWM()
{
  TCCR1A &= ~(0b10100000);
}