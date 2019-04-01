/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   EagerConstrainImpl.h
 * Author: bernardo
 *
 * Created on March 27, 2019, 3:20 PM
 */

#ifndef EAGERCONSTRAINIMPL_H
#define EAGERCONSTRAINIMPL_H

#include "EagerConstraint.h"
#include <vector>
#include "language/Atom.h"
#include "ExecutionManager.h"
#include "CompilationManager.h"
#include "datastructures/DenseMap.h"
#include <unordered_map>

struct LiteralHash {

    size_t operator()(const aspc::Literal & v) const {
        std::hash<unsigned> hasher;
        size_t seed = 0;
        for (unsigned i : v.getAtom().getIntTuple()) {
            seed ^= hasher(i) + (seed << 6) + (seed >> 2);
        }
        return (std::hash<std::string>()(v.getPredicateName())) ^ seed;
    }
};

class EagerConstraintImpl : public EagerConstraint {
public:
    EagerConstraintImpl();
    virtual ~EagerConstraintImpl();
    virtual void setFilename(const std::string & executablePath, const std::string & filename);
    virtual bool onLiteralTrue(int var);
    virtual void onLiteralsUndefined(const std::vector<int> & lits);
    virtual void getReason(const std::vector<int> & reason);
    virtual void addedVarName(int var, const std::string & atomString);
    virtual void onFact(int var);
    virtual const std::vector<unsigned int> & getVariablesToFreeze();
private:
    
    void performCompilation();
    DenseMap<aspc::Literal*> literals;
    ExecutionManager executionManager;
    CompilationManager compilationManager;
    std::unordered_map<aspc::Literal, int, LiteralHash> literalsMap;
    std::vector<unsigned> watchedAtoms;
    std::vector<unsigned> idbWatchedAtoms;
    std::unordered_set<int> facts;
    std::string filepath;
    bool compilationDone = false;
    std::string fileDirectory;
    std::string filename;
};

#endif /* EAGERCONSTRAINIMPL_H */

