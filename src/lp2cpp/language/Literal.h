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

#ifndef LITERAL_H_ASPC
#define LITERAL_H_ASPC

#include "Atom.h"
#include "Formula.h"

namespace aspc {

    class Literal : public aspc::Formula {
    public:
        Literal(const string & predicateName);
        Literal(const string & predicateName, bool negated);
        Literal(bool negated, const aspc::Atom & atom);
        Literal(const Literal &);
        virtual ~Literal();

        void negate();
        bool isNegated() const;
        const string & getTermAt(unsigned i) const;
        bool isVariableTermAt(unsigned i) const;
        const string & getPredicateName() const;
        unsigned getAriety() const;
        const aspc::Atom & getAtom() const;
        const vector<string>& getTerms() const;
        void addTerm(const string &);
        bool operator==(const Literal& right) const;
        Tuple getTuple(unsigned id) const;
        void setNegated(bool);
        
        virtual bool isBoundedRelation(const set<string> &) const override;
        virtual bool isBoundedLiteral(const set<string> &) const override;
        virtual bool isBoundedValueAssignment(const set<string> &) const override;
        virtual void addVariablesToSet(set<string> &) const override;
        virtual bool isPositiveLiteral() const override;
        virtual void print() const override;
        virtual bool isLiteral() const override;
        virtual unsigned firstOccurrenceOfVariableInLiteral(const string & v) const override;
        


        
    private:
        aspc::Atom atom;
        bool negated;

    };
}

#endif
