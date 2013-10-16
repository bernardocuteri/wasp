/*
 *
 *  Copyright 2013 Mario Alviano, Carmine Dodaro, Wolfgang Faber, Nicola Leone, Francesco Ricca, and Marco Sirianni.
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
 * File:   GringoNumericFormat.h
 * Author: Carmine Dodaro
 *
 * Created on 9 September 2013, 11.36
 */


#ifndef _GRINGONUMERICFORMAT_H
#define	_GRINGONUMERICFORMAT_H

#include "../inputBuilders/AbstractBuilder.h"

class GringoNumericFormat
{
    public:

        inline GringoNumericFormat( AbstractBuilder* p );
        ~GringoNumericFormat();

        /**
        * This function read instruction from standard input and
        * build the program.
        */
        void parse();

        /**
        * This function read instruction from input and
        * build the program.
        *
        * @param input The istream input.
        */
        void parse( istream& input );

    private:    

        void readAtomsTable( istream& input );
        void readNormalRule( istream& input );
        void readDisjunctiveRule( istream& input );
        void readChoiceRule( istream& input );
        void readConstraintRule( istream& input );

        void readTrueAtoms( istream& input );
        void readFalseAtoms( istream& input );

        void readErrorNumber( istream& input );

        void postprocessing();

        void addNewVariable( unsigned int variable );

        AbstractBuilder* builder;

        unsigned int max;              
};

GringoNumericFormat::GringoNumericFormat(
    AbstractBuilder* b ) : builder( b ), max( 1 )
{
}

#endif	/* _GRINGONUMERICFORMAT_H */