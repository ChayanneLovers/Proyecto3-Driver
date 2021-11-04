#include "rpi-uart.h"

uart* rpi_uart_addresses;
int init_uart_driver(int baud_rate, int word_len, int stop_bits, int parity) {
    int fd = -1;
    rpi_uart_addresses = malloc(sizeof(uart));

    uint32_t* base_address = 0x3f201000;

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

    if (set_baud_rate(baud_rate) == -1) {
        printf("Error in setting the baud rate\n");
        return -1;
    }
    if (set_word_len(word_len) == -1) {
        printf("Error in setting the word len\n");
        return -1;
    }
    if (set_fifo(0) == -1) {
        printf("Error in setting the char mode\n");
        return -1;
    }
    if (set_stop_bits(stop_bits) == -1) {
        printf("Error in setting the number of stop bits\n");
        return -1;
    }
    if (set_parity(parity) == -1) {
        printf("Error in setting the parity bits\n");
        return -1;
    }
    enable_uart();

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
    unsigned float float_part =
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

int set_word_len(int word_len) {
    int len = 0;
    switch (word_len) {
        case (8):
            *(rpi_uart_addresses->line_control_register) |= (3 << 5);
            len = 3;
            break;
        case (7):
            *(rpi_uart_addresses->line_control_register) &= ~(3 << 5);
            *(rpi_uart_addresses->line_control_register) |= (2 << 5);
            len = 2;
            break;
        case (6):
            *(rpi_uart_addresses->line_control_register) &= ~(3 << 5);
            *(rpi_uart_addresses->line_control_register) |= (1 << 5);
            len = 1;
            break;
        case (5):
            *(rpi_uart_addresses->line_control_register) &= ~(3 << 5);
            len = 0;
            break;
        default:
            *(rpi_uart_addresses->line_control_register) |= (3 << 5);
            break;
    }
    if ((*(rpi_uart_addresses->line_control_register) >> 5) & 3 != len) {
        return -1;
    }
    return 0;
}
void exit_uart_driver() { free(rpi_uart_addresses); }

int set_fifo(int mode) {
    if (mode) {
        *(rpi_uart_addresses->line_control_register) |= (1 << 4);
    } else {
        *(rpi_uart_addresses->line_control_register) &= ~(1 << 4);
    }
    if ((*(rpi_uart_addresses->line_control_register) >> 4) & 1 != mode) {
        return -1;
    }
    return 0;
}

int set_stop_bits(int stop_bits) {
    if (stop_bits > 1) {
        *(rpi_uart_addresses->line_control_register) |= (1 << 3);
    } else {
        *(rpi_uart_addresses->line_control_register) &= ~(1 << 3);
    }
    if ((*(rpi_uart_addresses->line_control_register) >> 3) &
        1 == stop_bits - 1) {
        return -1;
    }
    return 0;
}

int set_parity(int parity) {
    if (parity == -1) {
        *(rpi_uart_addresses->line_control_register) &= ~(1 << 1);
    } else {
        *(rpi_uart_addresses->line_control_register) |= (1 << 1);
        if (parity) {
            *(rpi_uart_addresses->line_control_register) |= (1 << 2);
        } else {
            *(rpi_uart_addresses->line_control_register) &= ~(1 << 2);
        }
    }

    return 0;
}

void enable_uart() {
    *(rpi_uart_addresses->control_register) |= 1;
    *(rpi_uart_addresses->control_register) |= (3 << 8);
}

void send_data(unsigned int data) {
    *(rpi_uart_addresses->data_register) = 0;
    *(rpi_uart_addresses->data_register) |= data & (0xFF);
}

uint8_t read_data() {
    while ((*(rpi_uart_addresses->raw_interrupt_register) >> 5) & 0xF != 1)
        ;

    *(rpi_uart_addresses->interrupt_clear_register) |= 1 << 5;
    return (uint8_t) * (rpi_uart_addresses->data_register) & 0xFF;
}