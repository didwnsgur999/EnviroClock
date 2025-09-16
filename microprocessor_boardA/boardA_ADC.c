#include <stm32f10x.h>
extern u16 light;
extern u16 temp;
extern int transfer_num;
u16 seq;


void ADC_setting (void){
	RCC->APB2ENR |= 0x00000011; //PC, AFIO clk enable
	// PC0(channel 10): light PC1(ch 11): temperature
	GPIOC->CRL &= 0xFFFFFF00; // PC0(channel 10): light PC1(ch 11): temperature set as 0,0 analog input
	//ADC12_IN10,11
	
	RCC->APB2ENR |= 0x00000200; //ADC1 enable
	ADC1->CR1 = 0x00004820; // discnum=2-> 2conversions occur on a trigger, discen = 1 discontinous mode, EOCIE enable;
	ADC1->CR2 = 0x001E0000; // EXTTRIG = 1, EXTsel = 111 =>SW start
	ADC1->SMPR1 = 0x00000024; // SMP10,SMP11==>100 each channel 41.5cycle
	ADC1->SQR1 = 0x00100000; // L = 1--> 2 conversion
	ADC1->SQR2 = 0x00000000; // no use -> reset
	ADC1->SQR3 = 0x0000016A; // 01011 / 01010 ==> channel 10 first, channel 11 second
	NVIC->ISER[0] |= (1<<18); // num34 ADC1_2 enable
	NVIC->IP[18] = 0x20;			//priority group1
	
	ADC1->CR2 |= 0x00000001; // ADC on converting enable
}

void ADC1_2_IRQHandler (void){
		if(ADC1->SR & 0x02){ // reduced by reading DR
		// first, second divide needed by integer	
			if ( seq == 0){
			light = ADC1->DR;  //channel 10
			seq++;
			}
			else{
			temp = ADC1->DR;   //channel 11
			seq = 0;
			}
			while(transfer_num != 0);
			USART1->CR1 |= 0x2008; //USART EN
			USART1->DR = 0xFF; //start byte for cts interrupt
			ADC1->SR &= 0xFFFFFFFD;
		}
}

