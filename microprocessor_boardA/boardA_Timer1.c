#include <stm32f10x.h>
extern int time;
extern int transfer_num;
extern u16 seq;

void boardA_timer_setting (void) {	
	RCC->APB2ENR |= 0x800;		// tim1 clk enable
	SCB->AIRCR = 0x05FA0400;		//3bit for priority group 1bits for subpriority
	TIM1->CR1 = 0x04;					// interrupt only at overflow
	TIM1->CR2 = 0x00;					// no use in cr2
	TIM1->DIER |= 0x01;				// UIE enable
	TIM1->PSC = 7199;					// 
	TIM1->ARR = 9999;					// 72M / (7200 * 10000) = 1hz
	NVIC->ISER[0] |= (1<<25); // tim1_up_IRQHandler
	NVIC->IP[25] = 0x40;			//priority group 2
	TIM1->CR1 |= 0x0001; 			//TIM1 enable for time count
}

void TIM1_UP_IRQHandler (void) {
		// timer1 update by 1sec
		if ((TIM1->SR & 0x1) != 0){ // by update event in SR
			time++; 					// time+1
			if(time >= 86400) time = 0;
			
		//usart start
			while(transfer_num != 0);
			USART1->CR1 |= 0x2008; //USART EN
			USART1->DR = 0xFF; //start byte for cts interrupt
			
			//ADC start
			seq = 0;
			ADC1->CR2 |= (1<<22);    // swstart = 1 ==> converting start
			
			TIM1->SR &= ~0x1;
//			TIM1->CR1 &= 0xFFFE;//tim1 disenable	
		}
}
