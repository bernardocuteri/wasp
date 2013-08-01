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
 * File:   NegativeLiteral.h
 * Author: Carmine Dodaro
 *
 * Created on 21 July 2013, 16.55
 */

#ifndef NEGATIVELITERAL_H
#define	NEGATIVELITERAL_H

#include "Literal.h"

/**
 * This class represents a negative literal.
 *  
 */
class NegativeLiteral : public Literal
{
    public:        
        inline NegativeLiteral();
        inline NegativeLiteral( Variable* var );
//        inline NegativeLiteral( const string& name );
        
        virtual bool isFalse() const;
        virtual bool isTrue() const;

        virtual bool setFalse();
        virtual bool setTrue();
        
    private:
        
        virtual ostream& print( ostream& out ) const;
};

NegativeLiteral::NegativeLiteral() : Literal()
{
}

NegativeLiteral::NegativeLiteral(
    Variable* var ) : Literal( var )
{
}

//NegativeLiteral::NegativeLiteral( 
//    const string& name ) : Literal( name )
//{
//}

#endif	/* NEGATIVELITERAL_H */

