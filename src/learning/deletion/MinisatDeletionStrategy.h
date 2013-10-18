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
 * File:   MinisatDeletionStrategy.h
 * Author: Carmine Dodaro
 *
 * Created on 8 August 2013, 15.27
 */

#ifndef MINISATDELETIONSTRATEGY_H
#define	MINISATDELETIONSTRATEGY_H

#include "DeletionStrategy.h"

class MinisatDeletionStrategy : public DeletionStrategy
{
    public:
        inline MinisatDeletionStrategy();
        virtual void onRestart();
        
    protected:
        virtual bool onLearningProtected( Solver& solver );        
        
    private:
        double learnedSizeFactor;
        double learnedSizeIncrement;
        double maxLearned;
};

MinisatDeletionStrategy::MinisatDeletionStrategy() : DeletionStrategy(), learnedSizeFactor( ( double ) 1 / ( double) 3 ), learnedSizeIncrement( 1.1 ), maxLearned( 0.0 )
{    
}

#endif	/* MINISATDELETIONSTRATEGY_H */
