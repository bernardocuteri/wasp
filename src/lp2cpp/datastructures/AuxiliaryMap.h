/*
 *
 *  Copyright 2016 Bernardo Cuteri, Francesco Ricca.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

/* 
 * File:   AuxiliaryMap.h
 * Author: bernardo
 *
 * Created on March 7, 2016, 2:17 PM
 */

#ifndef AUXILIARY_MAP_H
#define AUXILIARY_MAP_H
#include <list>
#include <vector>
#include <unordered_map>
#include <cmath>
#include "TupleWithoutReasons.h"

const unsigned AUX_MAPS_TOTAL_MEMORY = 0; // (sizeof NULL) * 1 * 1024 * 1024; //8MB in size -> 1KB in size for ariety 1

struct VectorHash {

    inline size_t operator()(const std::vector<int>& v) const {
        size_t seed = 0;
        for (int i : v) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }

};

template<class T>
class AuxiliaryMap {
public:

    AuxiliaryMap(const std::vector<unsigned> & keyIndices) :
    keySize(keyIndices.size()), keyIndices(keyIndices), lookup_bases(keySize, 0) {
        unsigned total_size = AUX_MAPS_TOTAL_MEMORY / (sizeof NULL);
        if (keySize == 0) {
            lookup_size = total_size = 1;
        } else if (keySize == 1) {
            lookup_size = total_size = (unsigned) sqrt(total_size);
        } else {
            lookup_size = (unsigned) std::pow(total_size, 1.0 / keySize);
        }
        lookup = std::vector<std::vector<const T *>*>(total_size, NULL);

    }

    virtual ~AuxiliaryMap() {
    }
    
    inline int keySetSize(){
        
        int size = lookupReferences.size();
        for(auto it = lookupReferences.begin(); it!=lookupReferences.end();it++){
            if(it->empty())
                size--;
        }
        return size;
    }
    inline const std::vector< const T* >& getValues(const std::vector<int>& key) const {
        if (canLookup(key)) {
            return lookup[valueToPos(key)] == NULL ? EMPTY_RESULT : *lookup[valueToPos(key)];
        }

        const auto it = tuples.find(key);
        if (it == tuples.end()) {
            return EMPTY_RESULT;
        }
        return it->second;
    }

    inline void insert2(const T & value) {
        std::vector<int> key(keySize);
        for (unsigned i = 0; i < keySize; i++) {
            key[i] = value[keyIndices[i]];
            //std::cout<<key[i]<<" ";
        }
        if (canLookup(key)) {
            unsigned pos = valueToPos(key);
            if (lookup[pos] == NULL) {
                lookupReferences.push_back(std::vector< const T* >());
                lookup[pos] = &lookupReferences.back();

            }
            lookup[pos]->push_back(&value);
            value.setCollisionListIndex(lookup[pos], (unsigned) (lookup[pos]->size()-1));
            return;
        }
        auto & collisionList = tuples[std::move(key)];
        value.setCollisionListIndex(&collisionList, collisionList.size());
        collisionList.push_back(&value);
        //std::cout<<"CollisionList size: "<<collisionList.size()<<std::endl;
    }

    void clear() {
        if (lookupReferences.size()) {
            std::fill(lookup.begin(), lookup.end(), (std::vector<const T *>*)NULL);
            lookupReferences.clear();
            std::fill(lookup_bases.begin(), lookup_bases.end(), 0);
        }
        tuples.clear();
    }
    inline unsigned size()const{
        
        unsigned size = tuples.size();
        for (const auto element : tuples){
            if(element.second.empty())
                size--;
        }
        return size;
    }
protected:

    inline bool canLookup(const std::vector<int> & key) const {
        for (unsigned i = 0; i < keySize; i++) {
            if ((key[i] - lookup_bases[i] >= lookup_size) || (key[i] - lookup_bases[i] < 0)) {
                return false;
            }
        }
        return true;
    }

    inline bool canLookup(const std::vector<int> & key) {
        for (unsigned i = 0; i < keySize; i++) {
            if ((key[i] - lookup_bases[i] >= lookup_size || key[i] - lookup_bases[i] < 0)) {
                if (lookupReferences.empty() && lookup_size > 0) {
                    lookup_bases[i] = key[i];
                } else {
                    return false;
                }
            }
        }
        return true;
    }

    inline unsigned valueToPos(const std::vector<int> & key) const {
        unsigned pos = 0;
        for (unsigned i = 0; i < keySize; i++) {
            pos += (key[i] - lookup_bases[i]) * std::pow(lookup_size, i);
        }
        return pos;
    }
    
    
    
    std::unordered_map<std::vector<int>, std::vector< const T* >, VectorHash > tuples;
    unsigned keySize;
    std::vector<unsigned> keyIndices;
    static const std::vector< const T* > EMPTY_RESULT;
    std::list<std::vector<const T *> > lookupReferences;
    std::vector<std::vector<const T *>*> lookup;
    int lookup_size;
    std::vector<int> lookup_bases;


};

template<class T>
const std::vector< const T* > AuxiliaryMap<T>::EMPTY_RESULT;


#endif /* AUXILIARYMAP_H */

