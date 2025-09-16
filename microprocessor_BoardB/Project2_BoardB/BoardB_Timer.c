#include <stm32f10x.h>
extern int time;
extern int last_time;
extern u32  trial;

void Timer3_setting(void){
	RCC->APB1ENR |= 0x2;	// TIM3EN
	TIM3->CR1 = 0x04; 		// over flow active only
	TIM3->CR2 = 0x00;
	TIM3->PSC = 3599;
	TIM3->ARR = 4999; 		// 0.5sec
	
	TIM3->DIER = 0x0001; 	//update enable
	NVIC->ISER[0] |= (1<<29); // TIM3 NVIC
	
	RCC->APB2ENR |= 0x4; 	//PA enable
	GPIOA->CRL = 0x00300000; //PA5 output led
	
	TIM3->CR1 |= 0x01; 		//timer start
}
// trial 3.5 second --> blink start
void TIM3_IRQHandler(void){
	if((TIM3->SR & 0x1) == 1){
		if( time != last_time ){
			GPIOA->BSRR = 0x0020; // PA5 on
			trial = 0;
		}
		else {
			if (trial > 7) { 
				if ((GPIOA->IDR & 0x20) == 0x20){ // if PA5 on
				GPIOA->BSRR = (0x0020<<16); //PA5 off
				} 
				else {GPIOA->BSRR = 0x0020;} //PA5 on
			}
			trial++;
		}
	last_time = time;
	TIM3->SR &= ~(0x1);
	}	
}
