#include "io.h"
#include "helper.h"
#include "lcd.h"

#define MAX_W	800
#define MAX_H	600
#define MAX_D	4

#define DMA_PACKED	__attribute__((packed))
#define DMA_ALIGNED(n)	__attribute__((aligned(n)))
#define DMA_SECTION	__attribute__((section(".dma")))
#define DMA_TO_PA(a)	kseg1_to_pa((const void *)a)

static struct DMA_PACKED DMA_ALIGNED(16 * 4) {
	uint32_t data[MAX_W * MAX_H];
} buf[2] DMA_SECTION;

static struct DMA_PACKED DMA_ALIGNED(16) {
	uint32_t da;
	uint32_t sa;
	uint32_t fid;
	uint32_t cmd;
} desc[2] DMA_SECTION;

static int desc_idx = 0;

static struct {
	uint32_t size;
	uint32_t w, h;
	uint32_t bpp, depth;
} info;

void lcd_init(hw_uart_t *uart)
{
	hw_lcd_t *lcd = LCD_BASE;

	uint32_t v = lcd->LCDDAH;
	info.w = (v & 0xffff) - (v >> 16);
	v = lcd->LCDDAV;
	info.h = (v & 0xffff) - (v >> 16);
	v = lcd->LCDCTRL & 0x7;
	static const int bpp[] = {1, 2, 4, 8, 16, 24, 0, 0};
	info.bpp = bpp[v];
	static const int depth[] = {0, 0, 0, 1, 2, 4, 0, 0};
	info.depth = depth[v];
	info.size = info.w * info.h * info.depth;

	if (info.size == 0 || info.depth != 4)
		return;

#if 1
	uart_puts(uart, "LCD init!\r\n");
	uart_putdec(uart, info.w);
	uart_puts(uart, " x ");
	uart_putdec(uart, info.h);
	uart_puts(uart, " x ");
	uart_putdec(uart, info.depth);
	uart_puts(uart, " => ");
	uart_putdec(uart, info.size);
	uart_puts(uart, "\r\n");
	uart_puthex(uart, (uint32_t)&desc[0], 8);
	uart_puts(uart, " => ");
	uart_puthex(uart, DMA_TO_PA(&desc[0]), 8);
	uart_puts(uart, "\r\n");
#endif

	// Quick disabling, clear ENA
	lcd->LCDCTRL &= ~(1ul << 3);
	while (!(lcd->LCDSTATE & (1ul << 7)));

#if 1
	// Fix LCD area issue with NP1xx0
	lcd->LCDDAV += 0x00010001;
#endif

	// LCD buffer descriptor
	desc[0].da = DMA_TO_PA(&desc[0]);
	desc[0].sa = DMA_TO_PA(&buf[0].data[0]);
	desc[0].fid = 0;
	desc[0].cmd = info.size / 4;

	// Fill LCD buffer with test pattern
	for (uint32_t i = 0; i < info.size / 4; i++)
		buf[0].data[i] = 0;

	// Commit LCD buffer descriptor
	lcd->LCDDA0 = DMA_TO_PA(&desc[0]);

	// Re-enable
	lcd->LCDSTATE = 0;
	lcd->LCDCTRL |= 1ul << 3;

	// Test pattern
	for (int y = 0; y < info.h; y++) {
		for (int x = 0; x < info.w; x++) {
			uint32_t c = (x << 10) | ((y << 2) & 0xff);
			buf[0].data[y * info.w + x] = c;
		}
	}

	for (uint32_t i = 0; i < info.w; i++) {
		buf[0].data[i] = 0x00ff00;	// 0rgb
		buf[0].data[(info.h - 1) * info.w + i] = 0xff0000;
	}

	for (uint32_t i = 0; i < info.h; i++) {
		buf[0].data[i * info.w] = 0xffff00;
		buf[0].data[i * info.w + (info.w - 1)] = 0x0000ff;
		buf[0].data[i * info.w + i] = 0x66ccff;
	}
}
