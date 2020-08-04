/*
 *
 *  Copyright 2013 Mario Alviano, Carmine Dodaro, and Francesco Ricca.
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

#include <cstdlib>
#include <csignal>
#include <iostream>
#include "WaspFacade.h"
#include "util/WaspOptions.h"
#include "PredicateMinimization.h"
#include "CautiousReasoning.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include "lp2cpp/CompilationManager.h"
#include "lp2cpp/ExecutionManager.h"
#include "lp2cpp/LazyConstraint.h"
#include "lp2cpp/LazyConstraintImpl.h"
#include "lp2cpp/utils/GraphWithTarjanAlgorithm.h"
#include "lp2cpp/datastructures/TupleWithoutReasons.h"
#include "lp2cpp/language/Literal.h"
#include "lp2cpp/utils/FilesManagement.h"
#include "lp2cpp/datastructures/PredicateSet.h"
using namespace std;

int EXIT_CODE = 0;

WaspFacade* waspFacadePointer = NULL;

void my_handler(int) {
    cerr << "Killed: Bye!" << endl;
    if (EXIT_CODE == 0)
        EXIT_CODE = 1;
    waspFacadePointer->onKill();
    delete waspFacadePointer;
    Statistics::clean();
    exit(EXIT_CODE);
}

void testDS() {
    PredicateSet<TupleWithoutReasons, TuplesHash> testW(2);
    string test = "test";
    testW.insert(TupleWithoutReasons({1,2}, &test));
    cout<<testW.getTuples().size()<<endl;
    testW.erase(TupleWithoutReasons({1,2}, &test));
    cout<<testW.getTuples().size()<<endl;
}

int main(int argc, char** argv) {

    // testDS();
    //    srand(unsigned(time(NULL)));
    wasp::Options::parse(argc, argv);
    waspFacadePointer = new WaspFacade();
    WaspFacade& waspFacade = *waspFacadePointer;
    wasp::Options::setOptions(waspFacade);

    signal(SIGINT, my_handler);
    signal(SIGTERM, my_handler);
    signal(SIGXCPU, my_handler);
   
    if (wasp::Options::compile_eager!="") {
        
        std::cout<<"Start compile"<<std::endl;
        string executablePathAndName = argv[0];
        string executablePath = executablePathAndName;
        for (int i = executablePath.size() - 1; i >= 0; i--) {
            if (executablePath[i] == '/') {
                executablePath = executablePath.substr(0, i);
                break;
            }
        }
        std::string executorPath = executablePath + "/src/lp2cpp/Executor.cpp";
        std::ofstream outfile(executorPath);
        CompilationManager manager(EAGER_MODE);
        manager.setOutStream(&outfile);
        manager.loadProgram(executablePath+"/"+wasp::Options::compile_eager);
        manager.lp2cpp();
        outfile.close();
        
        
        return 0;
    }
        

    if (wasp::Options::lp2cppDatalog) {
        // execute with option lp2cpp-datalog
        // usage ./exec --lp2cpp-datalog encoding instance 
        string executablePathAndName = argv[0];
        string executablePath = executablePathAndName;
        for (int i = executablePath.size() - 1; i >= 0; i--) {
            if (executablePath[i] == '/') {
                executablePath = executablePath.substr(0, i);
                break;
            }
        }
#ifdef LP2CPP_DEBUG
        ExecutionManager em;
        em.compileDynamicLibrary(executablePath, false);
        em.launchExecutorOnFile(argv[3]);
        return 0;
#endif
        FilesManagement fileManagement;
        std::string executorPath = executablePath + "/src/lp2cpp/Executor.cpp";
        int fd = fileManagement.tryGetLock(executorPath);
        string hash = fileManagement.computeMD5(executorPath);
        std::ofstream outfile(executorPath);
        CompilationManager manager(LAZY_MODE);
        manager.setOutStream(&outfile);
        if (!fileManagement.exists(argv[2])) {
            string filename = argv[2];
            throw std::runtime_error("Failed to compile logic program: file " + filename + " does not exist.");
        }
        manager.loadProgram(argv[2]);
        manager.lp2cpp(); //"/encodings/constants");
        outfile.close();
        string newHash = fileManagement.computeMD5(executorPath);
        ExecutionManager execManager;
        execManager.compileDynamicLibrary(executablePath, newHash != hash);
        fileManagement.releaseLock(fd, executorPath);
        //execManager.launchExecutorOnFile((executablePath+"/instances/constants").c_str());
        execManager.launchExecutorOnFile(argv[3]);
        return 0;
    }

    //test reading from hardcoded file
    bool readFromFile = false;
#ifdef EAGER_DEBUG 
    readFromFile = true;
#endif

    std::filebuf fb;
    if (readFromFile) {
        if (fb.open("test.in", std::ios::in)) {
            std::istream in(&fb);
            waspFacade.readInput(in);
        }
    } else {
        waspFacade.readInput(cin);
    }

    if (wasp::Options::predMinimizationAlgorithm != NO_PREDMINIMIZATION) {

        PredicateMinimization p(waspFacade);
        p.solve();
        
    } else if (wasp::Options::queryAlgorithm == ONE_QUERIES
            || wasp::Options::queryAlgorithm == KDYN_QUERIES
            || wasp::Options::queryAlgorithm == PREFERENCE_QUERIES
            || wasp::Options::queryAlgorithm == PMRES_QUERIES
            || wasp::Options::queryAlgorithm == ITERATIVE_COHERENCE_TESTING_PREFERENCES
            || wasp::Options::queryAlgorithm == PRIME_IMPLICATE) {
        
        CautiousReasoning c(waspFacade);
        c.solve();
    } else waspFacade.solve();
    waspFacade.onFinish();
    delete waspFacadePointer;
    Statistics::clean();

    if (readFromFile) {
        fb.close();
    }
    std::cout<<"EXIT_CODE "<<EXIT_CODE<<std::endl;
    return EXIT_CODE;
}
