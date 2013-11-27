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

#ifndef SOLVER_H
#define	SOLVER_H

#include <cassert>
#include <vector>
using namespace std;

#include "Clause.h"
#include "Variable.h"
#include "Variables.h"
#include "Literal.h"
#include "util/Options.h"
#include "util/Trace.h"
#include "WaspRule.h"
#include "stl/List.h"
#include "stl/UnorderedSet.h"
#include "Learning.h"
#include "outputBuilders/OutputBuilder.h"
#include "Heuristic.h"
#include "util/Assert.h"
#include "Satelite.h"

class Solver
{
    public:
        inline Solver();
        ~Solver();
        
        inline void init();
        bool solve();
        void propagate( Variable* variable );
        void propagateAtLevelZero( Variable* variable );
        void propagateAtLevelZeroSatelite( Variable* variable );

        inline bool preprocessing();
        inline void attachWatches();
        
        inline void addVariable( const string& name );
        inline void addVariable();        
        
        inline bool addClause( Clause* clause );
        inline void addLearnedClause( Clause* learnedClause );
        bool addClauseFromModelAndRestart();
        
        inline Literal getLiteral( int lit );
        
        inline Variable* getNextVariableToPropagate();
        inline bool hasNextVariableToPropagate() const;        
        
        inline Literal getOppositeLiteralFromLastAssignedVariable();
        inline bool hasNextAssignedVariable() const;
        inline void startIterationOnAssignedVariable();

        inline unsigned int getCurrentDecisionLevel();
        inline void incrementCurrentDecisionLevel();
        
        inline void assignLiteral( Literal literal );
        inline void assignLiteral( Clause* implicant );
        
        inline bool propagateLiteralAsDeterministicConsequence( Literal literal );
        inline bool propagateLiteralAsDeterministicConsequenceSatelite( Literal literal );
        
        inline void onLearningClause( Literal literalToPropagate, Clause* learnedClause, unsigned int backjumpingLevel );
        inline void onLearningUnaryClause( Literal literalToPropagate, Clause* learnedClause );        
        inline void doRestart();        
        
        inline unsigned int numberOfClauses();
        inline unsigned int numberOfLearnedClauses();        
        inline unsigned int numberOfAssignedLiterals();
        inline unsigned int numberOfVariables();
        inline unsigned int numberOfAuxVariables();
        
        inline void setAChoice( Literal choice );        
        
        inline void analyzeConflict();
        inline void clearConflictStatus();
        inline void chooseLiteral();
        inline bool conflictDetected();
        inline void foundIncoherence();
        inline bool hasUndefinedLiterals();
        inline Variable* getFirstUndefined() { return variables.getFirstUndefined(); }
        inline Variable* getNextUndefined( Variable* v ) { return variables.getNextUndefined( v ); }
        inline void printAnswerSet();        
        
        void unroll( unsigned int level );
        inline void unrollOne();
        inline void unrollLastVariable();
        
        /* OPTIONS */
        inline void setOutputBuilder( OutputBuilder* value );
        inline void setHeuristic( Heuristic* value );
        
        typedef vector< Clause* >::iterator ClauseIterator;
        typedef vector< Clause* >::reverse_iterator ClauseReverseIterator;
        typedef vector< Clause* >::const_iterator ConstClauseIterator;
        typedef vector< Clause* >::const_reverse_iterator ConstClauseReverseIterator;
        
        inline unsigned int numberOfClauses() const { return clauses.size(); }
        inline Clause* clauseAt( unsigned int i ) { assert( i < numberOfClauses() ); return clauses[ i ]; }
        
