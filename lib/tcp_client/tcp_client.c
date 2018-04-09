#include "c_types.h"
#include "esp_common.h"
#include "espconn.h"
#include "user_config.h"
#include "tcp_client.h"

#define DEMO_AP_SSID      "cc"
#define DEMO_AP_PASSWORD "12345678"
#define SOFT_AP_SSID "DEMO_AP"
#define SOFT_AP_PASSWORD "12345678"

/*--------------------------------------------------------------
 Tcp Client
 -------------------------------------------------------------*/
void TcpClientConnect(void*arg)
{
    struct espconn* tcp_server_local = arg;
#if TCP_CLIENT_KEEP_ALIVE_ENABLE
    espconn_set_opt(tcp_server_local, BIT(3)); //enable keep alive ,this api must call in connect callback

    uint32 keep_alvie = 0;
    keep_alvie = TCP_CLIENT_KEEP_ALIVE_IDLE_S;
    espconn_set_keepalive(tcp_server_local, ESPCONN_KEEPIDLE, &keep_alvie);
    keep_alvie = TCP_CLIENT_RETRY_INTVL_S;
    espconn_set_keepalive(tcp_server_local, ESPCONN_KEEPINTVL, &keep_alvie);
    keep_alvie = keep_alvie = TCP_CLIENT_RETRY_INTVL_S;
    espconn_set_keepalive(tcp_server_local, ESPCONN_KEEPCNT, &keep_alvie);
    DBG_PRINT("keep alive enable\n");
#endif
    DBG_LINES("TCP CLIENT CONNECT");
    DBG_PRINT("tcp client ip:%d.%d.%d.%d port:%d\n", tcp_server_local->proto.tcp->remote_ip[0],
            tcp_server_local->proto.tcp->remote_ip[1], tcp_server_local->proto.tcp->remote_ip[2],
            tcp_server_local->proto.tcp->remote_ip[3], tcp_server_local->proto.tcp->remote_port);
    espconn_send(tcp_server_local, TCP_CLIENT_GREETING, strlen(TCP_CLIENT_GREETING));

}

void TcpClientDisConnect(void* arg)
{
    struct espconn* tcp_server_local = arg;
    DBG_LINES("TCP CLIENT DISCONNECT");
    DBG_PRINT("tcp client ip:%d.%d.%d.%d port:%d\n", tcp_server_local->proto.tcp->remote_ip[0],
            tcp_server_local->proto.tcp->remote_ip[1], tcp_server_local->proto.tcp->remote_ip[2],
            tcp_server_local->proto.tcp->remote_ip[3], tcp_server_local->proto.tcp->remote_port);
}
void TcpClienSendCb(void* arg)
{
    struct espconn* tcp_server_local = arg;
    DBG_LINES("TCP CLIENT SendCb");
    DBG_PRINT("tcp client ip:%d.%d.%d.%d port:%d\n", tcp_server_local->proto.tcp->remote_ip[0],
            tcp_server_local->proto.tcp->remote_ip[1], tcp_server_local->proto.tcp->remote_ip[2],
            tcp_server_local->proto.tcp->remote_ip[3], tcp_server_local->proto.tcp->remote_port);
}

void TcpRecvCb(void *arg, char *pdata, unsigned short len)
{
    struct espconn* tcp_server_local = arg;
    DBG_PRINT("Recv tcp client ip:%d.%d.%d.%d port:%d len:%d\n", tcp_server_local->proto.tcp->remote_ip[0],
            tcp_server_local->proto.tcp->remote_ip[1], tcp_server_local->proto.tcp->remote_ip[2],
            tcp_server_local->proto.tcp->remote_ip[3], tcp_server_local->proto.tcp->remote_port, len);

    espconn_send(tcp_server_local, pdata, len);
}
void TcpReconnectCb(void *arg, sint8 err)
{
    struct espconn* tcp_server_local = arg;
    DBG_LINES("TCP CLIENT RECONNECT");
    DBG_PRINT("status:%d\n", err);
    DBG_PRINT("tcp client ip:%d.%d.%d.%d port:%d\n", tcp_server_local->proto.tcp->remote_ip[0],
            tcp_server_local->proto.tcp->remote_ip[1], tcp_server_local->proto.tcp->remote_ip[2],
            tcp_server_local->proto.tcp->remote_ip[3], tcp_server_local->proto.tcp->remote_port);
}


//main function used for task
void TcpLocalClient(void* arg)
{
    //Get the connection status of the ESP8266 WiFi station
    uint8 con_status = wifi_station_get_connect_status();

    while (con_status != STATION_GOT_IP) {
        con_status = wifi_station_get_connect_status();
        DBG_PRINT("Connect ap %s\n",
                con_status==STATION_IDLE?"IDLE":con_status==STATION_CONNECTING? "Connecting...":con_status==STATION_WRONG_PASSWORD? "Password":con_status==STATION_NO_AP_FOUND? "ap_not_find":con_status==STATION_CONNECT_FAIL?"Connect fail":"Get ip");
        vTaskDelay(100);
    }

    //espconn structure
    static struct espconn tcp_client;
    static esp_tcp tcp;
    tcp_client.type = ESPCONN_TCP;
    tcp_client.proto.tcp = &tcp;
    tcp.remote_port = TCP_SERVER_REMOTE_PORT;
    memcpy(tcp.remote_ip, tcp_server_ip, sizeof(tcp_server_ip));
    espconn_regist_connectcb(&tcp_client, TcpClientConnect);
    espconn_regist_recvcb(&tcp_client, TcpRecvCb);
    espconn_regist_reconcb(&tcp_client, TcpReconnectCb);
    espconn_regist_disconcb(&tcp_client, TcpClientDisConnect);
    espconn_regist_sentcb(&tcp_client, TcpClienSendCb);
    DBG_PRINT("tcp client connect server,server ip:%d.%d.%d.%d port:%d\n", tcp_client.proto.tcp->remote_ip[0],
            tcp_client.proto.tcp->remote_ip[1], tcp_client.proto.tcp->remote_ip[2], tcp_client.proto.tcp->remote_ip[3],
            tcp_client.proto.tcp->remote_port);
    espconn_connect(&tcp_client);
    vTaskDelete(NULL);
}
