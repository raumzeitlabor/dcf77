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
void INT0_falling(); //schaltet den INT0 auf falling flank detection
void INT0_rising(); //schaltet den INT0 auf rising flank detection
uint8_t signal; //hier steht drin, was der Empfänger findet
uint16_t time_rising; //darin wird die zeit der steigenden Flanke notiert
uint16_t time_diff; //darin wird die zeit der fallenden Flanke notiert
uint8_t has_fallen; //ein bool, ob er gerade gefallen ist oder nicht
uint8_t last_bit; //hier steht die 1 wenn wir eine gefunden haben.

int main(void)
{
  DDRC = 0xFF; //Port C auf Output setzen
  timer1_on(); //schalte den Timer für den 1Hz Interrupt an 
  INT0_on();
  INT0_rising();
  sei();

  while(42){
  
  }
}

SIGNAL(SIG_OUTPUT_COMPARE1A){ //Timerinterrupt für LEDs
  if (has_fallen == 1){
   if (last_bit == 1){
    signal = (signal << 1);
    last_bit = 0;
   }
   else
    signal = (signal << 0);  
  }
  //PORTC = signal;
}

ISR(INT0_vect){
  switch(ISC00){ //wir schauen ob wir gerade auf fallende oder steigende flanke achten
    case 1: //steigende Flanke hat den Interrupt ausgelost
      has_fallen = 0;
      time_rising = TCNT1;
      INT0_falling();
      PORTC = 0x0F;
      break;
    case 0: //fallende Flanke hat den Interrupt ausgelöst      
      if (TCNT1 >= time_rising)
        time_diff = TCNT1 - time_rising;
      else
        time_diff = time_rising - TCNT1;
      if ((time_diff > 3000) && (time_diff < 3250)){
        last_bit = 1;
      }
      has_fallen = 1;
      INT0_rising();
      PORTC = 0xF0;
      break;
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
}

void INT0_rising(){
  MCUCR |= (1 << ISC00);
  MCUCR |= (1 << ISC01);
  }
  
void INT0_falling(){
  MCUCR |= (0 << ISC00);
  MCUCR |= (1 << ISC01);
  }