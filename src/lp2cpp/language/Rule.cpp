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

#include "Rule.h"
#include "ArithmeticExpression.h"
#include <iostream>
#include <cassert>


unsigned aspc::Rule::rulesCounter = 0;
string aspc::Rule::inequalityStrings[] = {">=", "<=", ">", "<", "!=", "=="};

aspc::Rule::Rule(const vector<aspc::Atom> & head, const vector<aspc::Literal> & body, const vector<aspc::ArithmeticRelation> & arithmeticRelations) : head(head), body(body), ruleId(rulesCounter), arithmeticRelations(arithmeticRelations) {
    rulesCounter++;
}

aspc::Rule::Rule(const vector<Atom>& head, const vector<Literal> & body, const vector<aspc::ArithmeticRelation>& arithmeticRelations, bool) : Rule(head, body, arithmeticRelations) {
    vector<Literal> orderedBody(body.size());
    int start = 0;
    int end = body.size() - 1;
    for (unsigned i = 0; i < body.size(); i++) {
        if (body[i].isNegated()) {
            orderedBody[end--] = body[i];
        } else {
            orderedBody[start++] = body[i];
        }
    }
    this->body = orderedBody;
}

aspc::Rule::~Rule() {
}

const vector<aspc::Literal> & aspc::Rule::getBody() const {
    return body;
}

const vector<aspc::Atom> & aspc::Rule::getHead() const {
    return head;
}

RuleType aspc::Rule::getType() const {
    if (head.empty()) {
        return CONSTRAINT;
    }
    return GENERATIVE_RULE;
}

vector<unsigned> aspc::Rule::getHeadToBodyVariableMap() const {
    vector<unsigned>headToBodyVariableMap(head.front().getAriety(), -1);
    for (unsigned i = 0; i < head.front().getTermsSize(); i++) {
        for (unsigned j = 0; j < body.size(); j++) {
            for (unsigned k = 0; k < body[j].getAtom().getTermsSize(); k++) {
                if (head.front().isVariableTermAt(i) && head.front().getTermAt(i) == body[j].getTermAt(k)) {
                    headToBodyVariableMap[i] = k;
                }
            }
        }
    }
    return headToBodyVariableMap;
}

unsigned aspc::Rule::getRuleId() const {
    return ruleId;
}

unsigned aspc::Rule::getBodySize() const {
    return body.size();
}

vector<map<unsigned, pair<unsigned, unsigned> > > aspc::Rule::getJoinIndicesWithJoinOrder(const vector<unsigned>& order) const {
    vector<map<unsigned, pair<unsigned, unsigned> > > result(order.size() - 1);
    unsigned orderSize = order.size();
    //for all other atom X (in order)
    for (unsigned i = 0; i < orderSize - 1; i++) {
        unsigned o_i = order[i + 1];
        //for all term T1 in X
        for (unsigned t1 = 0; t1 < body[o_i].getAriety(); t1++) {
            //for all atom Y preceding X in the join
            for (unsigned j = 0; j <= i; j++) {
                unsigned o_j = order[j];
                //for all term T2 in Y
                for (unsigned t2 = 0; t2 < body[o_j].getAriety(); t2++) {
                    if (!body[o_j].isNegated() && body[o_i].isVariableTermAt(t1) && body[o_i].getTermAt(t1) == body[o_j].getTermAt(t2)) {
                        pair<unsigned, unsigned> toAdd(j, t2);
                        result[i][t1] = toAdd;
                    }
                }
            }
        }
    }
    return result;
}

