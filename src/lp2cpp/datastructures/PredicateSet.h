/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PredicateSet.h
 * Author: bernardo
 *
 * Created on September 7, 2018, 11:20 AM
 */

#ifndef PREDICATESET_H
#define PREDICATESET_H
#include <unordered_set>
#include <iostream>
#include <vector>
#include <list>
#include <cmath>

const unsigned PREDICATE_SETS_TOTAL_MEMORY = (sizeof NULL) * 1 * 1024 * 1024; //8MB in size

template <class T, class H>
class PredicateSet : protected std::unordered_set<T, H> {
public:

    //Size of NULL is typically 8 byte, i.e. 32 bit

    PredicateSet(unsigned ariety) : ariety(ariety), lookup_bases(ariety, 0) {
        unsigned total_size = PREDICATE_SETS_TOTAL_MEMORY / (sizeof NULL);
        if (ariety == 0) {
            lookup_size = total_size = 1;
        } else if (ariety == 1) {
            lookup_size = total_size = (unsigned) sqrt(total_size);
        } else {
            lookup_size = (unsigned) std::pow(total_size, 1.0 / ariety);
        }
        lookup = std::vector<T*>(total_size, NULL);
    }

    virtual ~PredicateSet() {
    }

    void clear() {

        if (lookupReferences.size() > 0) {
            std::fill(lookup.begin(), lookup.end(), (T*) NULL);
            lookupReferences.clear();
            std::fill(lookup_bases.begin(), lookup_bases.end(), 0);
        }
        std::unordered_set<T, H>::clear();
    }

    std::pair<const T *, bool> insert(T && value) {
        if (canLookup(value)) {
            unsigned pos = valueToPos(value);

            if (lookup[pos] == NULL) {
                lookupReferences.push_back(std::move(value));
                lookup[pos] = &lookupReferences.back();
                return std::make_pair(&lookupReferences.back(), true);
            }
            return std::make_pair(lookup[pos], false);

        }
        const auto & insertResult = std::unordered_set<T, H>::insert(std::move(value));
        return std::make_pair(&(*insertResult.first), insertResult.second);
    }

    const T * find(const T & value) {
        if (canLookup(value)) {
            return lookup[valueToPos(value)];
        }
        const auto & findResult = std::unordered_set<T, H>::find(value);
        if (findResult == std::unordered_set<T, H>::end()) {
            return NULL;
        }
        return &*findResult;
    }

    //    std::pair<const T &, bool> insert(T && value) {
    //        return insert(std::move(value));
    //    }

private:

    inline bool canLookup(const T & value) {
        for (unsigned i = 0; i < ariety; i++) {
            if (value[i] - lookup_bases[i] >= lookup_size) {
                if(lookupReferences.empty() && lookup_size>0) {
                    lookup_bases[i] = value[i];
                } else {
                    return false;
                }
            }
        }
        return true;
    }

    inline unsigned valueToPos(const T & value) const {
        unsigned pos = 0;
        for (unsigned i = 0; i < ariety; i++) {
            pos += (value[i]-lookup_bases[i]) * std::pow(lookup_size, i);
        }
        return pos;
    }

    std::vector<T*> lookup;
    std::list<T> lookupReferences;
    unsigned ariety;
    unsigned lookup_size;
    std::vector<unsigned> lookup_bases; 

};

#endif /* PREDICATESET_H */

