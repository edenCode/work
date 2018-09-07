#ifndef _UARTLIB_H_
#define _UARTLIB_H_

#include "ufx.h"

///*
//// UART baud-rate
//*/
//typedef enum UART_BAUDRATE
//{
//	UART_BAUDRATE_B1200 = 0,
//	UART_BAUDRATE_B1800,
//	UART_BAUDRATE_B2400,
//	UART_BAUDRATE_B4800,
//	UART_BAUDRATE_B9600,
//	UART_BAUDRATE_B19200,
//	UART_BAUDRATE_B38400,
//	UART_BAUDRATE_B57600,
//	UART_BAUDRATE_B115200,
//	UART_BAUDRATE_DEFAULT = UART_BAUDRATE_B115200
//} UART_BAUDRATE;

///*
//// UART data size
//*/
//typedef enum UART_DATASIZE
//{
//	UART_DATASIZE_5BITS = 0,
//	UART_DATASIZE_6BITS,
//	UART_DATASIZE_7BITS,
//	UART_DATASIZE_8BITS,
//	UART_DATASIZE_DEFAULT = UART_DATASIZE_8BITS
//} UART_DATASIZE;

///*
//// UART stop bit(s)
//*/
//typedef enum UART_STOPBITS
//{
//	UART_STOPBIT_1 = 0,
//	UART_STOPBIT_2,
//	UART_STOPBIT_DEFAULT = UART_STOPBIT_1
//} UART_STOPBITS;

///*
//// UART parity
//*/
//typedef enum UART_PARITY
//{
//	UART_PARITY_NONE = 0,
//	UART_PARITY_ODD,
//	UART_PARITY_EVEN,
//	UART_PARITY_DEFAULT = UART_PARITY_NONE
//} UART_PARITY;

/*
// UART attributes
*/

/* UART baud-rate */
typedef enum UART_BAUDRATE
{
	UART_BAUDRATE_B1200 = 0,
	UART_BAUDRATE_B1800,
	UART_BAUDRATE_B2400,
	UART_BAUDRATE_B4800,
	UART_BAUDRATE_B9600,
	UART_BAUDRATE_B19200,
	UART_BAUDRATE_B38400,
	UART_BAUDRATE_B57600,
	UART_BAUDRATE_B115200,
	UART_BAUDRATE_DEFAULT = UART_BAUDRATE_B115200
} UART_BAUDRATE;

/* UART data size */
typedef enum UART_DATASIZE
{
	UART_DATASIZE_5BITS = 0,
	UART_DATASIZE_6BITS,
	UART_DATASIZE_7BITS,
	UART_DATASIZE_8BITS,
	UART_DATASIZE_DEFAULT = UART_DATASIZE_8BITS
} UART_DATASIZE;

/* UART stop bit(s) */
typedef enum UART_STOPBITS
{
	UART_STOPBIT_1 = 0,
	UART_STOPBIT_2,
	UART_STOPBIT_DEFAULT = UART_STOPBIT_1
} UART_STOPBITS;

/* UART parity */
typedef enum UART_PARITY
{
	UART_PARITY_NONE = 0,
	UART_PARITY_ODD,
	UART_PARITY_EVEN,
	UART_PARITY_DEFAULT = UART_PARITY_NONE
}UART_PARITY;


typedef struct UART_ATTRS
{
	UART_BAUDRATE rate;  /* Baud-rate, ref. UART_BAUDRATE */
	UART_DATASIZE size;  /* Data size, ref. UART_DATASIZE */
	UART_STOPBITS stop;  /* Stop bits, ref. UART_STOPBIT */
	UART_PARITY parity;  /* Parity, ref. UART_PARITY */
} UART_ATTRS;

/* function declarations */

#ifdef __cplusplus
extern "C"
{
#endif

extern int Uart_Open ( int idx, UART_ATTRS *data );
extern int Uart_Close( int hndl );

extern int Uart_SendData( int hndl, char* buf, int len );
extern int Uart_RecvData( int hndl, char* buf, int len );

extern int Uart_SetParity  ( int hndl, UART_PARITY parity );
extern int Uart_SetBaudRate( int hndl, UART_BAUDRATE rate );
extern int Uart_SetDataSize( int hndl, UART_DATASIZE size );
extern int Uart_SetStopBits( int hndl, UART_STOPBITS bits );

#ifdef __cplusplus
}
#endif

#endif 

/* End of file */
