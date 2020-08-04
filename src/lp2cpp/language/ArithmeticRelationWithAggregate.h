/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ArithmeticRelationWithAggregate.h
 * Author: giuseppe
 *
 * Created on 20 marzo 2020, 9.57
 */

#ifndef ARITHMETICRELATIONWITHAGGREGATE_H
#define ARITHMETICRELATIONWITHAGGREGATE_H

#include "ArithmeticExpression.h"
#include "Aggregate.h"
#include "ArithmeticRelation.h"

namespace aspc {
    class ArithmeticRelationWithAggregate: public Formula{
        
        public:
            
            ArithmeticRelationWithAggregate(const aspc::ArithmeticExpression & expression, const aspc::Aggregate & aggregate, aspc::ComparisonType comparisonType,bool isNegated);
            virtual bool isBoundedRelation(const std::unordered_set<std::string> &) const override;
            virtual bool isBoundedLiteral(const std::unordered_set<std::string> &) const override;
            virtual bool isBoundedValueAssignment(const std::unordered_set<std::string> &) const override;
            virtual void addVariablesToSet(std::unordered_set<std::string> &) const override;
            virtual bool isPositiveLiteral() const override;
            virtual void print() const override;
            virtual bool isLiteral() const override;
            virtual bool containsAggregate() const override;
            virtual unsigned firstOccurrenceOfVariableInLiteral(const std::string &) const override;
            virtual ~ArithmeticRelationWithAggregate();
            bool isLessOrLessEqual() const{ return comparisonType == aspc::LT ||comparisonType == aspc::LTE;}
            void setFormulaIndex(unsigned formulaIndex){this->formulaIndex = formulaIndex;}
            unsigned getFormulaIndex()const{return this->formulaIndex;}
            const aspc::Aggregate & getAggregate() const {return aggregate;}
            aspc::ArithmeticExpression getGuard() const{return guard;}
            std::string getCompareTypeAsString()const{return aspc::ArithmeticRelation::comparisonType2String[comparisonType];}
            bool isNegated()const{return negated;}
            bool isPlusOne() const {return plusOne;}
            void setPlusOne(bool isPlusOne){
                this->plusOne=isPlusOne;
            }
            void setNegated(bool isNegated){
                this->negated=isNegated;
            }
            void setCompareType(aspc::ComparisonType type){
                comparisonType=type;
            }

        private:
            unsigned formulaIndex;
            aspc::Aggregate aggregate;
            aspc::ArithmeticExpression guard;
            aspc::ComparisonType comparisonType;
            bool negated;
            bool plusOne;
        };  
};

#endif /* ARITHMETICRELATIONWITHAGGREGATE_H */

