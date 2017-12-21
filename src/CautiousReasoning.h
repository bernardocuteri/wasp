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

#ifndef CAUTIOUSREASONING_H
#define	CAUTIOUSREASONING_H

#include "AnswerSetListener.h"
#include "util/WaspConstants.h"
#include "Literal.h"
#include "util/VariableNames.h"
#include <vector>
#include <unordered_map>
#include <iostream>
using namespace std;

#include "WaspFacade.h"

#define CAUTIOUS_MULTI_CORE 0
#define CAUTIOUS_COHERENT_STOP 1
#define CAUTIOUS_SINGLE_CORE 2
#define CAUTIOUS_ALREDY_FOUND_CORE 3

class CoreAdditional
{
    public:
        inline CoreAdditional(vector<Literal>& c, vector<Literal>& auxAtoms) { core.swap(c); additionalAtoms.swap(auxAtoms); }
        unsigned int size() const { return core.size(); }
        Literal getLiteral(unsigned int pos) const { assert(pos < core.size()); return core[pos]; }
        void copyCore(vector<Literal>& core_, vector<Literal>& auxAtoms) const { core_ = core; auxAtoms = additionalAtoms; } 

    private:
        vector<Literal> core;
        vector<Literal> additionalAtoms;
};

class CoreBasedUtil
{
    public:
        inline CoreBasedUtil() : numberOfCalls(1), next(0) {}
        inline ~CoreBasedUtil() {
            for(unsigned int i = 0; i < cores.size(); i++) delete cores[i];
            cores.clear();
        }

        inline void newComputation() { numberOfCalls++; }
        inline void resetComputation() { next = 0; }
        inline void setAssumption(Literal lit) { lit2Assumptions[lit.getId()] = numberOfCalls; }
        
        inline void addCore(vector<Literal>& core, vector<Literal>& auxAtoms) {
            assert(!core.empty()); assert(!auxAtoms.empty());
            cores.push_back(new CoreAdditional(core, auxAtoms));
            assert(core.empty()); assert(auxAtoms.empty());
        }

        inline bool checkExistenceOfCore(vector<Literal>& core, vector<Literal>& auxAtoms) {
            bool exists = false;
            while(next < cores.size()) {
                bool found = true;
                for(unsigned int i = 0; i < cores[next]->size(); i++)
                    if(!isAssumption(cores[next]->getLiteral(i))) { found=false; break; }
                if(found) { cores[next]->copyCore(core, auxAtoms); exists=true; break; }
                next++;
            }
            if(next >= cores.size()) next = UINT_MAX; //check is not needed anymore.
            return exists;
        }                
        
    private:
        unordered_map<int, unsigned int> lit2Assumptions;
        unsigned int numberOfCalls;
        unsigned int next;
        vector<CoreAdditional*> cores;
        inline bool isAssumption(Literal lit) const {
            return lit2Assumptions.find(lit.getId()) != lit2Assumptions.end() && lit2Assumptions.at(lit.getId()) == numberOfCalls;
        }    
};

class CautiousReasoning : public AnswerSetListener
{
    public:
        CautiousReasoning(WaspFacade& waspFacade_) : waspFacade(waspFacade_), coreUtil(NULL) {}
        virtual ~CautiousReasoning() { delete coreUtil; }
        void foundAnswerSet();
        void solve();
        
    private:
        vector<Var> candidates;
        vector<Var> answers;
        WaspFacade& waspFacade;               
        CoreBasedUtil* coreUtil;
        
        void addAnswer(Var v);
        void printCandidates();
        
        bool computeFirstModel();
        
        void iterativeCoherenceTesting();
        void iterativeCoherenceTestingRestricted(vector<Var>& myCandidates);
        void overestimateReduction();
        
        void minimalAlgorithm();
        void findMinimalModelPreferences();
        void findMinimalModelOne();
        void findMinimalModelK();
        void findMinimalModelPmres();
        void chunkDynamic(unsigned int chunkSize);
        void checkChunk(vector<Var>& myCandidates);
        void coreBased(unsigned int chunkSize);
        void checkCoreChunk(vector<Var>& myCandidates);
        void printAnswers();
        void initAssumptions(vector<Literal>& assumptions);
        
        unsigned int solveAndProcessCore(vector<Literal>& assumptions, vector<Literal>& conflict);
        
        #ifdef TRACE_ON
        inline void printVectorOfVars(const vector<Var>& v, const string& description) {
            cerr << description;
            for(unsigned int i = 0; i < v.size(); i++) if(!VariableNames::isHidden(v[i])) cerr << " " << VariableNames::getName(v[i]);
            cerr << endl;
        }
        
        inline void printVectorOfLiterals(const vector<Literal>& v, const string& description) {
            cerr << description;
            for(unsigned int i = 0; i < v.size(); i++) cerr << " " << v[ i ];
            cerr << endl;
        }
        #endif

};

#endif

