#include <stm32f10x.h>
extern int usart_err;

void Usart_error_detector(void){
	RCC->APB1ENR |= 0x2; 		//TIM3 en
	TIM3->CR2 = 0x04;					// interrupt only at overflow
	TIM3->CR2 = 0x00;					// no use in cr2
	TIM3->DIER |= 0x01;				// UIE enable
	TIM3->PSC = 7199;					// 
	TIM3->ARR = 4999;					// 36M / (7200 * 5000) = 1hz
	NVIC->ISER[0] |= (1<<29); // tim3_IRQHandler
	NVIC->IP[29] = 0x20;			//priority group 1
	TIM3->CR1 |= 0x0001; 			//TIM1 enable for time count
	
	GPIOA->CRL &= 0xFF0FFFFF;
	GPIOA->CRL |= 0x00300000;	//led for usart status PCA0 output with push-pull
	GPIOA->ODR |= 0x00000020;
}

void TIM3_IRQHandler(void){
	if(TIM3->SR &1){
		usart_err++;
		if(usart_err>= 4){
			if(GPIOA->ODR & 0x20){
				GPIOA->BSRR = 0x20<<16;
			}
			else
				GPIOA->BSRR = 0x20;
		}
		else
			GPIOA->BSRR = 0x20;
	}
	TIM3->SR &= 0x0;
}