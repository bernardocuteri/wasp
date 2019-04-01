/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MyCppEagerInterpreter.cpp
 * Author: bernardo
 * 
 * Created on March 27, 2019, 3:13 PM
 */

#include "MyCppEagerInterpreter.h"
#include "Interpreter.h"
#include "../lp2cpp/EagerConstraintImpl.h"
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "../lp2cpp/CompilationManager.h"
#include "../lp2cpp/ExecutionManager.h"
#include "../lp2cpp/utils/GraphWithTarjanAlgorithm.h"
#include "cassert"
#include "../propagators/ExternalPropagator.h"


const set<string> supportedMethods = {method_plugins_onLitTrue, method_plugins_getReason,
    method_plugins_onLiteralsUndefined, method_plugins_getVariablesToFreeze, method_plugins_onStartingSolver, method_plugins_onFact};

MyCppEagerInterpreter::~MyCppEagerInterpreter() {
}


MyCppEagerInterpreter::MyCppEagerInterpreter(char* filenameToCompile, const string & executablePath) : filenameToCompile(filenameToCompile) {
    eagerConstraint.setFilename(executablePath, filenameToCompile);
}

void MyCppEagerInterpreter::callListMethod(const string& method_name, const vector<int>& parameters, vector<int>& output) {
//    if (method_name == method_plugins_checkAnswerSet) {
//        if (!eagerConstraint.checkAnswerSet(parameters)) {
//            output.push_back(0);
//        }
//    } else if (method_name == method_plugins_getReasonsForCheckFailure) {
//        eagerConstraint.onCheckFail(output);
//    } else if (method_name == method_plugins_getVariablesToFreeze) {
//        output.insert(output.end(), lazyConstraint.getVariablesToFreeze().begin(), lazyConstraint.getVariablesToFreeze().end());
//    } else if(method_name == method_plugins_onStartingSolver) {
//        cout<<"Starting solver using CPP lazy propagator on "<<lazyConstraint.getFilepath()<<endl;
//    }

}

void MyCppEagerInterpreter::callListMethod(const string&, const vector<int>&, vector< uint64_t >&) {
    assert(false);
}

void MyCppEagerInterpreter::callVoidMethod(const string& method_name, int param1) {
    if (method_name == method_plugins_onFact) {
        eagerConstraint.onFact(param1);
    } 
}


void MyCppEagerInterpreter::callVoidMethod(const string& method_name, int param1, const string& param2) {
    if (method_name == method_plugins_addedVarName) {
        eagerConstraint.addedVarName(param1, param2);
    } 
}

bool MyCppEagerInterpreter::checkMethod(const string& method_name) const {
    return supportedMethods.count(method_name) ? true : false;

}