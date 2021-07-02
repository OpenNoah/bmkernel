#include "uart.h"
#include "wdt.h"

void wdt_reset(hw_uart_t *uart)
{
	uart_puts(uart, "\r\nReset using WDT\r\n\0\0\0");
	hw_wdt_t *wdt = WDT_BASE;
	wdt->TCSR = 1;	// PCLK as clock input
	wdt->TDR = 0;	// Reset on timer = 0
	wdt->TCER = 1;	// Enable WDT
	wdt->TCNT = 0;	// Timer set to 0
	for (;;);
}
