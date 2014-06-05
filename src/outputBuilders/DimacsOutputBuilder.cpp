/*
 *
 *  Copyright 2013 Mario Alviano, Carmine Dodaro, and Francesco Ricca.
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

#include "DimacsOutputBuilder.h"

#include "../Variable.h"
#include <cassert>
using namespace std;

extern int EXIT_CODE;

void
DimacsOutputBuilder::startModel()
{
    EXIT_CODE = 10;
    if( ++numberOfModels == 1 )
        cout << SOLUTION_DIMACS << " " << SAT << endl;
    
    cout << COMMENT_DIMACS << " solution " << numberOfModels << endl;    
    cout << VALUE_DIMACS;
}

void 
DimacsOutputBuilder::printVariable(
    Var v,
    bool isTrue )
{    
    if( isTrue )
    {
        cout << " " << v;
    }
    else
    {
        cout << " -" << v;
    }
}

void
DimacsOutputBuilder::endModel()
{
    cout << endl;
}

void
DimacsOutputBuilder::onProgramIncoherent()
{
    EXIT_CODE = 20;
    cout << SOLUTION_DIMACS << " " << UNSAT << endl;
}

void
DimacsOutputBuilder::greetings()
{
    cout << COMMENT_DIMACS << " " << WASP_STRING << endl;
}
