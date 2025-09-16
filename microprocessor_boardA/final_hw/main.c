/////////////////////////////////////////////////
						/*pin settings*/
/* L_SENSOR	: PA8 rising edge trigger */
/* R_SENSOR	: PB6 rising edge trigger */
/* EMERG 		: PA0 rising edge trigger (toggle)*/
//////////////////////////////////////////////////

#include<stm32f10x.h>

int main(void){
	RCC->APB2ENR = 0x81D; //TIM1, PC,PB,PA, AFIO enable
	RCC->APB1ENR = 0x7;		//TIM2,3,4 enable
	
	GPIOC->CRL = 0x33333333; //PC output push pull
	
	TIM2_setting();
	TIM1_setting();
	L_sensor_setting();
	TIM4_setting();
	TIM3_setting();
	R_sensor_setting();
	emergency_setting();
	
	while(1);
}
