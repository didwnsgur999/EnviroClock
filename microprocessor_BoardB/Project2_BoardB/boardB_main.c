#include <stm32f10x.h>
#include "boardB_USART.c"
#include "boardB_display.c"
#include "BoardB_Timer.c"

u8 whatis=0; // signal for USART. indicate what data is going to be received
int receive_complete = 0;// number of data transfer to receive complete
int tmp32 = 0;//for data receiving whose size is bigger than a byte
u16 tmp16 = 0;// same purpose of tmp32
u16 tmp16_2 = 0;
u8 shiftbit = 0; // shifted by n bit
u8 last_shift_mode = 0; // shifted_mode_register

//variables and whatis number
int time;					//1 real time
u8 time_sel;			//2 24/12 select
u8 temp_sel;			//3 C/F select
u8 mode;					//4 display mode  0 = time 1 = temp 2 = light
u8 setting;				//5 setting enable
u16 light;				//6 real light
u16 temp;					//7 real temp
u8 time_init;			//8 time_initial state condition
u8 time_hour_min_sec; //9 setting-> time setting condition
u8 setting_mode; 			//10 setting 0 : 24,12 1 : time 2 : C,F
int time_setting;  		//11 setted time

//display
u32 display_main[5];
u32 display_show[5];
u32 display_status[3];
u32 time_display[5];
u32 temp_display[5];
u32 light_display[5];
u32 time_setting_display[5];
u32 hour_min_sec_display[5];
u32 C_F_setting_display[5];

// disconnection
u32 trial;
int last_time;

u16 col;
u8 row = 0;


int main(void){
	usart_setting();  // usart
	Timer3_setting(); // trial
	matrix_setting();
	while(1){;}
}
