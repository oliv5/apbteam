#include <types.h>
// #include <zdo.h>
// #include <peer2peer.h>
#include <irq.h>
#include "sensors.h"
// #include <network.h>
// #include <serialInterface.h>
// #include <stdio.h>
// #include <debug.h>

/* External variables */ 
// extern APS_DataReq_t test;
// extern HAL_UsartDescriptor_t appUsartDescriptor; 
// extern uint8_t usartTxBuffer[APP_USART_TX_BUFFER_SIZE];

// extern unsigned int top_tour;
// extern unsigned int last_tour_time;
// extern int activation_asserv;
// extern int nb_top;
// extern long last_seen_laser;

int consigne_motor_ms = 100;
int security_ils = 1;
int old_top_tour = 0;
long angle = 0;
unsigned int status_temps_tour = 0;
unsigned int top_laser;
unsigned int counter_top_tour;


/********************************************************/
void init_sensors(void)
{
// 	HAL_RegisterIrq(IRQ_7,IRQ_RISING_EDGE,irq_laser);
}

void init_ILS(void)
{
 	PORTE = (1<<PE6);
 	HAL_RegisterIrq(IRQ_6,IRQ_RISING_EDGE,irq_ILS);
 	PORTE = (1<<PE6);
 	HAL_EnableIrq(IRQ_6); 
	PORTE = (1<<PE6);	
}

void irq_ILS(void)
{
// // 	HAL_DisableIrq(IRQ_6); 
// 	unsigned int temps_ms;
// 	if(security_ils == 1)
// 	{
// 		PORTD=~PORTD;
// 		security_ils = 0;
// 		temps_ms = calcul_temps();
// 		if(activation_asserv == 1)
// 		{
// 			regul_vitesse(temps_ms);
// 		}
// 		angle = last_seen_laser * 360 / counter_top_tour;
// 		counter_top_tour = 0;
// 		
// // 		SYS_PostTask(APL_TASK_ID); //For angle sending 
// 	}
// // 	HAL_EnableIrq(IRQ_6); 
}

int calcul_temps()
{
	long temps = 0;
// 	temps = counter_top_tour; 	//nb de top
// 	temps = temps * 128; 		//temps en us depuis le dernier tours
// 	temps = temps / 1000; 		//temps en ms depuis le dernier tours
// 	status_temps_tour = temps;
	return temps;
}

void regul_vitesse(int temps)
{
	int diff = 0;
	int correction = 0;
	
 	diff = consigne_motor_ms - temps;

	correction = OCR3A - diff;
	
	if(correction < 0)
	{
		OCR3A = 0;
	}
	else if(correction > 0xd5)
	{
		OCR3A = 0xd5;
	}
	else
	{
		OCR3A = correction;
	}
}

void init_ic(void)
{
	TCCR3A|=(1<<COM3A1)|(1<<COM3A0);
	TCCR3B|=(1<<CS30)|(1<<ICNC3)|(1<<ICES3);
	TIMSK3|= (1<<ICIE3) | (1<<TOIE3); 
	TIFR3|=(1<<ICF3);
	sei(); 
}

void irq_laser(void)
{
// 	HAL_DisableIrq(IRQ_7);
// 	top_laser=top_tour;
// 	nb_top++;
//  	sprintf(usartTxBuffer,"%d\r\n",nb_top);
//  	WRITE_USART(&appUsartDescriptor,usartTxBuffer,5);
// //  	PORTD=~(PORTD);
// // 	_delay_ms(50);
//  	appState = APP_NETWORK_JOINED_STATE;
//   	SYS_PostTask(APL_TASK_ID);      // Execute next step
}

void control_motor_ms(int value)
{
// 	activation_asserv = 1;
// 	OCR3A = 0x60;
// 	consigne_motor_ms = value;
}
