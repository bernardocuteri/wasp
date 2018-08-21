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

class FilesManagement {
public:
    FilesManagement();
    virtual ~FilesManagement();
    std::string computeMD5(const std::string & file_name);
    int tryGetLock(const std::string & path) const; 
    void releaseLock(int fd, const std::string & path) const;
    bool exists (const std::string& name) const;
private:
    void releaseLock(int fd, char const *lockName) const;
    int tryGetLock(char const *lockName) const;

};

#endif /* FILEHASHER_H */

