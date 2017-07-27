
//-----------------------------------------------------------------
/*! \file [main.c]
 *
 *  A brief description of what this file is.
 *
 *  \author [Craig Mullins]
 *  \date [24 July 2017]
 *
 */
//-----------------------------------------------------------------
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#define LED_HB GPIO_NUM_23
#define LED_WIFI_STATE GPIO_NUM_22

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

void HB_blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(LED_HB);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED_HB, GPIO_MODE_OUTPUT);
    int level = 0;
    while(1) {

    	gpio_set_level(LED_HB, level);
    	level = !level;
    	vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}

void wifi_blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(LED_WIFI_STATE);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED_WIFI_STATE, GPIO_MODE_OUTPUT);
    int level = 1;
    while(1) {

    	gpio_set_level(LED_WIFI_STATE, level);
    	vTaskDelay(200 / portTICK_PERIOD_MS);
    	level = !level;
    	gpio_set_level(LED_WIFI_STATE, level);
    	level = !level;
    	vTaskDelay(5000 / portTICK_PERIOD_MS);
    }}

void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid = "Mulnet",//"AndroidHotspot9968",//"mulnetportable",
            .password = "Onceuponatime",//"8ca31091e50f",
            .bssid_set = false
        }
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );

    xTaskCreate(&HB_blink_task, "heartbeat_led_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    xTaskCreate(&wifi_blink_task, "wifi_led_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);

    while (true) {

    }
}

