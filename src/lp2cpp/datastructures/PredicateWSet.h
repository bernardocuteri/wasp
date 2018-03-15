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

#ifndef PREDICATEWSET_H
#define PREDICATEWSET_H
#include <list>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
using namespace std;

class PredicateWSet;

class PredicateWSet {
public:

    enum MatchingType : int_fast8_t {
        INT, STRING, BOTH
    };
    PredicateWSet(unsigned height, unsigned predicateAriety, list <vector<unsigned> >& tuples);
    PredicateWSet(unsigned predicateAriety, list <vector<unsigned> >& tuples);
    bool insert(vector<unsigned> & value);
    virtual ~PredicateWSet();
    unsigned size() const;
    bool contains(vector<unsigned> & tuple) const;
    void printTuples(const string & predicateName) const;
    const list<vector<unsigned> > & getTuples() const;
    const vector<unsigned>& getLastTuple();
protected:
    unordered_map<unsigned, PredicateWSet*> nestedArray[2];
    unordered_set<unsigned> leaves[2];
    unsigned height;
    unsigned predicateAriety;
    list <vector<unsigned> > & tuples;


};

#endif /* PREDICATEWSET_H*/

