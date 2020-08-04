/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ArithmeticExpression.h
 * Author: bernardo
 *
 * Created on March 7, 2017, 2:22 PM
 */

#ifndef ARITHMETICEXPRESSION_H
#define ARITHMETICEXPRESSION_H

#include<string>
#include<ostream>
#include<unordered_set>
#include<string>
#include<vector>

namespace aspc {

    class Rule;
    
    class ArithmeticExpression {

        friend std::ostream & operator<<(std::ostream & out, const ArithmeticExpression & e) {
            out << e.term1;
            if (!e.singleTerm) {
                out << e.operation << e.term2;
            }
            return out;
        }
    public:

        ArithmeticExpression();
        ArithmeticExpression(const std::string &, const std::string &, char operation);
        ArithmeticExpression(const std::string &);
        bool isSingleTerm() const;

        const std::string & getTerm1() const {
            return term1;
        }

        const std::string & getTerm2() const {
            return term2;
        }

        char getOperation() const {
            return operation;
        }
        void setTerm1(std::string term){
            term1=term;
        }

        std::vector<std::string> getAllTerms() const;
        
        std::string getStringRep() const;


    private:
        std::string term1;
        std::string term2;
        char operation;
        bool singleTerm;

    };
}

#endif /* ARITHMETICEXPRESSION_H */

