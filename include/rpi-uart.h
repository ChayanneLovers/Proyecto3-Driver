#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h> //mmap
#include <err.h> //error handling
#include <fcntl.h> //file ops
#include <unistd.h> //usleep


unsigned long uart_frequency = 48000000;
typedef struct {
    volatile uint32_t* base;
    volatile uint32_t* data_register;
    volatile uint32_t* status_error_register;
    volatile uint32_t* flag_register;
    volatile uint32_t* ILPR;
    volatile uint32_t* integer_baud_rate_divisor;
    volatile uint32_t* fractional_baud_rate_divisor;
    volatile uint32_t* line_control_register;
    volatile uint32_t* control_register;
    volatile uint32_t* interrupt_fifo_register;
    volatile uint32_t* interrupt_mask_clear_register;
    volatile uint32_t* raw_interrupt_register;
    volatile uint32_t* masked_interrupt_register;
    volatile uint32_t* interrupt_clear_register;
    volatile uint32_t* DMA_control_register;
    volatile uint32_t* test_control_register;
    volatile uint32_t* intergration_test_input_register;
    volatile uint32_t* intergration_test_output_register;
    volatile uint32_t* data_test_register;
} uart;
int init_uart_driver(int baud_rate);

void exit_uart_driver();

int set_baud_rate(int baud_rate);

void set_uart_addresses();


void enable_uart();

void send_data(unsigned int data);
int read_data();

void stop_send();

void start_send();

void start_reading();

void stop_reading();
