#include <Arduino.h>
#include <Timer.h>

// https://www.exp-tech.de/blog/arduino-tutorial-timer
// https://wolles-elektronikkiste.de/timer-und-pwm-teil-1
// https://eleccelerator.com/avr-timer-calculator/ - Rechner

/*
8Bit Timer0 -> not usable -> millis(), micros(), delay() genutzt
16Bit Timer1
8Bit Timer2 -> used by IRRemote Library
*/

//  f = Systemclock / (N * (1 + TOP)
// 38kHz = (16MHz /(1 * (1 + 420.05)))

// N = Prescaler
// TOP = ICR1 -> Set Timer/Counter overflow flag (TOV1) when TCNT1(Compare Match) reaches TOP -> TCNT resets to 0

void Timer::Initialize()
{
  cli();

  // RESET ALL TIMER1 REGISTER
  TCCR1A = 0x00;
  TCCR1B = 0x00;
  TIMSK1 = 0x00;
  TCNT1 = 0x00;
  OCR1A = 0x00;
  OCR1B = 0x00;

  // SET TIMER1

  // TCCR1A – Timer/Counter1 Control Register A ->
  // Clear OC1A/OC1B on compare match, set OC1A/OC1B at BOTTOM (non-inverting mode)
  TCCR1A |= (1 << COM1A1) + (1 << COM1B1);

  // Fast PWM - Pulse Width Modulation -> Mode 14
  TCCR1A |= (1 << WGM11) + (0 << WGM10);
  TCCR1B |= (1 << WGM13) + (1 << WGM12);

  // TCCR1B – Timer/Counter Control Register B -> Prescaler
  TCCR1B |= (0 << CS12) + (0 << CS11) + (1 << CS10); // -> 1

  // TOP
  ICR1 = 420; //-> 38kHz

  // OCR1A – Output Compare Register 1 A
  OCR1A = 0; // PIN 9 -> Leave 0 no  PWM

  // OCR1B – Output Compare Register 1 B
  OCR1B = 210; // PIN 10 -> 50% Duty-Cycle

  DDRB |= (1 << PB2); // Port 10 Output

  sei(); // allow interrupts
}

void Timer::activatePWM()
{
  // PWM Aktivate [necessary after digitalWrite]
  // TCCR1A |= (1<<COM1A1) + (1<<COM1B1);
  TCCR1A |= 0b10100000;
}

void Timer::disablePWM()
{
  TCCR1A &= ~(0b10100000);
}