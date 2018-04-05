/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ArithmeticExpression.cpp
 * Author: bernardo
 * 
 * Created on March 7, 2017, 2:22 PM
 */

#include "ArithmeticExpression.h"

aspc::ArithmeticExpression::ArithmeticExpression() {

}


aspc::ArithmeticExpression::ArithmeticExpression(const std::string& term1, const std::string& term2, char operation):term1(term1), term2(term2), operation(operation), singleTerm(false){
}

aspc::ArithmeticExpression::ArithmeticExpression(const std::string& term1):term1(term1), singleTerm(true) {
}

bool aspc::ArithmeticExpression::isSingleTerm() const{
    return singleTerm;
}

std::set<std::string> aspc::ArithmeticExpression::getAllTerms() const {
    std::set<std::string> res;
    res.insert(term1);
    if(!isSingleTerm()) {
        res.insert(term2);
    }
    return res;
}
