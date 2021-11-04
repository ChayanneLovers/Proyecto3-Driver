#include "rpi-uart.h"


uart* rpi_uart_addresses;
int init_uart_driver(){
    int exit_code = 0;
    rpi_uart_addresses = malloc(sizeof(uart));

    rpi_uart_addresses->base = 0x3f201000;

    return exit_code;
}


void exit_uart_driver(){
    free(rpi_uart_addresses);
}