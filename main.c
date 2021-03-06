/**
 *  Ein kleiner Empfänger für DCF77
 *  
 *  Die empfangenen Bits werden auf den LEDs angezeigt
 */
 
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>

void timer1_on();
void INT0_on(); //schaltet nur den INT0 "an"
uint16_t int_begin;
uint16_t signaldauer;
uint8_t signal;
uint8_t debug;


int main(void)
{
  DDRC = 0xFF; //Port C auf Output setzen
  timer1_on(); //schalte den Timer für den 1Hz Interrupt an 
  INT0_on();
  sei();
  debug = 0;
  signal = 2;

  while(42){
  
  }
}

SIGNAL(SIG_OUTPUT_COMPARE1A){ //Timerinterrupt für LEDs
  switch(signal){
    case 2:
      PORTC = 0xFF;
    case 1:
      PORTC = 0x0F;
    case 0:
      PORTC = 0xF0;
  }
}

ISR(INT0_vect){
  /**
  switch(INT0){
    case 1: //wenn der INT high ist
      int_begin = TCNT1;
    case 0: //wenn der INT low ist
      if (TCNT1 > int_begin) // Wenn der Timerstand größer als der zuletzt "startende" Interrupt ist
       signaldauer = TCNT1 - int_begin; //wie lange war der Interrupt?
      if (TCNT1 <= int_begin) // wenn der Timer zwischen den INTs resettet wurde
       signaldauer = int_begin - TCNT1; //wie lange war der Interrupt?
      if ((signaldauer > 1475) && (signaldauer < 1650)){
       signal = 0; // wenn das Signal ungefähr 100msec war -> 0
       break;
      } 
      if ((signaldauer > 3037) && (signaldauer < 3213)){
        signal = 1; // wenn das Signal ungefähr 200msec war -> 1
        break;
        } 
        signal = 2; //wenn komisch
    }
    **/
    
    if (debug == 1){
      PORTC = 0x0F;
      debug = 0;
    }
    else if (debug == 0){
      PORTC = 0xF0;
      debug = 1;
    } 
    
}

void timer1_on(){
  TCCR1B |= (1 << WGM12); //CTC Modus
  TCCR1B |= (1 << CS12); // Prescaler auf 1024
  TCCR1B |= (1 << CS10); // Prescaler auf 1024
  TIMSK |= (1 << OCIE1A); // Interrupt auslösen wenn CTC-Match
  OCR1A = 15624; //Schwellenwert für CTC -> Interrupt 1Hz
}

void INT0_on(){
  GICR |= (1 << INT0);
  MCUCR |= (0 << ISC01);
  MCUCR |= (1 << ISC00);
}
