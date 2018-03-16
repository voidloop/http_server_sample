#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

#include "web_server.h"


#define DEFAULT_SSID CONFIG_WIFI_SSID
#define DEFAULT_PWD CONFIG_WIFI_PASSWORD


EventGroupHandle_t wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;
const static char* TAG = "http_server_sample";


static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
	    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
    	ESP_ERROR_CHECK(esp_wifi_connect());
	break;
    case SYSTEM_EVENT_STA_GOT_IP:
	    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
	    ESP_ERROR_CHECK(esp_wifi_connect());
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
	break;
    default:
        break;
    }
    return ESP_OK;
}


void initialise_wifi()
{
    tcpip_adapter_init();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
	    .sta = {
	        .ssid = DEFAULT_SSID,
    	    .password = DEFAULT_PWD,
  	    },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}


void app_main()
{    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    
    initialise_wifi();

	xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, 0, 0, portMAX_DELAY);
    
    web_server_t server;
    web_server_start(&server);
}
