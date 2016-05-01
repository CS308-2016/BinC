#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"driverlib/gpio.h"
#include"driverlib/pin_map.h"
#include"driverlib/sysctl.h"
#include"driverlib/uart.h"
#include"driverlib/adc.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include"inc/hw_ints.h"

uint32_t ui32ADC0Value[4];
volatile uint32_t ui32TempAvg;
volatile uint32_t ui32TempValueC;
volatile uint32_t ui32TempValueF;

bool setMode=false;
volatile uint32_t setTemp=30;
void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
	//
	// Loop while there are more characters to send.
	//
	while(ui32Count--)
	{
		//
		// Write the next character to the UART.
		//
		UARTCharPut(UART0_BASE, *pui8Buffer++);
	}
}
unsigned char num[4];
void send_u32(uint32_t n) {
	num[0] = (n%10) + 48;
	n/=10;
	num[1] = (n%10) + 48;
	n/=10;
	num[2] = (n%10) + 48;
	n/=10;
	num[3] = (n%10) + 48;
	UARTCharPut(UART0_BASE, num[1]);
	UARTCharPut(UART0_BASE, num[0]);
}
void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
}

void ledPinConfig(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);  // Pin-1 of PORT F set as output. Modifiy this to use other 2 LEDs.
}

void timerConfig(void)
{
	uint32_t ui32Period;
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ui32Period = (SysCtlClockGet() ) ;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);
}
void UARTConfig(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

}

void ADCConfig(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);
}
int main(void) {
	setup();
	UARTConfig();
	ADCConfig();
	ledPinConfig();
	timerConfig();

	while(1)
	{
		if(setMode){
			int newTemp;
			newTemp=0;
			char* prompt = "Enter the temperature: ";
			UARTSend((uint8_t *)prompt, 23);
			bool entered=false;
			while(!entered) //loop while there are chars
			{
				unsigned char p=UARTCharGet(UART0_BASE);
				if(p==13) {						//pressed return
					entered=true;
					setTemp=newTemp;
					UARTCharPut(UART0_BASE, '\n');
				}
				else if(p<58&&p>47){			// is a digit
					newTemp=newTemp*10+p-48;
					UARTCharPut(UART0_BASE, p);
				}
			}
			char* update = "Set Temperature updated to ";
			UARTSend((uint8_t *)update, 27);
			send_u32(setTemp);

			UARTCharPut(UART0_BASE, 167);
			UARTCharPut(UART0_BASE, 'C');
			UARTCharPut(UART0_BASE, '\n');
			setMode=false;
			IntMasterEnable();

		}
	}
	//while (1)
	//{
	//if (UARTCharsAvail(UART0_BASE)) UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
	//}
}


void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	ADCIntClear(ADC0_BASE, 1);
	ADCProcessorTrigger(ADC0_BASE, 1);

	while(!ADCIntStatus(ADC0_BASE, 1, false))
	{
	}

	ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
	ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
	ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
	ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;

	if(ui32TempValueC<setTemp) {GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 8);}
	else  {GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 2);}

	char* arr = "Current Temperature = ";
	UARTSend((uint8_t *)arr, 22);
	send_u32(ui32TempValueC);

	UARTCharPut(UART0_BASE, 167);
	UARTCharPut(UART0_BASE, 'C');
	UARTCharPut(UART0_BASE, ',');

	char* setTempStr = " Set Temp = ";
	UARTSend((uint8_t *)setTempStr, 12);
	send_u32(setTemp);

	UARTCharPut(UART0_BASE, 167);
	UARTCharPut(UART0_BASE, 'C');
	UARTCharPut(UART0_BASE, '\n');

}


void UARTIntHandler(void)
{
	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status
	UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts
	while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
	{
		char p=UARTCharGetNonBlocking(UART0_BASE);
		//UARTCharPutNonBlocking(UART0_BASE, p);
		//echo character
		if(p=='S') {IntMasterDisable(); setMode=true;}
	}
}
