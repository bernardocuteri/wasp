/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Aggregate.cpp
 * Author: giuseppe
 * 
 * Created on 17 marzo 2020, 17.20
 */

#include "Aggregate.h"
#include <iostream>


std::map<std::string,aspc::AggregateFunction> string2AggregateFunction = {
    {"#count", aspc::COUNT},
    {"#sum", aspc::SUM},
    {"#max", aspc::MAX},
    {"#min", aspc::MIN}

};

std::map<aspc::AggregateFunction,std::string> aggregateFunction2String = {
    {aspc::COUNT,"#count"},
    {aspc::SUM,"#sum"},
    {aspc::MAX,"#max"},
    {aspc::MIN,"#min"}

};
aspc::Aggregate::Aggregate(){
    
}

aspc::Aggregate::Aggregate(const std::vector<aspc::Literal> & literals, const std::set<std::string> & variables, std::string function): aggregateLiterals(literals), aggregateVariables(variables), aggregateFunction(string2AggregateFunction[function]){ 
    
}

bool aspc::Aggregate::isBoundedRelation(const std::unordered_set<std::string>& set) const {
    
    return true;
}

bool aspc::Aggregate::isBoundedLiteral(const std::unordered_set<std::string>&) const {
    return false;
}

bool aspc::Aggregate::isBoundedValueAssignment(const std::unordered_set<std::string>& boundVariables) const {

    return false;
}

void aspc::Aggregate::addVariablesToSet(std::unordered_set<std::string>& set) const {

}

bool aspc::Aggregate::isPositiveLiteral() const {
    return false;
}

std::string aspc::Aggregate::getAggregateFunction()const{
    return aggregateFunction2String[aggregateFunction];
}


bool aspc::Aggregate::isSum()const{
    return aggregateFunction==aspc::SUM;
}



void aspc::Aggregate::print() const {
    
    
    std::cout<<aggregateFunction2String[this->aggregateFunction]<<"{";
    
    unsigned size=this->aggregateVariables.size();
    int j=0;
    for(std::string s:aggregateVariables){
        std::cout<<s;
        if(j<size-1)
            std::cout<<", ";
    }
    std::cout<<" : ";
    size=this->aggregateLiterals.size();

    for(unsigned i=0;i<size;++i){
        std::cout<<aggregateLiterals[i].getPredicateName()<<"(";
        
        std::vector<std::string> terms = aggregateLiterals[i].getAtom().getTerms();
        unsigned num_terms = terms.size();
        
        for(unsigned j=0;j<num_terms;++j){
            std::cout<<terms[j];
            if(j<num_terms-1)
                std::cout<<", ";
        }
        std::cout<<")";
        if(i<size-1)
            std::cout<<", ";
    }
    std::cout<<"}";
    
}

bool aspc::Aggregate::isLiteral() const {
    return false;
}

unsigned aspc::Aggregate::firstOccurrenceOfVariableInLiteral(const std::string &) const {
    return 1;
}

aspc::Aggregate::~Aggregate(){
    
}


