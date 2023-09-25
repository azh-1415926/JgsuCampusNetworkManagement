#ifndef CALCMD5_H_
#define CALCMD5_H_

#include "md5/md5.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

#define READ_DATA_SIZE	1024
#define MD5_SIZE		16
#define MD5_STR_LEN		(MD5_SIZE * 2)

class calcMD5
{
    private:
        ;

    public:
        calcMD5() {  };
        ~calcMD5() {  };
        static inline std::string toMD5(const std::string& str)
        {
            unsigned char _str[str.length()];
            memcpy(_str,str.c_str(),str.length());
            char md5_str[MD5_STR_LEN + 1];
            parseStringToMD5(_str,str.length(),md5_str);
            return std::string(md5_str);
        }
    
    private:
        static void parseStringToMD5(unsigned char *dest_str, unsigned int dest_len, char *md5_str)
        {
            int i;
            unsigned char md5_value[MD5_SIZE];
            MD5_CTX md5;
            MD5Init(&md5);
            MD5Update(&md5, dest_str, dest_len);
            MD5Final(&md5, md5_value);
            for(i = 0; i < MD5_SIZE; i++)
                snprintf(md5_str + i*2, 2+1, "%02x", md5_value[i]);
        }
};

#endif