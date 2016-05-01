#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

enum KeyState {IDLE,PRESS,RELEASE};
/*
 ------ Global Variable Declaration
 */

unsigned int sw2Status=0;
enum KeyState sw1State;
enum KeyState sw2State;
uint8_t ui8LED=2;


/*

 * Function Name: setup()

 * Input: none

 * Output: none

 * Description: Set crystal frequency and enable GPIO Peripherals

 * Example Call: setup();

 */
void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

/*

 * Function Name: ledPinConfig()

 * Input: none

 * Output: none

 * Description: Set PORTF Pin 1, Pin 2, Pin 3 as output.

 * Example Call: ledPinConfig();

 */

void ledPinConfig(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);  // Pin-1 of PORT F set as output. Modifiy this to use other 2 LEDs.
}

/*

 * Function Name: switchPinConfig()

 * Input: none

 * Output: none

 * Description: Set PORTF Pin 0 and Pin 4 as input. Note that Pin 0 is locked.

 * Example Call: switchPinConfig();

 */
void switchPinConfig(void)
{
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0,GPIO_DIR_MODE_IN); // Set Pin-4 of PORT F as Input. Modifiy this to use another switch
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);

}


void timerConfig(void)
{
	uint32_t ui32Period;
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ui32Period = (SysCtlClockGet() / 100) ;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);
}

int main(void)
{
	setup();
	ledPinConfig();
	switchPinConfig();
	timerConfig();

	while(1){
	}
}

bool detectSW1Press()
{
	if(sw1State==IDLE){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0) sw1State=PRESS;
	}
	else if(sw1State==PRESS){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0) {
			sw1State=RELEASE;
			return 1;
		}

		sw1State=IDLE;
	}
	else if(sw1State==RELEASE){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)!=0) sw1State=IDLE;
	}
	return 0;
}

bool detectSW2Press()
{
	if(sw2State==IDLE){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0) sw2State=PRESS;
	}
	else if(sw2State==PRESS){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0) {
			sw2State=RELEASE;
			return 1;
		}

		sw2State=IDLE;
	}
	else if(sw2State==RELEASE){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)!=0) sw2State=IDLE;
	}
	return 0;
}

void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	if(detectSW1Press()){
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
		switch(ui8LED){
		case 2:ui8LED=8;break;
		case 4:ui8LED=2;break;
		case 8:ui8LED=4;break;
		}
	}

	if(detectSW2Press()){
		sw2Status++;
	}
}
