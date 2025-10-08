/* 
   Name: Riyadh B. Ali
   ID: 816035770
*/

#include <stdio.h>
//#include <ctype.h>
//#include <unistd.h>  
//#include <sys/times.h> 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"                      
#include "driver/uart.h"
#include "esp_err.h"

#define SLOTX 4
#define CYCLEX 5
#define SLOT_T 5000                                                                // 5 sec slot time (ms)

/*void config()
{
    uart_config_t uart_config =
    {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, 

    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);

    if (uart_set_baudrate(UART_NUM_0, baud_rate) == ESP_OK) 
    {
        printf("ESP_OK");
    } else 
    {
        printf("ESP_ERR_INVALID_ARG");
    }
}*/

void Sleep(int sec)
{
	vTaskDelay(sec * 1000/portTICK_PERIOD_MS);
}

void one() {                                                                        //task code 
    printf("task 1 running\n");
    Sleep(1); 
}
void two() {
    printf("task 2 running\n");
    Sleep(1);
}
void three() {
    printf("task 3 running\n");
    Sleep(1);
}
void four() {
    printf("task 4 running\n");
    Sleep(1);
}
void five() {
    printf("task 5 running\n");
    Sleep(1);
}
void burn() {
    TickType_t start = xTaskGetTickCount();
    while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(SLOT_T)) {
        // burn CPU cycles for SLOT_T ms
    }
    TickType_t elapsed = xTaskGetTickCount() - start;
    printf("burn time = %d ms\n\n", elapsed * portTICK_PERIOD_MS);
}

void (*ttable[SLOTX][CYCLEX])() = {
    {one, two, burn, burn, burn},
    {one, three, burn, burn, burn},
    {one, four, burn, burn, burn},
    {burn, burn, burn, burn, burn}
};

void dispatcher_task(void *pvParameter) {
    while (1) {
        for (int slot = 0; slot < SLOTX; slot++) {
            for (int cycle = 0; cycle < CYCLEX; cycle++) {
                (*ttable[slot][cycle])(); 
            }
        }
    }
}


void app_main() 
{
    //config();
    //sleep(1);
    
    
    printf("Starting cyclic task dispatcher...\n");
    xTaskCreate(&dispatcher_task, "dispatcher_task", 2048, NULL, 5, NULL);

    /*while(1)
    {
        dispatcher_task();
    }*/


}
