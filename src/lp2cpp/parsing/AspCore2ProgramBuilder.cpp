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
 * File:   AspCore2ProgramBuilder.cpp
 * Author: bernardo
 * 
 * Created on June 16, 2016, 2:53 PM
 */

#include "AspCore2ProgramBuilder.h"
#include <iostream>
#include <cstring>
#include <bits/unordered_map.h>
#include "../language/ArithmeticRelation.h"
#include "../language/Aggregate.h"
#include "../language/ArithmeticRelationWithAggregate.h"

using namespace std;

AspCore2ProgramBuilder::AspCore2ProgramBuilder() : naf(false), inequalitySign(aspc::UNASSIGNED) {
}

void AspCore2ProgramBuilder::buildExpression() {
    if (buildingTerms.size() == 1) {
        expression = aspc::ArithmeticExpression(buildingTerms[0]);
    } else {
        expression = aspc::ArithmeticExpression(buildingTerms[0], buildingTerms[1], arithOp);
    }
    buildingTerms.clear();
}

void AspCore2ProgramBuilder::onAggregate(bool negation) {
    isNegated=negation;

}

void AspCore2ProgramBuilder::clearAggregateFields(){

    aggreagateLiterals.clear();
    aggregateVariables.clear();
    aggregateFunction="None";
    inequalitySign=aspc::UNASSIGNED;
    isNegated=false;
}
void AspCore2ProgramBuilder::onAggregateElement() {

}

void AspCore2ProgramBuilder::onAggregateFunction(char* function) {
    this->aggregateFunction=function;
}

void AspCore2ProgramBuilder::onAggregateGroundTerm(char* groundTerm, bool negated) {
}

void AspCore2ProgramBuilder::onAggregateLowerGuard() {
    this->buildExpression();
    isLower=true;
}

void AspCore2ProgramBuilder::onAggregateNafLiteral() {
    
    this->aggreagateLiterals.push_back(aspc::Literal(naf,aspc::Atom(predicateName,buildingTerms)));
    buildingTerms.clear();

}

void AspCore2ProgramBuilder::onAggregateUnknownVariable() {

}

void AspCore2ProgramBuilder::onAggregateUpperGuard() {
    this->buildExpression();
    isLower=false;
}

void AspCore2ProgramBuilder::onAggregateVariableTerm(char* variableTerm) {
    this->aggregateVariables.insert(variableTerm);
}

void AspCore2ProgramBuilder::onArithmeticOperation(char op) {

    arithOp = op;
}

void AspCore2ProgramBuilder::onAtom(bool) {

}

void AspCore2ProgramBuilder::onBody() {

}

void AspCore2ProgramBuilder::onBodyLiteral() {
    if (inequalitySign != aspc::UNASSIGNED) {
        if(aggregateFunction != "None"){
            inequalitiesWithAggregate.push_back(aspc::ArithmeticRelationWithAggregate(expression,aspc::Aggregate(aggreagateLiterals,aggregateVariables,aggregateFunction),inequalitySign,isNegated));
            
            clearAggregateFields();
        }
        else if (buildingTerms.size() == 1) {
            inequalities.push_back(aspc::ArithmeticRelation(expression, aspc::ArithmeticExpression(buildingTerms[0]), inequalitySign));
        } else {
            inequalities.push_back(aspc::ArithmeticRelation(expression, aspc::ArithmeticExpression(buildingTerms[0], buildingTerms[1], arithOp), inequalitySign));
        }
    } else {
        
        buildingBody.push_back(aspc::Literal(naf, aspc::Atom(predicateName, buildingTerms)));
        if (arietyMap.count(predicateName)) {
            //assert((*arietyMap)[predicateName] == buildingTerms->size());
        } else {
            arietyMap[predicateName] = buildingTerms.size();
        }
    }
    inequalitySign = aspc::UNASSIGNED;
    buildingTerms.clear();
    naf = false;
}

void AspCore2ProgramBuilder::onBuiltinAtom() {

}

