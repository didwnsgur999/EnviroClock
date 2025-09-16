#include <stm32f10x.h>
#define OPENING 4999
#define CLOSING 9999
extern int emergency;

void TIM4_setting(void){
/*TIM4 setting: left door */
	//update event is not generated but can be reinitialized by UG bit
	TIM4->CR1 = 0x4;			//TIM4 update interrupt only at over/underflow
	
	//TIMx_CCMRx OCxM bits : output pin assignment
	//TIMx_CCMRx OCxPE : CCR register program with or without preload registers
	//TIMx_CCMRx CCxE : output enable
	TIM4->CCMR1 = 0x1010; //OCxREF 1,2 active high at match
	TIM4->CCMR2 = 0x1010; //OCxREF 3,4 active high at match
	
	//TIMx_CCER CCxP bit: output polarity setting
	TIM4->CCER = 0x1111; //OC1,2,3,4 active
	
	
	//TIMx_CCRx register: compare data
	TIM4->CCR1 = 900-1;
	TIM4->CCR2 = 1800-1;
	TIM4->CCR3 = 2700-1;
	TIM4->CCR4 = 3600-2;
	
	//TIMx_DIER CCXIE bit: CCX interrupt enable
	TIM4->DIER = 0x1F; //CC4,CC3, CC2, CC1, U interrupt enable
	
	TIM4->ARR = 3600 - 1;
	TIM4->PSC = OPENING;
	
	NVIC->ISER[0] = 1<<30; // TIM4 interrupt enable
	
	//TIM4->CR1 |= 1;
}

void TIM4_IRQHandler(void){
	if(TIM4->SR & 0x1){					//UIF
		if(emergency ==0){		//not EMERGENCY
		TIM4->CR1 &= 0xFFFFFFFE; //CEN disable
		
		if(TIM4->CR1 & 0x10){//when door closed completely
			TIM4->CR1 &= ~0x10;	//DIR =0: countup
			TIM3->CR1 &= ~0x1; 	//TIM3 CEN clear
			TIM4->PSC = OPENING;
		}
		else{									//when door opened completely
			TIM4->CR1 |= 0x10; 	//TIM4 DIR = 1
			TIM3->CR1 |= 0x1; 	//TIM3 CEN
			TIM4->PSC = CLOSING;
		}
	}
		
		else{										//EMERGENCY
			TIM4->CR1 = 0;		//TIM4 off
			TIM4->CR1 |= 0x10;//DIR = 1
		}
		
		TIM4->EGR |= 0x1; 	//TIM4 update
		TIM4->SR &= ~0x1;		//flag clear
	}
	
	
	if (TIM4->SR & 0x10){	//CC4IF
		if(GPIOC->ODR & 0x1){
			GPIOC->ODR &= ~0x1;
		}
		else
			GPIOC->ODR |= 0x1;
		TIM4->SR &= ~0x10;
	}
	
	
	if(TIM4->SR & 0x8){		//CC3IF
		if(GPIOC->ODR & 0x2){
			GPIOC->ODR &= ~0x2;
		}
		else
			GPIOC->ODR |= 0x2;
		TIM4->SR &= ~0x8;	// flag clear
	}
	
	if(TIM4->SR & 0x4){		//CC2IF
		if(GPIOC->ODR & 0x4){
			GPIOC->ODR &= ~0x4;
		}
		else
			GPIOC->ODR |= 0x4;
		TIM4->SR &= ~0x4;	// flag clear
	}
	
	if(TIM4->SR & 0x2){	//CC1IF
		if(GPIOC->ODR & 0x8){
			GPIOC->ODR &= ~0x8;
		}
		else
			GPIOC->ODR |= 0x8;
		TIM4->SR &= ~0x2;	// flag clear
	}
}