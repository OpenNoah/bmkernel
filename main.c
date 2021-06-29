#include <stdint.h>
#include "uart.h"
#include "wdt.h"

void uart_putc(hw_uart_t *uart, char c)
{
	while (!(uart->ULSR & (1 << 6)));	// TEMP
	uart->UTHR = c;
}

void uart_puts(hw_uart_t *uart, const char *s)
{
	while (*s != '\0')
		uart_putc(uart, *s++);
}

int main()
{
	hw_uart_t *uart = UART0_BASE;
	uart->ULCR = 0b11;	// DLAB = 0, 8-bit

	uart_puts(uart, "Test kernel run!\r\n");
	for (;;) {
		while (!(uart->ULSR & 1));
		char c = uart->URBR;
		uart_putc(uart, c);
		if (c == '\r' || c == '\n')
			break;
	}

	uart_puts(uart, "\r\nReset using WDT\r\n");
	hw_wdt_t *wdt = WDT_BASE;
	wdt->TCSR = 1;	// PCLK as clock input
	wdt->TDR = 0;	// Reset on timer = 0
	wdt->TCER = 1;	// Enable WDT
	wdt->TCNT = 0;	// Timer set to 0

	return 0;
}

void entry()
{
	main();
	for (;;);
}
