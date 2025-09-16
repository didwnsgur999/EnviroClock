#include <stm32f10x.h>
#include <stm32f10x.h>
extern u8 time_sel;
extern u8 temp_sel;
extern u8 mode;
u8 tmp1,tmp2,tmp3;

// programming

void flash_unlock(void){ // lock diasble
	while ((FLASH->SR & 0x1) != 0){} //bsy = 0
	if ((FLASH->CR & 0x80) != 0 ){ 
		FLASH->KEYR = FLASH_KEY1;
		FLASH->KEYR = FLASH_KEY2;
	}
	else {}
}
void flash_erase(void){
	flash_unlock();
	FLASH->CR |= 0x2;  //PER = 1
	FLASH->AR = 0x08007C00; //page 31
	FLASH->CR |= 0x40; //erase start

	while ((FLASH->SR & 0x1) != 0){} // bsy check

	FLASH->CR &= 0xFFFD; // 0x2 reset 
}
// ok button stroke --> flash 
void flash_programming (void) { // data programming
	flash_unlock();
	flash_erase();
	FLASH->CR |= 0x1; // PG = 1

	*(volatile u16*)(0x08007C00) = (u16)time_sel;
	*(volatile u16*)(0x08007C04) = (u16)temp_sel;
	*(volatile u16*)(0x08007C08) = (u16)mode;

	while ((FLASH->SR & 0x1) != 0){} // bsy check
		
	FLASH->CR |= 0x80;	//control locked
}


// at main started --> if initial --> 0, else flash memory

void flash_read (void){
	tmp1 = *(volatile u8 *)(0x08007C00);
	tmp2 = *(volatile u8 *)(0x08007C04);
	tmp3 = *(volatile u8 *)(0x08007C08);
	if (tmp1 == 0xFF) time_sel = 0;
	else time_sel = tmp1;
	if (tmp2 == 0xFF) temp_sel = 0;
	else temp_sel = tmp2;
	if (tmp3 == 0xFF) mode = 0;
	else mode = tmp3;
}

