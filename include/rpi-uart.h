#include "stdint.h"
#include "stdlib.h"
typedef struct 
{
    uint32_t* base;
    uint32_t* data_register;
    uint32_t* status_error_register;
    uint32_t* flag_register;
    uint32_t* ILPR;
    uint32_t* integer_baud_rate_divisor;
    uint32_t* fractional_baud_rate_divisor;
    uint32_t* line_control_register;
    uint32_t* control_register;
    uint32_t* interrupt_fifo_register;
    uint32_t* interrupt_mask_clear_register;
    uint32_t* raw_interrupt_register;
    uint32_t* masked_interrupt_register;
    uint32_t* interrupt_clear_register;
    uint32_t* DMA_control_register;
    uint32_t* test_control_register;
    uint32_t* intergration_test_input_register;
    uint32_t* intergration_test_output_register;
    uint32_t* data_test_register;
}uart;



int init_uart_driver();


void exit_uart_driver();

