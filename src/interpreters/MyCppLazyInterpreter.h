/*
 *
 *  Copyright 2018 Bernardo Cuteri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

/* 
 * File:   MyCppInterpreter.h
 * Author: bernardo
 *
 * Created on March 14, 2018, 9:43 AM
 */

#ifndef MYCPPINTERPRETER_H
#define MYCPPINTERPRETER_H

#include "Interpreter.h"
#include "../lp2cpp/LazyConstraintImpl.h"
#include <string>
#include <vector>
using namespace std;


class MyCppLazyInterpreter: public Interpreter {
public:
        MyCppLazyInterpreter(char* filenameToCompile, const string & executablePath);
        ~MyCppLazyInterpreter();

        void callListMethod( const string& method_name, const vector< int >& parameters, vector< int >& output );
        void callListMethod( const string& method_name, const vector< int >& parameters, vector< uint64_t >& output );
        void callVoidMethod( const string& method_name, int param1, const string& param2 );
        void callVoidMethod( const string& method_name, int param1);
        bool checkMethod( const string& method_name ) const;
        
    private:
        char* filenameToCompile;
        LazyConstraintImpl lazyConstraint;
private:

};

#endif /* MYCPPINTERPRETER_H */

