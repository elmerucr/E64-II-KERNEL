#include <stdint.h>
#include <stddef.h>
#include "definitions.h"
#include "tty.h"

#ifndef KERNEL_H
#define KERNEL_H

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

void repl();

// poke functionality
#define	POKEB(A, B)	*(volatile u8  *)A = B
#define POKEW(A, B)	*(volatile u16 *)A = B
#define POKEL(A, B)	*(volatile u32 *)A = B

// peek functionality
#define	PEEKB(A)	*(volatile u8  *)A
#define	PEEKW(A)	*(volatile u16 *)A
#define	PEEKL(A)	*(volatile u32 *)A

// void *memcpy(u8 *dest, const u8 *src, size_t count);
// u8 *memset(u8 *dest, u8 val, size_t count);

void build_character_ram(u8 *source, u16 *dest);
int  update_vector_number(u8 vector_no, void *exception_handler);

//u8   get_interrupt_priority_level();
void set_interrupt_priority_level(u16 value);
void disable_interrupts();
void restore_interrupts();

void address_error_exception_handler();

void panic();

/*
 * Global vars and objects accessible from within kernel
 */
extern void *heap_start;
extern void *heap_end;
extern void *character_ram;
extern void *user_start;
extern struct tty tty0;

#endif
