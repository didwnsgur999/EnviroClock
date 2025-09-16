#include <stm32f10x.h>


// time 1, time_sel 2, temp_sel = 3, mode = 4, setting = 5 ,light 6, temp = 7, time_init = 8
// hour_min_sec 9, setting_mode 10, time_temp 11						
int time;			// time as seconds
u8 time_sel;	// 24 or 12 time
u8 temp_sel;	// Celcius, Fahrenheit
u8 mode;  // display mode 0 - time 1 - temp 2 - light
u8 setting;		// setting enable
u8 time_init;	// time_initial state
u16 light;		// lux ADC
u16 temp;			// temperature ADC
u8 lastmode;  // after mode is changed
u8 time_hour_min_sec; // setting hour min sec
u8 setting_mode ; 			// 0 : 24/12 setting  1 : time setting  2 : c/f setting
int time_temp;        // time_temporary_

int transfer_num = 0; // u32 --> 4send --> 1~4   usart transfer number
int usart_err; //variable represent Usart status

	
	
int main (void) {
	flash_read();
	//usart1, PA9 and PA11 setting
	USART1_Setting();	
	//tim1 setting
	boardA_timer_setting();
	keymatrix_setting();
	timer2_setting();
	ADC_setting();
	Usart_error_detector();
	
  
	while (1) {
		//USART1_action();
		//__WFI();
	}
}
