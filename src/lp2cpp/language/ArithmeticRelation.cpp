/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ArithmeticRelation.cpp
 * Author: bernardo
 * 
 * Created on March 16, 2018, 5:07 PM
 */

#include "ArithmeticRelation.h"
#include <iostream>
#include <cassert>
bool isVariable(const string & v) {
    return (v[0]>='A' && v[0]<='Z');
    
}

void aspc::ArithmeticRelation::addVariablesToSet(set<string>& set) {
    assert(isBoundedValueAssignment(set));
    set.insert(left.getTerm1());

}

bool aspc::ArithmeticRelation::isBoundedExpression(const set<string>& set) const {
    for(const string & t: left.getAllTerms()) {
        if(isVariable(t) && !set.count(t)) {
            return false;
        }
    }
     for(const string & t: right.getAllTerms()) {
        if(isVariable(t) && !set.count(t)) {
            return false;
        }
    }
    return true;
}

bool aspc::ArithmeticRelation::isBoundedLiteral(const set<string>&) const {
    return false;
}

bool aspc::ArithmeticRelation::isBoundedValueAssignment(const set<string>& set) const {
    
    if(comparisonType == aspc::EQ && left.isSingleTerm() && isVariable(left.getTerm1()) && !set.count(left.getTerm1())) {
        return true;
    }
    return false;
}

void aspc::ArithmeticRelation::print() const {
    std::cout<<left<<" " << comparisonType<<" "<<right;

}

bool aspc::ArithmeticRelation::isPositiveLiteral() const {
    return false;
}
