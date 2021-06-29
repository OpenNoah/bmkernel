#pragma once

#include "io.h"

#define WDT_BASE	((hw_wdt_t *)0x10002000)

typedef struct {
	_IO uint32_t TDR;
	_IO uint32_t TCER;
	_IO uint32_t TCNT;
	_IO uint32_t TCSR;
} hw_wdt_t;
