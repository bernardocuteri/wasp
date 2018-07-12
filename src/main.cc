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
#include "lp2cpp/datastructures/Tuple.h"
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

int main(int argc, char** argv) {

    //    srand(unsigned(time(NULL)));
    
    // put true to test lp2cpp 
    // usage ./exec encoding instance
    if (false) {
        string executablePathAndName = argv[0];
        string executablePath = executablePathAndName;
        for (int i = executablePath.size() - 1; i >= 0; i--) {
            if (executablePath[i] == '/') {
                executablePath = executablePath.substr(0, i);
                break;
            }
        }
        std::ofstream outfile(executablePath + "/src/lp2cpp/Executor.cpp");
        CompilationManager manager;
        manager.setOutStream(&outfile);
        manager.lp2cpp(argv[1]); //"/encodings/constants");
        outfile.close();
        ExecutionManager execManager;
        execManager.compileDynamicLibrary(executablePath, true);
        //execManager.launchExecutorOnFile((executablePath+"/instances/constants").c_str());
        execManager.launchExecutorOnFile(argv[2]);
        return 0;
    }

    
    
    wasp::Options::parse(argc, argv);
    waspFacadePointer = new WaspFacade();
    WaspFacade& waspFacade = *waspFacadePointer;
    wasp::Options::setOptions(waspFacade);

    signal(SIGINT, my_handler);
    signal(SIGTERM, my_handler);
    signal(SIGXCPU, my_handler);

    //test reading from hardcoded file
    bool readFromFile = false;
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
    return EXIT_CODE;
}
