#include <stm32f10x.h>

// time 1, time_sel 2, temp_sel = 3, mode = 4, setting = 5 ,light 6, temp = 7, time_init = 8
extern int time;		// time as seconds
extern u8 time_sel;	// 24 or 12 time
extern u8 temp_sel;	// Celcius, Fahrenheit
extern u8 mode;  // display mode 0 - time 1 - temp 2 - light
extern u8 setting;		// setting enable
extern u8 time_init;	// time_initial state
extern u16 light;		// lux ADC
extern u16 temp;			// temperature ADC
extern u8 time_init;	// time to setting
extern u8 time_hour_min_sec; //time setting objects
extern u8 setting_mode;
extern int time_temp;


extern u8 lastmode;  // after mode is changed

extern int transfer_num; // u32 --> 4send --> 1~4
extern int usart_err;

void USART1_Setting (void) {	
	RCC->APB2ENR |= 0x4005; // USART1, GPIOA clk en, AFIOEN
	RCC->APB1ENR |= (1<<25); 	//CAN clock EN
	GPIOA->CRH &= ~(0xF0F0u); // PA9, PA11 reset to 0
	GPIOA->CRH |= (0x40B0); 	// TX(PA9) : output push pull, PA 11 : input 
	AFIO->MAPR |= 0x00002000;		//CAN not used
	
	USART1->BRR = 0x0EA6;       // USART1 BRR 10.1
	USART1->CR1 = 0x00000008;   // TX en
	USART1->CR2 = 0x00000000;		// 
	USART1->CR3 = 0x00000600;  	// CTSIE, CTSE	
	NVIC->ISER[1] |= (1<<5);		//USART1 interrupt set enable(53 itrrpt)
	USART1->CR1 |= 0x2000; //USART EN
}


void USART1_IRQHandler (void) {
	if ((USART1->SR & 0x200) != 0) {	  				// if CTS interrupt
			if ((USART1->SR & 0x80) != 0) { 				// if TXE = 1		
				usart_err = 0;
					if(transfer_num<4){									// time
						USART1->DR = (u8)(time>>(8*transfer_num++));
					}
					else if (transfer_num==4){								// time_sel
						USART1->DR = time_sel;
						transfer_num++;
					}
					else if (transfer_num == 5){				//temp_sel
						USART1->DR = temp_sel;
						transfer_num++;
					}
					else if (transfer_num ==6){					// mode
						USART1->DR = mode;
						transfer_num++;
					}
					else if (transfer_num == 7){				// setting
						USART1->DR = setting;
						transfer_num++;
					}
					else if (transfer_num <10){					// light
						USART1->DR = (u8)(light>>(8*(transfer_num-8)));
						transfer_num++;
					}
					else if (transfer_num <12){					// temp
						USART1->DR = (u8)(temp>>(8*(transfer_num-10)));
						transfer_num++;
					}
					else if (transfer_num == 12){				//time_init
						USART1->DR = time_init;
						transfer_num++;
					}
					else if (transfer_num == 13){				//hour_min_sec
						USART1->DR = time_hour_min_sec;
						transfer_num++;
					}
					else if (transfer_num == 14){				//setting_mode
						USART1->DR = setting_mode;
						transfer_num++;
					}
					else if (transfer_num < 19){				//time_temp
						USART1->DR = (u8)(time_temp>>(8*(transfer_num-15)));
						transfer_num++;
						if(transfer_num ==19){
							transfer_num = 0;
							USART1->CR1 &= 0xFFF7;						//USART TX dienable
							USART1->CR1 &= 0xDFFF;				//USART disenable	
							NVIC->ICPR[1] = (1<<5);			//USART interrupt pending clear
							//TIM1->CR1 |= 0x0001;
						}
					}
					else {															//USART end
						
					}
			}
		}
	USART1->SR &= ~0x200;  			//CTS flag software reset
}
