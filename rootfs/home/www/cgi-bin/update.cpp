#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include "json/json.h"

#define NAME        "root"
#define PASW        "root123"

using namespace std;

void formatX(char *str, int len)
{
    for(int i = 0; i < len; i++) {
        if(!((str[i] >= '0'
                    && str[i] <= '9') || (str[i] == '.'))) {
            str[i] = '\0';
        }
    }
}

int main()
{
    char *data;   
    char *method;
    char name[50],pwd[20];   
    char file_buf[512];
    char ip_addr[32];
    char mac_addr[32];
    char pref_mode[32];
    char update_cycle[32];
    char ftp_addr[32];
    char ftp_port[32];

    char lines[10][32];

    FILE *fp;
    printf("content-type:text/html;charset=gb2312\n\n");  

    method = getenv("REQUEST_METHOD");
    data=getenv("QUERY_STRING");  
    if(data==NULL) {
        printf("<p>ERROR!!</p>");  
    } else {
        //printf("%s\n", data);
        if(strlen(data) < 10) {
            //reset
            system("/bin/action r");
            return 0;
        }
#if 1
        char *token = strtok(data, "&");
        int i = 0;
        while(token != NULL) {
            printf("%s\n", token);
            memcpy(lines[i++], token, strlen(token));
            token = strtok(NULL, "&");
        }
        memset(ip_addr, 0x00, 32);
        memset(mac_addr, 0x00, 32);
        memset(pref_mode, 0x00, 32);
        memset(update_cycle, 0x00, 32);
        memset(ftp_addr, 0x00, 32);
        memset(ftp_port, 0x00, 32);

        memcpy(ip_addr, lines[0], strlen(lines[0]));
        memcpy(mac_addr, lines[1], strlen(lines[1]));
        memcpy(pref_mode, lines[2], strlen(lines[2]));
        memcpy(update_cycle, lines[3], strlen(lines[3]));
        memcpy(ftp_addr, lines[4], strlen(lines[4]));
        memcpy(ftp_port, lines[5], strlen(lines[5]));

        ifstream ifs;
        ifs.open("/etc/boa/device.json");
        Json::Reader reader;
        Json::Value root;
        if(!reader.parse(ifs, root, false)) {
            ifs.close();
            return -1;
        }
        ifs.close();

        formatX(ip_addr, strlen(ip_addr));
        formatX(mac_addr, strlen(mac_addr));
        formatX(pref_mode, strlen(pref_mode));
        formatX(update_cycle, strlen(update_cycle));
        formatX(ftp_addr, strlen(ftp_addr));
        formatX(ftp_port, strlen(ftp_port));

        printf("%s\n%s\n%s\n%s\n%s\n%s\n",
                ip_addr,
                mac_addr,
                pref_mode,
                update_cycle,
                ftp_addr,
                ftp_port);

        root["ip_addr"] = ip_addr;
        root["mac_addr"] = mac_addr;
        root["pref_mode"] = atoi(pref_mode);
        root["update_cycle"] = atoi(update_cycle);
        root["ftp_addr"] = ftp_addr;
        root["ftp_port"] = atoi(ftp_port);
        Json::FastWriter writer;
        string file = writer.write(root);

        ofstream ofs;
        ofs.open("/etc/boa/device.json");
        ofs << file;
#endif
    }   
    return 0; 
}
