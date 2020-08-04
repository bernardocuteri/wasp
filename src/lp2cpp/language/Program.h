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

#ifndef PROGRAM_H_ASPC
#define PROGRAM_H_ASPC
#include <vector>
#include <map>
#include <list>
#include <set>
#include "Rule.h"


namespace aspc {

    class Program {
    public:
        Program();
        virtual ~Program();
        void addRule(const aspc::Rule & r);
        void addFact(const aspc::Atom & f);
        void addPredicate(const std::string& name, const unsigned ariety);
        void addAggregatePredicate(const std::string& name, const unsigned ariety);
        void addArithmeticRelationToRule(unsigned index,aspc::ArithmeticRelationWithAggregate r);
        void changeCompareTypeToRule(unsigned index,int aggrIndex,aspc::ComparisonType type);
        const std::set< std::pair<std::string, unsigned> >& getPredicates() const;
        const std::set< std::pair<std::string, unsigned> >& getAggregatePredicates() const;
        unsigned getRulesSize() const;
        const std::vector<aspc::Rule>& getRules() const;
        std::vector<aspc::Rule>& getRules();
        unsigned getFactsSize() const;
        const aspc::Rule & getRule(unsigned i) const;
        const aspc::Atom & getFact(unsigned i) const;
        const std::vector<aspc::Atom> & getFacts() const;
        const std::map<RuleType, std::map<std::string, std::set<unsigned> > > & getRulesByType() const;
        const std::set<unsigned> & getRulesByTypeAndPredicateName(const std::string & predicateName, RuleType type) const;
        void clearFacts();
        void print() const;
        std::set<std::string> getBodyPredicates() const;
        std::set<std::string> getHeadPredicates() const;
        bool hasConstraintWithLiteral()const;
        bool hasConstraint() const;

    private:
        std::vector<aspc::Rule> rules; //only rules are compiled
        std::vector<aspc::Atom> facts; //not compiled
        std::set< std::pair<std::string, unsigned> > predicates;
        std::set< std::pair<std::string, unsigned> > aggregatePredicates;

        std::map<RuleType, std::map<std::string, std::set<unsigned> > > rules_by_type;

    };
}

#endif

