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

#ifndef HCCOMPONENT_H
#define	HCCOMPONENT_H

#include "PostPropagator.h"
#include "Literal.h"
#include "Solver.h"
#include "outputBuilders/OutputBuilder.h"
#include "outputBuilders/NoopOutputBuilder.h"

class Clause;
class GUSData;
class Learning;
class Rule;

class HCComponent : public PostPropagator
{
    public:
        HCComponent( vector< GUSData* >& gusData_, Solver& s ) : PostPropagator(), id( 0 ), gusData( gusData_ ), solver( s ), numberOfCalls( 0 ), hasToTestModel( false )
        {
            outputBuilder = new NoopOutputBuilder();
            checker.setOutputBuilder( outputBuilder );
        }
        virtual ~HCComponent();

        virtual bool onLiteralFalse( Literal ) = 0;
        virtual Clause* getClauseToPropagate( Learning& ) = 0;
        virtual void reset() = 0;
        
        void computeReasonForUnfoundedAtom( Var v, Learning& learning );
        
        inline void addHCVariable( Var v ) { hcVariables.push_back( v ); addHCVariableProtected( v ); }
        inline Var getVariable( unsigned int pos ) const { assert( pos < hcVariables.size() ); return hcVariables[ pos ]; }        
        inline unsigned int size() const { return hcVariables.size(); }
                
        virtual void processRule( Rule* rule ) = 0;
        virtual void processComponentBeforeStarting() = 0;
        
        void setId( unsigned int i ) { id = i; }
        unsigned int getId() const { return id; }                
        
    protected:
        virtual void addHCVariableProtected( Var v ) = 0;
        void createDefiningRules( Rule* rule, Clause* clause );
        
        GUSData& getGUSData( Var v ) { assert( v < gusData.size() ); return *( gusData[ v ] ); }  
        inline bool sameComponent( Var v ) const { return solver.getHCComponent( v ) == this; }
        inline bool isInUnfoundedSet( Var v ) { assert_msg( v < inUnfoundedSet.size(), "v = " << v << "; inUnfoundedSet.size() = " << inUnfoundedSet.size() ); return inUnfoundedSet[ v ] == numberOfCalls; }
        inline void setInUnfoundedSet( Var v ) { assert_msg( v < inUnfoundedSet.size(), "v = " << v << "; inUnfoundedSet.size() = " << inUnfoundedSet.size() ); inUnfoundedSet[ v ] = numberOfCalls; }
        inline void attachLiterals( Literal lit );
        inline void setHasToTestModel( bool b ) { hasToTestModel = b; }
    
        unsigned int id;
        vector< GUSData* >& gusData;
        vector< Var > hcVariables;
        Solver& solver;
        Solver checker;
        Vector< unsigned int > inUnfoundedSet;
        unsigned int numberOfCalls;
        Vector< Var > unfoundedSet;
        Vector< Literal > unfoundedSetCandidates;        
        bool hasToTestModel;
        OutputBuilder* outputBuilder;
        
    private:
        vector< Clause* > toDelete;
};

void
HCComponent::attachLiterals(
    Literal lit )
{
    solver.setFrozen( lit.getVariable() );
    solver.addPostPropagator( lit, this );
    solver.addPostPropagator( lit.getOppositeLiteral(), this );
}

#endif