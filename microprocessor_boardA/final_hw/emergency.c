#include <stm32f10x.h>
#define OPENING 4999
#define CLOSING 9999

int emergency;
void emergency_setting(void){
	GPIOA->CRL = 0x8; //PA0 as emergency button
	
	EXTI->RTSR |= 0x1;
	EXTI->IMR |= 0x1;
	AFIO->EXTICR[0] = 0x0000;
	NVIC->ISER[0] = 1<<6;
}

void EXTI0_IRQHandler(void){
	if(EXTI->PR & 0x1){
		if(emergency ==0){
			emergency = 1;
			TIM1->CR1 = 0x0;
			TIM3->CR1 = 0x0;
			TIM2->PSC = OPENING;
			TIM4->PSC = OPENING;
			TIM2->CR1 = 0x1; //TIM2 upcount: open door
			TIM4->CR1 = 0x1;
		}
		else{
			emergency = 0;
			TIM2_setting();
			TIM4_setting();
			TIM1_setting();
			TIM3_setting();
			TIM2->CR1 |= 0x10;//dir=1
			TIM4->CR1 |= 0x10;
			TIM2->PSC = CLOSING;
			TIM4->PSC = CLOSING;
			TIM2->CR1 |= 0x1; //close door
			TIM4->CR1 |= 0x1;
		}
		EXTI->PR = 1;
	}
}