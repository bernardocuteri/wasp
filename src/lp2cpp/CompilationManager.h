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

#ifndef COMPILATIONMANAGER_H
#define	COMPILATIONMANAGER_H

#include "language/Program.h"
#include "parsing/AspCore2ProgramBuilder.h"
#include "utils/Indentation.h"
#include "datastructures/PredicateWSet.h"
#include <string>
#include <set>

class CompilationManager {
public:
    CompilationManager();
    void lp2cpp(const std::string & filename);
    void generateStratifiedCompilableProgram(aspc::Program & program, AspCore2ProgramBuilder* builder);
    void setOutStream(std::ostream* outputTarget);
    const std::set<std::string> & getBodyPredicates();
    
private:
    
    void handleRuleLoops(const aspc::Rule& r,unsigned start) ;
    void handleDataStructuresDeclaration(const aspc::Rule& r, unsigned start);
    bool matchConstants(const aspc::Rule & rule, const aspc::Atom & atom, Indentation & ind);
    void generateHeadTupleAndMatchConstants(const aspc::Rule & rule, Indentation & ind, const set<string> & bodyPredicates);
    void setHeadVariables(Indentation & ind, const aspc::Rule & rule);
    bool checkInequalities(const aspc::Rule & rule, Indentation & ind);
    void declareArithmeticVariables(const aspc::Rule & rule, Indentation & ind);
    bool handleEqualCardsAndConstants(const aspc::Rule & r,unsigned i,const vector<unsigned>& joinOrder);
    bool handleExpression(const aspc::Rule& r, const aspc::ArithmeticRelation &, unsigned, const set<string> &);
    
    std::ostream* out;
    std::set<std::string> bodyPredicates;
    
    Indentation ind;
    
    std::set<std::string> declaredMaps;
    
    vector< unordered_map < unsigned, vector< vector< pair < unsigned , unsigned > > > > >  joinKeys; 
    
    vector< vector < vector < string > > > auxiliaryMapsNameByRuleAndStartIndex;
    
    unordered_map<string, set<string> > predicateToAuxiliaryMaps;

};

#endif	/* COMPILATIONMANAGER_H */

