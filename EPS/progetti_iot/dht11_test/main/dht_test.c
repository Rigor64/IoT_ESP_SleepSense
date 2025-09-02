/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp32-dht11.h"

static const char *TAG = "example_DHT";

#define CONFIG_DHT11_PIN 25
#define CONFIG_CONNECTION_TIMEOUT 5


static void configure_sensor(void)
{
    ESP_LOGI(TAG, "Example configured to read DHT 11 sensor");
}

void app_main(void)
{

    /* Configure the peripheral according to the LED type */
    configure_sensor();

     dht11_t dht11_sensor;
    dht11_sensor.dht11_pin = CONFIG_DHT11_PIN;

    // Read data
    while(1)
    {
      if(!dht11_read(&dht11_sensor, CONFIG_CONNECTION_TIMEOUT))
      {  
        printf("[Temperature]> %.2f \n",dht11_sensor.temperature);
        printf("[Humidity]> %.2f \n",dht11_sensor.humidity);
      }
      vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}