void AspCore2ProgramBuilder::onChoiceAtom() {

}

void AspCore2ProgramBuilder::onChoiceElement() {

}

void AspCore2ProgramBuilder::onChoiceElementAtom() {

}

void AspCore2ProgramBuilder::onChoiceElementLiteral() {

}

void AspCore2ProgramBuilder::onChoiceLowerGuard() {

}

void AspCore2ProgramBuilder::onChoiceUpperGuard() {

}

void AspCore2ProgramBuilder::onConstraint() {
    

    int size=program.getRules().size();
    
    for (const aspc::Literal& l : buildingBody) {
        int currentBodyId = predicateIDs.size();
        unordered_map<string, int>::iterator i = predicateIDs.find(l.getPredicateName());
        if (i != predicateIDs.end())
            currentBodyId = i->second;
        else {
            predicateIDs[l.getPredicateName()] = currentBodyId;
            vertexByID[currentBodyId] = Vertex(currentBodyId, l.getPredicateName());
        }
    }
    for (const aspc::Literal& l : buildingBody) {
        program.addPredicate(l.getPredicateName(), l.getAriety());
    }
    
    //adding literal in aggregate
    bool notEqual = false;
    for (aspc::ArithmeticRelationWithAggregate& r : inequalitiesWithAggregate){
        if(r.getCompareTypeAsString() == "=="){
            r.setCompareType(aspc::GTE);
            if(r.isNegated()){
                notEqual = true;
            }else{
                aspc::ArithmeticRelationWithAggregate r_(r.getGuard(),r.getAggregate(),aspc::GT,true);
                inequalitiesWithAggregate.push_back(r_);
            }
            
        }
        for(const aspc::Literal& l: r.getAggregate().getAggregateLiterals()){
            program.addAggregatePredicate(l.getPredicateName(),l.getAriety());
        }
    }
    aspc::Rule constraint(buildingHead, buildingBody, inequalities,std::vector<aspc::ArithmeticRelationWithAggregate>(inequalitiesWithAggregate), true);
    program.addRule(constraint);
    if(notEqual){

        for(aspc::ArithmeticRelationWithAggregate& r : inequalitiesWithAggregate){
            //si assume ci sia solo un aggregato
            r.setNegated(false);
            r.setPlusOne(true);
            aspc::Rule copy_(buildingHead, buildingBody, inequalities,inequalitiesWithAggregate, true);
            program.addRule(copy_);

        }

    }
    buildingBody.clear();
    buildingHead.clear();
    inequalities.clear();
    inequalitiesWithAggregate.clear();

}

void AspCore2ProgramBuilder::onDirective(char*, char*) {

}

void AspCore2ProgramBuilder::onEqualOperator() {
    inequalitySign = aspc::EQ;
    if(aggregateFunction == "None")
        buildExpression();
}

void AspCore2ProgramBuilder::onExistentialAtom() {

}

void AspCore2ProgramBuilder::onExistentialVariable(char*) {

}

void AspCore2ProgramBuilder::onFunction(char*, int) {

}

void AspCore2ProgramBuilder::onGreaterOperator() {

    inequalitySign = aspc::GT;
    if(aggregateFunction == "None")
        buildExpression();
}

void AspCore2ProgramBuilder::onGreaterOrEqualOperator() {

    inequalitySign = aspc::GTE;
    if(aggregateFunction == "None")
        buildExpression();
}

void AspCore2ProgramBuilder::onHead() {
}

void AspCore2ProgramBuilder::onHeadAtom() {

    buildingHead.push_back(aspc::Atom(predicateName, buildingTerms));
    if (arietyMap.count(predicateName)) {
        //assert((*arietyMap)[predicateName] == buildingTerms->size());
    } else {
        arietyMap[predicateName] = buildingTerms.size();
    }
    buildingTerms.clear();
}

void AspCore2ProgramBuilder::onLessOperator() {

    inequalitySign = aspc::LT;
    if(aggregateFunction == "None")
        buildExpression();
}

