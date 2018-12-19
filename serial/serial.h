#ifndef SERIAL_H
#define SERIAL_H

#ifdef __cplusplus
extern "C"
{
#endif

void SetSpeed(int fd, int speed);
int SetParity(int fd, int databits,int stopbits,int parity);
int OpenDev(const char *Dev);

#ifdef __cplusplus
}
#endif

#endif // SERIAL_H

