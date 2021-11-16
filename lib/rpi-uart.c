#include "rpi-uart.h"

static uart *rpi_uart_addresses;
int init_uart_driver(int baud_rate)
{
    int fd = -1;
    rpi_uart_addresses = malloc(sizeof(uart));

    unsigned int base_address = 0x3f215040;

    if ((fd = open("/dev/mem", O_RDWR, 0)) == -1)
    {
        printf("Error opening /dev/mem\n");
        return -1;
    }
    rpi_uart_addresses->base = (uint32_t *)mmap(
        0, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, base_address);

    if (rpi_uart_addresses->base == MAP_FAILED)
    {
        printf("Error during mapping UART\n");
        return -1;
    }

    set_uart_addresses();

    *(rpi_uart_addresses->AUX_MU_LCR_REG) = 0x81;

    if (set_baud_rate(baud_rate) == -1)
    {
        printf("Error in setting the baud rate\n");
        return -1;
    }
    *(rpi_uart_addresses->AUX_MU_IIR_REG) = 0x3;
    *(rpi_uart_addresses->AUX_MU_CNTL_REG) = 0x3;

    return 0;
}

void set_uart_addresses()
{
    rpi_uart_addresses->AUX_MU_IO_REG = rpi_uart_addresses->base;
    rpi_uart_addresses->AUX_MU_IIR_REG = rpi_uart_addresses->base + (0x4 / 4);
    rpi_uart_addresses->AUX_MU_IER_REG = rpi_uart_addresses->base + (0x8 / 4);
    rpi_uart_addresses->AUX_MU_LCR_REG = rpi_uart_addresses->base + (0xC / 4);
    rpi_uart_addresses->AUX_MU_MCR_REG = rpi_uart_addresses->base + (0x10 / 4);
    rpi_uart_addresses->AUX_MU_LSR_REG = rpi_uart_addresses->base + (0x14 / 4);
    rpi_uart_addresses->AUX_MU_MSR_REG = rpi_uart_addresses->base + (0x18 / 4);
    rpi_uart_addresses->AUX_MU_SCRATCH = rpi_uart_addresses->base + (0x1C / 4);
    rpi_uart_addresses->AUX_MU_CNTL_REG = rpi_uart_addresses->base + (0x20 / 4);
    rpi_uart_addresses->AUX_MU_STAT_REG = rpi_uart_addresses->base + (0x24 / 4);
    rpi_uart_addresses->AUX_MU_BAUD = rpi_uart_addresses->base + (0x28 / 4);
}
int set_baud_rate(int baud_rate)
{
    uint32_t baud_reg_value = (uint32_t)((CLOCK_UART / (8 * (baud_rate))) - 1);

    *(rpi_uart_addresses->AUX_MU_BAUD) = baud_reg_value & 0xFFFF;

    if (*(rpi_uart_addresses->AUX_MU_BAUD) & 0xFFFF != baud_reg_value)
    {
        return -1;
    }
    *(rpi_uart_addresses->AUX_MU_LCR_REG) = 0x1;
    return 0;
}

void exit_uart_driver() { free(rpi_uart_addresses); }

void send_data(unsigned int data)
{

    *(rpi_uart_addresses->AUX_MU_IO_REG) |= (data & 0xFF);
    while ((*(rpi_uart_addresses->AUX_MU_STAT_REG) >> 24) & 0xF != 0)
        ;
}

char *read_data()
{
    int number_messages = (*(rpi_uart_addresses->AUX_MU_STAT_REG) >> 16) & 0xF;
    char *string_received = malloc(sizeof(char) * number_messages + 1);
    for (int i = 0; i < number_messages; i++)
    {
        string_received[i] = (char)(*(rpi_uart_addresses->AUX_MU_IO_REG) & 0xFF);
    }
    string_received[number_messages] = 0;
    return string_received;
}