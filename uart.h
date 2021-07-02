#pragma once

#include "io.h"

#define UART0_BASE	((hw_uart_t *)0xb0030000)
#define UART1_BASE	((hw_uart_t *)0xb0031000)
#define UART2_BASE	((hw_uart_t *)0xb0032000)
#define UART3_BASE	((hw_uart_t *)0xb0033000)

typedef struct hw_uart_t hw_uart_t;

void uart_init(hw_uart_t *uart);
void uart_putc(hw_uart_t *uart, char c);
void uart_puts(hw_uart_t *uart, const char *s);
void uart_puthex(hw_uart_t *uart, uint32_t v, int w);
char uart_getc(hw_uart_t *uart);
char *uart_get_line(hw_uart_t *uart);
