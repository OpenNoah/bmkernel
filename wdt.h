#pragma once

#include "uart.h"
#include "helper.h"

#define WDT_BASE	((hw_wdt_t *)PA_TO_KSEG1(0x10002000))

void wdt_reset(hw_uart_t *uart);
