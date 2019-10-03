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

#ifndef __EXECUTOR_H__
#define __EXECUTOR_H__

#include <unordered_set>
#include <unordered_map>
#include <list>
#include <vector>
#include "language/Literal.h"
#include "datastructures/AuxiliaryMap.h"
#include <iostream>
#include <algorithm>
namespace aspc {

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

    class Executor {
    public:
        Executor();
        virtual ~Executor();
        virtual void executeProgramOnFacts(const std::vector<aspc::Literal*> & facts);
        virtual void executeProgramOnFacts(const std::vector<int> & facts);
        virtual void onLiteralTrue(const aspc::Literal* l);
        virtual void onLiteralTrue(int var);
        virtual void onLiteralUndef(const aspc::Literal* l);
        virtual void onLiteralUndef(int var);
        virtual void addedVarName(int var, const std::string & atom);
        virtual void executeFromFile(const char* factsFile);
        virtual void init();
        virtual void clear();
        virtual void clearPropagations();

        virtual const std::vector<std::vector<aspc::Literal> > & getFailedConstraints() {
            return failedConstraints;
        }

        virtual const std::vector<std::string> & getBodyLiterals() {
            return bodyLiterals;
        }

        virtual void shuffleFailedConstraints() {
            std::random_shuffle(failedConstraints.begin(), failedConstraints.end());
        }

        virtual const std::unordered_map<int, std::vector<int> > & getPropagatedLiteralsAndReasons() const {
            return propagatedLiteralsAndReasons;
        }


    private:
        std::vector<std::vector<aspc::Literal> > failedConstraints;
        std::unordered_map<int, std::vector<int> > propagatedLiteralsAndReasons;
        //std::unordered_map<aspc::Literal, std::vector<aspc::Literal>, LiteralHash> propagatedLiteralsAndReasons;
        std::vector<std::string> bodyLiterals;


    };
}
#endif
