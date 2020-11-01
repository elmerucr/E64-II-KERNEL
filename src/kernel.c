#include "kernel.h"
#include "vicv.h"

struct surface_blit default_screen =
{
	0b00001000,     // flags 0 - tile mode, simple color, color per tile
	0b00000000,     // flags 1 - no stretching, mirroring etc
	0b01010110,     // height 2^%101 = 32 chars = 256 pixels, width 2^%110 = 64 chars  = 512 pixels
	0b00000000,     // currently unused.... :-)
	0,              // x
	16,             // y
	0xF0A0,         // foreground color
	0xF222,         // background color
    (uint16_t *)9,
    (uint8_t *)10,
    (uint16_t *)11,
    (uint16_t *)12,
    (void *)13
};

int update_vector_table(uint8_t vector_no, void *exception_handler)
{
    if( (vector_no < 2) ) return 0xff;

    // the value being written by this function is a void * (address to an exception handler)
    // this value needs to be written into a memory location that is
    // calculated based on the vector number which is a pointer by itself
    // hence, the type is void **
    void **vector_address = (void *)((uint32_t)vector_no << 2);

    *vector_address = exception_handler;

    return 0;
}


uint8_t peekb(uint32_t address)
{
    return *(uint8_t *)address;
}


uint16_t peekw(uint32_t address)
{
    return *(uint16_t *)address;
}


uint32_t peekl(uint32_t address)
{
    return *(uint32_t *)address;
}


void pokeb(uint32_t address, uint8_t  byte)
{
    *(uint8_t *)address = byte;
}


void pokew(uint32_t address, uint16_t word)
{
    *(uint16_t *)address = word;
}


void pokel(uint32_t address, uint32_t lwrd)
{
    *(uint32_t *)address = lwrd;
}


uint8_t *memcpy(uint8_t *dest, const uint8_t *src, size_t count)
{
    for(uint32_t i=0; i<count; i++) dest[i] = src[i];
    return dest;
}


uint8_t *memset(uint8_t *dest, uint8_t val, size_t count)
{
    for(uint32_t i=0; i<count; i++) dest[i] = val;
    return dest;
}


void build_character_ram(uint8_t *source, uint16_t *dest)
{
    for(int i=0; i<2048; i++)
    {
        uint8_t byte = source[i];
        uint8_t count = 8;
        while(count--)
        {
            *dest = (byte & 0b10000000) ? C64_GREY : 0x0000;
            dest++;
            byte = byte << 1;
        }
    }
}


void set_interrupt_priority_level(uint16_t value)
{
    value = (value & 0b00000111) << 8;

    __asm__ __volatile__
    (
        "move.w %0,%%d0      \n\t"
        "move.w %%sr,%%d1    \n\t"
        "andi.w #0xf8ff,%%d1 \n\t"
        "or.w %%d0,%%d1      \n\t"
        "move.w %%d1,%%sr    "
        :               /* outputs */
        : "g"(value)    /* inputs  */
        :               /* clobbered regs */
    );
}