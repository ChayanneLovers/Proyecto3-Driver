#include "rpi-uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){

    init_uart_driver(115200);
    struct timespec t;
    t.tv_nsec = 0;
    t.tv_sec = 2;
    int light = 0;
    int moisture = 0;
    int temp = 0;
    int humity = 0;
    int i = 0;
    int mode = 0;
    char r;
    send_data(4);
    while (1){
        r = (char) read_data();
        printf("Llego -> %x\n",r);
        if (mode  == 0){
            if (r == 'L'){
                mode = 1;
                i++;
                light = 0;
                continue;
            }else if (r == 'S')
            {
                i++;
                moisture = 0;
                mode = 2;
                continue;
            }else if (r == 'H')
            {
                i++;
                humity = 0;
                mode = 3;
                continue;
            }else if (r == 'T')
            {
                i++;
                temp = 0;
                mode = 4;
                continue;
            }else if (r == '\n'){
                mode = 0;
                if (i != 0){
                    break;
                }
            }
            
        }else if (mode == 1){
            if (r == ' '){
                mode = 0;
                continue;
            }
            int tmp = atoi(&r);
            light = light*10 + tmp;
            continue;
        }
        else if (mode == 2){
            if (r == ' '){
                mode = 0;
                continue;
            }
            int tmp = atoi(&r);
            moisture = moisture*10 + tmp;

            continue;
        }else if (mode == 3){
            if (r == ' '){
                mode = 0;
                continue;
            }
            int tmp = atoi(&r);
            humity = humity*10 + tmp;

            continue;
        }
        else if (mode == 4){
            if (r == ' '){
                mode = 0;
                continue;
            }
            int tmp = atoi(&r);
            temp = temp*10 + tmp;

            continue;
        }
    }
    
    printf("{\"soilhum\":%i, \"lum\":%i, \"temp\":%i, \"hum\":%i}\n", moisture, light, temp, humity);
    

    return 0;
}