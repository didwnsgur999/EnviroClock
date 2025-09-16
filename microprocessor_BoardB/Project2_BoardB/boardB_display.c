
u8 font5x4[20][5] = {
	{0x7, 0x5, 0x5, 0x5, 0x7},					//0
	{0x6, 0x2, 0x2, 0x2, 0x7},					//1
	{0x7, 0x1, 0x7, 0x4, 0x7},					//2
	{0x7, 0x1, 0x7, 0x1, 0x7},					//3
	{0x5, 0x5, 0x7, 0x1, 0x1},					//4
	{0x7, 0x4, 0x7, 0x1, 0x7},					//5
	{0x7, 0x4, 0x7, 0x5, 0x7},					//6
	{0x7, 0x1, 0x1, 0x1, 0x1},					//7
	{0x7, 0x5, 0x7, 0x5, 0x7},					//8
	{0x7, 0x5, 0x7, 0x1, 0x7},					//9
	{0x0, 0x4, 0x0, 0x4, 0x0},					//: 10
	{0x0, 0x0, 0x0, 0x0, 0x4}, 					//. 11
	{0x4, 0x3, 0x4, 0x4, 0x3},					//C 12
	{0x4, 0x3, 0x2, 0x3, 0x2},					//F 13
	{0x2, 0x5, 0x7, 0x5, 0x5},					//A 14
	{0x4, 0x4, 0x7, 0x5, 0x5},   				//h 15
	{0x5, 0x5, 0x7, 0x5, 0x7},					//m 16
	{0x3, 0x4, 0x2, 0x1, 0x6},					//s 17
	{0x0, 0x0, 0x0, 0x0, 0x0}, 					//_ 18
	{0x7, 0x5, 0x7, 0x4, 0x4},					//p 19
};
u16 lx_font[5] = {0x40, 0x40, 0x45, 0x42, 0x75};
	
void matrix_setting(void){
	//row: PC0-7, PA0-7, column: PB8-15
	//PB8-15 left matrix, PA0-7 right matrix
	RCC->APB2ENR |= 0x1D;		//IOPCEN, IOPBEN, IOPAEN, AFIOEN
	GPIOC->CRL = 0x33333333;//row: PC0-7 config as output with push-pull
	GPIOB->CRH = 0x33333333;//column1:PB8-15 config as output with push-pull
	GPIOC->CRH = 0x00333333;//column2-5:0 :PC8-13 config as output with push-pull
	GPIOA->CRL |= 0x00000033;//column2-7:6 :PA0-1 config as output with push-pull
	
	RCC->APB2ENR |= 0x0000081D;//TIM1EN, PCEN, PBEN, PAEN, AFIOEN
	TIM1->CR1 = 0x00;
	TIM1->CR2 = 0x00;
	TIM1->PSC = 0x00FF;
	TIM1->ARR = 0x0080;//matrix brightness
	
	TIM1->DIER = 0x0001;//update enable
	NVIC->ISER[0] = 0x02000000;
	
	RCC->APB1ENR |= 1;//TIM2EN
	TIM2->CR1 = 0x00;
	TIM2->CR2 = 0x00;
	TIM2->PSC = 3599;
	TIM2->ARR = 4999; // 0.5sec
	
	TIM2->DIER = 0x0001; //update enable
	NVIC->ISER[0] |= (1<<28); // TIM2 NVIC
	TIM2->CR1 |= 0x0001;//TIM2 start 
	TIM1->CR1 |= 0x0001;//TIM1 start
}

extern u8 row; 
extern u16 col;
//display
extern u32 display_main[5]; // input catch
extern u32 display_show[5]; // input show main with shift
extern u32 display_status[3];
extern u32 time_display[5];
extern u32 light_display[5];
extern u32 temp_display[5];
extern u32 time_setting_display[5];//24,12
extern u32 hour_min_sec_display[5];
extern u32 C_F_setting_display[5];
//
extern int time;
extern u8 time_sel;
extern u8 temp_sel;
extern u8 mode ;
extern u8 setting;
extern u16 light;
extern u16 temp;
extern u8 time_init;
extern u8 setting_mode; 	// setting_mode 0 = 12,24 1 = hour_min 2 = C,F
extern int time_setting;  // setting_hour_min_sec
//shift display
extern u8 shiftbit; // shift bit memory
extern u8 last_shift_mode; //last_shift_mode register
// lux
int lux;

