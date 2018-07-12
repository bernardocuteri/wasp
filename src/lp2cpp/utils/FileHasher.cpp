/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FileHasher.cpp
 * Author: bernardo
 * 
 * Created on July 11, 2018, 3:15 PM
 */

#include "FileHasher.h"
#include <iostream>

#define STR_VALUE(val) #val
#define STR(name) STR_VALUE(name)

#define PATH_LEN 256
#define MD5_LEN 32

FileHasher::FileHasher() {
}

FileHasher::FileHasher(const FileHasher& orig) {
}

FileHasher::~FileHasher() {
}

std::string FileHasher::computeMD5(const std::string & file_name) {
#define MD5SUM_CMD_FMT "md5sum %." STR(PATH_LEN) "s 2>/dev/null"
    char cmd[PATH_LEN + sizeof (MD5SUM_CMD_FMT)];
    sprintf(cmd, MD5SUM_CMD_FMT, file_name.c_str());
#undef MD5SUM_CMD_FMT

    std::string md5_sum;
    FILE *p = popen(cmd, "r");
    if (p == 0) return 0;

    int i, ch;
    for (i = 0; i < MD5_LEN && isxdigit(ch = fgetc(p)); i++) {
        md5_sum += ch;
    }

    pclose(p);
    //std::cout << "md5 of " << file_name << " is " << md5_sum << std::endl;
    return md5_sum;
}
