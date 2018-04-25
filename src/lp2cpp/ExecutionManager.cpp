/*
 *
 *  Copyright 2016 Bernardo Cuteri, Alessandro De Rosis, Francesco Ricca.
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
 * File:   ExecutionManager.cpp
 * Author: bernardo
 * 
 * Created on March 1, 2016, 2:43 PM
 */

#include "ExecutionManager.h"
#include "DLV2libs/input/InputDirector.h"
#include "parsing/AspCore2InstanceBuilder.h"
#include "Executor.h"
#include <dlfcn.h>
#include <stdlib.h>
#include "language/Program.h"
#include <cassert>

ExecutionManager::ExecutionManager() {
}

ExecutionManager::~ExecutionManager() {
#ifndef LP2CPP_DEBUG
    destroy(executor);
#else 
    delete executor;
#endif
}

void ExecutionManager::launchExecutorOnFile(const char *filename) {
    executor->executeFromFile(filename);
    //print failed constraints
    if (!executor->getFailedConstraints().empty()) {
        cout << "failed constraints:" << endl;
    }
    for (unsigned i = 0; i < executor->getFailedConstraints().size(); i++) {
        for (unsigned j = 0; j < executor->getFailedConstraints()[i].size(); j++) {
            executor->getFailedConstraints()[i][j].print();
            cout << " ";
        }
        cout << "\n";
    }
}

void ExecutionManager::parseFactsAndExecute(const char *filename) {
    DLV2::InputDirector director;
    AspCore2InstanceBuilder* builder = new AspCore2InstanceBuilder();
    director.configureBuilder(builder);
    vector<const char*> fileNames;
    fileNames.push_back(filename);
    director.parse(fileNames);
    executeProgramOnFacts(builder->getProblemInstance());
    delete builder;
}

#ifndef LP2CPP_DEBUG
void ExecutionManager::compileDynamicLibrary(const string & executablePath) {

    string command = "cd " + executablePath + " && make -f DynamicLibraryMake -s";
    //cout<<command<<endl;
    int commandReturn = system(command.c_str());
    if (commandReturn) {
        throw std::string("Failed to execute command " + command);
    }
    string executorFile = executablePath + "/Executor.so";
    void* handle = dlopen(executorFile.c_str(), RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    Executor * (*create)();


    create = (Executor * (*)())dlsym(handle, "create_object");

    destroy = (void (*)(Executor*))dlsym(handle, "destroy_object");

    executor = (Executor*) create();
}
#else 
void ExecutionManager::compileDynamicLibrary(const string &) {
    executor = new Executor();
}
#endif

void ExecutionManager::executeProgramOnFacts(const std::vector<aspc::Atom*> & program) {
    executor->executeProgramOnFacts(program);

}

const std::vector<std::vector<aspc::Literal> > & ExecutionManager::getFailedConstraints() {
    return executor->getFailedConstraints();
}

const Executor & ExecutionManager::getExecutor() {
    return *executor;
}

void ExecutionManager::shuffleFailedConstraints() {
    executor-> shuffleFailedConstraints();
    
}