u32 shift_display (u32 display_main, u8 shiftbit,u8 length){
		u32 result = 0;
		if (shiftbit == 0) {result = display_main;}
		else{
			int mask = 0; int mask_length = 0;
			int count = shiftbit; //mask = shift bit number lsb 1
			while(count>0){
				mask <<= 1;
				mask |= 1;
				count--;
			}
			count = length;
			while(count>0){
				mask_length <<= 1; // for length = 5--> 2_011111
				mask_length |= 1;
				count--;
			}
		
			u32 data = (display_main & mask_length); //lsb "length" bit 2_1111111111111111 
			u32 msb = (data >> (length-shiftbit)) & mask; //msb "shiftbit" bit
			u32 merged = (data<<shiftbit) | msb;
			result = (display_main & (~mask_length)) | merged;
			}
		return result;
}

//tim1 display real
void TIM1_UP_IRQHandler(void){
	if(TIM1->SR & 0x0001){
		int brightness; // row reset brightness;
		if(row<5){//display of upper 5rows
			if (setting == 0 && mode == 0){  //time mode
				for (int N=0; N<5; N++){
					display_main[N] = time_display[N];
				}
			}
			else if (setting == 0 && mode == 1){ //temp mode
				for (int N=0; N<5; N++){
					display_main[N] = temp_display[N];
				} 
			}
			else if (setting == 0 && mode == 2){ // light mode 
				for (int N=0; N<5; N++){
					display_main[N] = light_display[N];
				}
			}
			else if (setting == 1 && setting_mode == 0){ // 24,12 setting
				for (int N=0; N<5; N++){
					display_main[N] = time_setting_display[N];
				}
			}
			else if (setting == 1 && setting_mode == 1){ // time setting
				for (int N=0; N<5; N++){
					display_main[N] = hour_min_sec_display[N];
				}
			}
			else if (setting == 1 && setting_mode == 2){ // temp setting
			  for (int N=0; N<5; N++){
					display_main[N] = C_F_setting_display[N];
				}
			}
			GPIOC->ODR = ~(1<<row);
			col = (u16)display_show[row]; // display_main to display show
			GPIOB->BSRR = (((~col)&0xFF00) << 16) | (col&0xFF00); //left side matrix
			GPIOC->BSRR = (((~col)&0x003F)<<24)|((col&0x003F)<<8);//right side matrix
			GPIOA->BSRR = (((~col)&0x00C0)<<10)|((col&0x00C0)>>6);//PB14,PB15-->PA0,1
		}
		
		else{		//display of lower 3rows
			display_status[0] = display_status[1] = 0;
			// mode select led 2~4
			if(mode == 0){ 
				display_status[2] |= 0x2;
				display_status[2] &= ~(0xc);
			}
			else if(mode == 1){
				display_status[2] |= 0x4;
				display_status[2] &= ~(0xa);
			}
			else if(mode == 2){
				display_status[2] |= 0x8;
				display_status[2] &= ~(0x6);
			}
			else {display_status[2] |= 0xE;}
			// 12, 24 setting
			if (time_sel == 0){ 
					display_status[2] |= 0x10;
			}
			else display_status[2] &= ~(0x10);
			
			GPIOC->ODR = ~(1<<row);
			col = (u16)display_status[row-5]; // 0,1,2
			GPIOB->BSRR = (((~col)&0xFF00) << 16) | (col&0xFF00); //left side matrix
			GPIOC->BSRR = (((~col)&0x003F)<<24)|((col&0x003F)<<8);//right side matrix
			GPIOA->BSRR = (((~col)&0x00C0)<<10)|((col&0x00C0)>>6);//PA0,1
			}
		row++;
		if(light<0x400){
			brightness = 0;
		}
		else if (light<0xF00){
			brightness = (int)((float)(light-0x400)* 52 / 0xB00);
		}
		else {
			brightness = 52; 
		}
		if(row == (60-brightness)) row = 0; // row 8-->highest, 60-->lowest
		TIM1->SR &= ~(1<<0);
	}
}

