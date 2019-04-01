/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   EagerConstrainImpl.cpp
 * Author: bernardo
 * 
 * Created on March 27, 2019, 3:20 PM
 */

#include "EagerConstraintImpl.h"
#include <fstream>
#include "CompilationManager.h"
#include "ExecutionManager.h"
#include "language/Literal.h"
#include "../util/WaspOptions.h"
#include "utils/FilesManagement.h"

EagerConstraintImpl::EagerConstraintImpl(): compilationManager(EAGER_MODE){
}

EagerConstraintImpl::~EagerConstraintImpl() {
    for (auto & a : literals) {
        delete a.second;
    }
}

void EagerConstraintImpl::performCompilation() {
    string executablePathAndName = wasp::Options::arg0;
    string executablePath = executablePathAndName;
    for (int i = executablePath.size() - 1; i >= 0; i--) {
        if (executablePath[i] == '/') {
            executablePath = executablePath.substr(0, i);
            break;
        }
    }

    FilesManagement fileManagement;
    string executorPath = executablePath + "/src/lp2cpp/Executor.cpp";
    int fd = fileManagement.tryGetLock(executorPath);
    string hash = fileManagement.computeMD5(executorPath);
    std::ofstream outfile(executorPath);
    compilationManager.setOutStream(&outfile);
    compilationManager.lp2cpp();
    outfile.close();
    string newHash = fileManagement.computeMD5(executorPath);
    executionManager.compileDynamicLibrary(executablePath, newHash != hash);
    fileManagement.releaseLock(fd, executorPath);
    compilationDone = true;
}

void EagerConstraintImpl::setFilename(const std::string & executablePath, const std::string & filename) {
    this-> fileDirectory = fileDirectory;
    this -> filename = filename;
    this -> filepath = fileDirectory + "/" + filename;
    FilesManagement fileManagement;
    if (!fileManagement.exists(filepath)) {
        throw std::runtime_error("Failed to compile eager program: file " + filepath + " does not exist.");
    }
    compilationManager.loadProgram(filepath);

};

bool EagerConstraintImpl::onLiteralTrue(int var) {

};

void EagerConstraintImpl::onLiteralsUndefined(const std::vector<int> & lits) {

};

void EagerConstraintImpl::getReason(const std::vector<int> & reason) {

};

//TODO remove duplication
aspc::Literal* parseLiteral2(const std::string & literalString) {
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

void EagerConstraintImpl::addedVarName(int var, const std::string & literalString) {
    aspc::Literal * atom = parseLiteral2(literalString);
    //atom.print();
    //cout<<endl;
    this->literals[var] = atom;
    literalsMap[*atom] = var;
    compilationManager.insertModelGeneratorPredicate(atom->getPredicateName());
    if (compilationManager.getBodyPredicates().count(atom->getPredicateName())) {
        watchedAtoms.push_back(var);
    }
};

void EagerConstraintImpl::onFact(int var) {
    facts.insert(var);

};

const std::vector<unsigned int> & EagerConstraintImpl::getVariablesToFreeze() {

};