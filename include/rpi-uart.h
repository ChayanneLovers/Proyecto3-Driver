#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h> //mmap
#include <err.h> //error handling
#include <fcntl.h> //file ops
#include <unistd.h> //usleep


#define CLOCK_UART 250000000

typedef struct {
    volatile uint32_t* base;
    volatile uint32_t* AUX_MU_IO_REG;
    volatile uint32_t* AUX_MU_IIR_REG;
    volatile uint32_t* AUX_MU_IER_REG;
    volatile uint32_t* AUX_MU_LCR_REG;
    volatile uint32_t* AUX_MU_MCR_REG;
    volatile uint32_t* AUX_MU_LSR_REG;
    volatile uint32_t* AUX_MU_MSR_REG;
    volatile uint32_t* AUX_MU_SCRATCH;
    volatile uint32_t* AUX_MU_CNTL_REG;
    volatile uint32_t* AUX_MU_STAT_REG;
    volatile uint32_t* AUX_MU_BAUD;

} uart;

int init_uart_driver(int baud_rate);

void exit_uart_driver();

int set_baud_rate(int baud_rate);

void set_uart_addresses();


void send_data(unsigned int data);
char* read_data();


