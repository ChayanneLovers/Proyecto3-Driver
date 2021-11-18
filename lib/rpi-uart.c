#include "rpi-uart.h"

static uart *rpi_uart_addresses;
int init_uart_driver(int baud_rate)
{
    int fd = -1;
    rpi_uart_addresses = malloc(sizeof(uart));

    unsigned int base_address = 0x3f215000;
    unsigned int base_address_gpio = 0x3f200000;


    if ((fd = open("/dev/mem", O_RDWR, 0)) == -1)
    {
        printf("Error opening /dev/mem\n");
        return -1;
    }
    rpi_uart_addresses->base = (uint32_t *)mmap(
        0, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, base_address);

    volatile uint32_t* gpio_r= (uint32_t*)mmap(
        0, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, base_address_gpio);

    volatile uint32_t* gpfsel1 =gpio_r + 1;
    *gpfsel1 &= ~(0xEF000);
    *gpfsel1 |= 0x12000;


    if (rpi_uart_addresses->base == MAP_FAILED | gpio_r == MAP_FAILED) {
        printf("Error during mapping UART\n");
        return -1;
    }

    rpi_uart_addresses->base += (0x40/4); 

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

    *(rpi_uart_addresses->base - (0x40/4) + (0x4/4)) = 0x1;

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

void send_data(char data)
{

    *(rpi_uart_addresses->AUX_MU_IER_REG) |= 0x4;
    *(rpi_uart_addresses->AUX_MU_IO_REG) = data;
    while (*(rpi_uart_addresses->AUX_MU_IER_REG) != 195);
        ;
    *(rpi_uart_addresses->AUX_MU_IER_REG) |= 0x4;
}

char read_data()
{
       
   while (*(rpi_uart_addresses->AUX_MU_IER_REG) != 196);
    char result = (char)(*(rpi_uart_addresses->AUX_MU_IO_REG));
    *(rpi_uart_addresses->AUX_MU_IER_REG) |= 0x2; //0 000000  00000 0 000  0 00 0010
    return result;
}