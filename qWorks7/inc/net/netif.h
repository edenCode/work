#ifndef _NETIF_H_
#define _NETIF_H_

#include <qWorks.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Hw and Ip string length */
#define MAX_HWADDR_LEN 6
#define MAX_IPADDR_LEN 15

/* Ethernet interface */
#define ETH0_NAME "eth0"
#define ETH1_NAME "eth1"

#define IPV4(a,b,c,d)   ((d<<24)|(c<<16)|(b<<8)|(a))
#define IPV4_ANY        IPV4(0,0,0,0)
#define IPV4_BROADCAST  IPV4(255,255,255,255)
#define IPV4_INVALID    ((uint32_t)(-1))

/* function declarations */

int Net_TcpSend( int hndl, char* buf, int len );
int Net_TcpRecv( int hndl, char* buf, int len );

int Net_SetFlag( char* ifname, int data );
int Net_GetFlag( char* ifname );
int Net_ClrFlag( char* ifname, int data );

int Net_SetAddress( char* ifname, uint32_t data );
int Net_SetNetmask( char* ifname, uint32_t data );
int Net_SetGateway( uint32_t data );
int Net_SetDnsAddr( uint32_t data );

uint32_t Net_GetAddress( char* ifname );
uint32_t Net_GetNetmask( char* ifname );
uint32_t Net_GetGateway( void );
uint32_t Net_GetDnsAddr( void );
int Net_GetEthAddr( char* ifname, uint8_t* data );

#ifdef __cplusplus
}
#endif

#endif 

/* End of file */
