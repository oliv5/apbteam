#include <types.h>
// #include <zdo.h>
// #include <peer2peer.h>
#include <irq.h>
#include <avr/interrupt.h>
// #include <serialInterface.h>
// #include <stdio.h>
// #include <debug.h>

/*


extern uint8_t usartTxBuffer[APP_USART_TX_BUFFER_SIZE];
extern HAL_UsartDescriptor_t appUsartDescriptor; 
extern AppState_t appState;
extern unsigned int counter_top_tour;
extern int security_ils;*/

unsigned int top_tour = 0;
unsigned int timer3_overflow = 0;
int nb_top = 0;
long last_seen_laser = 0;

void init_timer3(void)
{

	//Fpwm = f_IO / (prescaler * (1 + TOP)) = 7200 Hz. */
	OCR3A = 0; //Motor speed = 0
	
	/* Fast PWM 10bits with TOP=0x03FF */
	TCCR3A |= (1<<WGM31)|(1<<WGM30);
	TCCR3B |= (1<<WGM32);
	
	/* Prescaler = 1 */
	TCCR3B |= (1<<CS30);
	
	/* Postive Logic */
	TCCR3A |= (1<<COM3A1);
	
	/* Select ouptut */
	DDRE |= (1<<PE3);
	
	
	/* Configure Overflow and Input compare interrupts for Laser Interrupt*/
	TCCR3B |= (1<<ICNC3)|(1<<ICES3);
	TIMSK3 |= (1<<ICIE3) | (1<<TOIE3); 
	TIFR3 |= (1<<ICF3);

	/* Enable Interrupts */
	sei(); 

}

ISR(TIMER3_CAPT_vect)
{
	unsigned char sreg;
	static unsigned int old_time=0;
	unsigned int current_time;
	unsigned int diff_time;
	static unsigned int timer3_overflow_old = 0;

	/* Disable Interrupt */
	sreg = SREG;
 	cli();
	
	/* save timestamp and calcultate diff */
	current_time = ICR3;
// 	if(current_time > old_time)
// 	{
// 		diff_time =  current_time-old_time;
// 	}
// 	else
// 	{
// 		diff_time = (65535 - old_time) + current_time;
// 	}
	diff_time = current_time - old_time;
	
	/* Check if it's not a glitch, ie at least 1 timer3 overflow occured */
	/* If it's not a glitch and it's a rising edge, it's the reflector */
// 	if((TCCR3B&0x40) && ((timer3_overflow - timer3_overflow_old)  > 6))
// 	{
// 		nb_top++;
// 		sprintf(usartTxBuffer,"nb top=%d\r\n\0",nb_top);
// 		WRITE_USART(&appUsartDescriptor,usartTxBuffer,strlen(usartTxBuffer));
// 		timer3_overflow_old = timer3_overflow;
// 		old_time = current_time;
// 	}
	
	/* Invert the edge capture of the interrupt */
	if((TCCR3B&0x40))
	{
 		TCCR3B &= 0b10111111;
	}
	else
	{
		TCCR3B |= (1<<ICES3);
	}

// 	last_seen_laser = counter_top_tour;

	/* Enable interrupt */
	SREG=sreg;
	sei();
}

ISR(TIMER3_OVF_vect)
{
	static int counter = 0;
	static int counter_security_ils = 0;
	
	//each 128us
	counter++;
// 	counter_top_tour++;
// 	if(counter_top_tour == 	7815)
// 	{
// 		PORTD=~PORTD;
// 		counter_top_tour = 0;
// 		
// 	}
	if(counter == 63) //each 8ms
	{
		timer3_overflow++; //8min avant overflow
		counter = 0;
// 		if(security_ils == 0)
// 		{
// 			if(counter_security_ils > 5)
// 			{
// 				security_ils = 1;
// 				counter_security_ils = 0;
// 			}
// 			else
// 			{
// 				counter_security_ils++;
// 			}
// 		}
	}
}


