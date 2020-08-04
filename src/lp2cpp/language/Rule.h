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

#ifndef RULE_H_ASPC
#define RULE_H_ASPC
#include <vector>
#include <map>
#include <unordered_map>
#include <list>
#include <tuple>
#include "Atom.h"
#include "Literal.h"
#include "ArithmeticRelation.h"
#include "ArithmeticRelationWithAggregate.h"

enum RuleType {
    GENERATIVE_RULE, CONSTRAINT
};




namespace aspc {
    

    class Rule {
    public:
        static unsigned rulesCounter;
        static std::string inequalityStrings[];
        Rule(const std::vector<aspc::Atom> & head, const std::vector<aspc::Literal> & body, const std::vector<ArithmeticRelation> & arithmeticRelation,const std::vector<aspc::ArithmeticRelationWithAggregate> & arithmeticRelationsWithAggregate);
        Rule(const std::vector<aspc::Atom> & head, const std::vector<aspc::Literal> & body, const std::vector<ArithmeticRelation> & inequalities, bool reorderBody);
        Rule(const std::vector<aspc::Atom> & head, const std::vector<aspc::Literal> & body, const std::vector<ArithmeticRelation> & inequalities,const std::vector<ArithmeticRelationWithAggregate> & inequalitiesWithAggregate, bool reorderBody);
        Rule(const Rule& other);

        virtual ~Rule();
        const std::vector<aspc::Atom> & getHead() const;
        const std::vector<aspc::Literal> & getBodyLiterals() const;
        const std::vector<ArithmeticRelation> & getArithmeticRelations() const;
        const std::vector<ArithmeticRelationWithAggregate> & getArithmeticRelationsWithAggregate() const;
        void addArithmeticRelationsWithAggregate(ArithmeticRelationWithAggregate r);
        RuleType getType() const;
        unsigned getRuleId() const;
        std::vector<std::map<unsigned, std::pair<unsigned, unsigned> > > getJoinIndicesWithJoinOrder(const std::vector<unsigned> & order) const;
        std::map<unsigned, std::pair<unsigned, unsigned> > getBodyToHeadVariablesMap() const;
        unsigned getBodySize() const;
        void print() const;
        bool containsNegation() const;
        bool containsLiteral() const;
        bool containsAggregate() const;
        bool isConstraint() const;
        std::pair<int, int> findFirstOccurrenceOfVariableByStarter(const std::string & var, unsigned starter) const;

        void bodyReordering();
        void bodyReordering(const std::vector<unsigned> & starters);
        void printOrderedBodies() const;


        const std::vector<unsigned> & getOrderedBodyIndexesByStarter(unsigned start) const;
        const std::vector<const aspc::Formula*>& getOrderedBodyByStarter(unsigned start) const;
        std::vector<unsigned> getStarters() const;

        const std::vector<const aspc::Formula*> & getFormulas() const;
        std::vector<const aspc::Formula*> getOrderedBodyForReasons(std::unordered_set<std::string> boundVariables) const;
        
        aspc::Literal* getAggregateLiteral (unsigned formulas_index,unsigned literal_index)const{
            return (aggregateLiterals.find(formulas_index)->second)[literal_index];
        }

        void changeCompareTypeToAggregate(int aggrIndex,aspc::ComparisonType type){
            arithmeticRelationsWithAggregate[aggrIndex].setCompareType(type);
        }
        
        std::vector<aspc::Literal*> getAggregateLiteral (unsigned formulas_index)const{
            return aggregateLiterals.find(formulas_index)->second;
        }

        bool containAggregatePredicate(std::string predicateName)const{
            for(const aspc::ArithmeticRelationWithAggregate& ar:arithmeticRelationsWithAggregate)
                for(const aspc::Literal& li : ar.getAggregate().getAggregateLiterals())
                    if(li.getPredicateName() == predicateName)
                        return true;
            return false;
        }

    private:
        std::vector<aspc::Atom> head;
        std::vector<aspc::Literal> bodyLiterals;
        int ruleId;
        std::vector<ArithmeticRelation> arithmeticRelations;
        std::vector<ArithmeticRelationWithAggregate> arithmeticRelationsWithAggregate;
        std::vector<const aspc::Formula*> formulas;
        std::unordered_map<unsigned,std::vector<aspc::Literal*> > aggregateLiterals;

        std::unordered_map<unsigned, std::vector<const aspc::Formula*> > orderedBodyByStarters;
        std::unordered_map<unsigned, std::vector<unsigned> > orderedBodyIndexesByStarters;

    };
}

#endif

