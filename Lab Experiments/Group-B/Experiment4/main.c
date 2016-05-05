#include<stdint.h>
#include<stdbool.h>
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"driverlib/gpio.h"
#include"driverlib/pin_map.h"
#include"driverlib/sysctl.h"
#include"driverlib/uart.h"
#include"driverlib/debug.h"
#include"driverlib/adc.h"
#include <string.h>
void Test2(void);
void print(char str[]);
void get_temp(void);
void print_temc(void);
uint32_t ui32ADC0Value[4];
volatile uint32_t ui32TempAvg;
volatile uint32_t ui32TempValueC;
//volatile uint32_t ui32TempValueF;
char sd1 ='2';
char sd2 ='5';
char cd1;
char cd2;
void ledPinConfig(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);  // Pin-1 of PORT F set as output. Modifiy this to use other 2 LEDs.
}
int main(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_SYSDIV_5| SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	ledPinConfig();
	while (1)
	{
		Test2();
		//print_temc();
		SysCtlDelay(SysCtlClockGet()/3);

	}
}
void print_temc(void){
	print("Current Temperature ");
	get_temp();
	UARTCharPut(UART0_BASE, ui32TempValueC/10 + 48);
	UARTCharPut(UART0_BASE, ui32TempValueC%10 + 48);
	UARTCharPut(UART0_BASE, 96);
	UARTCharPut(UART0_BASE, 'C');
	UARTCharPut(UART0_BASE, ' ');
}
void get_temp(void){
	ADCIntClear(ADC0_BASE, 1);
	ADCProcessorTrigger(ADC0_BASE, 1);
	while(!ADCIntStatus(ADC0_BASE, 1, false))
	{
	}
	ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
	ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
	ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
}

void print(char str[]){
	int len = strlen(str);
	int i;
	for(i=0;i<len;i++){
		UARTCharPut(UART0_BASE,str[i]);
	}
}
void print_tc(char d1,char d2){
	UARTCharPut(UART0_BASE,d1);
	UARTCharPut(UART0_BASE,d2);
	UARTCharPut(UART0_BASE,96);
	UARTCharPut(UART0_BASE,'C');
	UARTCharPut(UART0_BASE,' ');
}

void Test2(void){
	print( " : ");
	//if (UARTCharsAvail(UART0_BASE)) UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
	if (UARTCharsAvail(UART0_BASE)){
		if(UARTCharGet(UART0_BASE) =='s'){
			print( "Enter the temperature  : ");
			sd1 = UARTCharGet(UART0_BASE);
			UARTCharPut(UART0_BASE,sd1);
			sd2 = UARTCharGet(UART0_BASE);
			UARTCharPut(UART0_BASE,sd2);
			print("Set Temperature updated to ");
			print_tc(sd1,sd2);
		}
	}
	else{
		get_temp();
		cd1 = ui32TempValueC/10 +48;
		cd2 = ui32TempValueC%10 + 48;
		if(cd1 >sd1){
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,2);
		}
		else if(cd1 == sd1){
			if(cd2 >sd2){
				GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,2);
			}
			else{
				GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,8);
			}
		}
		else{
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,8);
		}
		print("Current Temp = ");
		print_tc(cd1,cd2);
		print(", Set Temp = ");
		print_tc(sd1,sd2);
	}
}
