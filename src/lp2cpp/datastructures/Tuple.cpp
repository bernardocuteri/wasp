/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Tuple.cpp
 * Author: bernardo
 * 
 * Created on April 9, 2018, 1:19 PM
 */

#include "Tuple.h"



Tuple::Tuple(size_type id, const string& predicateName, bool negated) : id(id), predicateName(predicateName), negated(negated) {
}


Tuple::Tuple(const Tuple& orig):vector<unsigned>(orig), id(orig.id), predicateName(orig.predicateName), negated(orig.negated) {
}

Tuple::~Tuple() {
}
