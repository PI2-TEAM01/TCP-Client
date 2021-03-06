#include "esp_common.h"
#include "espconn.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "conn_ap.h"

#define DEMO_AP_SSID      "cc"
#define DEMO_AP_PASSWORD "12345678"

#define SOFT_AP_SSID "DEMO_AP"
#define SOFT_AP_PASSWORD "12345678"

//*********************************************************

//callback events function
void wifi_handle_event_cb(System_Event_t *evt)
{
    printf("event %x\n", evt->event_id);

    switch (evt->event_id) {
        case EVENT_STAMODE_CONNECTED:
            printf("connect to ssid %s, channel %d\n", evt->event_info.connected.ssid,
                    evt->event_info.connected.channel);
            break;
        case EVENT_STAMODE_DISCONNECTED:
            printf("disconnect from ssid %s, reason %d\n", evt->event_info.disconnected.ssid,
                    evt->event_info.disconnected.reason);
            break;
        case EVENT_STAMODE_AUTHMODE_CHANGE:
            printf("mode: %d -> %d\n", evt->event_info.auth_change.old_mode, evt->event_info.auth_change.new_mode);
            break;
        case EVENT_STAMODE_GOT_IP:
            printf("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR, IP2STR(&evt->event_info.got_ip.ip),
                    IP2STR(&evt->event_info.got_ip.mask), IP2STR(&evt->event_info.got_ip.gw));
            printf("\n");
            break;
        case EVENT_SOFTAPMODE_STACONNECTED:
            printf("station: " MACSTR "join, AID = %d\n", MAC2STR(evt->event_info.sta_connected.mac),
                    evt->event_info.sta_connected.aid);
            break;
        case EVENT_SOFTAPMODE_STADISCONNECTED:
            printf("station: " MACSTR "leave, AID = %d\n", MAC2STR(evt->event_info.sta_disconnected.mac),
                    evt->event_info.sta_disconnected.aid);
            break;
        default:
            break;
    }
}

//connection init function
void conn_ap_init(void)
{
    wifi_set_opmode(STATIONAP_MODE);
    struct station_config config;
    memset(&config, 0, sizeof(config));  //set value of config from address of &config to width of size to be value '0'

    sprintf(config.ssid, DEMO_AP_SSID);
    sprintf(config.password, DEMO_AP_PASSWORD);

    wifi_station_set_config(&config);
    wifi_set_event_handler_cb(wifi_handle_event_cb);
    wifi_station_connect();
}