void AspCore2ProgramBuilder::onLessOrEqualOperator() {

    inequalitySign = aspc::LTE;
    if(aggregateFunction == "None")
        buildExpression();
}

void AspCore2ProgramBuilder::onNafLiteral(bool naf) {
    if (naf) {
        this->naf = true;
    }

}

void AspCore2ProgramBuilder::onPredicateName(char* predicateName) {

    //why the following does not work?
    //this->predicateName = predicateName;
    this->predicateName = predicateName;



}

void AspCore2ProgramBuilder::onQuery() {

}

void AspCore2ProgramBuilder::onRule() {
    if (buildingBody.empty()) {
        program.addFact(buildingHead.back());
        
    } else {
        aspc::Rule rule = aspc::Rule(buildingHead, buildingBody, inequalities,inequalitiesWithAggregate, true);
        program.addRule(rule);
        //adding edges to dependency graph
        for (const aspc::Atom& a : buildingHead) {
            int currentHeadId = predicateIDs.size();
            unordered_map<string, int>::iterator i = predicateIDs.find(a.getPredicateName());
            if (i != predicateIDs.end())
                currentHeadId = i->second;
            else {
                predicateIDs[a.getPredicateName()] = currentHeadId;
                vertexByID[currentHeadId] = Vertex(currentHeadId, a.getPredicateName());
            }
            vertexByID[currentHeadId].rules.push_back(rule.getRuleId());

            for (const aspc::Literal& l : buildingBody) {
                int currentBodyId = predicateIDs.size();
                unordered_map<string, int>::iterator i = predicateIDs.find(l.getPredicateName());
                if (i != predicateIDs.end())
                    currentBodyId = i->second;
                else {
                    predicateIDs[l.getPredicateName()] = currentBodyId;
                    vertexByID[currentBodyId] = Vertex(currentBodyId, l.getPredicateName());
                }
                //vertexByID[currentBodyId].rules.push_back(rule.getRuleId());
                graphWithTarjanAlgorithm.addEdge(currentBodyId, currentHeadId);
            }
        }


    }
    //add predicates to program
    for (const aspc::Atom& a : buildingHead) {
        program.addPredicate(a.getPredicateName(), a.getAriety());
    }
    for (const aspc::Literal& l : buildingBody) {
        program.addPredicate(l.getPredicateName(), l.getAriety());
    }

    buildingBody.clear();
    buildingHead.clear();
    inequalities.clear();
    inequalitiesWithAggregate.clear();
}

void AspCore2ProgramBuilder::onTerm(int) {

}

void AspCore2ProgramBuilder::onTerm(char* value) {
    buildingTerms.push_back(value);
    
}

void AspCore2ProgramBuilder::onTermDash() {

}

void AspCore2ProgramBuilder::onTermParams() {

}

void AspCore2ProgramBuilder::onTermRange(char*, char*) {

}

void AspCore2ProgramBuilder::onUnequalOperator() {

    inequalitySign = aspc::NE;
    //if(aggregateFunction!="None")
        buildExpression();

}

void AspCore2ProgramBuilder::onUnknownVariable() {
    buildingTerms.push_back("_");
}

void AspCore2ProgramBuilder::onWeakConstraint() {

}

void AspCore2ProgramBuilder::onWeightAtLevels(int, int, int) {

}

aspc::Program & AspCore2ProgramBuilder::getProgram() {
    return program;
}

const map<string, unsigned> & AspCore2ProgramBuilder::getArietyMap() {
    return arietyMap;
}

GraphWithTarjanAlgorithm& AspCore2ProgramBuilder::getGraphWithTarjanAlgorithm() {
    return graphWithTarjanAlgorithm;
}

const unordered_map<int, Vertex>& AspCore2ProgramBuilder::getVertexByIDMap() const {
    return vertexByID;
}

const unordered_map<string, int>& AspCore2ProgramBuilder::getPredicateIDsMap() const {
    return predicateIDs;
}



