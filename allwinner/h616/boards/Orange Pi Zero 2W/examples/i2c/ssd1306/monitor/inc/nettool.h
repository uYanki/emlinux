#ifndef __NET_TOOLS_H__
#define __NET_TOOLS_H__

#define ENABLE_LOG

typedef enum {
    STATE_CONNECT = 0,
    STATE_DISCONNECT,
    STATE_FAIL,
} net_state_e;

net_state_e GetWirelessState();
net_state_e GetEthernetState();
net_state_e GetNetState();
int         GetLocalIP(char* ifname, char* ip);
int         GetCurNetFlow(char* netname, long int* rx_rate, long int* tx_rate);

#endif
