#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void runCmd(const char *cmd, char *result)
{
    FILE *fstream = NULL;
    char buf[128];

    if(NULL == (fstream = popen(cmd, "r"))) {
        return ;
    }
    memset(buf, 0x00, 128);
    int offset = 0;
    while(NULL !=fgets(buf, 128, fstream)) {
        memcpy(result + offset, buf, strlen(buf));
        offset += strlen(buf);
        memset(buf, 0x00, 128);
    }

    pclose(fstream);
}

int main()
{
    char file_buf[512];
    FILE *fp;

    printf("content-type:text/html;charset=gb2312\n\n");  

    system("/bin/action u");

    usleep(5000 * 1000);

    if((fp = fopen("/home/status.txt", "rb")) == NULL) {
        return -1;
    }
    memset(file_buf, 0x00, 512);
    fread(file_buf, 512, 1, fp);
    printf("%s", file_buf);
    fclose(fp);

    return 0; 
}
