/**
 *  Ein kleiner Empfänger für DCF77
 *  
 *  Die empfangenen Bits werden auf den LEDs angezeigt
 */
 
#define BAUD 19200
#include <util/setbaud.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>

void uart_puts();
uint16_t int_begin;
uint16_t signaldauer;
uint8_t signal;
uint8_t debug;


int main(void) {
    DDRC = 0xFF; //Port C auf Output setzen
    DDRD = 0xC0; //ersten beiden pins auf ausgang, rest eingang
    DDRD |= (1 << PD3);
    PORTD |= (1 << PD3);
  
    debug = 0;
    signal = 2;
  
    sei(); // interrupts aktivieren
  
    /* initialize timer0 (10ms) */
    TCCR0 |= (1 << WGM01) | (0 << WGM00);  // ctc modus
    TCCR0 |= (1 << CS02) | (0 << CS01) | (0 << CS00); // 256 prescaler
    TIMSK |= (1 << OCIE0); // interrupt aktivieren
    OCR0 = (uint8_t) 624; // 625 = 10ms
 
    /* initialize timer1 (1s) */
    TCCR1B |= (1 << WGM12); //CTC Modus
    TCCR1B |= (1 << CS12) | (1 << CS10); // 1024 prescaler
    TIMSK |= (1 << OCIE1A); // Interrupt auslösen wenn CTC-Match
    OCR1A = (uint16_t) 15624; //Schwellenwert für CTC -> Interrupt 1Hz
   
    // initialize INT1 (external interrupt)
    GICR |= (1 << INT1);
    MCUCR |= (0 << ISC11);
    MCUCR |= (1 << ISC10);
  
    // initialize UART
    UCSRB |= (1 << TXEN) | (1 << TXCIE); // tx enable, tx complete irq
    UCSRB |= (1 << RXEN) | (1 << RXCIE); // rx enable, rx complete irc
    UCSRC |= (1 << UCSZ1) | (1 << UCSZ0); // 8bit character size
    UBRRH = UBRRH_VALUE; // UBRRH vor UBRRL
    UBRRL = UBRRL_VALUE;
  
    uart_puts("init\r\n");
  
    while(42) {}
}

ISR(TIMER0_COMP_vect) {
    PORTC ^= (1 << PD6); // zu schnell fürs auge, leuchtet daher nur
}

SIGNAL(SIG_OUTPUT_COMPARE1A) { //timerinterrupt für LEDs
    PORTC ^= (1 << PD7); // timer aktivität anzeigen
}

ISR(INT1_vect){
    PORTC ^= (1 << PD0);
  /*switch(INT1){
    case 1: //wenn der INT high ist
      int_begin = TCNT1;
    case 0: //wenn der INT low ist
      if (TCNT1 > int_begin) // Wenn der Timerstand größer als der zuletzt "startende" Interrupt ist
       signaldauer = TCNT1 - int_begin; //wie lange war der Interrupt?
      if (TCNT1 <= int_begin) // wenn der Timer zwischen den INTs resettet wurde
       signaldauer = int_begin - TCNT1; //wie lange war der Interrupt?
      if ((signaldauer > 1) && (signaldauer < 1650)){
       signal = 0; // wenn das Signal ungefähr 100msec war -> 0
       break;
      } 
      if ((signaldauer > 3037) && (signaldauer < 3213)){
        signal = 1; // wenn das Signal ungefähr 200msec war -> 1
        break;
        } 
        signal = 2; //wenn komisch
    }
    
    
    if (debug == 1){
      PORTC = 0x0F;
      debug = 0;
    }
    else if (debug == 0){
      PORTC = 0xF0;
      debug = 1;
    } 
    */
}

ISR (USART_TXC_vect) {
}

void uart_putc(const char c) {
    while (!(UCSRA & (1<<UDRE)));
    UDR = c;
}

void uart_puts(const char* str) {
    const char* walk;
    for (walk = str; *walk != '\0'; walk++) {
        uart_putc(*walk);
    }
}

void uart_puti(const uint16_t foo) {
  int i = 0;
  for (i = 0; i < 16; i++) {
    UDR = (foo << 1)+48;
  }
}