        inline ClauseIterator clauses_begin() { return clauses.begin(); }
        inline ClauseIterator clauses_end() { return clauses.end(); }
        inline ClauseReverseIterator clauses_rbegin() { return clauses.rbegin(); }
        inline ClauseReverseIterator clauses_rend() { return clauses.rend(); }
        inline ClauseIterator learnedClauses_begin() { return learnedClauses.begin(); }
        inline ClauseIterator learnedClauses_end() { return learnedClauses.end(); }
        inline ClauseReverseIterator learnedClauses_rbegin() { return learnedClauses.rbegin(); }
        inline ClauseReverseIterator learnedClauses_rend() { return learnedClauses.rend(); }
        inline ConstClauseIterator clauses_begin() const { return clauses.begin(); }
        inline ConstClauseIterator clauses_end() const { return clauses.end(); }
        inline ConstClauseReverseIterator clauses_rbegin() const { return clauses.rbegin(); }
        inline ConstClauseReverseIterator clauses_rend() const { return clauses.rend(); }
        inline ConstClauseIterator learnedClauses_begin() const { return learnedClauses.begin(); }
        inline ConstClauseIterator learnedClauses_end() const { return learnedClauses.end(); }
        inline ConstClauseReverseIterator learnedClauses_rbegin() const { return learnedClauses.rbegin(); }
        inline ConstClauseReverseIterator learnedClauses_rend() const { return learnedClauses.rend(); }

        inline void deleteLearnedClause( ClauseIterator iterator );
        inline void deleteClause( Clause* clause );
        
        void printProgram() const;
        
        inline void initClauseData( Clause* clause ) { assert( heuristic != NULL ); heuristic->initClauseData( clause ); }
//        inline Heuristic* getHeuristic() { return heuristic; }
        inline void onLiteralInvolvedInConflict( Literal l ) { assert( heuristic != NULL ); heuristic->onLiteralInvolvedInConflict( l ); }
        inline void onClauseInvolvedInConflict( Clause* learnedClause ){ assert( heuristic != NULL ); heuristic->onClauseInvolvedInConflict( learnedClause ); }
        inline void finalizeDeletion( unsigned int newVectorSize ){ learnedClauses.resize( newVectorSize ); }
        
    private:
        Solver( const Solver& ) : learning( *this )
        {
            assert( "The copy constructor has been disabled." && 0 );
        }

        unsigned int currentDecisionLevel;
        Variables variables;
        
        vector< Clause* > clauses;
        vector< Clause* > learnedClauses;
        
        vector< unsigned int > unrollVector;
        
        Literal conflictLiteral;
        Clause* conflictClause;
        
        Learning learning;
        OutputBuilder* outputBuilder;
        Heuristic* heuristic;
        
        Satelite* satelite;
        
        bool conflictAtLevelZero;
        unsigned int getNumberOfUndefined() const;
        bool allClausesSatisfied() const;
};

Solver::Solver() 
: currentDecisionLevel( 0 ),
  conflictLiteral( NULL ),
  conflictClause( NULL ),
  learning( *this ),
  outputBuilder( NULL ),
  heuristic( NULL ),
  conflictAtLevelZero( false )
{
    satelite = new Satelite( *this );
}

void
Solver::setOutputBuilder(
    OutputBuilder* value )
{
    assert( value != NULL );
    if( outputBuilder != NULL )
        delete outputBuilder;
    outputBuilder = value;
}

void
Solver::setHeuristic(
    Heuristic* value )
{
    assert( value != NULL );
    if( heuristic != NULL )
        delete heuristic;
    heuristic = value;
}

void
Solver::addVariable( 
    const string& name )
{    
    Variable* variable = new Variable( variables.numberOfVariables() + 1 );
    VariableNames::setName( variable, name );
    variables.push_back( variable );
    assert( variables.numberOfVariables() == variable->getId() );
    assert( heuristic!= NULL );
    heuristic->onNewVariable( *variable );
    learning.onNewVariable();
}

void
Solver::addVariable()
{
    Variable* variable = new Variable( variables.numberOfVariables()+1 );
    variables.push_back( variable );
    assert( variables.numberOfVariables() == variable->getId() );
    assert( heuristic != NULL );
    heuristic->onNewVariable( *variable );
    learning.onNewVariable();
}

Literal
Solver::getLiteral(
    int lit )
{
    assert( "Lit is out of range." && static_cast< unsigned >( abs( lit ) ) <= variables.numberOfVariables() && abs( lit ) > 0);
    return lit > 0 ? Literal( variables[ lit ], POSITIVE ) : Literal( variables[ -lit ], NEGATIVE );
//    if( lit > 0 )
//    {
//        Literal literal( variables[ lit ] );
//        return literal;
//    }
//    else
//    {
//        Literal literal( variables[ -lit ], false );
//        return literal;
//    }    
}

void
Solver::init()
{
    variables.init();    
    cout << COMMENT_DIMACS << " " << WASP_STRING << endl;
}

