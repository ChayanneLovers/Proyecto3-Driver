#include "rpi-uart.h"

volatile uart* rpi_uart_addresses;
int init_uart_driver(int baud_rate) {
    int fd = -1;
    rpi_uart_addresses = malloc(sizeof(uart));

    unsigned int base_address = 0x3f201000;

    if ((fd = open("/dev/mem", O_RDWR, 0)) == -1) {
        printf("Error opening /dev/mem\n");
        return -1;
    }
    rpi_uart_addresses->base = (uint32_t*)mmap(
        0, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, base_address);

    if (rpi_uart_addresses->base == MAP_FAILED) {
        printf("Error during mapping UART\n");
        return -1;
    }

    set_uart_addresses();
   

    *(rpi_uart_addresses->control_register) &= 0xFFFFFFF0; 
    *(rpi_uart_addresses->control_register) |= 0x200;
    *(rpi_uart_addresses->interrupt_clear_register) |= 0xFF;
    *(rpi_uart_addresses->line_control_register) &= 0xFFFFFF6F;
    


    if (set_baud_rate(baud_rate) == -1) {
        printf("Error in setting the baud rate\n");
        return -1;
    }
    enable_uart();
    printf("El registro de mascaraa de seteo es -> %u\n", *(rpi_uart_addresses->interrupt_mask_clear_register));
    *(rpi_uart_addresses->intergration_test_output_register) |= 400;
    return 0;
}

void set_uart_addresses() {
    rpi_uart_addresses->data_register = rpi_uart_addresses->base;
    rpi_uart_addresses->status_error_register =
        rpi_uart_addresses->base + (0x4 / 4);
    rpi_uart_addresses->flag_register = rpi_uart_addresses->base + (0x18 / 4);
    rpi_uart_addresses->ILPR = rpi_uart_addresses->base + (0x20 / 4);
    rpi_uart_addresses->integer_baud_rate_divisor =
        rpi_uart_addresses->base + (0x24 / 4);
    rpi_uart_addresses->fractional_baud_rate_divisor =
        rpi_uart_addresses->base + (0x28 / 4);
    rpi_uart_addresses->line_control_register =
        rpi_uart_addresses->base + (0x2C / 4);
    rpi_uart_addresses->control_register =
        rpi_uart_addresses->base + (0x30 / 4);
    rpi_uart_addresses->interrupt_fifo_register =
        rpi_uart_addresses->base + (0x34 / 4);
    rpi_uart_addresses->interrupt_mask_clear_register =
        rpi_uart_addresses->base + (0x38 / 4);
    rpi_uart_addresses->raw_interrupt_register =
        rpi_uart_addresses->base + (0x3C / 4);
    rpi_uart_addresses->masked_interrupt_register =
        rpi_uart_addresses->base + (0x40 / 4);
    rpi_uart_addresses->interrupt_clear_register =
        rpi_uart_addresses->base + (0x44 / 4);
    rpi_uart_addresses->DMA_control_register =
        rpi_uart_addresses->base + (0x48 / 4);
    rpi_uart_addresses->test_control_register =
        rpi_uart_addresses->base + (0x80 / 4);
    rpi_uart_addresses->intergration_test_input_register =
        rpi_uart_addresses->base + (0x84 / 4);
    rpi_uart_addresses->intergration_test_output_register =
        rpi_uart_addresses->base + (0x88 / 4);
    rpi_uart_addresses->data_test_register =
        rpi_uart_addresses->base + (0x8C / 4);
}
int set_baud_rate(int baud_rate) {
    unsigned long integer_part_divisor =
        (unsigned long)48000000 / (16 * baud_rate);
    float float_part =
        integer_part_divisor - ((unsigned long)48000000 / (16 * baud_rate));
    unsigned long float_part_divisor = (unsigned long)(float_part * 64 + 0.5);

    *(rpi_uart_addresses->integer_baud_rate_divisor) =
        integer_part_divisor & 0xFFFF;
    *(rpi_uart_addresses->fractional_baud_rate_divisor) =
        float_part_divisor & 0x3F;

    if (*(rpi_uart_addresses->integer_baud_rate_divisor) !=
                integer_part_divisor &
            0xFFFF ||
        *(rpi_uart_addresses->fractional_baud_rate_divisor) !=
                float_part_divisor &
            0x3F) {
        return -1;
    }


    return 0;
}

void exit_uart_driver() { free(rpi_uart_addresses); }



void enable_uart() {
    *(rpi_uart_addresses->masked_interrupt_register) |= 0x50;
    *(rpi_uart_addresses->control_register) |= 0x1;
}

void send_data(unsigned int data) {
    *(rpi_uart_addresses->data_register) = data&0xFF;
    while (*(rpi_uart_addresses->flag_register)&(1<<7) != 1<<7);
    while (*(rpi_uart_addresses->flag_register)&(1<<3) != 1<<3);
}


int read_data() {
    
    while (*(rpi_uart_addresses->raw_interrupt_register) != 0x20);
     
    
    int character = *(rpi_uart_addresses->data_register);
    *(rpi_uart_addresses->interrupt_clear_register) |= 0x20;
    
    return character;
}