
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))
_Bool state = false;
_Bool state2 = false;
_Bool pressed =true;
_Bool pressed2 =true;
uint8_t ui8LED = 2;
int sw2Status = 0;
unsigned char detectKeyPress();

void switchPinConfig(void)
{
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0|GPIO_PIN_4;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0,GPIO_DIR_MODE_IN); // Set Pin-4 of PORT F as Input. Modifiy this to use another switch
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}

int main(void)
{
	uint32_t ui32Period;
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ui32Period = (SysCtlClockGet() / 10) / 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);
	switchPinConfig();
	while(1)
	{

	}
}
//========================SW!===============================================
void idle(void){

	if(!pressed){
	if(ui8LED == 8){
		ui8LED = 4;
	}
	else if(ui8LED == 2){
		ui8LED = 8;
	}
	else if(ui8LED == 4){
			ui8LED = 2;
	}
	pressed=true;
	}

}
void press(void){
	state = true;
}
void release(void){
	if(pressed){
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
		pressed = false;
	}
}
// ====================SW@===========================================
void idle2(void){

	if(!pressed2){
		pressed2=true;
	}

}
void press2(void){
	 state2 = true;
}
void release2(void){
	if(pressed2){
		sw2Status++;
		pressed2 = false;
	}
}

//**************************************************************************************
unsigned char detectKeyPress(void)
{
	unsigned char flag =0;
	if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00){
		if(!state){//state==0
			 press();

		}
		else if(state){
			release();
			flag =1;
		}
	}
	else{
		idle();
		 state = false;
	}

	if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){
			if(!state2){//state==0
				 press2();

			}
			else if(state2){
				release2();
				flag =1;
			}
		}
		else{
			idle2();
			 state2 = false;
		}


	return flag;
}

void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	// Read the current state of the GPIO pin and
	// write back the opposite state
	 detectKeyPress();

}
