#include <stm32f10x.h>
#define OPENING 4999
#define CLOSING 9999
void TIM1_setting(void){
	TIM1->CR1 = 0x4;		//Update only at over/underflow
	TIM1->CCMR1 = 0x01;	//CC1S = 01 : CC1channel as input, IC1 is mapped on TI1
	TIM1->SMCR = 0x54;	//TS = 101: filtered timer input1 SMS = 100: reset mode
	TIM1->DIER = 0x1;		//UIE
	TIM1->ARR = 3600-1;
	TIM1->PSC = 10000-1;
	
	NVIC->ISER[0] |= 1<<25; //TIM1 update interrupt en
}

void TIM1_UP_IRQHandler(void){
	if(TIM1->SR & 0x1){//when door opened for seconds
		TIM1->CR1 &= ~0x1;	//TIM1 disable
		TIM1->EGR |= 0x1;		//TIM1 update
		TIM2->CR1 |= 0x11; //TIM2 countdown, enable
		TIM1->SR &= ~0x1;		//flag clear
	}
}