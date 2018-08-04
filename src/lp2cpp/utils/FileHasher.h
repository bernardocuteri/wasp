/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FileHasher.h
 * Author: bernardo
 *
 * Created on July 11, 2018, 3:15 PM
 */

#ifndef FILEHASHER_H
#define FILEHASHER_H
#include <string>

class FileHasher {
public:
    FileHasher();
    virtual ~FileHasher();
    std::string computeMD5(const std::string & file_name);
private:

};

#endif /* FILEHASHER_H */

