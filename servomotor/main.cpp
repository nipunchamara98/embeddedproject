#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "USART.h"

volatile uint16_t timeri=0;
volatile uint16_t timerj=0;
volatile uint16_t timerk=0;
volatile uint8_t servo=0;

#define DDRulrtra DDRD
#define PORTultra PORTD
#define PINultra PIND
#define TRIG1 7
#define TRIG2 6
#define ECHO1 2
#define ECHO2 3

char txdata[20];

uint8_t range[4]={90,120,120,150};

int main(void)
{
	USART_Init(9600);
	USART_TxStringln("Started");
	DDRulrtra|=(1<<TRIG1)|(1<<TRIG2); //D7 as output
	DDRA=0xFF;
	
	TCCR0|=(1<<WGM01);
	TCCR0|=(1<<CS00);
	OCR0=5;
	
	MCUCR|=(1<<ISC00);
	MCUCR|=(1<<ISC10);
	GICR|=(1<<INT0)|(1<<INT1);
	
	TCCR1B|=(1<<WGM12);
	TCCR1B|=(1<<CS11)|(1<<CS10);
	TCNT1=0;
	OCR1A=100;
	TIMSK|=(1<<OCIE1A);
	
	sei();
	
	while (1)
	{
		
		
		
		PORTultra|=(1<<TRIG1);
		_delay_us(10);
		PORTultra&=~(1<<TRIG1);
		
		
		_delay_ms(2);
		sprintf(txdata,"%u",timerj);
		USART_TxString(txdata);
		
		
		PORTultra|=(1<<TRIG2);
		_delay_us(10);
		PORTultra&=~(1<<TRIG2);
		
		USART_TxString(" ");
		
		_delay_ms(2);
		sprintf(txdata,"%u",timerk);
		USART_TxStringln(txdata);
		
		_delay_ms(1);
	}
}

ISR(TIMER0_COMP_vect){
	timeri++;
}

ISR(INT0_vect){
	if (PINultra&(1<<ECHO1))
	{timeri=0;
	timerj=0;
	TIMSK|=(1<<OCIE0);
	} 
	else
	{
		timerj=timeri;
		TIMSK&=~(1<<OCIE0);
	}
}

ISR(INT1_vect){
	if (PINultra&(1<<ECHO2))
	{timeri=0;
		timerk=0;
		TIMSK|=(1<<OCIE0);
	}
	else
	{
		timerk=timeri;
		TIMSK&=~(1<<OCIE0);
	}
}



ISR(TIMER1_COMPA_vect){
	servo++;
	if (servo>200)
	{servo=0;
		PORTA=(0b111);
	}
	
	if (servo==15)
	{	
		if ((timerj>range[0])&&(timerj<range[1]))
		{PORTA&=~(1<<0);
		}
		if ((timerk>range[2])&&(timerk<range[3]))
		{PORTA&=~(1<<1);
		}
		
		
	}
	
	
	
	 
	if (servo==17)
	{
		if (!((timerj>range[0])&&(timerj<range[1])))
		{PORTA&=~(1<<0);
		}
		if (!((timerk>range[2])&&(timerk<range[3])))
		{PORTA&=~(1<<1);
		}
		
		
	}
	
	
	
	TCNT1=0;
	
}