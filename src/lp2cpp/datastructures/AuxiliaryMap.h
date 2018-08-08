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
#include "Tuple.h"

using namespace std;

struct VectorHash {

    inline size_t operator()(const std::vector<unsigned>& v) const {
        size_t seed = 0;
        for (unsigned i : v) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }

};

class AuxiliaryMap {
public:
    AuxiliaryMap(const vector<unsigned> & keyIndices);
    void insert2(const Tuple & value);
    const vector<const Tuple* >& getValues(const vector<unsigned>& tuple) const;
protected:
    unordered_map<vector<unsigned>, vector< const Tuple* >, VectorHash > tuples;
    unsigned keySize;
    vector<unsigned> keyIndices;
    static const vector< const Tuple* > EMPTY_RESULT;
};

#endif /* AUXILIARYMAP_H */

