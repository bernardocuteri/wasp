/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Aggregate.h
 * Author: giuseppe
 *
 * Created on 17 marzo 2020, 17.20
 */

#ifndef AGGREGATE_H
#define AGGREGATE_H
#include "Formula.h"
#include "Literal.h"
#include <map>
#include <vector>
#include <set>


namespace aspc {
    
    enum AggregateFunction {
        COUNT = 0, SUM, MAX, MIN, UNKNOWN
    };
    
    class Aggregate : public Formula{
    
    public:
        Aggregate();
        Aggregate(const std::vector<aspc::Literal> & literals, const std::set<std::string> & variables, std::string function);

        virtual bool isBoundedRelation(const std::unordered_set<std::string> &) const override;
        virtual bool isBoundedLiteral(const std::unordered_set<std::string> &) const override;
        virtual bool isBoundedValueAssignment(const std::unordered_set<std::string> &) const override;
        virtual void addVariablesToSet(std::unordered_set<std::string> &) const override;
        virtual bool isPositiveLiteral() const override;
        virtual void print() const override;
        virtual bool isLiteral() const override;
        virtual unsigned firstOccurrenceOfVariableInLiteral(const std::string &) const override;
        const std::vector<aspc::Literal> & getAggregateLiterals()const{return aggregateLiterals;}
        std::set<std::string> getAggregateVariables()const {return aggregateVariables;}
        bool containsVar(std::string v)const{
            return aggregateVariables.count(v);
        }
        bool isSum()const;
        
        std::string getAggregateFunction()const;
        virtual ~Aggregate();
        
    private:
        std::vector<aspc::Literal> aggregateLiterals;
        std::set<std::string> aggregateVariables;
        aspc::AggregateFunction aggregateFunction;
        
    };
}
#endif /* AGGREGATE_H */

