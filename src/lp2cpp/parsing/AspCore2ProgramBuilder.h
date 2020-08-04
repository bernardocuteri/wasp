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

#include "../DLV2libs/input/InputBuilder.h"
#include "../language/Program.h"
#include "../language/ArithmeticExpression.h"
#include "../utils/GraphWithTarjanAlgorithm.h"
#include "../language/ArithmeticRelation.h"
#include "../language/ArithmeticRelationWithAggregate.h"
#include <vector>
#include <unordered_map>
#include "../language/Aggregate.h"

class AspCore2ProgramBuilder : public DLV2::InputBuilder {
private:
    aspc::Program program;
    
    std::string aggregateFunction="None";
    std::set<std::string> aggregateVariables;
    std::vector<aspc::Literal> aggreagateLiterals;
    bool isLower;
    bool isNegated;
    
    std::vector<aspc::Aggregate> aggregates;
    
    std::vector<std::string> buildingTerms;
    std::vector<aspc::Literal> buildingBody;
    std::vector<aspc::Atom> buildingHead;
    std::map<std::string, unsigned> arietyMap;
    bool naf;
    aspc::ComparisonType inequalitySign;
    char arithOp;
    aspc::ArithmeticExpression expression;
    std::vector<aspc::ArithmeticRelation> inequalities;
    std::vector<aspc::ArithmeticRelationWithAggregate> inequalitiesWithAggregate;
    std::string predicateName;
    GraphWithTarjanAlgorithm graphWithTarjanAlgorithm;
    std::unordered_map<std::string, int> predicateIDs;
    std::unordered_map<int, Vertex> vertexByID;
    void buildExpression();
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
    
    aspc::Program & getProgram();
    
    const  std::map<std::string, unsigned> & getArietyMap();
    
//    const void printSCC(){
//        std::vector<std::vector<int> > SCC = graphWithTarjanAlgorithm.SCC();
//        for(int i = 0;i< SCC.size();i++)
//        {
//            cout<< "componente "<< i <<endl;
//            for(int j = 0;j< SCC[i].size();j++)
//            {
//                std::unordered_map<int, Vertex>::iterator it = vertexByID.find(SCC[i][j]);
//                Vertex current = it->second;
//                cout<< current.id << "  " << current.name<<endl;
//                for(int c = 0; c< current.rules.size();c++)
//                    getProgram().getRule(current.rules[c]).print();
//            }
//        }
//    }
    void clearAggregateFields();
    GraphWithTarjanAlgorithm& getGraphWithTarjanAlgorithm();

    const std::unordered_map<int, Vertex>& getVertexByIDMap() const;
    const std::unordered_map<std::string, int>& getPredicateIDsMap() const;

};

#endif	/* ASPCORE2PROGRAMBUILDER_H */

