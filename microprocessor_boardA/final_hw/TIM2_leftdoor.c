#include <stm32f10x.h>
#define OPENING 4999
#define CLOSING 9999
extern int emergency;

void TIM2_setting(void){
/*TIM2 setting: left door */
	//update event is not generated but can be reinitialized by UG bit
	TIM2->CR1 = 0x4;			//TIM2 update interrupt only at over/underflow
	
	//TIMx_CCMRx OCxM bits : output pin assignment
	//TIMx_CCMRx OCxPE : CCR register program with or without preload registers
	//TIMx_CCMRx CCxE : output enable
	TIM2->CCMR1 = 0x1010; //OCxREF 1,2 active high at match
	TIM2->CCMR2 = 0x1010; //OCxREF 3,4 active high at match
	
	//TIMx_CCER CCxP bit: output polarity setting
	TIM2->CCER = 0x1111; //OC1,2,3,4 active
	
	
	//TIMx_CCRx register: compare data
	TIM2->CCR1 = 900-1;
	TIM2->CCR2 = 1800-1;
	TIM2->CCR3 = 2700-1;
	TIM2->CCR4 = 3600-2;
	
	//TIMx_DIER CCXIE bit: CCX interrupt enable
	TIM2->DIER = 0x1F; //CC4,CC3, CC2, CC1, U interrupt enable
	
	TIM2->ARR = 3600 - 1;
	TIM2->PSC = OPENING;
	
	NVIC->ISER[0] = 1<<28; // TIM2 interrupt enable
	
	//TIM2->CR1 |= 1;
}

void TIM2_IRQHandler(void){
	if(TIM2->SR & 0x1){					//UIF
		if(emergency ==0){		//not EMERGENCY
		TIM2->CR1 &= 0xFFFFFFFE; //CEN disable
		
		if(TIM2->CR1 & 0x10){//when door closed completely
			TIM2->CR1 &= ~0x10;	//DIR =0: countup
			TIM1->CR1 &= ~0x1; 	//TIM1 CEN clear
			TIM2->PSC = OPENING;
		}
		else{									//when door opened completely
			TIM2->CR1 |= 0x10; 	//TIM2 DIR = 1
			TIM1->CR1 |= 0x1; 	//TIM1 CEN
			TIM2->PSC = CLOSING;
		}
	}
		
		else{										//EMERGENCY
			TIM2->CR1 = 0;		//TIM2 off
			TIM2->CR1 |= 0x10;//DIR = 1
		}
		
		TIM2->EGR |= 0x1; 	//TIM2 update
		TIM2->SR &= ~0x1;		//flag clear
	}
	
	
	if (TIM2->SR & 0x10){	//CC4IF
		if(GPIOC->ODR & 0x80){
			GPIOC->ODR &= ~0x80;
		}
		else
			GPIOC->ODR |= 0x80;
		TIM2->SR &= ~0x10;
	}
	
	
	if(TIM2->SR & 0x8){		//CC3IF
		if(GPIOC->ODR & 0x40){
			GPIOC->ODR &= ~0x40;
		}
		else
			GPIOC->ODR |= 0x40;
		TIM2->SR &= ~0x8;	// flag clear
	}
	
	if(TIM2->SR & 0x4){		//CC2IF
		if(GPIOC->ODR & 0x20){
			GPIOC->ODR &= ~0x20;
		}
		else
			GPIOC->ODR |= 0x20;
		TIM2->SR &= ~0x4;	// flag clear
	}
	
	if(TIM2->SR & 0x2){	//CC1IF
		if(GPIOC->ODR & 0x10){
			GPIOC->ODR &= ~0x10;
		}
		else
			GPIOC->ODR |= 0x10;
		TIM2->SR &= ~0x2;	// flag clear
	}
}