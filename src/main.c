#include "rpi-uart.h"
#include <stdio.h>
#include <time.h>

int main(){

    init_uart_driver(115200);
    struct timespec t;
    t.tv_nsec = 0;
    t.tv_sec = 2;
    
    int i = 0;
    while (i!=1000000){
        char r = (char) read_data();
        printf("%c\n", r);
        if (r == '\n'){
            break;
        }
        i++;
    }
    

    return 0;
}