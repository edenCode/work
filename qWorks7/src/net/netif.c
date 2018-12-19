#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <unistd.h>
#include <signal.h>
#include <asm/types.h>
#include <linux/sockios.h>
#include <netif.h>

#define RESOLV_CONF			"/etc/resolv.conf"
#define PROCNET_ROUTE_PATH	"/proc/net/route"

/* 
// Locals 
*/ 
static struct sockaddr_in sa = 
{
	sin_family:	PF_INET,
	sin_port  : 0
};

/*
// Net_SetFlag - Set interface flags
//
// RETURNS: net address
*/
int Net_SetFlag( char *ifname, int flag )
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
		close(skfd);
		return (-1);
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_flags |= flag;
	if (ioctl(skfd, SIOCSIFFLAGS, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	close(skfd);
	
	return (0);
}

/*
// Net_GetFlag - Get interface flags
//
// RETURNS: net address
*/
int Net_GetFlag( char *ifname )
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0))<0 ) {
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	close(skfd);
	
	return ifr.ifr_flags;
}

/*
// Net_ClrFlag - Clear interface flags
//
// RETURNS: net address
*/
int Net_ClrFlag( char *ifname, int flag )
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_flags &= ~flag;
	if (ioctl(skfd, SIOCSIFFLAGS, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	close(skfd);
	
	return (0);
}

/*
// Net_GetAddress - get address of an interface
//
// RETURNS: net address
*/
uint32_t Net_GetAddress( char *ifname )
{
	struct ifreq ifr;
	struct sockaddr_in *saddr;
	int skfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (skfd < 0 ) return -1;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0)
	{
		close(skfd);
		return -1;
	}
	close(skfd);
	saddr = (struct sockaddr_in *) &ifr.ifr_addr;
	
	return saddr->sin_addr.s_addr;
}

/*
// Net_SetAddress - set ip of an interface
//
// RETURNS: net address
*/
int Net_SetAddress( char *ifname, uint32_t addr )
{
	struct ifreq ifr;
	int skfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (skfd < 0 ) return -1;
	sa.sin_addr.s_addr = addr;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	memcpy((char *) &ifr.ifr_addr, (char *)&sa, sizeof(struct sockaddr));
	if (ioctl(skfd, SIOCSIFADDR, &ifr) < 0)
	{
		close(skfd);
		return -1;
	}
	close(skfd);
	
	return 0;
}

/*
// Net_GetNetmask - get netmask address of an interface
//
// RETURNS: address
*/
uint32_t Net_GetNetmask( char *ifname )
{
	struct ifreq ifr;
	struct sockaddr_in *saddr;
	int skfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (skfd < 0 ) return -1;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) 
	{
		close(skfd);
		return -1;
	}
	close(skfd);
	saddr = (struct sockaddr_in *) &ifr.ifr_addr;
	
	return saddr->sin_addr.s_addr;
}

/*
// Net_SetNetmask - set netmask address of an interface
//
// RETURNS: 0 : success ; -1 : fail
*/
int Net_SetNetmask( char *ifname, uint32_t addr )
{
	struct ifreq ifr;
	int skfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (skfd < 0 ) return -1;
	sa.sin_addr.s_addr = addr;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	memcpy((char *) &ifr.ifr_addr, (char *) &sa, sizeof(struct sockaddr));
	if (ioctl(skfd, SIOCSIFNETMASK, &ifr) < 0) 
	{
		close(skfd);
		return -1;
	}
	close(skfd);
	
	return 0;
}

/*
// Net_GetEthAddr - get mac address of an interface
//
// RETURNS: 0 : success ; -1 : fail
*/
int Net_GetEthAddr( char *ifname, uint8_t *mac )
{
	struct ifreq ifr;
	int skfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (skfd < 0 ) return -1;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) 
	{
		close(skfd);
		return -1;
	}
	close(skfd);
	memcpy(mac, ifr.ifr_ifru.ifru_hwaddr.sa_data, IFHWADDRLEN);
	
	return 0;
}

/*
// Net_AddGateway - add a gateway
//
// RETURNS: 0 : success ; -1 : fail
*/
int Net_AddGateway( uint32_t addr )
{
	struct rtentry rt;
	int skfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (skfd < 0 ) return -1;
	
	/* Clean out the RTREQ structure. */
	memset((char *)&rt, 0, sizeof(struct rtentry));
	
	/* Fill in the other fields. */
	rt.rt_flags = (RTF_UP | RTF_GATEWAY);
	rt.rt_dst.sa_family = PF_INET;
	rt.rt_genmask.sa_family = PF_INET;

	sa.sin_addr.s_addr = addr;
	memcpy((char *)&rt.rt_gateway, (char*)&sa, sizeof(struct sockaddr));
	
	/* Tell the kernel to accept this route. */
	if (ioctl(skfd, SIOCADDRT, &rt) < 0) 
	{
		close(skfd);
		return -1;
	}
	
	/* Close the socket. */
	close(skfd);
	
	return (0);
}

