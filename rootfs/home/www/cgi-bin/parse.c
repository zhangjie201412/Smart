#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME        "root"
#define PASW        "root123"

int main()
{
    char *data;   
    char *method;
    char name[50],pwd[20];   
    char file_buf[512];
    FILE *fp;
    printf("content-type:text/html;charset=gb2312\n\n");  

    method = getenv("REQUEST_METHOD");
    data=getenv("QUERY_STRING");  
    if(data==NULL) {
        printf("<p>ERROR!!</p>");  
    } else {
        //printf("<p>date:%s</p>", date);
        sscanf(data,"name=%[^&]&pasw=%s",name,pwd);
        if((strcmp(name, NAME) == 0) && (strcmp(pwd, PASW) == 0)) {
            //printf("<p>Successfully!</p>");
            //read file to data
#if 1
            if((fp = fopen("/etc/boa/device.json", "rb")) == NULL) {
                return -1;
            }
            memset(file_buf, 0x00, 512);
            fread(file_buf, 512, 1, fp);
            printf("%s", file_buf);
            fclose(fp);
#endif
        } else {
            printf("{}");
        }
    }   
    return 0; 
}
