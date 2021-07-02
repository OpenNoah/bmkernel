#include <stdint.h>
#include "uart.h"
#include "wdt.h"

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

static const char *get_hex_u32(const char *s, uint32_t *pv)
{
	uint32_t v = 0;
	for (;;) {
		char c = *s;
		if (c == '\0')
			break;
		else if (c >= '0' && c < '9')
			v = (v << 4) | (c - '0');
		else if (c >= 'a' && c < 'f')
			v = (v << 4) | (c - 'a' + 0xa);
		else if (c >= 'A' && c < 'F')
			v = (v << 4) | (c - 'A' + 0xa);
		else if (c == 'x' || c == 'X')
		else
			break;
		s++;
	}
	*pv = v;
	return s;
}

static void mem_read_line(hw_uart_t *uart, const char *line)
{
	uint32_t addr = 0;
	get_hex_u32(&line[2], &addr);
	if (addr == 0)
		return;
	uint32_t v = *(uint32_t *)addr;
	uart_puts(uart, "0x");
	uart_puthex(uart, addr, 8);
	uart_puts(uart, " = 0x");
	uart_puthex(uart, v, 8);
	uart_puts(uart, "\r\n");
}

static void mem_write_line(hw_uart_t *uart, const char *line)
{
	uint32_t addr = 0;
	line = get_hex_u32(&line[2], &addr);
	if (addr == 0 || *line++ == '\0')
		return;
	uint32_t v = 0;
	get_hex_u32(line, &v);
	*(uint32_t *)addr = v;

	v = *(uint32_t *)addr;
	uart_puts(uart, "0x");
	uart_puthex(uart, addr, 8);
	uart_puts(uart, " = 0x");
	uart_puthex(uart, v, 8);
	uart_puts(uart, "\r\n");
}

int main()
{
	hw_uart_t *uart = UART0_BASE;
	uart_init(uart);

	uart_puts(uart, "Test kernel run!\r\n");
	//reg_dump(uart);

	for (;;) {
		uart_puts(uart, "> ");
		char *line = uart_get_line(uart);
		if (line[0] == 0)
			continue;

		switch (line[0]) {
		case 'r':
			mem_read_line(uart, line);
			break;
		case 'w':
			mem_write_line(uart, line);
			break;
		case '*':
			wdt_reset(uart);
			break;
		}
		//uart_puts(uart, line);
		//uart_puts(uart, "\r\n");
	}

	wdt_reset(uart);
	return 0;
}

void entry()
{
	main();
	for (;;);
}
