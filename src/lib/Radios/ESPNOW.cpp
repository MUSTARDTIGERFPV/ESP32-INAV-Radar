
#include "ESPNOW.h"
#include "RadioManager.h"

#if defined(PLATFORM_ESP8266)
void espnow_receive(uint8_t *mac, uint8_t *incomingData, uint8_t packetSize)
#elif defined(PLATFORM_ESP32)
void espnow_receive(const uint8_t *mac_addr, const uint8_t *incomingData, int packetSize)
#endif
{
    RadioManager::getSingleton()->receive(incomingData, packetSize);
}

ESPNOW *espnowInstance = NULL;

ESPNOW::ESPNOW()
{
}

ESPNOW* ESPNOW::getSingleton()
{
    if (espnowInstance == NULL)
    {
        espnowInstance = new ESPNOW();
    }
    return espnowInstance;
}

void ESPNOW::transmit(air_type0_t *air_0)
{
    uint8_t buf[sizeof(air_type0_t)];
    memcpy_P(buf, air_0, sizeof(air_type0_t));
    esp_now_send(broadcastAddress, buf, sizeof(air_type0_t));
}

int ESPNOW::begin()
{
#ifdef PLATFORM_ESP32
    WiFi.mode(WIFI_MODE_AP);
#elif defined(PLATFORM_ESP8266)
    WiFi.mode(WIFI_AP);
#endif
    WiFi.disconnect();
    esp_now_init();
#ifdef PLATFORM_ESP8266
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
#elif defined(PLATFORM_ESP32)
    memset((void*)&peerInfo, 0, sizeof(esp_now_peer_info_t));
    peerInfo.ifidx = WIFI_IF_STA;
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
#endif
    esp_now_register_recv_cb(espnow_receive);
    return 0;
}

void ESPNOW::loop()
{
}