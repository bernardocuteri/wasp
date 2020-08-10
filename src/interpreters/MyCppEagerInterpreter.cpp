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


const set<string> supportedMethods = {method_plugins_onLitTrue, method_plugins_getReasonForLiteral, method_plugins_addedVarName, 
    method_plugins_onLiteralsUndefined, method_plugins_getVariablesToFreeze, method_plugins_onStartingSolver, method_plugins_onFact, method_plugins_getLiterals, method_plugins_simplifyAtLevelZero};

MyCppEagerInterpreter::~MyCppEagerInterpreter() {
}


MyCppEagerInterpreter::MyCppEagerInterpreter(char* filenameToCompile, const string & executablePath) : filenameToCompile(filenameToCompile) {
    eagerConstraint.setFilename(executablePath, filenameToCompile);
}

void MyCppEagerInterpreter::callListMethod(const string& method_name, const vector<int>& parameters, vector<int>& output) {
    
    
    if (method_name == method_plugins_onLitTrue) {
        eagerConstraint.onLiteralTrue(parameters[0], parameters[1], output);           
    
    } else if (method_name == method_plugins_getReasonForLiteral) {
        eagerConstraint.getReasonForLiteral(parameters[0], output);
        
    } else if (method_name == method_plugins_getVariablesToFreeze) {
        output.insert(output.end(), eagerConstraint.getVariablesToFreeze().begin(), eagerConstraint.getVariablesToFreeze().end());
        
    } else if (method_name == method_plugins_getLiterals) {

        for(unsigned i = 1; i<parameters.size(); i++){
            eagerConstraint.onFact(parameters[i]);
        }
        output.insert(output.end(), eagerConstraint.getVariablesToFreeze().begin(), eagerConstraint.getVariablesToFreeze().end());
        
    } else if (method_name == method_plugins_onLiteralsUndefined) {
        eagerConstraint.onLiteralsUndefined(parameters);
        
    } else if(method_name == method_plugins_onStartingSolver) {
        cout<<"Starting solver using CPP eager propagator on "<<eagerConstraint.getFilepath()<<endl;
        
    } else if(method_name == method_plugins_simplifyAtLevelZero) {
        eagerConstraint.simplifyAtLevelZero(output);
    }

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