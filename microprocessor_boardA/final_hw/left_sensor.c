#include <stm32f10x.h>
#define OPENING 9999
#define CLOSING 4999

void L_sensor_setting(void){
	GPIOA->CRH  &= 0xFFFFFFF0;
	GPIOA->CRH |= 0x8;		//PA8 setting as sensor input floating
	
	EXTI->RTSR = 0x0100;
	EXTI->IMR = 0x0100;
	AFIO->EXTICR[2] = 0x0000;
	NVIC->ISER[0] = 1<<23;	//EXTI8 interrupt
}

void EXTI9_5_IRQHandler(void){
	if(EXTI->PR & 0x100){				
		if((TIM2->CR1 & 0x11)==0){	//when door is closed, start opening
			TIM2->CR1 |= 0x1;
		}
		else{																	//when door is closing, change direction
			TIM2->CR1 &= ~0x10;			//TIM2 countup DIR = 0
			TIM2->PSC = OPENING;
		}
		EXTI->PR = 0x100;				//pending clear
		TIM1->SR &= ~0x1;				//TIM1 UIF clear
	}
	if(EXTI->PR & 0x40){				
		if((TIM4->CR1 & 0x11)==0){	//when door is closed, start opening
			TIM4->CR1 |= 0x1;
		}
		else{																	//when door is closing, change direction
			TIM4->CR1 &= ~0x10;			//TIM4 countup DIR = 0
			TIM4->PSC = OPENING;
		}
		EXTI->PR = 0x040;				//pending clear
		TIM3->SR &= ~0x1;				//TIM1 UIF clear
	}
}