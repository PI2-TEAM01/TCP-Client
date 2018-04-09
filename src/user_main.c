#include "conn_ap.h"
#include "esp_common.h"
#include "tcp_client.h"

uint32 user_rf_cal_sector_set(void) {
  flash_size_map size_map = system_get_flash_size_map();
  uint32 rf_cal_sec = 0;

  switch (size_map) {
  case FLASH_SIZE_4M_MAP_256_256:
    rf_cal_sec = 128 - 5;
    break;

  case FLASH_SIZE_8M_MAP_512_512:
    rf_cal_sec = 256 - 5;
    break;

  case FLASH_SIZE_16M_MAP_512_512:
  case FLASH_SIZE_16M_MAP_1024_1024:
    rf_cal_sec = 512 - 5;
    break;

  case FLASH_SIZE_32M_MAP_512_512:
  case FLASH_SIZE_32M_MAP_1024_1024:
    rf_cal_sec = 1024 - 5;
    break;

  default:
    rf_cal_sec = 0;
    break;
  }
  return rf_cal_sec;
}

void TcpLocalClient_Test() {
  conn_ap_init();
  vTaskDelay(1000);
  TcpLocalClient();
}

void user_init(void) {
  espconn_init();
  xTaskCreate(TcpLocalClient_Test, "TcpLocalClient_Test", 500, NULL, 6, NULL);
}
