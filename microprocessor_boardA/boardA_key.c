#include <stm32f10x.h>
//key scanning
// time 1(4), time_sel 2(1), temp_sel = 3(1), mode = 4(1), setting = 5(1) ,
// light 6(2), temp = 7(2), time_init = 8(1), hour_min_sec 9(1), setting_mode 10(1), time_temp 11(4)
extern int transfer_num;
extern int time;

extern u8 mode;
extern u8 setting;
extern u8 time_init;
extern u16 light;
extern u16 temp;
extern u8 time_hour_min_sec;
extern u8 setting_mode;
extern int time_temp;
extern u8 time_sel;
extern u8 temp_sel;



void keymatrix_setting(void){
	RCC->APB2ENR |= 0x00000029; //IOPDEN, IOPBEN, AFIOEN
//	AFIO->MAPR |= 0x04000000; //SWJ_CFG = 100 : disable JTAG-DP and SW-DP for PB3
	GPIOA->CRL &= 0xFFFFFFFF0; //PA0 config clear
	GPIOA->CRL |= 0x3; 	//(PA0)key_col push-pull output
	GPIOB->CRH = 0x8888u;		//(PB8-11)key_row input
	GPIOA->ODR &= 0xFFFFFFFE;						//column(PA0) low output
	GPIOB->ODR = 0x00000F00;		//row(PB8-11) input with pull-up
	
	EXTI->IMR |= 0xF00;
	EXTI->FTSR |= 0xF00;
	AFIO->EXTICR[2] |= 0x1111; //PB EXTI select
	
	NVIC->ISER[0] |= 1<<23; //EXTI9-5
	NVIC->ISER[1] |= 1<<8;		//EXTI15-10
	NVIC->IP[23]	= 0x60;
	NVIC->IP[40] 	= 0x60;

	
}

void EXTI9_5_IRQHandler(void){//mode, setting
	for(int i = 0; i<1000000; i++); //debouncing
	
	if(EXTI->PR & (1<<8) ){																	//mode button
		if(setting == 0){						//display mode
			mode++;
			if(mode == 3)mode = 0;
		}
		else{												//setting mode
			TIM2->EGR |= 0x1;		//TIM2 UG
		}
		EXTI->PR |= (1<<8);		//pending clear
	}
	
	
	if(EXTI->PR & (1<<9)){																	//setting button
		if(setting == 0){						//display mode
			setting = 1;
			TIM2->CR1 |= 0x1;		//TIM2 CEN
		}
		else {											//setting mode
			if(setting_mode == 2) {
				setting_mode = 0;
			}
			else setting_mode++;
			if(setting_mode == 1){
				time_hour_min_sec = 0;// time setting reset to hour setting
				time_temp = time;			// load current time
			}
			
			TIM2->EGR |= 0x1;		//TIM2 UG
		}
		EXTI->PR |= (1<<9);		//pending clear
	}
	
	
	while(transfer_num != 0);				//if another transfer is on processing, wait until the process end
	USART1->CR1 |= 0x2008; //USART EN
	USART1->DR = 0xFF; //start byte for cts interrupt
}

void EXTI15_10_IRQHandler(void){//up, ok
	for(int i = 0; i<1000000; i++); //debouncing
	
	if(EXTI->PR & (1<<10)){																	//up button
		if(setting ==1){
			if(setting_mode == 0){
				if(time_sel == 0) time_sel = 1;//24/12 toggle
				else time_sel = 0;
			}
			else if (setting_mode ==1){
				if (time_hour_min_sec == 0){	//hour setting
					time_temp += 3600;
					if (time >= 86400) time_temp -= 86400;
				}
				else if(time_hour_min_sec ==1){//min setting
					if(time_temp%3600 >= 59*60)time_temp -= 59*60;//min 59->0
					else time_temp += 60;
				}
				else {												//sec setting
					if(time_temp%60 >= 59) time_temp -= 59; //sec 59->0
					else time_temp += 1;
				}
				if (time_temp >= 86400) time_temp-=86400;
			}
			else {//setting mode = 2: celcius
				if (temp_sel == 0) temp_sel = 1;
				else temp_sel = 0;
			}
			TIM2->EGR |= 0x1;		//TIM2 UG
		}
		EXTI->PR |= (1<<10);
	}
		
	if(EXTI->PR & (1<<11)){																	//ok button
		if(setting == 1){
			if(setting_mode ==1){
				time = time_temp;		//save modified time
				time_init = 1;
				if(time_hour_min_sec == 0) time_hour_min_sec = 1;
				else if(time_hour_min_sec ==1) time_hour_min_sec = 2;
				else if(time_hour_min_sec == 2) time_hour_min_sec = 0;
			}
			TIM2->EGR |= 0x1;
		}
		else{										//setting ==0 : display mode
			flash_programming();
		}
		EXTI->PR |= (1<<22);
	}
	while(transfer_num !=0 );
	USART1->CR1 |= 0x2008; //USART EN
	USART1->DR = 0xFF; //start byte for cts interrupt
}


