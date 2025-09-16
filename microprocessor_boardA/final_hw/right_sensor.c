#include <stm32f10x.h>
#define OPENING 4999
#define CLOSING 9999

void R_sensor_setting(void){
	GPIOB->CRL  &= 0xF0FFFFFF;
	GPIOB->CRL |= 0x08000000;		//PB6 setting as sensor input floating
	
	
	EXTI->RTSR |= 0x0040;
	EXTI->IMR |= 0x0040;
	AFIO->EXTICR[1] |= 0x0100;
	NVIC->ISER[0] = 1<<23;	//EXTI5-9 interrupt
}

/* included in left_sensor.c
void EXTI9_5_IRQHandler(void){
	if(EXTI->PR & 0x40){				
		if((TIM4->CR1 & 0x11)==0){	//when door is closed, start opening
			TIM4->CR1 |= 0x1;
		}
		else{																	//when door is closing, change direction
			TIM4->CR1 &= ~0x10;			//TIM4 countup DIR = 0
			TIM4->PSC = OPENING;
		}
		EXTI->PR = 0x100;				//pending clear
		TIM3->SR &= ~0x1;				//TIM1 UIF clear
	}
}
*/