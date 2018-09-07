#include <cksum.h>

/*
// Polynomial
*/
#define POLY    (0x1070U << 3)

/*
// do_crc8 - Do 8bits CRC.
//
// RETURNS: Check sum.
*/
static uint8_t do_crc8( uint16_t data )
{
	int i;

	for (i=0; i<8; i++) {
		if (data & 0x8000)
			data = data ^ POLY;
		data = data << 1;
	}
	return (uint8_t)(data >> 8);
}

/*
// crc8 - Do 8bits CRC.
//
// RETURNS: Check sum.
*/
uint8_t crc8( uint8_t crc, uint8_t *p, int count )
{
	int i;

	for (i=0; i<count; i++)
		crc = do_crc8((crc ^ p[i]) << 8);
	return crc;
}

/*
// sum8 - Addition check sum.
//
// RETURNS: Check sum.
*/
uint8_t sum8( uint8_t* buf, int len )
{
	uint8_t sum = 0;
	int idx;

	for (idx=0; idx<len; idx++) {
		sum += buf[idx];
	}
	return (sum & 0xFF);
}

/*
// sum16 - Addition check sum, RFC 1071.
//
// RETURNS: Check sum.
*/
uint16_t sum16( uint16_t* pAddr, int len )
{
    int     nLeft = len;
    int     sum   = 0;
    uint16_t *w     = pAddr;
    uint16_t  answer;

    while (nLeft>1) {
        sum   += *w++;
        nLeft -= 2;
    }
    if (nLeft==1) sum += *(uint8_t *) w;
    sum    = (sum >> 16) + (sum & 0xffff);
    sum    += (sum >> 16);
    answer = sum;

    return (~answer & 0xffff);
}

/* End of file */
