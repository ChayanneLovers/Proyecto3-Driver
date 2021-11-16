#include "rpi-uart.h"
#include <stdio.h>
#include <time.h>

int main(){

    init_uart_driver(115200, 8, 1, -1);
    struct timespec t;
    t.tv_nsec = 0;
    t.tv_sec = 2;
    

    while (1){
        char r = (unsigned char) read_data();

        printf("%c\n", r);
        nanosleep(&t, NULL);
        send_data((unsigned int)r);
    }
    

    return 0;
}