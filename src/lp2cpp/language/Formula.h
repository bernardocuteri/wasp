/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Formula.h
 * Author: bernardo
 *
 * Created on March 16, 2018, 3:20 PM
 */

#ifndef FORMULA_H
#define FORMULA_H
#include <set>
#include <string>
using namespace std;
namespace aspc {

    class Formula {
    public:
        virtual bool isBoundedExpression(const set<string> &) const = 0;
        virtual bool isBoundedLiteral(const set<string> &) const = 0;
        virtual bool isBoundedValueAssignment(const set<string> &) const = 0;
        virtual bool isPositiveLiteral() const = 0;
        virtual void addVariablesToSet(set<string> &) = 0;
        virtual void print() const = 0;

    };
}

#endif /* FORMULA_H */

