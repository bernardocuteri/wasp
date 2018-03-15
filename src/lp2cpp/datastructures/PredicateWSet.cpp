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
 * File:   PredicateWSet.cpp
 * Author: bernardo
 * 
 * Created on February 26, 2016, 11:17 AM
 */

#include <vector>

#include "PredicateWSet.h"
#include "../utils/ConstantsManager.h"
#include <iostream>
#include <algorithm>
#include <climits>
#include <map>

const unsigned halfMaxInt = UINT_MAX / 2;
//TODO test different initializations

PredicateWSet::PredicateWSet(unsigned height, unsigned predicateAriety, list <vector<unsigned> > & tuples) : height(height),
predicateAriety(predicateAriety), tuples(tuples) {

}

PredicateWSet::PredicateWSet(unsigned predicateAriety, list <vector<unsigned> > & tuples) : PredicateWSet(predicateAriety, predicateAriety, tuples) {

}

PredicateWSet::~PredicateWSet() {

    for (int type = 0; type <= 1; type++)
        for (const auto& nested : nestedArray[type]) {
            delete nested.second;
        }
}



const vector<unsigned>& PredicateWSet::getLastTuple() {
    return tuples.back();
}

bool PredicateWSet::insert(vector<unsigned>& tuple) {
    //leaf array
    unsigned v = tuple[predicateAriety - height];
    int type = 0;
    bool duplicate = true;
    if (v >= halfMaxInt) {
        v -= halfMaxInt;
        type = 1;
    }

    if (height == 1) {
        if (leaves[type].find(v) == leaves[type].end()) {
            tuples.push_back(tuple);
            leaves[type].insert(v);
            duplicate = false;
        }
        return !duplicate;
    }//nested array
    else {
        unordered_map<unsigned, PredicateWSet*>::iterator it = nestedArray[type].find(v);
        if (it != nestedArray[type].end()) {
            return it->second->insert(tuple);
        } else {
            PredicateWSet* toInsert = new PredicateWSet(height - 1, predicateAriety, tuples);
            nestedArray[type][v] = toInsert;
            toInsert->insert(tuple);
            return true;
        }
    }


}

unsigned PredicateWSet::size() const {
    return tuples.size();
}

void PredicateWSet::printTuples(const string & predicateName) const {
    for (const vector<unsigned> & tuple : tuples) {
        cout << predicateName << "(";
        for (unsigned i = 0; i < tuple.size(); i++) {
            if (i > 0) {
                cout << ",";
            }
            cout << tuple[i];
        }
        cout << ").\n";
    }
}

const list <vector<unsigned> > & PredicateWSet::getTuples() const {
    return tuples;

}
