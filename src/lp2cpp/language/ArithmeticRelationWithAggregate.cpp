/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ArithmeticRelationWithAggregate.cpp
 * Author: giuseppe
 * 
 * Created on 20 marzo 2020, 9.57
 */

#include "ArithmeticRelationWithAggregate.h"
#include "../utils/SharedFunctions.h"

aspc::ArithmeticRelationWithAggregate::ArithmeticRelationWithAggregate(const aspc::ArithmeticExpression & expression, const aspc::Aggregate & aggregate, aspc::ComparisonType compare,bool isNegated): aggregate(aggregate),guard(expression),comparisonType(compare),negated(isNegated){
    plusOne=false;
    if(comparisonType == aspc::GT){
        comparisonType = aspc::GTE;
        plusOne=true;
    }else if(comparisonType == aspc::LT){
        negated=!negated;
        comparisonType = aspc::GTE;
    }else if(comparisonType==aspc::LTE){
        negated=!negated;
        comparisonType = aspc::GTE;
        plusOne=true;
    }
}


bool aspc::ArithmeticRelationWithAggregate::isBoundedRelation(const std::unordered_set<std::string> & boundVariables) const {
    
    for (const std::string & v : guard.getAllTerms()) {
        if (isVariable(v) && !boundVariables.count(v)) {
            return false;
        }
    }
    
    return true;
}

bool aspc::ArithmeticRelationWithAggregate::isBoundedLiteral(const std::unordered_set<std::string> &) const {
    
    return false;
}
bool aspc::ArithmeticRelationWithAggregate::isBoundedValueAssignment(const std::unordered_set<std::string> & boundVariables) const {
    
    //#count{..}=X #count{..}=X+5 se X non è bound è un boundedValueAssignment 
    //#count{..}=X+Y se entrambe non sono bound allora non è un boundedValueAssignment
    
    if(this->comparisonType != aspc::EQ )
        return false;
    
    unsigned unassignedVariables=0;
    for (const std::string & v : guard.getAllTerms()) {
        if (!boundVariables.count(v) && isVariable(v)) {
            unassignedVariables++;
        }
    }
    return unassignedVariables == 1;
    
}
void aspc::ArithmeticRelationWithAggregate::addVariablesToSet(std::unordered_set<std::string> & set) const {
    for (const std::string & v : guard.getAllTerms()) {
        if (!set.count(v) && isVariable(v)) {
            set.insert(v);
        }
    }
    
}
bool aspc::ArithmeticRelationWithAggregate::isPositiveLiteral() const {
    return false;
}
void aspc::ArithmeticRelationWithAggregate::print() const {
    std::string negation = negated ? "not":"";
    std::cout<<negation<<" ";
    aggregate.print();
    std::cout << " " << aspc::ArithmeticRelation::comparisonType2String[comparisonType] << " " << guard;
    std::string plus = plusOne ? "+1":"";
    std::cout<<plus;
    
}
bool aspc::ArithmeticRelationWithAggregate::isLiteral() const {
    return false;
}
bool aspc::ArithmeticRelationWithAggregate::containsAggregate() const{
    return true;
}
unsigned aspc::ArithmeticRelationWithAggregate::firstOccurrenceOfVariableInLiteral(const std::string &) const{
    return -1;
}

aspc::ArithmeticRelationWithAggregate::~ArithmeticRelationWithAggregate() {
}

