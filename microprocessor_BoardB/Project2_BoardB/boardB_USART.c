
extern u8 whatis;// signal for USART. indicate what data is going to be received
extern int tmp32;//for data receiving whose size is bigger than a byte
extern u16 tmp16;// same purpose of tmp32
extern u16 tmp16_2;

//variables and whatis number
extern int time;						//1
extern u8 time_sel;					//2
extern u8 temp_sel;					//3
extern u8 mode;							//4
extern u8 setting;					//5
extern u16 light;						//6
extern u16 temp;						//7
extern u8 time_init;				//8
extern u8 time_hour_min_sec; //9
extern u8 setting_mode; 		//10
extern int time_setting;  	//11

extern u32 time_display[5];
extern u32 temp_display[5];
extern u32 light_display[5];

void time_transform();
void light_transform();
void temp_transform();
void time_setting_transform();

void usart_setting(void){
	SCB->AIRCR = 0x05FA0400; // priority 3bit msb
	NVIC->IP[37] = 0x20; // 001 usart
	NVIC->IP[25] = 0x00; // 000 tim1 up
	NVIC->IP[28] = 0x60; // 011 tim2
	NVIC->IP[29] = 0x70; // 111 tim3
	RCC->APB2ENR |= 0x00004005;//USART1, EN IOPA EN
	RCC->APB1ENR |= (1<<25); //CAN clock to RTS enable
	AFIO->MAPR |= 0x2000;//CAN_RX, TX not used
	GPIOA->CRH &= ~(0xF0Fu << 8);//RX(PA10), RTS(PA12) clearing
	GPIOA->CRH |= 0xB0400;//RTS(PA12): alternate function with push-pull, RX(PA10): input floating  
	USART1->BRR = 0x0EA6; //baud rate
	USART1->CR1 = 0x00000020;//RXNEIE set,
	USART1->CR2 = 0x00000000;
	USART1->CR3 = 0x00000100;//RTS enable,
	NVIC->ISER[1] |= (1<<5);//NVIC_interrupt enable(USART1 global interrupt)
	
	USART1->CR1 |= 0x00000004;//USART1 Receiver enable
	USART1->CR1 |= 0x00002000;//USART1 enable
}

extern int receive_complete;// number of data transfer to receive complete
// 1~4 = tmp32, 5 = time_sel, 6 = temp_sel, 7 = mode, 8 = setting, 9~10 = light, 11~12 = temp, 13 = timeinit, 
void USART1_IRQHandler(void){
	if(USART1->SR & 0x20){
		u8 usart_start;
		if (receive_complete == 0){
			usart_start = USART1->DR;
			if (usart_start != 0xFF){receive_complete = 0;}
			else receive_complete++;
		}
		else if (receive_complete<5){	//1,2,3,4 
			tmp32 += (USART1->DR << (8*(receive_complete-1)));//'time' is 4byte so receive 4 times
			receive_complete++;
			if ( receive_complete == 5) {
				time = tmp32;
				tmp32 = 0;
				time_transform();
			}
		}
		else if (receive_complete == 5){
			time_sel = USART1->DR;
			receive_complete++;
		}
		else if (receive_complete == 6){
			temp_sel = USART1->DR;
			receive_complete++;
		}
		else if (receive_complete == 7){
			mode = USART1->DR;
			receive_complete++;
		}
		else if (receive_complete == 8){
			setting = USART1->DR;
			receive_complete++;
		}
		else if (receive_complete < 11){ // 9,10
			tmp16 += (USART1->DR << (8*(receive_complete-9)));
			receive_complete++;
			if(receive_complete == 11){
				light = tmp16;
				tmp16 = 0;
				light_transform();
			}
		}
		else if (receive_complete <13){ // 11, 12
			tmp16_2 += (USART1->DR << (8*(receive_complete-11)));
			receive_complete++;
			if(receive_complete == 13){
				temp = tmp16_2;
				tmp16_2 = 0;
				temp_transform();
			}
		}
		else if (receive_complete == 13){
			time_init = USART1->DR;
			receive_complete++;
		}
		else if (receive_complete == 14){
			time_hour_min_sec = USART1->DR;
			receive_complete++;
		}
		else if (receive_complete == 15){
			setting_mode = USART1->DR;
			receive_complete++;
		}
		else if (receive_complete < 20){ //16,17,18,19
			tmp32 += (USART1->DR << (8*(receive_complete-16)));//'time_setting' is 4byte so receive 4 times
			receive_complete++;
			if (receive_complete == 20) {
				time_setting = tmp32;
				tmp32 = 0;
				time_setting_transform();
				receive_complete = 0;
			}
		}
		else;
		
		USART1->SR &= ~(0x20);
	}
}
