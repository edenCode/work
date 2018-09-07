#include <qWorks.h>

/*
// bcd2bin - BCD to Binary.
//
// RETURNS: Binary value.
*/
uint32_t bcd2bin( uint8_t val )
{
	return (val & 0x0f) + (val >> 4) * 10;
}

/*
// bin2bcd - Binary to BCD.
//
// RETURNS: BCD value.
*/
uint8_t bin2bcd( uint32_t val )
{
	return ((val / 10) << 4) + val % 10;
}

/*
// xtod - Hex to DEC.
//
// RETURNS: DEC value.
*/
static char xtod( char c )
{
    if (c>='0' && c<='9')
        return c - '0';
    if (c>='A' && c<='F')
        return c - 'A' + 10;
    if (c>='a' && c<='f')
        return c - 'a' + 10;

    return c = 0; /* not Hex digit */
}

static int hex2dec( char *hex, int l )
{
    if (*hex==0) return (l);

    return hex2dec(hex + 1, l * 16 + xtod(*hex));
}

/*
// xstr2i - Hex string to integer.
//
// RETURNS: Integer value.
*/
int xstr2i( char *hex )
{
    return hex2dec(hex, 0);
}

/* End of file */
