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
using namespace std;

namespace aspc {

    enum ComparisonType {
        GTE = 0, LTE, GT, LT, NE, EQ, UNASSIGNED
    };

    class ArithmeticRelation : public Formula {
    public:
        ArithmeticRelation(aspc::ArithmeticExpression left, aspc::ArithmeticExpression right, aspc::ComparisonType comparisonType) :
        left(left), right(right), comparisonType(comparisonType) {
        }

        virtual bool isBoundedExpression(const set<string> &) const override;
        virtual bool isBoundedLiteral(const set<string> &) const override;
        virtual bool isBoundedValueAssignment(const set<string> &) const override;
        virtual void addVariablesToSet(set<string> &) override;
        virtual bool isPositiveLiteral() const override;
        virtual void print() const override;
    private:
        aspc::ArithmeticExpression left;
        aspc::ArithmeticExpression right;
        aspc::ComparisonType comparisonType;


    };
}

#endif /* ARITHMETICRELATION_H */

