/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ArithmeticRelation.h
 * Author: bernardo
 *
 * Created on March 16, 2018, 5:07 PM
 */

#ifndef ARITHMETICRELATION_H
#define ARITHMETICRELATION_H
#include "Formula.h"
#include "ArithmeticExpression.h"
#include <set> 
#include <string>
#include <map>
using namespace std;

namespace aspc {
    

    enum ComparisonType {
        GTE = 0, LTE, GT, LT, NE, EQ, UNASSIGNED
    };

    class ArithmeticRelation : public Formula {
    public:
        static map<ComparisonType, string> comparisonType2String;



    public:
        ArithmeticRelation(const aspc::ArithmeticExpression & left, const aspc::ArithmeticExpression & right, aspc::ComparisonType comparisonType);

        virtual bool isBoundedRelation(const set<string> &) const override;
        virtual bool isBoundedLiteral(const set<string> &) const override;
        virtual bool isBoundedValueAssignment(const set<string> &) const override;
        virtual void addVariablesToSet(set<string> &) const override;
        virtual bool isPositiveLiteral() const override;
        virtual void print() const override;
        virtual bool isLiteral() const override;
        virtual unsigned firstOccurrenceOfVariableInLiteral(const string &) const override;

        virtual ~ArithmeticRelation() {

        }
        
        aspc::ComparisonType getComparisonType() const {
            return comparisonType;
        }

        aspc::ArithmeticExpression getLeft() const {
            return left;
        }

        aspc::ArithmeticExpression getRight() const {
            return right;
        }




    private:
        aspc::ArithmeticExpression left;
        aspc::ArithmeticExpression right;
        aspc::ComparisonType comparisonType;


    };
}

#endif /* ARITHMETICRELATION_H */

