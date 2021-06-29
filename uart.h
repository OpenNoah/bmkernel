#pragma once

#include "io.h"

#define UART0_BASE	((hw_uart_t *)0x10030000)
#define UART1_BASE	((hw_uart_t *)0x10031000)
#define UART2_BASE	((hw_uart_t *)0x10032000)
#define UART3_BASE	((hw_uart_t *)0x10033000)

typedef struct {
	union {
		struct {
			union {
				_I uint32_t URBR;
				_O uint32_t UTHR;
			};
			_IO uint32_t UIER;
		};
		struct {
			_IO uint32_t UDLLR;
			_IO uint32_t UDLHR;
		};
	};
	union {
		_I uint32_t UIIR;
		_O uint32_t UFCR;
	};
	_IO uint32_t ULCR;
	_IO uint32_t UMCR;
	_I uint32_t ULSR;
	_I uint32_t UMSR;
	_IO uint32_t USPR;
	_IO uint32_t ISR;
} hw_uart_t;
