#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"
#include "hsv.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

//Define min and max
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))

enum KeyState {IDLE,PRESS,RELEASE};
enum Mode {AUTO, MANUAL1,MANUAL2,MANUAL3};

/*
 ------ Global Variable Declaration
 */

enum KeyState sw1State;
enum KeyState sw2State;
int keyState;
enum Mode mode=AUTO;
uint32_t ui32Period;
uint32_t ui32Load;
int redIntensity=255;
int blueIntensity=1;
int greenIntensity=1;
double transitionSpeed=30;
double hue=0;
int keyHeldTime=0;
void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}


void ledPinConfig(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);  // Pin-1 of PORT F set as output. Modifiy this to use other 2 LEDs.
}


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
	TimerEnable(TIMER0_BASE, TIMER_A);
}

void pwmConfig()
{
	volatile uint32_t ui32PWMClock;
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinConfigure(GPIO_PF1_M1PWM5);
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);
	ui32Load = SysCtlClockGet()/64000 ;
	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);
	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui32Load*redIntensity/255);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui32Load*blueIntensity/255);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui32Load*greenIntensity/255);
	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT|PWM_OUT_6_BIT|PWM_OUT_7_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_2);
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);

}

int main(void)
{
	setup();
	ledPinConfig();
	switchPinConfig();
	timerConfig();
	pwmConfig();

	while(1){

	}
}

unsigned int keyPress(){
	//0=nothing 1= manual1 2= manual2 3=manual3 4=increment 5=decrement
	if(keyState==0){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0) keyState=1;
		else if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0) keyState=8;
	}
	else if(keyState==1){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0) keyState=2;
	}
	else if(keyState==2){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)!=0) { keyState=0;return 5;}
		else if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0) keyState=3;
	}
	else if(keyState==3){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0&&GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0) {keyState=4;}
		else if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0) keyState=2;
	}
	else if(keyState==4){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)!=0&&GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)!=0) {keyState=0; keyHeldTime=0; return 1;}
		else if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)!=0) {keyState=5; keyHeldTime=0;}
		else {
			keyHeldTime++;
			if(keyHeldTime>100) {keyState=7;keyHeldTime=0;return 3;}
		}
	}
	else if(keyState==5){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)!=0) {keyState=0; return 1;}
		else if (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0) keyState=6;
	}
	else if(keyState==6){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)!=0) {keyState=0; return 1;}
		else if (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0) {keyState=7; return 2;}
	}
	else if(keyState==7){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)!=0&&GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)!=0) keyState=0;
	}
	else if(keyState==8){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0) {keyState=9;return 4;}
	}
	else if(keyState==9){
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)!=0) keyState=0;
	}
	return 0;
}


void changeColor(){
	hsv in;
	hue=(hue+transitionSpeed/100.0);
	if(hue>360.0)
		hue=hue-360.0;
	in.h=hue;
	in.v=1;
	in.s=1;
	rgb out=hsv2rgb(in);
	redIntensity=max(1,out.r*255);
	blueIntensity=max(1,out.b*255);
	greenIntensity=max(1,out.g*255);

}

void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	unsigned int action=keyPress();
	if(action==1) mode=MANUAL1;
	else if(action==2) mode=MANUAL2;
	else if(action==3) mode=MANUAL3;
	else if(action==4){
		if(mode==AUTO) transitionSpeed=transitionSpeed+1;
		else if(mode==MANUAL1)
			redIntensity=min(redIntensity+10,255);
		else if(mode==MANUAL2)
			blueIntensity=min(blueIntensity+10,255);
		else if(mode==MANUAL3)
			greenIntensity=min(greenIntensity+10,255);

	}
	else if(action==5){
		if(mode==AUTO) transitionSpeed=transitionSpeed-1;
		else if(mode==MANUAL1)
			redIntensity=max(redIntensity-10,1);
		else if(mode==MANUAL2)
			blueIntensity=max(blueIntensity-10,1);
		else if(mode==MANUAL3)
			greenIntensity=max(greenIntensity-10,1);
	}

	if(mode==AUTO) changeColor();

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, redIntensity * ui32Load / 255);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, blueIntensity * ui32Load / 255);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, greenIntensity * ui32Load / 255);

}
