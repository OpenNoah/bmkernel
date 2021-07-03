#pragma once

#include <stdint.h>

#define PA_TO_KSEG1(v)	(0xa0000000 + (v))

const char *get_hex_u32(const char *s, uint32_t *pv);

static inline uint32_t kseg0_to_pa(const void *p)
{
	return (uint32_t)p - 0x80000000;
}

static inline uint32_t kseg1_to_pa(const void *p)
{
	return (uint32_t)p - 0xa0000000;
}
