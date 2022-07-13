#include <uart.h>

// Takes 4 byte word to write to register
static inline void mmio_write(uint32_t reg, uint32_t data)
{
    *(volatile uint32_t*)reg = data;
}

// Returns 4 byte word from register
static inline uint32_t mmio_read(uint32_t reg)
{
    return *(volatile uint32_t*)reg;
}

// Loop <delay> times in a way that the compiler won't optimize away
static inline void delay(int32_t count)
{
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
            : "=r"(count): [count]"0"(count) : "cc");
}

// Sets up UART hardware for use
void uart_init()
{
    // UART's control register, disables all aspects of UART hardware
    mmio_write(UART0_CR, 0x00000000);

    // Marks that pins should be disabled
    mmio_write(GPPUD, 0x00000000);
    delay(150);

    // Marks 14 and 15 as pins to be disabled
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);

    // Makes it take effect
    mmio_write(GPPUDCLK0, 0x00000000);

    // Sets all flags in Interrupt Clear Register (clears all pending interrupts from UART hardware)
    mmio_write(UART0_ICR, 0x7FF);

    // Sets baud rate of connection
    mmio_write(UART0_IBRD, 1);
    mmio_write(UART0_FBRD, 40);

    /* Writes bit 4, 5 and 6 to line control register
    Bit 4 means UART hardware will hold data in 8 item deep FIFO, instead of 1 item deep register
    Setting 5 and 6 means data sent or received will have 8-bit long words */
    mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

    // Disables all interrupts from UART
    mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
            (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));

    /* Writes bits 0, 8 and 9
    Bit 0 enables UART hardware
    Bit 8 enables ability to receive data
    Bit 9 enables ability to transmit data */
    mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

// Allows writing of a character
void uart_putc(unsigned char c)
{
    while ( mmio_read(UART0_FR) & (1 << 5) ) { }
    mmio_write(UART0_DR, c);
}

// Allows reading of a character
unsigned char uart_getc()
{
    while ( mmio_read(UART0_FR) & (1 << 4) ) { }
    return mmio_read(UART0_DR);
}

// Allows writing of whole strings
void uart_puts(const char* str)
{
    for (size_t i = 0; str[i] != '\0'; i ++)
        uart_putc((unsigned char)str[i]);
}