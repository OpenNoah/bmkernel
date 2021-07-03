#include "io.h"
#include "uart.h"
#include "wdt.h"

typedef struct {
	_IO uint32_t TDR;
	_IO uint32_t TCER;
	_IO uint32_t TCNT;
	_IO uint32_t TCSR;
} hw_wdt_t;

void wdt_reset(hw_uart_t *uart)
{
	uart_puts(uart, "\r\nReset using WDT\r\n\r\n");
	hw_wdt_t *wdt = WDT_BASE;
	wdt->TCSR = 1;	// PCLK as clock input
	wdt->TDR = 0;	// Reset on timer = 0
	wdt->TCER = 1;	// Enable WDT
	wdt->TCNT = 0;	// Timer set to 0
	for (;;);
}
