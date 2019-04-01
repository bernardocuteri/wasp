/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MyCppEagerInterpreter.h
 * Author: bernardo
 *
 * Created on March 27, 2019, 3:13 PM
 */

#ifndef MYCPPEAGERINTERPRETER_H
#define MYCPPEAGERINTERPRETER_H
#include "Interpreter.h"
#include "../lp2cpp/EagerConstraintImpl.h"
#include <string>
#include <vector>

class MyCppEagerInterpreter : public Interpreter {
public:
    MyCppEagerInterpreter(char* filenameToCompile, const string & executablePath);
    ~MyCppEagerInterpreter();
    void callListMethod(const string& method_name, const vector< int >& parameters, vector< int >& output);
    void callListMethod(const string& method_name, const vector< int >& parameters, vector< uint64_t >& output);
    void callVoidMethod(const string& method_name, int param1, const string& param2);
    void callVoidMethod(const string& method_name, int param1);
    bool checkMethod(const string& method_name) const;
private:
    char* filenameToCompile;
    EagerConstraintImpl eagerConstraint;

};

#endif /* MYCPPEAGERINTERPRETER_H */