map<unsigned, pair<unsigned, unsigned> > aspc::Rule::getBodyToHeadVariablesMap() const {
    //TODO handle disjunction, j can only be 0 
    map<unsigned, pair<unsigned, unsigned> > resultMap;
    const Atom & headAtom = head.front();
    for (unsigned t1 = 0; t1 < headAtom.getAriety(); t1++) {
        if (headAtom.isVariableTermAt(t1)) {
            for (unsigned i = 0; i < body.size(); i++) {
                Literal bodyLiteral = body[i];
                if (!bodyLiteral.isNegated()) {
                    for (unsigned t2 = 0; t2 < bodyLiteral.getAriety(); t2++) {
                        if (bodyLiteral.getTermAt(t2) == headAtom.getTermAt(t1)) {
                            pair<unsigned, unsigned> toAddPair(i, t2);
                            resultMap[t1] = toAddPair;
                        }
                    }
                }
            }
        }
    }
    return resultMap;
}

const vector<aspc::ArithmeticRelation> & aspc::Rule::getArithmeticRelations() const {
    return arithmeticRelations;
}

void aspc::Rule::print() const {
    for (const Atom & atom : head) {
        atom.print();
        std::cout << " ";
    }
    std::cout << ":-";
    for (const aspc::Literal & literal : body) {
        literal.print();
        cout << " ";
    }
    for (const ArithmeticRelation & arithmeticRelation : arithmeticRelations) {
        arithmeticRelation.print();
        cout << " ";
    }
    cout << "\n";
}

bool aspc::Rule::containsNegation() const {
    for (unsigned i = 0; i < body.size(); i++) {
        if (body[i].isNegated()) {
            return true;
        }
    }
    return false;
}

bool aspc::Rule::isConstraint() const {
    return getType() == CONSTRAINT;
}

void aspc::Rule::bodyReordering() {
    vector<unsigned> starters;
    for(unsigned i = 0;i<body.size();i++) {
        if(body[i].isPositiveLiteral()) {
            starters.push_back(i);
            break;
        }
    }
    bodyReordering(starters);
    
}

void aspc::Rule::bodyReordering(const vector<unsigned>& starters) {

    if(starters.empty()) {
        bodyReordering();
    }

    for (unsigned starter : starters) {

        set<string> boundVariables;
        body[starter].addVariablesToSet(boundVariables);

        orderedBodyByStarter[starter].push_back(&body[starter]);

        list<Formula*> allFormulas;
        for (unsigned i = 0; i < body.size(); i++) {
            if (i != starter) {
                allFormulas.push_back(& body[i]);
            }

        }
        for (unsigned i = 0; i < arithmeticRelations.size(); i++) {
            allFormulas.push_back(& arithmeticRelations[i]);
        }
        while (!allFormulas.empty()) {
            Formula* boundExpression = NULL;
            Formula* boundLiteral = NULL;
            Formula* boundValueAssignment = NULL;
            Formula* positiveLiteral = NULL;
            Formula* selectedFormula = NULL;

            for (list<Formula*>::reverse_iterator formula = allFormulas.rbegin(); formula != allFormulas.rend(); formula++) {
                if ((*formula)->isBoundedExpression(boundVariables)) {
                    boundExpression = *formula;
                } else if ((*formula)->isBoundedLiteral(boundVariables)) {
                    boundLiteral = *formula;
                } else if ((*formula)->isBoundedValueAssignment(boundVariables)) {
                    boundValueAssignment = *formula;
                } else if ((*formula)->isPositiveLiteral()) {
                    positiveLiteral = *formula;
                }
            }

            if (boundExpression) {
                selectedFormula = boundExpression;
            } else if (boundLiteral) {
                selectedFormula = boundLiteral;
            } else if (boundValueAssignment) {
                selectedFormula = boundValueAssignment;
            } else {
                selectedFormula = positiveLiteral;
            }
            assert(selectedFormula);
            if(selectedFormula!=boundExpression && selectedFormula!=boundLiteral) {
                selectedFormula->addVariablesToSet(boundVariables);
            }
            orderedBodyByStarter[starter].push_back(selectedFormula);
            allFormulas.remove(selectedFormula);

        }

    }



}

void aspc::Rule::printOrderedBodies() const {
    
    for(const auto& entry:orderedBodyByStarter) {
        for(Formula* f:entry.second) {
            f->print();
            cout<<" ";
        }
        cout<<endl;
    }
    cout<<endl;

}
