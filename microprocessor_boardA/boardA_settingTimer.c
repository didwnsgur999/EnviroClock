/*after 2seconds exit setting mode*/

#include <stm32f10x.h>
extern u8 setting;
extern int transfer_num;
extern u8 setting_mode;
extern u8 time_hour_min_sec;

void timer2_setting(){
	RCC->APB1ENR |= 0x1;		//tim2 clk enable
	TIM2->CR1 = 0x04;				//URS
	TIM2->CR2 = 0x00;
	//TIM2->SMCR  = 0x4;			//resetmode
	TIM2->DIER |= 0x01; 		//UIE
	TIM2->PSC = 7199;
	TIM2->ARR = 39999;
	
	NVIC->IP[28] 	= 0x20;		//interrupt priority higher than GPIO lower than USART
	NVIC->ISER[0]	|= (1<<28);//TIM2 interrupt no.44
	
}

void TIM2_IRQHandler(void){
	if(TIM2->SR & 0x01){
		setting = 0;
		setting_mode = 0;
		time_hour_min_sec = 0;
		TIM2->SR &= ~0x01;
	}
	TIM2->CR1 &= 0xE;				//TIM2 CEN disable
	while(transfer_num != 0);
	USART1->CR1 |= 0x2008; //USART EN
	USART1->DR = 0xFF; //start byte for cts interrupt
}