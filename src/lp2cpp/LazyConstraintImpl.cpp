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
 * File:   LazyConstraintImpl.cpp
 * Author: bernardo
 * 
 * Created on January 9, 2017, 11:05 AM
 */

#include "LazyConstraintImpl.h"
#include <fstream>
#include "CompilationManager.h"
#include "ExecutionManager.h"
#include "language/Literal.h"
#include "../util/WaspOptions.h"
#include "utils/FileHasher.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>

int tryGetLock( char const *lockName )
{
    mode_t m = umask( 0 );
    int fd = open( lockName, O_RDWR|O_CREAT, 0666 );
    umask( m );
    if( fd >= 0 && flock( fd, LOCK_EX ) < 0 )
    {
        close( fd );
        fd = -1;
    }
    return fd;
}

void releaseLock( int fd, char const *lockName )
{
    if( fd < 0 )
        return;
    remove( lockName );
    close( fd );
}

void LazyConstraintImpl::setFilename(const std::string & fileDirectory, const std::string & filename) {

    string executablePathAndName = wasp::Options::arg0;
    string executablePath = executablePathAndName;
    for (int i = executablePath.size() - 1; i >= 0; i--) {
        if (executablePath[i] == '/') {
            executablePath = executablePath.substr(0, i);
            break;
        }
    }

    string executorPath = executablePath + "/src/lp2cpp/Executor.cpp";
    string executorLock = executorPath + ".lock";
    int fd = tryGetLock(executorLock.c_str());
    if(fd < 0) {
        throw std::runtime_error(executorLock+" lock failed");
    }
    FileHasher hasher;
    string hash = hasher.computeMD5(executorPath);
    std::ofstream outfile(executorPath);
    compilationManager.setOutStream(&outfile);
    filepath = fileDirectory + "/" + filename;
    compilationManager.lp2cpp(filepath);
    outfile.close();
    string newHash = hasher.computeMD5(executorPath);
    executionManager.compileDynamicLibrary(executablePath, newHash != hash);
    releaseLock(fd, executorLock.c_str());
    


}

aspc::Literal* parseLiteral(const std::string & literalString) {
    string predicateName;
    unsigned i = 0;
    for (i = 0; i < literalString.size(); i++) {
        if (literalString[i] == '(') {
            predicateName = literalString.substr(0, i);
            break;
        }
        if (i == literalString.size() - 1) {
            predicateName = literalString.substr(0);
        }
    }
    aspc::Literal* literal = new aspc::Literal(predicateName);
    for (; i < literalString.size(); i++) {
        char c = literalString[i];
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
            int start = i;
            int openBrackets = 0;
            while ((c != ' ' && c != ',' && c != ')') || openBrackets > 0) {
                if (c == '(') {
                    openBrackets++;
                } else if (c == ')') {
                    openBrackets--;
                }
                i++;
                c = literalString[i];
            }
            literal->addTerm(literalString.substr(start, i - start));
        }
    }
    return literal;

}

void LazyConstraintImpl::addedVarName(int var, const std::string & literalString) {
    //cout<<atomString<<endl;
    aspc::Literal * atom = parseLiteral(literalString);
    //atom.print();
    //cout<<endl;
    this->literals[var] = atom;
    literalsMap[*atom] = var;
    if (compilationManager.getBodyPredicates().count(atom->getPredicateName())) {
        watchedAtoms.push_back(var);
    }
    compilationManager.insertModelGeneratorPredicate(atom->getPredicateName());

}

bool LazyConstraintImpl::checkAnswerSet(const std::vector<int> & interpretation) {

    //add only needed atoms
    std::vector<aspc::Literal*> facts;
    for (unsigned atomId : watchedAtoms) {
        aspc::Literal* lit = literals[atomId];
        if(interpretation[atomId] > 0) {
            lit->setNegated(false);
        } else {
            lit->setNegated(true);
        }
        facts.push_back(lit);
    }
    executionManager.executeProgramOnFacts(facts);
    cout<<"bad "<<executionManager.getFailedConstraints().size()<<endl;
    return executionManager.getFailedConstraints().empty();

}

const std::vector<unsigned int> & LazyConstraintImpl::getVariablesToFreeze() {
    return watchedAtoms;
}

void LazyConstraintImpl::onCheckFail(std::vector<int> & constraints) {
    //For every failed constraint, the list of atoms that failed the constraint
    constraints.push_back(0);
    //executionManager.shuffleFailedConstraints();
    for (unsigned i = 0; i < executionManager.getFailedConstraints().size(); i++) {
        //cout << "reasons: \n";
        for (const aspc::Literal & literal : executionManager.getFailedConstraints()[i]) {
            //TODO avoid double access
            const auto& atomIt = literalsMap.find(literal);
            if (!literal.isNegated()) {
                //cerr << literal.getAtom().getTermAt(0) << " ";
            } else {
                //cerr << literal.getAtom().getTermAt(1) << " ";
            }

            if (atomIt != literalsMap.end()) {
                //literal.print();
                if (literal.isNegated()) {
                    constraints.push_back(atomIt->second);
                } else {
                    constraints.push_back(-atomIt->second);
                }
            } else {
//                cerr<<"literal not found\n";
            }
        }
        //cerr << endl;
        constraints.push_back(0);
    }
    if (constraints.size() > 2) {
        //cerr << " bad " << executionManager.getFailedConstraints().size() << endl;
    }
}

const string& LazyConstraintImpl::getFilepath() const {
    return filepath;
}

LazyConstraintImpl::~LazyConstraintImpl() {
    for (auto & a : literals) {
        delete a.second;
    }
}
