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
#include "TupleWithoutReasons.h"

struct VectorHash {

    inline size_t operator()(const std::vector<unsigned>& v) const {
        size_t seed = 0;
        for (unsigned i : v) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }

};

template<class T>
class AuxiliaryMap {
public:

    AuxiliaryMap(const std::vector<unsigned> & keyIndices) :
        keySize(keyIndices.size()), keyIndices(keyIndices) {

    }

    inline const std::vector< const T* >& getValues(const std::vector<unsigned>& key) const {

        const auto it = tuples.find(key);
        if (it == tuples.end()) {
            return EMPTY_RESULT;
        }
        return it->second;
    }

    inline void insert2(const T & value) {
        std::vector<unsigned> key(keySize);
        for (unsigned i = 0; i < keySize; i++) {
            key[i] = value[keyIndices[i]];
        }
        tuples[std::move(key)].push_back(&value);
    }

    void clear() {
        tuples.clear();
    }
protected:
    std::unordered_map<std::vector<unsigned>, std::vector< const T* >, VectorHash > tuples;
    unsigned keySize;
    std::vector<unsigned> keyIndices;
    static const std::vector< const T* > EMPTY_RESULT;


};

template<class T>
const std::vector< const T* > AuxiliaryMap<T>::EMPTY_RESULT;

#endif /* AUXILIARYMAP_H */

