// Dorian LE QUERE
// Amandine PAILLAT
// SRT2025

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "lora.h"

#define WIFI_SSID "iPhone de Dorian"
#define WIFI_PASS "dorian22"
#define MQTT_BROKER_URI "mqtt://test.mosquitto.org"
#define MQTT_TOPIC "SALAD"
#define MQTT_PWD "POTATOTO"

static const char *LORA_TAG = "LoRa";

static const char *TAG = "wifi_mqtt";
static esp_mqtt_client_handle_t mqtt_client;

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch (event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connecté, publication...");
            esp_mqtt_client_publish(client, MQTT_TOPIC, MQTT_PWD, 0, 1, 0);
            ESP_LOGI(TAG, "Message envoyé : POTATOTO");
            vTaskDelay(1);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT déconnecté");
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Message reçu MQTT reçu: %.*s",
            event->data_len, event->data);
            lora_send_packet((uint8_t *)event->data, event->data_len);
            ESP_LOGI(LORA_TAG, "Message LoRa envoyé !");
        default:
            break;
    }
}

void mqtt_init() {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WiFi perdu, reconnexion...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "Connecté, adresse IP: " IPSTR, IP2STR(&event->ip_info.ip));
        mqtt_init();
    }
}

void wifi_init() {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi initialisé en mode station");
}

void task_rx(void *pvParameters)
{
    ESP_LOGI(pcTaskGetName(NULL), "Start");
    uint8_t buf[256];
    while(1) {
        lora_receive();
        if (lora_received()) {
            int rxLen = lora_receive_packet(buf, sizeof(buf));
            //if (strcmp((char *)buf, MQTT_PWD) == 0) {
            ESP_LOGI(pcTaskGetName(NULL), "%d byte packet received:[%.*s]", rxLen, rxLen, buf);
            //}
        }
        vTaskDelay(1);
    }
}

void task_tx(void *pvParameters) {
    ESP_LOGI(LORA_TAG, "Envoi d'un message LoRa...");
    lora_send_packet((uint8_t *)"Hello LoRa", 10);
    ESP_LOGI(LORA_TAG, "Message envoyé !");
    vTaskDelay(pdMS_TO_TICKS(5000));
}

void app_main() {
    wifi_init();
    if (lora_init() == 0) {
		ESP_LOGE(pcTaskGetName(NULL), "Does not recognize the module");
		while(1) {
			vTaskDelay(1);
		}
	}
    ESP_LOGI(pcTaskGetName(NULL), "Frequency is 868MHz");
	lora_set_frequency(868e6);
    lora_enable_crc();

	int cr = 1;
	int bw = 7;
	int sf = 7;
    lora_set_coding_rate(cr);
	ESP_LOGI(pcTaskGetName(NULL), "coding_rate=%d", cr);

	lora_set_bandwidth(bw);
	ESP_LOGI(pcTaskGetName(NULL), "bandwidth=%d", bw);

	lora_set_spreading_factor(sf);
	ESP_LOGI(pcTaskGetName(NULL), "spreading_factor=%d", sf);

    xTaskCreate(&task_rx, "RX", 1024*3, NULL, 5, NULL);
    // xTaskCreate(&task_tx, "TX", 1024*3, NULL, 5, NULL);
}