void
Solver::assignLiteral(
    Literal literal )
{
    if( !variables.assign( currentDecisionLevel, literal ) )
    {
        conflictLiteral = literal;
        conflictClause = NULL; 
    }
}

void
Solver::assignLiteral(
    Clause* implicant )
{
    assert( implicant != NULL );
    if( !variables.assign( currentDecisionLevel, implicant ) )
    {
        conflictLiteral = implicant->getAt( 0 );
        conflictClause = implicant;        
    }
}

bool
Solver::addClause(
    Clause* clause )
{
    assert( clause != NULL );
    unsigned int size = clause->size();
    if( size > 1 )
    {
//        clause->attachClause();
        clause->attachClauseToAllLiterals();
        clause->setPositionInSolver( clauses.size() );
        clauses.push_back( clause );
        return true;
    }

    if( size == 1 )
    {
        Literal literal = clause->getAt( 0 );
        if( !literal.isTrue() && !propagateLiteralAsDeterministicConsequence( literal ) )
        {
            conflictLiteral = literal;
        }
        else
        {
            delete clause;
            return true;
        }
    }

    conflictAtLevelZero = true;
    delete clause;
    return false;
}

void
Solver::addLearnedClause( 
    Clause* learnedClause )
{
    assert( learnedClause != NULL );
    learnedClause->attachClause();
    learnedClauses.push_back( learnedClause );
}

Variable*
Solver::getNextVariableToPropagate()
{
    assert( variables.hasNextVariableToPropagate() );
    return variables.getNextVariableToPropagate();
}
        
bool
Solver::hasNextVariableToPropagate() const
{
    return variables.hasNextVariableToPropagate();
}

unsigned int
Solver::getCurrentDecisionLevel()
{
    return currentDecisionLevel;
}

void
Solver::incrementCurrentDecisionLevel()
{
    currentDecisionLevel++;
    unrollVector.push_back( variables.numberOfAssignedLiterals() );
    
    assert( currentDecisionLevel == unrollVector.size() );
}

void
Solver::unrollLastVariable()
{
    variables.unrollLastVariable();
}

void
Solver::unrollOne()
{
    unroll( currentDecisionLevel - 1 );
}

void
Solver::doRestart()
{
    assert_msg( heuristic != NULL, "Heuristic unset" );
    trace( solving, 2, "Performing restart.\n" );
    heuristic->onRestart();
    unroll( 0 );
}

void
Solver::deleteLearnedClause( 
    ClauseIterator iterator )
{
    Clause* learnedClause = *iterator;
    trace_msg( solving, 4, "Deleting learned clause " << *learnedClause );
    learnedClause->detachClause();
    delete learnedClause;
//    learnedClauses.erase( iterator );
}

void
Solver::deleteClause( 
    Clause* clause )
{
    unsigned int position = clause->getPositionInSolver();
    assert_msg( position < clauses.size(), "Position " << position << " is greater than size of the clauses vector " << clauses.size() );
    assert_msg( clause == clauses[ position ], "The clause to delete " << *clause << " is not equal to the clause " << clauses[ position ] << " in position " << position  );
    trace_msg( solving, 4, "Deleting clause " << *clause );

    clauses[ position ] = clauses.back();
    trace_msg( solving, 6, "Swapping clause " << *clause << " and " << *clauses[ position ] );
    clauses[ position ]->setPositionInSolver( position );
    clauses.pop_back();
    delete clause;
}

unsigned int
Solver::numberOfClauses()
{
    return clauses.size();
}

unsigned int
Solver::numberOfLearnedClauses()
{
    return learnedClauses.size();
}

bool
Solver::conflictDetected()
{
    return conflictLiteral != Literal::null;
}

bool
Solver::hasUndefinedLiterals()
{
    return variables.numberOfAssignedLiterals() < variables.numberOfVariables();
}

void
Solver::printAnswerSet()
{
    variables.printAnswerSet( outputBuilder );
}

void
Solver::foundIncoherence()
{
    outputBuilder->onProgramIncoherent();
}

void
Solver::chooseLiteral()
{
    Literal choice = heuristic->makeAChoice();
    trace( solving, 1, "Choice: %s.\n", toString( choice ).c_str() );
    setAChoice( choice );    
}

