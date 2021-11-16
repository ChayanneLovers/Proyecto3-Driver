#include "rpi-uart.h"
#include <stdio.h>
#include <time.h>

int main(){

    init_uart_driver(115200);
    struct timespec t;
    t.tv_nsec = 0;
    t.tv_sec = 1;
    

    while (1){
        char r = 'G';
        send_data((unsigned int)r);
        nanosleep(&t, NULL);
    }
    

    return 0;
}