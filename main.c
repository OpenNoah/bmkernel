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

void uart_puthex(hw_uart_t *uart, uint32_t v, int w)
{
	for (int i = 0; i < w; i++) {
		uint8_t fv = (v >> (4 * (w - 1 - i))) & 0xf;
		char c = fv < 10 ? fv + '0' : fv + 'a' - 10;
		uart_putc(uart, c);
	}
}

static void reg_dump(hw_uart_t *uart)
{
	uart_puts(uart, "Register dump:\r\n");

	static const struct {
		uint32_t base;
		uint32_t size;
	} regs[] = {
		{0x13010010, 0x00000030},
		{0x13010050, 0x00000004},
		{0x13010080, 0x00000030},
		{0x13010100, 0x00000030},
		{0x13050000, 0x00000060},
		{0, 0},
	};
	for (uint32_t idx = 0; regs[idx].base; idx++) {
		for (uint32_t i = 0; i < regs[idx].size; i += 4) {
			volatile uint32_t *p = (volatile uint32_t *)(regs[idx].base + i);
			uint32_t v = *p;
			uart_puts(uart, "0x");
			uart_puthex(uart, (uint32_t)p, 8);
			uart_puts(uart, ": 0x");
			uart_puthex(uart, v, 8);
			uart_puts(uart, "\r\n");
		}
	}
}

int main()
{
	hw_uart_t *uart = UART0_BASE;
	uart->ULCR = 0b11;	// DLAB = 0, 8-bit

	uart_puts(uart, "Test kernel run!\r\n");
	reg_dump(uart);

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
