#include "io.h"
#include "uart.h"

#define LINE_BUFFER_SIZE	32

void uart_init(hw_uart_t *uart)
{
	uart->ULCR = 3;	// DLAB = 0, 8-bit
}

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

void uart_putdec(hw_uart_t *uart, uint32_t v)
{
	uint32_t base = 1000000000;
	while (v && v / base == 0)
		base /= 10;
	while (base) {
		uint32_t vv = v / base;
		v -= vv * base;
		base /= 10;
		char c = vv + '0';
		uart_putc(uart, c);
	}
}

char uart_getc(hw_uart_t *uart)
{
	while (!(uart->ULSR & 1));
	return uart->URBR;
}

char *uart_get_line(hw_uart_t *uart)
{
	static char line[LINE_BUFFER_SIZE];
	int i = 0;
	for (;;) {
		char c = uart_getc(uart);
		if (c == '\x7f') {
			if (i != 0) {
				uart_putc(uart, c);
				i--;
			}
		} else if (c == '\r' || c == '\n') {
			uart_puts(uart, "\r\n");
			break;
		} else {
			if (i != LINE_BUFFER_SIZE - 1) {
				uart_putc(uart, c);
				line[i++] = c;
			}
		}
	}
	line[i] = 0;
	return line;
}
