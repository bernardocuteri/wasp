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
 * File:   AspCore2ProgramBuilder.h
 * Author: bernardo
 *
 * Created on June 16, 2016, 2:53 PM
 */

#ifndef ASPCORE2PROGRAMBUILDER_H
#define	ASPCORE2PROGRAMBUILDER_H

#include "DLV2libs/input/InputBuilder.h"
#include "Program.h"
#include <vector>
using namespace std;

class AspCore2ProgramBuilder : public DLV2::InputBuilder {
private:
    aspc::Program program;
    vector<string> buildingTerms;
    vector<aspc::Literal> buildingBody;
    vector<aspc::Atom> buildingHead;
    map<string, unsigned> arietyMap;
    bool naf;
    InequalitySign inequalitySign;
    vector<tuple<string, InequalitySign, string> > inequalities;
    string predicateName;
public:
    AspCore2ProgramBuilder();

    virtual void onAggregate(bool naf);

    virtual void onAggregateElement();

    virtual void onAggregateFunction(char* functionSymbol);

    virtual void onAggregateGroundTerm(char* value, bool dash);

    virtual void onAggregateLowerGuard();

    virtual void onAggregateNafLiteral();

    virtual void onAggregateUnknownVariable();

    virtual void onAggregateUpperGuard();

    virtual void onAggregateVariableTerm(char* value);

    virtual void onArithmeticOperation(char arithOperator);

    virtual void onAtom(bool isStrongNeg);

    virtual void onBody();

    virtual void onBodyLiteral();

    virtual void onBuiltinAtom();

    virtual void onChoiceAtom();

    virtual void onChoiceElement();

    virtual void onChoiceElementAtom();

    virtual void onChoiceElementLiteral();

    virtual void onChoiceLowerGuard();

    virtual void onChoiceUpperGuard();

    virtual void onConstraint();

    virtual void onDirective(char* directiveName, char* directiveValue);

    virtual void onEqualOperator();

    virtual void onExistentialAtom();

    virtual void onExistentialVariable(char* var);

    virtual void onFunction(char* functionSymbol, int nTerms);

    virtual void onGreaterOperator();

    virtual void onGreaterOrEqualOperator();

    virtual void onHead();

    virtual void onHeadAtom();

    virtual void onLessOperator();

    virtual void onLessOrEqualOperator();

    virtual void onNafLiteral(bool naf);

    virtual void onPredicateName(char* name);

    virtual void onQuery();

    virtual void onRule();

    virtual void onTerm(int value);

    virtual void onTerm(char* value);

    virtual void onTermDash();

    virtual void onTermParams();

    virtual void onTermRange(char* lowerBound, char* upperBound);

    virtual void onUnequalOperator();

    virtual void onUnknownVariable();

    virtual void onWeakConstraint();

    virtual void onWeightAtLevels(int nWeight, int nLevel, int nTerm);
    
    const aspc::Program & getProgram();
    
    const map<string, unsigned> & getArietyMap();


};

#endif	/* ASPCORE2PROGRAMBUILDER_H */

