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
 * File:   AuxiliaryMap.cpp
 * Author: bernardo
 * 
 * Created on March 7, 2016, 2:17 PM
 */

#include <vector>

#include "AuxiliaryMap.h"
#include <iostream>
#include <map>
#include <algorithm>
#include <climits>
#include <unordered_map>

using namespace std;

const vector<const Tuple* > AuxiliaryMap::EMPTY_RESULT;

AuxiliaryMap::AuxiliaryMap(const vector<unsigned> & keyIndices) :
keySize(keyIndices.size()), keyIndices(keyIndices) {

}


void AuxiliaryMap::insert2(const Tuple & value) {
    vector<unsigned> key(keySize);
    for(unsigned i = 0;i<keySize;i++) {
        key[i] = value[keyIndices[i]];
    }
    tuples[std::move(key)].push_back(&value);    
}

const vector< const Tuple* >& AuxiliaryMap::getValues(const vector<unsigned>& key) const {

    const auto it = tuples.find(key);
    if(it == tuples.end()) {
        return EMPTY_RESULT;
    }
    return it->second;
}

void AuxiliaryMap::clear() {
    tuples.clear();
}
