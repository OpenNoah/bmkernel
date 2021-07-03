#pragma one

#include "helper.h"
#include "uart.h"

#define LCD_BASE	((hw_lcd_t *)PA_TO_KSEG1(0x13050000))

typedef struct hw_lcd_t {
	_IO uint32_t LCDCFG;
	_IO uint32_t LCDVSYNC;
	_IO uint32_t LCDHSYNC;
	_IO uint32_t LCDVAT;
	_IO uint32_t LCDDAH;
	_IO uint32_t LCDDAV;

	_IO uint32_t LCDPS;
	_IO uint32_t LCDCLS;
	_IO uint32_t LCDSPL;
	_IO uint32_t LCDREV;
	_IO uint32_t _RESERVED0[2];

	_IO uint32_t LCDCTRL;
	_IO uint32_t LCDSTATE;
	_I  uint32_t LCDIID;
	_IO uint32_t _RESERVED1[1];

	_IO uint32_t LCDDA0;
	_I  uint32_t LCDSA0;
	_I  uint32_t LCDFID0;
	_I  uint32_t LCDCMD0;

	_IO uint32_t LCDDA1;
	_I  uint32_t LCDSA1;
	_I  uint32_t LCDFID1;
	_I  uint32_t LCDCMD1;
} hw_lcd_t;

void lcd_init(hw_uart_t *uart);
