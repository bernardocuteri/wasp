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
 * File:   MyCppInterpreter.cpp
 * Author: bernardo
 * 
 * Created on March 14, 2018, 9:43 AM
 */

#include "MyCppLazyInterpreter.h"
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "../lp2cpp/CompilationManager.h"
#include "../lp2cpp/ExecutionManager.h"
#include "../lp2cpp/LazyConstraint.h"
#include "../lp2cpp/LazyConstraintImpl.h"
#include "../lp2cpp/utils/GraphWithTarjanAlgorithm.h"
#include "cassert"
#include "../propagators/ExternalPropagator.h"


const set<string> supportedMethods = {method_plugins_checkAnswerSet, method_plugins_getReasonsForCheckFailure,
    method_plugins_addedVarName, method_plugins_getVariablesToFreeze, method_plugins_onStartingSolver, method_plugins_onFact};

MyCppLazyInterpreter::MyCppLazyInterpreter(char* filenameToCompile, const string & executablePath) : filenameToCompile(filenameToCompile) {
    lazyConstraint.setFilename(executablePath, filenameToCompile);


}

void MyCppLazyInterpreter::callListMethod(const string& method_name, const vector<int>& parameters, vector<int>& output) {
    if (method_name == method_plugins_checkAnswerSet) {
        if (!lazyConstraint.checkAnswerSet(parameters)) {
            output.push_back(0);
        }
    } else if (method_name == method_plugins_getReasonsForCheckFailure) {
        lazyConstraint.onCheckFail(output);
    } else if (method_name == method_plugins_getVariablesToFreeze) {
        output.insert(output.end(), lazyConstraint.getVariablesToFreeze().begin(), lazyConstraint.getVariablesToFreeze().end());
    } else if(method_name == method_plugins_onStartingSolver) {
        cout<<"Starting solver using CPP lazy propagator on "<<lazyConstraint.getFilepath()<<endl;
    }

}

void MyCppLazyInterpreter::callListMethod(const string&, const vector<int>&, vector< uint64_t >&) {
    assert(false);
}

void MyCppLazyInterpreter::callVoidMethod(const string& method_name, int param1) {
    if (method_name == method_plugins_onFact) {
        lazyConstraint.onFact(param1);
    } 
}


void MyCppLazyInterpreter::callVoidMethod(const string& method_name, int param1, const string& param2) {
    if (method_name == method_plugins_addedVarName) {
        lazyConstraint.addedVarName(param1, param2);
    } 

}

bool MyCppLazyInterpreter::checkMethod(const string& method_name) const {
    return supportedMethods.count(method_name) ? true : false;

}

MyCppLazyInterpreter::~MyCppLazyInterpreter() {

}
