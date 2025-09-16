#include <stm32f10x.h>
#define OPENING 4999
#define CLOSING 9999
void TIM3_setting(void){
	TIM3->CR1 = 0x4;		//Update only at over/underflow
	TIM3->CCMR1 = 0x01;	//CC1S = 01 : CC1channel as input, IC1 is mapped on TI1
	TIM3->SMCR = 0x54;	//TS = 101: filtered timer input1 SMS = 100: reset mode
	TIM3->DIER = 0x1;		//UIE
	TIM3->ARR = 3600-1;
	TIM3->PSC = 10000-1;
	
	NVIC->ISER[0] |= 1<<29; //TIM3 update interrupt en
}

void TIM3_IRQHandler(void){
	if(TIM3->SR & 0x1){//when door opened for seconds
		TIM3->CR1 &= ~0x1;	//TIM3 disable
		TIM3->EGR |= 0x1;		//TIM3 update
		TIM4->CR1 |= 0x11; //TIM2 countdown, enable
		TIM3->SR &= ~0x1;		//flag clear
	}
}