// shifting only when TIM2 interrupt occurs
void TIM2_IRQHandler(void){
	if(TIM2->SR & 0x0001){
		u8 length = 0;
		if (setting == 0){
			if (mode != last_shift_mode) {shiftbit = 0;}
			if (mode == 0){ // time_display mode
				length = 32;  // 32 data bit
				last_shift_mode = 0; // mode registered
			}	
			else if(mode == 1){	// temp_display mode
				length = 22;			// 22 data bit
				last_shift_mode = 1;
			}
			else if(mode == 2){ // light display mode 
				length = 26;			// 26 data bit
				last_shift_mode = 2;
			}
			else {} // mode is error
			for(int j=0;j<5; j++){
				display_show[j] = shift_display(display_main[j],shiftbit,length);
			}
			shiftbit++;
			if (shiftbit == length) {shiftbit = 0;}
			if (shiftbit == 33) {shiftbit = 0;}
		}
		else{
			for (int j = 0; j<5; j++){
				display_show[j] = display_main[j];
			}
		}
		// display_status[2] for 0.25sec blink
		if (time_init == 0){
				if ((display_status[2] & 0x1) != 0) {
					display_status[2] &= ~0x1;
				}
				else { 
					display_status[2] |= 0x1;
				}
		}
		else display_status[2] |= 0x1;
		// pending off
		TIM2->SR &= ~(1<<0);
	}
}
// time --> displayable time
void time_transform(void){
	int hour, min, sec;
	int digit[9];
	u32 time_tmp;
	// time error 24hour 
	if(time < 86400){
		time_tmp = time;	
		digit[2] = 10; // :
		digit[5] = 10; // :
		if (time_sel == 0){ // 12hour
			hour = ((time_tmp/60)/60)%12;
			if(hour == 0) hour = 12;
			if (time < 43200) {digit[8] = 14;} //A
			else {digit[8] = 19;} // P
		}
		// time_sel == 1 --> 24hour
		else {
			hour = ((time_tmp/60)/60)%24;
			digit[8] = 18;	//blank
		}
		sec = time_tmp%60;
		min = (time_tmp/60)%60;
	
		digit[7] = hour/10; digit[6] = hour%10;
		digit[4] = min/10; digit[3] = min%10;
		digit[1] = sec/10; digit[0] = sec%10;
	}
	else { //time error
			for (int N = 0; N<8; N++){
				if(N == 2 || N == 5){
					digit[N] = 10;
				}	
				else	digit[N] = 8;
			}
	}
	// time display for digit[N]
	for(int k = 0; k<5; k++){
		time_display[k]= (font5x4[digit[0]][k])|(font5x4[digit[1]][k]<<4)|(font5x4[digit[2]][k]<<6)|(font5x4[digit[3]][k]<<10)
											|(font5x4[digit[4]][k]<<14)|(font5x4[digit[5]][k]<<16)|(font5x4[digit[6]][k]<<20)|font5x4[digit[7]][k]<<24
											|(font5x4[digit[8]][k]<<28);
	}
	
	// time_sel --> time setting display
	for(int N = 0; N < 5; N++){
			if (time_sel == 0){
				time_setting_display[N] = (font5x4[2][N]<<8)|(font5x4[1][N]<<12); // setting 12 font
			}
			else {
				time_setting_display[N] = (font5x4[4][N]<<8)|(font5x4[2][N]<<12); // setting 24 font
			}
		}
}

// light lux transform
void light_transform(void){
	int hund_l,ten_l,one_l,point_l;
	int digit_l[4];
	int light_tmp;
	float light_float;
	//calibration 
	
	light_tmp = (int) light;
	if (light_tmp<0xA35){ lux = 0;}
	else if (light_tmp<0xFFF){
		light_float = (float)(light_tmp-3168) * 1500/335 + 2500; // 1793 = 0 lux
		lux = (int)light_float;
	}
	else {lux = 6800;}
	
	digit_l[0] = lux%10; digit_l[1] = (lux%100)/10;
	digit_l[2] = (lux%1000)/100; digit_l[3] = lux/1000;
	
	for (int L = 0; L<5; L++){ // hund_ten_one.point_lx
		light_display[L] = (lx_font[L])|(font5x4[digit_l[0]][L]<<8)|(font5x4[11][L]<<10)|(font5x4[digit_l[1]][L]<<14)
											|(font5x4[digit_l[2]][L]<<18)|(font5x4[digit_l[3]][L]<<22);
	}
}

