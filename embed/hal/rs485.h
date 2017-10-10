#ifndef __RS485_H__
#define __RS485_H__

int rs485_open();
void rs485_close();
int rs485_send(char *buf, int size);
int rs485_read(char *buf, int size);

#endif
