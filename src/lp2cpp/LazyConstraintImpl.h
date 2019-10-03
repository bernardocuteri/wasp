/*
 *
 *  Copyright 2016 Bernardo Cuteri, Francesco Ricca.
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
 * File:   LazyConstraintImpl.h
 * Author: bernardo
 *
 * Created on January 9, 2017, 11:05 AM
 */

#ifndef LAZYCONSTRAINTIMPL_H
#define LAZYCONSTRAINTIMPL_H

#include "LazyConstraint.h"
#include <vector>
#include "language/Atom.h"
#include "ExecutionManager.h"
#include "CompilationManager.h"
#include "datastructures/DenseMap.h"
#include <unordered_map>


class LazyConstraintImpl: public LazyConstraint {
public: 
    LazyConstraintImpl();
    virtual void setFilename(const std::string & executablePath, const std::string & filename) override;
    virtual void addedVarName(int var, const std::string & atomString) override;
    virtual void onFact(int var) override;
    virtual bool checkAnswerSet(const std::vector<int> & interpretation) override;
    virtual void onCheckFail(std::vector<int> & constraints) override;
    virtual const std::vector<unsigned int> & getVariablesToFreeze() override;
    virtual const std::string & getFilepath() const;
    virtual ~LazyConstraintImpl();



private:
    void performCompilation();
    DenseMap<aspc::Literal*> literals;
    ExecutionManager executionManager;
    CompilationManager compilationManager;
    std::unordered_map<aspc::Literal, int, aspc::LiteralHash> literalsMap;
    std::vector<unsigned> watchedAtoms;
    std::vector<unsigned> idbWatchedAtoms;
    std::unordered_set<int> facts;
    std::string filepath;
    bool compilationDone = false;
    
    std::string fileDirectory;
    std::string filename;
    
};

#endif /* LAZYCONSTRAINTIMPL_H */