/*
// Net_DelGateway - delete a gateway
//
// RETURNS: 0 : success ; -1 : fail
*/
int Net_DelGateway( uint32_t addr )
{
	struct rtentry rt;
	int skfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (skfd < 0 ) return -1;
	
	/* Clean out the RTREQ structure. */
	memset((char *) &rt, 0, sizeof(struct rtentry));
	
	/* Fill in the other fields. */
	rt.rt_flags = (RTF_UP | RTF_GATEWAY);
	rt.rt_dst.sa_family = PF_INET;
	rt.rt_genmask.sa_family = PF_INET;

	sa.sin_addr.s_addr = addr;
	memcpy((char *)&rt.rt_gateway, (char *)&sa, sizeof(struct sockaddr));

	/* Tell the kernel to accept this route. */
	if (ioctl(skfd, SIOCDELRT, &rt) < 0) 
	{
		close(skfd);
		return -1;
	}
	
	/* Close the socket. */
	close(skfd);
	
	return (0);
}

/*
// Net_SearchGateway - search gateway
//
// RETURNS: 0 : success ; -1 : fail
*/
int Net_SearchGateway( char *buf, uint32_t *gate_addr )
{
	char iface[16];
	unsigned long dest, gate;
	int iflags;

	sscanf(buf, "%s\t%08lX\t%08lX\t%8X\t", iface, &dest, &gate, &iflags);
	if ( (iflags & (RTF_UP | RTF_GATEWAY)) == (RTF_UP | RTF_GATEWAY) )
	{
		*gate_addr = gate;
		return 0;
	}
	
	return -1;
}

/*
// Net_SetGateway - set gateway
//
// RETURNS: 0 : success ; -1 : fail
*/
int Net_SetGateway( uint32_t addr )
{
	uint32_t gate_addr;
	char buff[132];
	FILE *fp = fopen(PROCNET_ROUTE_PATH, "r");

	if (!fp) return -1;
	fgets(buff, 130, fp);
	while (fgets(buff, 130, fp) != NULL) 
	{
		if (Net_SearchGateway(buff, &gate_addr) == 0) 
		{
			Net_DelGateway(gate_addr);
		}
	}
	fclose(fp);
	
	return Net_AddGateway(addr);
}

/*
// Clean network gateway address.
//
// RETURNS: 0 : success; -1 : fail
*/
int Net_CleanGateway( void )
{
	uint32_t gate_addr;
	char buff[132];
	FILE *fp = fopen(PROCNET_ROUTE_PATH, "r");

	if (!fp) return -1;
	fgets(buff, 130, fp);
	while (fgets(buff, 130, fp) != NULL) 
	{
		if (Net_SearchGateway(buff, &gate_addr) == 0)
		{
			Net_DelGateway(gate_addr);
		}
	}
	fclose(fp);
	
	return 0;
}

/*
// Get network gateway address.
//
// RETURNS: gateway address.
*/
uint32_t Net_GetGateway( void )
{
	uint32_t gate_addr;
	char buff[132];
	FILE *fp = fopen(PROCNET_ROUTE_PATH, "r");

	if (!fp) return (INADDR_ANY);
	fgets(buff, 130, fp);
	while (fgets(buff, 130, fp) != NULL)
	{
		if (Net_SearchGateway(buff, &gate_addr) == 0) 
		{
			fclose(fp);
			return gate_addr;
		}
	}
	fclose(fp);
	
	return (INADDR_ANY);
}

/*
// Get network DNS address.
//
// RETURNS: gateway address.
*/
uint32_t Net_GetDnsAddr( void )
{
	FILE *fp;
	char dnsname[128];

	fp = fopen(RESOLV_CONF, "r");
	if (fp) 
	{
		if (fscanf(fp, "nameserver %s\n", dnsname) != EOF) 
		{
			fclose(fp);
			return inet_addr(dnsname);
		}
	}
	
	return INADDR_ANY;
}

/*
// Set network gateway address.
//
// RETURNS: 0 : success; -1 : fail
*/
int Net_SetDnsAddr( uint32_t data )
{
	FILE *fp;
	struct in_addr in;

	fp = fopen(RESOLV_CONF, "w");
	if (fp) 
	{
		in.s_addr = data;
		fprintf(fp, "nameserver %s\n", inet_ntoa(in));
		fclose(fp);
		return 0;
	}
	
	return -1;
}

/*
// Net_TcpSend - Send data via TCP socket. 
//  
// RETURNS: Sent bytes; -1 : fail.
*/ 
int Net_TcpSend( int hndl, char* buf, int len )
{
	char *ptr = buf;
	int nwrite, nleft, total;

	nwrite = 0; nleft = len; total  = 0;
	while(nleft > 0) 
	{
		nwrite = send(hndl, ptr, nleft, 0);
		if (nwrite <= 0) break;
		nleft -= nwrite; ptr += nwrite; total += nwrite;
	}

	return (nleft > 0) ? (-1) : total;
}

/*
// Net_TcpRecv - Receive data via TCP socket. 
//  
// RETURNS: Received bytes; -1 : fail.
*/
int Net_TcpRecv( int hndl, char* buf, int len )
{
	char *ptr = buf;
	int nread, nleft, total;

	nread = 0; nleft = len; total = 0;
	while(nleft > 0) 
	{
		nread = recv(hndl, ptr, nleft, 0);
		if (nread <= 0) break; /* shut-down by peer */
		nleft -= nread; ptr += nread; total += nread;
	}

	return (nleft > 0) ? (-1) : total;
}

/* End of file */

