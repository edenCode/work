#include "IOHandlers.hh"
#include "TunnelEncaps.hh"

//##### TEMP: Use a single buffer, sized for UDP tunnels:
//##### This assumes that the I/O handlers are non-reentrant
static unsigned const maxPacketLength = 50*1024; // bytes
// This is usually overkill, because UDP packets are usually no larger
// than the typical Ethernet MTU (1500 bytes).  However, I've seen
// reports of Windows Media Servers sending UDP packets as large as
// 27 kBytes.  These will probably undego lots of IP-level
// fragmentation, but that occurs below us.  We just have to hope that
// fragments don't get lost.
static unsigned const ioBufferSize = maxPacketLength + TunnelEncapsulationTrailerMaxSize;
static unsigned char ioBuffer[ioBufferSize];


void socketReadHandler(Socket* sock, int /*mask*/) 
{
	unsigned bytesRead;
	struct sockaddr_in fromAddress;
	UsageEnvironment& saveEnv = sock->env();
	// because handleRead(), if it fails, may delete "sock"
	if (!sock->handleRead(ioBuffer, ioBufferSize, bytesRead, fromAddress)) {
		saveEnv.reportBackgroundError();
	}
}
