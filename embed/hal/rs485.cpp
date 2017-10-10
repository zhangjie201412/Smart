#include "rs485.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <linux/serial.h>
#include <sys/ioctl.h>

#include "common.h"

#define TIOCGRS485              0x542E
#define TIOCSRS485              0x542F

/* Test GCC version, this structure is consistent in GCC 4.8, thus no need to overwrite */
#if (__GNUC__ == 4 && __GNUC_MINOR__ == 3)

struct my_serial_rs485
{
    unsigned long   flags;                      /* RS485 feature flags */
#define SER_RS485_ENABLED           (1 << 0)        /* If enabled */
#define SER_RS485_RTS_ON_SEND       (1 << 1)    /* Logical level for RTS pin when sending */
#define SER_RS485_RTS_AFTER_SEND    (1 << 2)    /* Logical level for RTS pin after sent*/
#define SER_RS485_RX_DURING_TX      (1 << 4)
    unsigned long   delay_rts_before_send;  /* Delay before send (milliseconds) */
    unsigned long   delay_rts_after_send;   /* Delay after send (milliseconds) */
    unsigned long   padding[5];     /* Memory is cheap, new structs are a royal PITA .. */
};

#endif

static struct termios newtios, oldtios;
static int rs485_fd;

static void reset_tty_atexit(void)
{
    if(rs485_fd != -1)
    {
        tcsetattr(rs485_fd, TCSANOW, &oldtios);
    } 
}

/*cheanup signal handler */
static void reset_tty_handler(int signal)
{
    if(rs485_fd != -1)
    {
        tcsetattr(rs485_fd, TCSANOW, &oldtios);
    }
    _exit(EXIT_FAILURE);
}

int rs485_open()
{
    struct sigaction sa;
    int fd;
    int rc = 0;

#if (__GNUC__==4 && __GNUC_MINOR__==3)
    struct my_serial_rs485 rs485conf;
    struct my_serial_rs485 rs485conf_bak;
#else
    struct serial_rs485 rs485conf;
    struct serial_rs485 rs485conf_bak;
#endif

    fd = open("/dev/ttyS2", O_RDWR | O_NOCTTY, 0);
    if(fd < 0) {
        LOGE("%s: failed to open %s\n", __func__, "/dev/ttyS2");
        rc = -1;
        goto ERR_OPEN;
    }

    /*get serial port parnms,save away */
    tcgetattr(fd,&newtios);
    memcpy(&oldtios,&newtios,sizeof newtios);
    /* configure new values */
    cfmakeraw(&newtios); /*see man page */
    newtios.c_iflag |=IGNPAR; /*ignore parity on input */
    newtios.c_oflag &= ~(OPOST | ONLCR | OLCUC | OCRNL | ONOCR | ONLRET | OFILL); 
    newtios.c_cflag = CS8 | CLOCAL | CREAD;
    newtios.c_cc[VMIN]=1; /* block until 1 char received */
    newtios.c_cc[VTIME]=0; /*no inter-character timer */
    /* 115200 bps */
    cfsetospeed(&newtios,B115200);
    cfsetispeed(&newtios,B115200);
    /* register cleanup stuff */
    atexit(reset_tty_atexit);
    memset(&sa,0,sizeof sa);
    sa.sa_handler = reset_tty_handler;
    sigaction(SIGHUP,&sa,NULL);
    sigaction(SIGINT,&sa,NULL);
    sigaction(SIGPIPE,&sa,NULL);
    sigaction(SIGTERM,&sa,NULL);
    /*apply modified termios */
    tcflush(fd,TCIFLUSH);
    tcsetattr(fd,TCSADRAIN,&newtios);


    if (ioctl (fd, TIOCGRS485, &rs485conf) < 0) 
    {
        /* Error handling.*/ 
        LOGE("ioctl TIOCGRS485 error.\n");
    }
    /* Enable RS485 mode: */
    rs485conf.flags |= SER_RS485_ENABLED;

    /* Set logical level for RTS pin equal to 1 when sending: */
    rs485conf.flags |= SER_RS485_RTS_ON_SEND;
    //rs485conf.flags |= SER_RS485_RTS_AFTER_SEND;

    /* set logical level for RTS pin equal to 0 after sending: */ 
    rs485conf.flags &= ~(SER_RS485_RTS_AFTER_SEND);
    //rs485conf.flags &= ~(SER_RS485_RTS_ON_SEND);

    /* Set rts delay after send, if needed: */
    rs485conf.delay_rts_after_send = 0x80;

    if(ioctl(fd, TIOCSRS485, &rs485conf) < 0)
    {
        /* Error handling.*/ 
        LOGE("ioctl TIOCSRS485 error.\n");
    }

    if(ioctl(fd, TIOCGRS485, &rs485conf_bak) < 0)
    {
        /* Error handling.*/ 
        LOGE("ioctl TIOCGRS485 error.\n");
    }
    else
    {
        //LOGD("rs485conf_bak.flags 0x%x.\n", rs485conf_bak.flags);
        //LOGD("rs485conf_bak.delay_rts_before_send 0x%x.\n", rs485conf_bak.delay_rts_before_send);
        //LOGD("rs485conf_bak.delay_rts_after_send 0x%x.\n", rs485conf_bak.delay_rts_after_send);
    }

    rs485_fd = fd;

ERR_OPEN:
    return rc;
}

void rs485_close()
{
    if(rs485_fd > 0) {
        close(rs485_fd);
        rs485_fd = -1;
    }
}

int rs485_send(char *buf, int size)
{
    if(rs485_fd > 0) {
        return write(rs485_fd, buf, size);
    } else {
        LOGE("No device\n");
        return -1;
    }
}

int rs485_read(char *buf, int size)
{
    if(rs485_fd > 0) {
        return read(rs485_fd, buf, size);
    } else {
        LOGE("No device\n");
        return -1;
    }
}
