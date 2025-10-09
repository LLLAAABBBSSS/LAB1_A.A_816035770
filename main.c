/*
    Name: Riyadh B. Ali  
    ID: 816035770
*/
 
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_spi_flash.h"

#define UART_PORT       UART_NUM_0                                          //UART port 0 
#define BUF_SIZE        1024
#define LED_GPIO        2                                                   //LED stuff from lab0
#define five_MS         500                                                 //time debounce from question(500ms)
 
typedef enum                                                                //done to define the states for easy readability 
{
    STATE_OFF = 0,
    STATE_ON
} fsm_state_t;

fsm_state_t state = STATE_OFF;                                              //upon startup; Global Vars
char same_char = 0;
TickType_t last_tick = 0;

/*void config_uart() 
{
    const uart_config_t uart_config = 
    {
        .baud_rate = 115200, 
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,                                                   
    };
    uart_param_config(UART_PORT, &uart_config);
    uart_driver_install(UART_PORT, BUF_SIZE, 0, 0, NULL, 0);

    if (uart_set_baudrate(UART_NUM_0, baud_rate) == ESP_OK) 
    {
        printf("ESP_OK");
    } else 
    {
        printf("ESP_ERR_INVALID_ARG");
    }
}*/

/*void config_led()
{
    gpio_set_level(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
}*/

void update(char c, TickType_t now) 
{
    TickType_t interval = ((now - last_tick) * portTICK_PERIOD_MS);         //interval = time gap obv 

    if (state == STATE_ON) 
    {
        if ((c == same_char) && (interval < five_MS))                       //check for like character in 500ms to ignore
        {
            
        } else if ((c == same_char) && (interval > five_MS))                //check for like character after 500ms 
        {
            state = STATE_OFF;                                              //ON to OFF 
        } else if (c != same_char)                                          //check for diff character 
        {
            state = STATE_OFF;                                              //ON to OFF
        }
    }
    else if (state == STATE_OFF) 
    {
        if ((c == same_char) && (interval < five_MS))                       //check for like character in 500ms to ignore 
        {
            
        } else if ((c == same_char) && (interval > five_MS))                //check for like character after 500ms 
        {
            state = STATE_ON;                                               //OFF to ON 
        } else if (c != same_char)                                          //check for diff character 
        {
            state = STATE_ON;                                               //OFF to ON 
        }
    }

    if (state == STATE_ON) 
    {
        gpio_set_level(LED_GPIO, 1);                                        //setting pin to high
    } else                                                                  //function from ESP8266 RTOS API guide
    {
        gpio_set_level(LED_GPIO, 0);                                        //setting pin to low 
    }

    same_char = c;                                                          //storage 
    last_tick = now;
}


void uart_fsm_task(void *pvParameters) 
{
    uint8_t data[1];                                                                           //buffer 
    while (1) 
    {
        int info = uart_read_bytes(UART_PORT, data, 1, 20 / portTICK_PERIOD_MS);               //function used to read character 
        if (info > 0) 
        {
            TickType_t now = xTaskGetTickCount();                                              //current tick
            update((char)data[0], now);
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);                                                   //every 50ms, factor of 500ms, run
    }
}

void app_main() 
{
    //config_uart();
    //config_led();
    xTaskCreate(uart_fsm_task, "uart_fsm_task", 2048, NULL, 5, NULL);
}
    