void
Solver::analyzeConflict()
{
    Clause* learnedClause = learning.onConflict( conflictLiteral, conflictClause );
    assert( "Learned clause has not been calculated." && learnedClause != NULL );
    
    if( learnedClause->size() == 1 )
    {
        doRestart();
        assignLiteral( learnedClause->getAt( 0 ) );
        delete learnedClause;
    }
    else
    {
        //Be careful. UIP should be always in position 0.
        assert( learnedClause->getAt( 0 ).getDecisionLevel() == currentDecisionLevel );
        assert( learnedClause->getAt( 1 ).getDecisionLevel() == learnedClause->getMaxDecisionLevel( 1, learnedClause->size() ) );
        addLearnedClause( learnedClause );

        if( heuristic->hasToRestart() )
        {
            doRestart();
        }
        else
        {
            assert( learnedClause->getAt( 1 ).getDecisionLevel() != 0 );
            assert( "Backjumping level is not valid." && learnedClause->getAt( 1 ).getDecisionLevel() < currentDecisionLevel );
            trace( solving, 2, "Learned clause and backjumping to level %d.\n", learnedClause->getAt( 1 ).getDecisionLevel() );
            unroll( learnedClause->getAt( 1 ).getDecisionLevel() );    
            
            assert( "Each learned clause has to be an asserting clause." && learnedClause->getAt( 0 ) != Literal::null );
            
            assignLiteral( learnedClause );
            
            heuristic->onLearning( learnedClause );  // FIXME: this should be moved outside
        }
    }
    
    heuristic->onConflict();
    clearConflictStatus();
}

void
Solver::clearConflictStatus()
{
    conflictLiteral = Literal::null;
    conflictClause = NULL;
}

unsigned int
Solver::numberOfAssignedLiterals()
{
    return variables.numberOfAssignedLiterals();
}

unsigned int
Solver::numberOfVariables()
{
    return variables.numberOfVariables();
}

void
Solver::setAChoice(
    Literal choice )
{
    assert( choice != Literal::null );
    incrementCurrentDecisionLevel();
    assert( choice.isUndefined() );
    assignLiteral( choice );
}

Literal
Solver::getOppositeLiteralFromLastAssignedVariable()
{
    return variables.getOppositeLiteralFromLastAssignedVariable();
}

bool
Solver::hasNextAssignedVariable() const
{
    return variables.hasNextAssignedVariable();
}

void
Solver::startIterationOnAssignedVariable()
{
    variables.startIterationOnAssignedVariable();
}

bool
Solver::propagateLiteralAsDeterministicConsequence(
    Literal literal )
{
    assignLiteral( literal );
    if( conflictDetected() )
        return false;

    while( hasNextVariableToPropagate() )
    {
        Variable* variableToPropagate = getNextVariableToPropagate();
        propagateAtLevelZero( variableToPropagate );

        if( conflictDetected() )
            return false;
    }    
    assert( !conflictDetected() );

    return true;
}

bool
Solver::propagateLiteralAsDeterministicConsequenceSatelite(
    Literal literal )
{
    assignLiteral( literal );
    if( conflictDetected() )
        return false;

    while( hasNextVariableToPropagate() )
    {
        Variable* variableToPropagate = getNextVariableToPropagate();
        propagateAtLevelZeroSatelite( variableToPropagate );

        if( conflictDetected() )
            return false;
    }    
    assert( !conflictDetected() );

    return true;
}

void
Solver::attachWatches()
{
    for( ClauseIterator it = clauses.begin(); it != clauses.end(); it++ )
    {
        assert( !( *it )->hasBeenDeleted() );
        ( *it )->attachClause();
    }
}

bool
Solver::preprocessing()
{
    assert( satelite != NULL );
    return satelite->simplify();
}

//bool
//Solver::existsAuxLiteral(
//    unsigned int id ) const
//{
//    return( id < auxLiterals.size() );
//}
//
//AuxLiteral*
//Solver::getAuxLiteral(
//    unsigned int id )
//{
//    assert( existsAuxLiteral( id ) );
//    return auxLiterals[ id ];
//}

//unsigned int
//Solver::numberOfAuxVariables()
//{
//    return auxLiterals.size();
//}

#endif	/* SOLVER_H */