// temperature transform
void temp_transform(void) {
	extern u8 temp_sel;
	int Cel,Fah;
	int temp_t;
	float tmp;
	int hund_t,ten_t,one_t,point_t;
	int digit_t[4];
	
	temp_t = (int)temp;
	// calibration by 25.0 C 340 30.0 C 390 50C->0x50 10
	if((temp_t > 0x1B0) && (temp_t < 0x640)){ // 0x1B0 = 0 C , 0x640 = 100 C 
	tmp = (float)(temp_t - 432) * 5 / 8 ; //calibration
	Cel = (int)tmp;
	Fah = (int)(Cel*9/5) + 320; 
	}
	else {Cel = Fah = 8888;} //error
	
	// celcius or fah by temp_sel 
	if ( temp_sel == 0){
	  hund_t = Cel/1000; ten_t = (Cel%1000)/100; one_t = ((Cel%1000)%100)/10; point_t = ((Cel%1000)%100)%10;
		if (hund_t == 0) hund_t = 18;
	}
	else if( temp_sel == 1){
		hund_t = Fah/1000; ten_t = (Fah%1000)/100; one_t = ((Fah%1000)%100)/10; point_t = ((Fah%1000)%100)%10;
		if (hund_t == 0) hund_t = 18;
	}
	
	
	//calibration --> celcius, Fahrenheit
	digit_t[0] = point_t; digit_t[1] = one_t; digit_t[2] = ten_t; digit_t[3] = hund_t;
	
	if (temp_sel == 0){//celcius
		for (int M = 0; M<5; M++){ //celcius hund_ten_one.point_C
			temp_display[M] = (font5x4[12][M])|(font5x4[digit_t[0]][M]<<4)|(font5x4[11][M]<<6)|(font5x4[digit_t[1]][M]<<10)
												|(font5x4[digit_t[2]][M]<<14)|(font5x4[digit_t[3]][M]<<18);
		}
		for (int N = 0; N<5; N++){
			C_F_setting_display[N] = font5x4[12][N];
		}
	}
	else {//Fahrenheit = 1.8*C+32
		for (int M = 0; M<5; M++){ //celcius hund_ten_one.point_F
			temp_display[M] = (font5x4[13][M])|(font5x4[digit_t[0]][M]<<4)|(font5x4[11][M]<<6)|(font5x4[digit_t[1]][M]<<10)
												|(font5x4[digit_t[2]][M]<<14)|(font5x4[digit_t[3]][M]<<18);
		}
		for (int N = 0; N<5; N++){
			C_F_setting_display[N] = font5x4[13][N];
		}
	}	
}

void time_setting_transform (void){ // at time setting, hour min sec decided
	int hour_s,min_s,sec_s;
	int digit_s[4]; 
	u32 time_tmp;
  // time_setting decoding
	if(time_setting < 86400){ //time_setting < 24hour	
		time_tmp = time_setting;
		if (time_sel == 0){
			hour_s = ((time_tmp/60)/60)%12; //0~11
			if (hour_s == 0) hour_s = 12;
		}
		else {
			hour_s = ((time_tmp/60)/60)%24; //0~23
		}
		sec_s = time_tmp%60; //0~59
		min_s = (time_tmp/60)%60; // 0~59
		// time_hour_min_sec check
		if (time_hour_min_sec == 0){
			digit_s[0] = 15;
			digit_s[1] = 10;
			digit_s[2] = hour_s%10; // 1
			digit_s[3] = hour_s/10; // 10
		}
		else if (time_hour_min_sec == 1){
			digit_s[0] = 16;
			digit_s[1] = 10;
			digit_s[2] = min_s%10; // 1
			digit_s[3] = min_s/10; // 10
		}
		else if (time_hour_min_sec == 2){
			digit_s[0] = 17;
			digit_s[1] = 10;
			digit_s[2] = sec_s%10;
			digit_s[3] = sec_s/10;
		}
		else { // error by time_hour_min_sec > 2
			digit_s[0] = digit_s[1] = digit_s[2] = digit_s[3] = 10;
		}
	}
	else{ // error by time_setting > 86400
		digit_s[0] = digit_s[1] = digit_s[2] = digit_s[3] = 8;
	}
	
	// display
	for(int N = 0; N<5; N++){
		hour_min_sec_display[N] = (font5x4[digit_s[0]][N])|(font5x4[digit_s[1]][N]<<4)
															|(font5x4[digit_s[2]][N]<<8)|(font5x4[digit_s[3]][N]<<12);//0,1,2,3
	}
}

