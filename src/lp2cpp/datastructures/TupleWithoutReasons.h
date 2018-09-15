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

#ifndef TUPLE_WITHOUT_REASONS_H
#define TUPLE_WITHOUT_REASONS_H
#include <vector>
#include <string>
#include <unordered_set>

struct TuplesHash;

class TupleWithoutReasons : public std::vector<unsigned> {
public:
    TupleWithoutReasons(): predicateName(NULL) {
        
    }

    TupleWithoutReasons(const std::string* predicateName, bool negated = false) : predicateName(predicateName), negated(negated) {
    }

    TupleWithoutReasons(const TupleWithoutReasons& orig) : std::vector<unsigned>(orig), predicateName(orig.predicateName), negated(orig.negated) {
    }
    
    
    virtual ~TupleWithoutReasons() {
        
    }

    TupleWithoutReasons(const std::initializer_list<unsigned> & l, bool negated = false) :
    std::vector<unsigned>(l), predicateName(NULL), negated(negated) {
    }

    TupleWithoutReasons(const std::initializer_list<unsigned> & l, const std::string * predicateName, bool negated = false) :
    vector<unsigned>(l), predicateName(predicateName), negated(negated) {
    }
    
    TupleWithoutReasons(const std::vector<unsigned> & l, const std::string * predicateName, bool negated = false) :
    vector<unsigned>(l), predicateName(predicateName), negated(negated) {
    }

    const std::string* getPredicateName() const {
        return predicateName;
    }

    bool isNegated() const {
        return negated;
    }

    //    void addPositiveReason(const Tuple* r) const {
    //        positiveReasons.push_back(r);
    //    }
    //    
    //    void addNegativeReason(const Tuple & r) const {
    //        negativeReasons.push_back(r);
    //    }
    //
    //    
    //    const vector<const Tuple*> & getPositiveReasons() const {
    //        return positiveReasons;
    //    }
    //    
    //    const vector<Tuple> & getNegativeReasons() const {
    //        return negativeReasons;
    //    }

private:
    const std::string * predicateName;
    bool negated;
    //    mutable vector<const Tuple*> positiveReasons;
    //    mutable vector<Tuple> negativeReasons;

};

struct TuplesHash {

    inline std::size_t operator()(const TupleWithoutReasons & v) const {
        std::size_t seed = 0;
        for (unsigned i : v) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }

};



#endif /* TUPLE_H */

