#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "../common/common.h"
#include "../hal/rs485.h"

bool is_continue;

char test_buf[39] = "0123456789abcdefghikjlmnopqrstuvwxyz\r\n";

int main(int argc, char **argv)
{
    int rc;
    unsigned char buf[64];

    if(argc != 3) {
        printf("invalid parameters\n");
        printf("usage: ./rs485_utils -s/h  <message>\n");
        return -1;
    }

    int length = strlen(argv[2]);
    if(length > 64) {
        printf("size limited to 64 bytes\n");
        return -1;;
    }

    is_continue = false;
    if(strcmp(argv[1], "-s") == 0) {
        memcpy(buf, argv[2], length);
    } else if(strcmp(argv[1], "-h") == 0) {
        str_to_hex(argv[2], buf, length);
        length /= 2;
    } else if(strcmp(argv[1], "-c") == 0) {
        is_continue = true;
    }

    rc = rs485_open();
    if(rc < 0) {
        printf("failed to open rs485 device\n");
        return -1;
    }
    if(!is_continue) {
        rc = rs485_send((char *)buf, length);
        if(rc < 0) {
            printf("failed to write rs485 device\n");
        } else {
            printf("write %d bytes\n", rc);
            rc = 0;
        }
    } else {
        while(1) {
            rc = rs485_send(test_buf, 39);
            if(rc < 0) {
                printf("failed to write rs485 device\n");
            } else {
                printf("write %d bytes\n", rc);
            }
            usleep(1000*1000);
        }
    }

    rs485_close();
#if 0
    for(int i = 0; i < length / 2; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
#endif

    return rc;
}
