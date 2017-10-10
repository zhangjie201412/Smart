#include <vector>
#include <string>
#include <stdio.h>

void split_str(std::string& s, std::vector<std::string>& v, const std::string& c)
{
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(std::string::npos != pos2) {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }

    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}

int str_to_hex(char *string, unsigned char *cbuf, int len)
{
    char high, low;
    int idx = 0, i = 0;

    for(i = 0; i < len; i += 2) {
        high = string[i];
        low = string[i + 1];

        //printf("high = %02x, low = %02x\n", high, low);

        if(high >= '0' && high <= '9')
            high = high - '0';
        else if(high >= 'A' && high <= 'F')
            high = high - 'A' + 10;
        else if(high >= 'a' && high <= 'f')
            high = high - 'a' + 10;
        else
            return -1;

        if(low >= '0' && low <= '9')
            low = low - '0';
        else if(low >= 'A' && low <= 'F')
            low = low - 'A' + 10;
        else if(low >= 'a' && low <= 'f')
            low = low - 'a' + 10;
        else
            return -1;

        cbuf[idx ++] = high << 4 | low;
    }

    return 0;
}
