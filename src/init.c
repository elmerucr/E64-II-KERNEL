#include "kernel.h"
#include "blitter.h"
#include "vicv.h"
#include "sids.h"
#include "tty.h"
#include "timer.h"
#include "cia.h"
#include "command.h"
#include "allocation.h"

#include <stdlib.h>

void move_sections_rom_to_ram_set_heap();
void update_vector_table();

void init()
{
	move_sections_rom_to_ram_set_heap();
	update_vector_table();

	user_start = (void *)INITIAL_SSP;

	/*
	 * Before malloc etc can be called, need to init the memory allocation
	 * system.
	 */
	allocation_init();

	character_ram = malloc(256 * 64 * sizeof(u16));
	if (character_ram == 0)
		panic();

	build_character_ram((u8 *)CHAR_ROM, (u16 *)character_ram);
	blitter_init();

	VICV->horizontal_border_size = 16;
	VICV->horizontal_border_color = C64_BLACK;

	BLITTER->clear_color = C64_BLUE;

	CIA->keyboard_repeat_delay = 50;
	CIA->keyboard_repeat_speed = 5;
	CIA->control_register = 0b00000001;

	tty_set_current(&tty0);

	tty_init(
		BLIT_X__64_TILES | BLIT_Y__32_TILES,
		0,
		16,
		C64_LIGHTBLUE,
		0x0000
	);

	tty_clear();
	tty_puts("E64-II Virtual Computer System\n");

	tty0.current_mode = SHELL;
	tty0.interpreter = &command_interprete;
	tty0.prompt = command_prompt();

	blitter_add_action(&tty0.screen_blit);

	timer_update_handler(TIMER0, tty_timer_callback);
	timer_turn_on(TIMER0, 3600);

	/*
	 * Enable all interrupts with level 2 and higher.
	 */
	set_interrupt_priority_level(1);

	sids_reset();
	sids_welcome_sound();

	repl();
}

void move_sections_rom_to_ram_set_heap()
{
	/*
	 * Move relevant portion of initialized data to ram.
	 * A few labels have been declared in the rom.ld file.
	 */
	extern char etext, data, edata, bss, bssend;
	char *src = &etext;
	char *dst = &data;

	/*
	 * ROM has .data section at end of .text, copy it to the right location
	 */
	while (dst < &edata)
		*dst++ = *src++;

	/*
	 * zero out the .bss section
	 */
	for (dst = &bss; dst< &bssend; dst++)
		*dst = 0x00;

	/*
	 * Initialize the heap pointers
	 */
	heap_start = (void *)&bssend;
	heap_end   = (void *)END_OF_HEAP;
}

void update_vector_table()
{
	update_vector_number(3,  address_error_exception_handler);
	update_vector_number(26, vicv_vblank_exception_handler);	// irq 2
	update_vector_number(28, timer_exception_handler);		// irq 4
}
