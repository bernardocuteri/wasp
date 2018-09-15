/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Tuple.h
 * Author: bernardo
 *
 * Created on April 9, 2018, 1:19 PM
 */

#ifndef TUPLE_WITH_REASONS_H
#define TUPLE_WITH_REASONS_H
#include <vector>
#include <string>
#include <unordered_set>
#include "TupleWithoutReasons.h"


class TupleWithReasons : public TupleWithoutReasons {
public:
    
    TupleWithReasons() {
        
    }

    TupleWithReasons(const std::string* predicateName, bool negated = false) : TupleWithoutReasons(predicateName, negated) {
    }

    TupleWithReasons(const TupleWithReasons& orig) : TupleWithoutReasons(orig), positiveReasons(orig.positiveReasons),
    negativeReasons(orig.negativeReasons) {
    }
  
    
    virtual ~TupleWithReasons() {

    }

    TupleWithReasons(const std::initializer_list<unsigned> & l, bool negated = false) : TupleWithoutReasons(l, negated) {
    }

    TupleWithReasons(const std::initializer_list<unsigned> & l, const std::string * predicateName, bool negated = false) :
    TupleWithoutReasons(l, predicateName, negated) {
    }
    
    TupleWithReasons(const std::vector<unsigned> & l, const std::string * predicateName, bool negated = false) :
    TupleWithoutReasons(l, predicateName, negated) {
    }


    void addPositiveReason(const TupleWithReasons* r) const {
        positiveReasons.push_back(r);
    }

    void addNegativeReason(const TupleWithReasons & r) const {
        negativeReasons.push_back(r);
    }

    const vector<const TupleWithReasons*> & getPositiveReasons() const {
        return positiveReasons;
    }

    const vector<TupleWithReasons> & getNegativeReasons() const {
        return negativeReasons;
    }

private:
    mutable vector<const TupleWithReasons*> positiveReasons;
    mutable vector<TupleWithReasons> negativeReasons;

};



#endif /* TUPLE_H */

