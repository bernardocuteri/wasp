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

#ifndef TUPLE_H
#define TUPLE_H
#include <vector>
#include <string>
#include <unordered_set>
using namespace std;

struct TupleHash;

class Tuple : public vector<unsigned> {
public:
    Tuple(size_type id, const string* predicateName, bool negated=false);

    Tuple(const Tuple& orig);
    virtual ~Tuple();
    
    Tuple(const std::initializer_list<unsigned> & l, bool negated = false) :
        vector<unsigned>(l), id(0), predicateName(NULL), negated(negated){
    }

    Tuple(const std::initializer_list<unsigned> & l, size_type id, const string * predicateName, bool negated = false) :
        vector<unsigned>(l), id(id), predicateName(predicateName), negated(negated) {
    }

    size_type getId() const {
        return id;
    }

    const string* getPredicateName() const {
        return predicateName;
    }

    bool isNegated() const {
        return negated;
    }
    
    void addPositiveReason(const Tuple* r) const {
        positiveReasons.push_back(r);
    }
    
    void addNegativeReason(const Tuple & r) const {
        negativeReasons.push_back(r);
    }

    
    const vector<const Tuple*> & getPositiveReasons() const {
        return positiveReasons;
    }
    
    const vector<Tuple> & getNegativeReasons() const {
        return negativeReasons;
    }
    
private:
    size_type id;
    const string * predicateName;
    bool negated;
    mutable vector<const Tuple*> positiveReasons;
    mutable vector<Tuple> negativeReasons;
    
};

struct TupleHash {

    inline size_t operator()(const Tuple & v) const {
        size_t seed = 0;
        for (unsigned i : v) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }

};



#endif /* TUPLE_H */

