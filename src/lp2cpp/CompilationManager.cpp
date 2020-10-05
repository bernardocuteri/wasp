/*
 *
 *  Copyright 2016 Bernardo Cuteri, Alessandro De Rosis, Francesco Ricca.
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

#include "language/Rule.h"
#include "CompilationManager.h"
#include "utils/Indentation.h"
#include "utils/SharedFunctions.h"
#include <ostream>
#include <fstream>
#include <cassert>
#include <set>
#include <list>
#include <unordered_map>
#include "DLV2libs/input/InputDirector.h"
#include "parsing/AspCore2ProgramBuilder.h"
#include "language/ArithmeticExpression.h"
#include "language/ArithmeticRelation.h"
#include "language/ArithmeticRelationWithAggregate.h"
#include "datastructures/BoundAnnotatedLiteral.h"
#include <algorithm>
using namespace std;

const std::string tab = "    ";

CompilationManager::CompilationManager(int mode) : out(&std::cout), ind(0) {
    this->mode = mode;
    
}

void CompilationManager::setOutStream(std::ostream* outputTarget) {
    this->out = outputTarget;
}

void CompilationManager::lp2cpp() {
    generateStratifiedCompilableProgram(builder->getProgram(), builder);
    delete builder;
}

void CompilationManager::loadProgram(const std::string& filename) {
    DLV2::InputDirector director;
    builder = new AspCore2ProgramBuilder();
    director.configureBuilder(builder);
    std::vector<const char*> fileNames;
    fileNames.push_back(filename.c_str());
    director.parse(fileNames);
    bodyPredicates = builder->getProgram().getBodyPredicates();
    headPredicates = builder->getProgram().getHeadPredicates();
    
}

void CompilationManager::initRuleBoundVariables(std::unordered_set<std::string> & ruleBoundVariables, const BoundAnnotatedLiteral & lit, const aspc::Atom & head, bool printVariablesDeclaration) {
    unsigned counter = 0;
    for (unsigned i = 0; i < lit.getBoundIndexes().size(); i++) {
        if (lit.getBoundIndexes().at(i) && head.isVariableTermAt(i)) {
            if (printVariablesDeclaration && !ruleBoundVariables.count(head.getTermAt(i))) {
                *out << ind << "int " << head.getTermAt(i) << " = " << "lit[" << counter << "];\n";
            }
            ruleBoundVariables.insert(head.getTermAt(i));
            counter++;
        }
    }
}

bool possiblyAddToProcessLiteral(const BoundAnnotatedLiteral & lit, list<BoundAnnotatedLiteral> & toProcessLiterals,
        list<BoundAnnotatedLiteral> & processedLiterals) {
    if (find(toProcessLiterals.begin(), toProcessLiterals.end(), lit) == toProcessLiterals.end()) {
        if (find(processedLiterals.begin(), processedLiterals.end(), lit) == processedLiterals.end()) {
            toProcessLiterals.push_back(lit);
            return true;
        }
    }
    return false;
}

void CompilationManager::writeNegativeReasonsFunctions(const aspc::Program & program, const BoundAnnotatedLiteral & lit,
        list<BoundAnnotatedLiteral> & toProcessLiterals, list<BoundAnnotatedLiteral> & processedLiterals, std::unordered_map <std::string, std::string> & functionsMap) {

    if (lit.isNegated()) {
        *out << ind++ << "void explain_" << lit.getStringRep() << "(const std::vector<int> & lit, std::unordered_set<std::string> & open_set, std::vector<const Tuple *> & output){\n";
        if (lit.isGround()) {

            functionsMap[lit.getPredicateName()] = "explain_" + lit.getStringRep();
        }
        if (modelGeneratorPredicates.count(lit.getPredicateName())) {
            if (lit.isGround()) {
                *out << ind << "const auto& find = neg_w" << lit.getPredicateName() << ".find(Tuple(lit, &_" << lit.getPredicateName() << "));\n";
                *out << ind++ << "if(find) {\n";
                *out << ind << "output.push_back(find);\n";
                *out << --ind << "}\n";
            } else {
                //iterate over map of negatives
                std::string mapName = "false_p" + lit.getPredicateName() + "_";
                for (unsigned i = 0; i < lit.getBoundIndexes().size(); i++) {
                    if (lit.getBoundIndexes()[i]) {
                        mapName += std::to_string(i) + "_";
                    }
                }
                *out << ind++ << "for(const Tuple* reason: " << mapName << ".getValues(lit)) {\n";
                *out << ind << "output.push_back(reason);\n";
                *out << --ind << "}\n";
            }
            *out << --ind << "}\n";
            return;

        }

        *out << ind << "std::string canonicalRep = _" << lit.getPredicateName() << ";\n";
        unsigned counter = 0;
        for (unsigned i = 0; i < lit.getBoundIndexes().size(); i++) {
            if (i > 0) {
                *out << ind << "canonicalRep += \",\";\n";
            }
            if (lit.getBoundIndexes()[i]) {
                *out << ind << "canonicalRep += std::to_string(lit[" << counter++ << "]);\n";
            } else {
                *out << ind << "canonicalRep += \"_\";\n";
            }
        }

        *out << ind++ << "if(open_set.find(canonicalRep)!=open_set.end()){\n";
        *out << ind << "return;\n";
        *out << --ind << "}\n";
        *out << ind << "open_set.insert(canonicalRep);\n";


    }

    for (const aspc::Rule & r : program.getRules()) {
        //TODO runtime unification
        if (!r.isConstraint() && lit.getPredicateName() == r.getHead()[0].getPredicateName()) {
            if (lit.isNegated()) {
                *out << ind++ << "{\n";
            }
            unsigned forCounter = 0;
            std::unordered_set<std::string> ruleBoundVariables;
            const aspc::Atom & head = r.getHead()[0];
            initRuleBoundVariables(ruleBoundVariables, lit, head, lit.isNegated());
            std::vector<const aspc::Formula*> orderedFormulas = r.getOrderedBodyForReasons(ruleBoundVariables);
            for (unsigned i = 0; i < r.getBodySize(); i++) {
                const aspc::Formula * f = orderedFormulas[i];
                if (f -> isLiteral()) {
                    const aspc::Literal * bodyLit = (const aspc::Literal *) f;
                    if (lit.isNegated()) {
                        if (!bodyLit->isNegated()) {
                            std::vector<bool> coveredMask;
                            bodyLit->getAtom().getBoundTermsMask(ruleBoundVariables, coveredMask);
                            BoundAnnotatedLiteral bodyBoundLit = BoundAnnotatedLiteral(bodyLit->getPredicateName(), coveredMask, true);
                            possiblyAddToProcessLiteral(bodyBoundLit, toProcessLiterals, processedLiterals);

                            *out << ind << "explain_" << bodyBoundLit.getStringRep() << "({";
                            printLiteralTuple(bodyLit, coveredMask);

                            *out << "}, open_set, output);\n";
                            if (i < r.getBodySize() - 1) {
                                std::string mapVariableName = bodyLit->getPredicateName() + "_";
                                for (unsigned index = 0; index < coveredMask.size(); index++) {
                                    if (bodyBoundLit.getBoundIndexes()[index]) {
                                        mapVariableName += std::to_string(index) + "_";
                                    }
                                }
                                if (bodyBoundLit.isGround()) {
                                    *out << ind++ << "if (w" << bodyLit->getPredicateName() << ".find({";
                                    printLiteralTuple(bodyLit);
                                    *out << "})){\n";
                                } else {
                                    *out << ind++ << "for(const Tuple* tuple" << i << ": p" << mapVariableName << ".getValues({";
                                    printLiteralTuple(bodyLit, coveredMask);
                                    *out << "})){\n";
                                    for (unsigned index = 0; index < coveredMask.size(); index++) {
                                        if (!coveredMask[index]) {
                                            *out << ind << "int " << bodyLit->getTermAt(index) << " = " << "(*tuple" << i << ")[" << index << "];\n";
                                        }
                                    }
                                }

                                forCounter++;
                            }

                            //declareDataStructuresForReasonsOfNegative(program, *bodyLit, true, ruleBoundVariables, openSet);
                        } else {
                            BoundAnnotatedLiteral bodyBoundLit = BoundAnnotatedLiteral(bodyLit->getPredicateName(), std::vector<bool>(bodyLit->getAriety(), true), false);
                            possiblyAddToProcessLiteral(bodyBoundLit, toProcessLiterals, processedLiterals);
                            *out << ind << "const auto & it = w" << bodyLit->getPredicateName() << ".find({";
                            for (unsigned term = 0; term < bodyLit->getAriety(); term++) {
                                if (term > 0) {
                                    *out << ",";
                                }
                                if (!bodyLit->isVariableTermAt(term)) {
                                    *out << "ConstantsManager::getInstance().mapConstant(\"" << escapeDoubleQuotes(bodyLit->getTermAt(term)) << "\")";
                                } else {
                                    *out << bodyLit->getTermAt(term);
                                }
                            }
                            *out << "});\n";
                            *out << ind++ << "if(it) {\n";
                            *out << ind << "explainPositiveLiteral(it, open_set, output);\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "else {\n";

                            forCounter++;
                        }
                    } else {
                        if (bodyLit->isNegated()) {
                            BoundAnnotatedLiteral bodyBoundLit = BoundAnnotatedLiteral(bodyLit->getPredicateName(), std::vector<bool>(bodyLit->getAriety(), true), true);
                            possiblyAddToProcessLiteral(bodyBoundLit, toProcessLiterals, processedLiterals);
                        }
                    }
                    for (unsigned t = 0; t < bodyLit->getAriety(); t++) {
                        if (bodyLit -> isVariableTermAt(t)) {
                            ruleBoundVariables.insert(bodyLit->getTermAt(t));
                        }
                    }
                } else {
                    //account value invention relations
                    if (lit.isNegated()) {
                        const aspc::ArithmeticRelation * relation = (const aspc::ArithmeticRelation *) f;
                        if (f->isBoundedValueAssignment(ruleBoundVariables)) {
                            *out << ind << "int " << relation->getAssignmentStringRep(ruleBoundVariables) << ";\n";
                            ruleBoundVariables.insert(relation->getAssignedVariable(ruleBoundVariables));
                        } else {
                            *out << ind++ << "if(" << relation->getStringRep() << ") {\n";
                            forCounter++;
                        }
                    }

                }
            }
            for (unsigned i = 0; i < forCounter; i++) {
                *out << --ind << "}\n";
            }
            if (lit.isNegated()) {
                *out << --ind << "}\n";
            }
        }
    }
    if (lit.isNegated()) {
        *out << ind << "open_set.erase(canonicalRep);\n";
        *out << --ind << "}\n";
    }
}

void CompilationManager::writeNegativeReasonsFunctionsPrototypes(const aspc::Program & program, const BoundAnnotatedLiteral & lit,
        list<BoundAnnotatedLiteral> & toProcessLiterals, list<BoundAnnotatedLiteral> & processedLiterals, std::unordered_map <std::string, std::string> & functionsMap) {


    if (lit.isNegated()) {
        *out << ind << "void explain_" << lit.getStringRep() << "(const std::vector<int> &, std::unordered_set<std::string> &, std::vector<const Tuple *> &);\n";
        if (modelGeneratorPredicates.count(lit.getPredicateName())) {
            return;
        }
    }
    for (const aspc::Rule & r : program.getRules()) {
        //TODO runtime unification
        if (!r.isConstraint() && lit.getPredicateName() == r.getHead()[0].getPredicateName()) {
            std::unordered_set<std::string> ruleBoundVariables;
            const aspc::Atom & head = r.getHead()[0];
            initRuleBoundVariables(ruleBoundVariables, lit, head, false);
            std::vector<const aspc::Formula*> orderedFormulas = r.getOrderedBodyForReasons(ruleBoundVariables);
            for (unsigned i = 0; i < r.getBodySize(); i++) {
                const aspc::Formula * f = orderedFormulas[i];
                if (f -> isLiteral()) {
                    const aspc::Literal * bodyLit = (const aspc::Literal *) f;
                    if (lit.isNegated()) {
                        if (!bodyLit->isNegated()) {
                            std::vector<bool> coveredMask;
                            bodyLit->getAtom().getBoundTermsMask(ruleBoundVariables, coveredMask);
                            BoundAnnotatedLiteral bodyBoundLit = BoundAnnotatedLiteral(bodyLit->getPredicateName(), coveredMask, true);
                            possiblyAddToProcessLiteral(bodyBoundLit, toProcessLiterals, processedLiterals);
                        } else {
                            BoundAnnotatedLiteral bodyBoundLit = BoundAnnotatedLiteral(bodyLit->getPredicateName(), std::vector<bool>(bodyLit->getAriety(), true), false);
                            possiblyAddToProcessLiteral(bodyBoundLit, toProcessLiterals, processedLiterals);
                        }
                    } else {
                        if (bodyLit->isNegated()) {
                            BoundAnnotatedLiteral bodyBoundLit = BoundAnnotatedLiteral(bodyLit->getPredicateName(), std::vector<bool>(bodyLit->getAriety(), true), true);
                            possiblyAddToProcessLiteral(bodyBoundLit, toProcessLiterals, processedLiterals);
                        }
                    }
                    for (unsigned t = 0; t < bodyLit->getAriety(); t++) {
                        if (bodyLit -> isVariableTermAt(t)) {
                            ruleBoundVariables.insert(bodyLit->getTermAt(t));
                        }
                    }
                }
            }

        }
    }
}

BoundAnnotatedLiteral literalToBoundAnnotatedLiteral(const aspc::Literal & lit) {

    return BoundAnnotatedLiteral(lit.getPredicateName(), std::vector<bool>(lit.getAriety(), true), lit.isNegated());

}

BoundAnnotatedLiteral literalToBoundAnnotatedLiteral(const aspc::Literal & lit, std::unordered_set<std::string> & boundVariables) {

    BoundAnnotatedLiteral boundAnnotatedLiteral = BoundAnnotatedLiteral(lit.getPredicateName(), lit.isNegated());
    for (unsigned i = 0; i < lit.getAriety(); i++) {
        if (lit.isVariableTermAt(i) && boundVariables.count(lit.getTermAt(i))) {
            boundAnnotatedLiteral.addBoundInformation(true);
        } else {
            boundAnnotatedLiteral.addBoundInformation(false);
        }
    }
    return boundAnnotatedLiteral;

}

void CompilationManager::writeNegativeReasonsFunctionsPrototypes(aspc::Program & program) {
    *out << ind << "//printing functions prototypes for reasons of negative literals;\n";

    list<BoundAnnotatedLiteral> toProcessLiterals;
    list<BoundAnnotatedLiteral> processedLiterals;
    std::unordered_map <std::string, std::string> functionsMap;

    for (const aspc::Rule & r : program.getRules()) {
        if (r.isConstraint()) {
            for (const aspc::Formula * f : r.getFormulas()) {
                if (f->isLiteral()) {
                    const aspc::Literal & lit = (const aspc::Literal &) * f;
                    possiblyAddToProcessLiteral(literalToBoundAnnotatedLiteral(lit), toProcessLiterals, processedLiterals);
                }
            }
        }
    }
    while (!toProcessLiterals.empty()) {
        BoundAnnotatedLiteral next = toProcessLiterals.back();
        toProcessLiterals.pop_back();
        processedLiterals.push_back(next);
        writeNegativeReasonsFunctionsPrototypes(program, next, toProcessLiterals, processedLiterals, functionsMap);
    }
}

void CompilationManager::writeNegativeReasonsFunctions(aspc::Program & program) {

    *out << ind << "//printing functions for reasons of negative literals;\n";

    list<BoundAnnotatedLiteral> toProcessLiterals;
    list<BoundAnnotatedLiteral> processedLiterals;
    std::unordered_map <std::string, std::string> functionsMap;

    for (const aspc::Rule & r : program.getRules()) {
        if (r.isConstraint()) {
            for (const aspc::Formula * f : r.getFormulas()) {
                if (f->isLiteral()) {
                    const aspc::Literal & lit = (const aspc::Literal &) * f;
                    possiblyAddToProcessLiteral(literalToBoundAnnotatedLiteral(lit), toProcessLiterals, processedLiterals);
                }
            }
        }
    }
    while (!toProcessLiterals.empty()) {
        BoundAnnotatedLiteral next = toProcessLiterals.back();
        toProcessLiterals.pop_back();
        processedLiterals.push_back(next);
        writeNegativeReasonsFunctions(program, next, toProcessLiterals, processedLiterals, functionsMap);
    }

    *out << ind++ << "void createFunctionsMap() {\n";
    for (const auto & entry : functionsMap) {
        *out << ind << "explainNegativeFunctionsMap[&_" << entry.first << "] = " << entry.second << ";\n";
    }
    *out << --ind << "}\n";
}
void CompilationManager::checkExistsShareVariableMap(int ruleId, int aggrIndex,std::string& sharedVariables,bool create){

    //*out << ind << "std::cout<<\"check exists shared value tuple\"<<std::endl;\n";
    
    int countVar=0;
    std::string indexesToString="";
    for(unsigned varIndex : aggregateVariablesIndex[std::to_string(ruleId)+":"+std::to_string(aggrIndex)]){
        indexesToString+=std::to_string(varIndex);
        if(countVar < aggregateVariablesIndex[std::to_string(ruleId)+":"+std::to_string(aggrIndex)].size()-1)
            indexesToString+=",";
        countVar++;
    }
    *out << ind << "Tuple sharedTuple(std::vector<int>({"<<sharedVariables<<"}));\n";
    if(create){
        *out << ind++ << "if(!sharedVariables_"<<ruleId<<"_ToAggregate_"<<aggrIndex<<".count(sharedTuple)){\n";
            *out << ind << "sharedVariables_"<<ruleId<<"_ToAggregate_"<<aggrIndex
                    <<"[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({"<<indexesToString
                    <<"})),new AuxMap(std::vector<unsigned>({"<<indexesToString
                    <<"})));\n";
            /**out << ind << "string key = \"sharedVariablesMaps_"<<ruleId<<":"<<aggrIndex<<"_\";\n";
            *out << ind++ << "for(int v : sharedTuple)\n";
            *out << ind-- << "key+=std::to_string(v)+\"_\"";
            *out << ind << "predicateToAuxiliaryMaps[&key].push_backsharedVariables_"<<ruleId<<"_ToAggregate_"<<aggrIndex<<"[sharedTuple].first*/
    }else{
        *out << ind++ << "if(sharedVariables_"<<ruleId<<"_ToAggregate_"<<aggrIndex<<".count(sharedTuple)!=0){\n";
    }
}
void CompilationManager::addJoinTupleToSharedVariablesMap(int ruleId, int aggrIndex,std::string auxMapIndex,std::string joinTupleName){
    
    for (std::string structureName : sharedVariablesMapForAggregateBody[joinTupleName]){
        unsigned pos = structureName.find("_");
        if(pos!=std::string::npos)
            structureName=structureName.substr(pos+1,structureName.length());
        pos = structureName.find("_");
        std::string aggrIndentifier =structureName.substr(0,pos)+":";
        ruleId = std::stoi(structureName.substr(0,pos));

        structureName=structureName.substr(pos+1,structureName.length());
        pos = structureName.find("_");
        aggrIndentifier+=structureName.substr(pos+1,structureName.length());
        std::string sharedVariables = sharedVariablesMap[aggrIndentifier];
        aggrIndex = std::stoi(structureName.substr(pos+1,structureName.length()));

        if(sharedVariables!=""){
                *out << ind++ << "{\n";
                    checkExistsShareVariableMap(ruleId,aggrIndex,sharedVariables,false);
                    *out << ind << "auto joinTuples = sharedVariables_"<<ruleId<<"_ToAggregate_"<<aggrIndex<<"[sharedTuple];\n";
                    *out << ind << "joinTuples->"<<auxMapIndex<<"->insert2(*insertResult.first);\n";
                    *out << --ind << "}\n";
                *out << --ind << "}\n";
        
        }
    }
    
}
void CompilationManager::updateSums(std::string mapName,char op){ 
    *out << ind << "sum_it = "<<mapName<<".find(tuple.getPredicateName());\n";
    *out << ind ++ << "if (sum_it == "<<mapName<<".end()) {\n";
    *out << --ind; 
    *out << ind++ <<"}else{\n";
    *out << ind << "int val = tuple.at(aggregatePredicatesAndVars[tuple.getPredicateName()][0]);\n";
    if(op == '-')
        *out << ind++ <<"if("<< mapName<<"[tuple.getPredicateName()]>0){\n";
    *out << ind << mapName<<"[tuple.getPredicateName()]"<<op<<"=val;\n";
    if(op == '-')
        *out << --ind <<"}\n";
    *out << --ind << "}\n";
    return;
}
void CompilationManager::printVars(const aspc::Literal& li,std::string tupleName,std::unordered_set<std::string> & boundVars)const{
    for (unsigned tiIndex = 0; tiIndex < li.getTerms().size(); tiIndex++) {
        if(li.isVariableTermAt(tiIndex))
            if(!boundVars.count(li.getTermAt(tiIndex))){
                *out << ind << "int "<<li.getTermAt(tiIndex) << " = "<<tupleName<<"at("<<tiIndex<<");\n";
                boundVars.insert(li.getTermAt(tiIndex));
            }
    }
}
bool CompilationManager::checkTupleFormat(const aspc::Literal& li,std::string buildIndex,bool tuplePointer){
    
    std::string point = tuplePointer ? "->":".";
    bool checkVariablesEquals=false;
    for(unsigned i=0;i<li.getAriety();i++)
        for(unsigned j=i+1;j<li.getAriety();j++)
            if(li.isVariableTermAt(i) && li.isVariableTermAt(j) && li.getTermAt(i)==li.getTermAt(j)){
                if(!checkVariablesEquals){
                    *out << ind++ << "if(tuple"<<buildIndex<<point<<"at("<<i<<") == tuple"<<buildIndex<<point<<"at("<<j<<")";
                    checkVariablesEquals=true;
                }else
                    *out << "&& tuple"<<buildIndex<<point<<"at("<<i<<") == tuple"<<buildIndex<<point<<"at("<<j<<")";
            }
    if(checkVariablesEquals){
        *out << "){\n";
    }
    return checkVariablesEquals;
}
void CompilationManager::builJoinTupleStartingFromLiteral(int ruleId,int aggrIndex,const aspc::Literal& li,const aspc::ArithmeticRelationWithAggregate& aggr,int index,bool isLiteralTrue,bool negatedLiteral){

    
    std::unordered_set<std::string> boundVariables;
    int buildIndex=0;

    //stampo le variabili non bound dello starter
    printVars(li,"tuple.",boundVariables);

    li.addVariablesToSet(boundVariables);
    std::string aggrIdentifier = std::to_string(ruleId)+":"+std::to_string(aggrIndex);
    
    //contiene tutte le variabili del join interno all'aggregato
    std::string joinTuple="";
    
    std::string joinTupleName = aggregateLiteralToPredicateWSet[li.getPredicateName()+"_"+aggrIdentifier];
    int pars = 0;

    //controllo che rispetti le uguaglianze imposte nel letterale
    if(checkTupleFormat(li,"",false))
        pars++;

    bool first = false;
    *out << ind << "bool buildUndef=false;\n";
    *out << ind << "bool buildFalse=false;\n";
    
    for(const aspc::Literal& buildLit: aggr.getAggregate().getAggregateLiterals()){
        
        std::string mapVariableName = buildLit.getPredicateName() + "_";

        for (unsigned tiIndex = 0; tiIndex < buildLit.getTerms().size(); tiIndex++) {

            joinTuple=joinTuple+buildLit.getTermAt(tiIndex)+",";
            if((buildLit.isVariableTermAt(tiIndex) && boundVariables.count(buildLit.getTermAt(tiIndex)))|| !buildLit.isVariableTermAt(tiIndex))
                mapVariableName += std::to_string(tiIndex) + "_";

            /*if(!buildLit.isBoundedLiteral(boundVariables) || index==buildIndex){
                if ((buildLit.isVariableTermAt(tiIndex) && boundVariables.count(buildLit.getTermAt(tiIndex)))!=0 || !buildLit.isVariableTermAt(tiIndex)) {
                    mapVariableName += std::to_string(tiIndex) + "_";
                }
            }else if(buildLit.isBoundedLiteral(boundVariables)){
                mapVariableName += std::to_string(tiIndex) + "_";
            }*/
        }


        if(index!=buildIndex){
            
            if(isLiteralTrue){
                if(!negatedLiteral){
                    *out << ind << "const std::vector<const Tuple*>& tuples"<<buildIndex<<" = p" << mapVariableName << ".getValues({";
                            printLiteralTuple(&buildLit,boundVariables);
                    *out <<"});\n";
                    *out << ind << "const std::vector<const Tuple*>& tuplesU"<<buildIndex<<" = u" << mapVariableName << ".getValues({";
                            printLiteralTuple(&buildLit,boundVariables);
                    *out <<"});\n";
                    
                    //*out << ind << "std::cout<<\"Undefined matching size: \"<<tuplesU"<<buildIndex<<".size()<<std::endl;\n";
                    //*out << ind << "std::cout<<\"True matching size: \"<<tuples"<<buildIndex<<".size()<<std::endl;\n";
                }
                else{

                    *out << ind << "const std::vector<const Tuple*>& tuplesU"<<buildIndex<<" = u" << mapVariableName << ".getValues({";
                        printLiteralTuple(&buildLit,boundVariables);
                    *out <<"});\n";
                    *out << ind << "const std::vector<const Tuple*>& tuples"<<buildIndex<<" = p" << mapVariableName << ".getValues({";
                        printLiteralTuple(&buildLit,boundVariables);
                    *out <<"});\n";
                    if(aggr.isNegated()){
                        *out << ind << "const std::vector<const Tuple*>& tuplesF"<<buildIndex<<" = f" << mapVariableName << ".getValues({";
                            printLiteralTuple(&buildLit,boundVariables);
                        *out <<"});\n";
                    }
                    
                }

                if(aggr.isNegated() && negatedLiteral){
                    *out << ind++ << "for(int i=0;i<tuples"<<buildIndex<<".size()+tuplesU"<<buildIndex<<".size()+tuplesF"<<buildIndex<<".size();i++){\n";
                        if(!first){
                            first=true;
                            *out << ind << "buildUndef=false;\n";
                            *out << ind << "buildFalse=false;\n";
                        }
                        *out << ind << "const Tuple* tuple"<<buildIndex<<";\n";
                        
                        *out << ind++ << "if(i<tuples"<<buildIndex<<".size())\n";
                        
                            *out << ind << "tuple"<<buildIndex<<"=tuples"<<buildIndex<<"[i];\n";
                        *out << --ind << "else if (i < tuplesU"<<buildIndex<<".size()+tuples"<<buildIndex<<".size()){\n";
                            *out << ++ind << "buildUndef=true;\n";
                            *out << ind << "tuple"<<buildIndex<<"=tuplesU"<<buildIndex<<"[i-tuples"<<buildIndex<<".size()];\n";
                        *out << --ind << "}else{\n";
                            *out << ++ind << "tuple"<<buildIndex<<"=tuplesF"<<buildIndex<<"[i-tuples"<<buildIndex<<".size()-tuplesU"<<buildIndex<<".size()];\n";
                            *out << ind << "buildFalse=true;\n";
                        *out << --ind << "}\n";
                }else{
                    *out << ind++ << "for(int i=0;i<tuples"<<buildIndex<<".size()+tuplesU"<<buildIndex<<".size();i++){\n";
                        if(!first){
                            first=true;
                            *out << ind << "bool buildUndef=false;\n";
                        }
                        *out << ind << "const Tuple* tuple"<<buildIndex<<";\n";
                        
                        *out << ind++ << "if(i<tuples"<<buildIndex<<".size())\n";
                        
                            *out << ind << "tuple"<<buildIndex<<"=tuples"<<buildIndex<<"[i];\n";
                        *out << --ind << "else{\n";
                            *out << ++ind << "buildUndef=true;\n";
                            *out << ind << "tuple"<<buildIndex<<"=tuplesU"<<buildIndex<<"[i-tuples"<<buildIndex<<".size()];\n";
                        *out << --ind << "}\n";
                }
                
                if(checkTupleFormat(buildLit,std::to_string(buildIndex),true))
                    pars++;  
            }else{
                *out << ind << "const std::vector<const Tuple*>& tuplesU"<<buildIndex<<" = p" << mapVariableName << ".getValues({";
                    printLiteralTuple(&buildLit,boundVariables);
                *out <<"});\n";
                *out << ind << "const std::vector<const Tuple*>& tuples"<<buildIndex<<" = u" << mapVariableName << ".getValues({";
                    printLiteralTuple(&buildLit,boundVariables);
                *out <<"});\n";
                *out << ind++ << "for(int i=0;i<tuples"<<buildIndex<<".size()+tuplesU"<<buildIndex<<".size();i++){\n";
                    *out << ind << "const Tuple* tuple"<<buildIndex<<";\n";
                    
                    *out << ind++ << "if(i<tuples"<<buildIndex<<".size())\n";
                        *out << ind << "tuple"<<buildIndex<<"=tuples"<<buildIndex<<"[i];\n";
                    *out << --ind << "else{\n";
                        *out << ++ind << "tuple"<<buildIndex<<"=tuplesU"<<buildIndex<<"[i-tuples"<<buildIndex<<".size()];\n";
                    *out << --ind << "}\n";
                    
                    if(checkTupleFormat(buildLit,std::to_string(buildIndex),true))
                        pars++;    

            }
            
            printVars(buildLit,"tuple"+std::to_string(buildIndex)+"->",boundVariables);
            
        }
        if(buildIndex==aggr.getAggregate().getAggregateLiterals().size()-1){

            //*out << ind << "std::cout<<\"Building new jointuple\"<<std::endl;\n"<<ind<<"t.print();\n";        
            if(isLiteralTrue){
                if(!negatedLiteral){
                    //*out << ind << "std::cout<<\"new true join tuple \";t.print();std::cout<<std::endl;\n";
                    *out << ind << "Tuple t(std::vector<int>({"<<joinTuple.substr(0,joinTuple.size()-1)<<"}),&_"<<joinTupleName<<");\n";

                    *out << ind++ << "if(!buildUndef";

                    if(aggr.isNegated()){
                        *out << " && !buildFalse";
                    }
                    *out << "){\n";

                        *out << ind++ << "if(w"<<joinTupleName<<".find(t)==NULL){\n";
                            
                            *out << ind << "const auto& insertResult = w" << joinTupleName <<".insert(Tuple(t));\n";
                            *out << ind++ << "if (insertResult.second) {\n";
                                *out << ind++ << "for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_"<<joinTupleName<<"]){\n";
                                    *out << ind << "auxMap -> insert2(*insertResult.first);\n";
                                *out << --ind << "}\n";
                                addJoinTupleToSharedVariablesMap(ruleId,aggrIndex,"first",joinTupleName);
                            
                                *out << ind << "u" << joinTupleName <<".erase(t);\n";
                            *out << --ind << "}\n";
                        *out << --ind << "}else{\n";
                            ind++;
                            //*out << ind << "std::cout<<\"Already Added"<<ruleId<<"\"<<std::endl;\n";

                        *out << --ind << "}\n";
                    *out << --ind << "}else{\n";
                        ind++;
                        if(aggr.isNegated()){
                            *out << ind++ << "if(!buildFalse){\n";
                        }
                        *out << ind++ << "if(u"<<joinTupleName<<".find(t)==NULL){\n";
                            *out << ind << "const auto& insertResult = u" << joinTupleName <<".insert(Tuple(t));\n";
                            *out << ind++ << "if (insertResult.second) {\n";
                                *out << ind++ << "for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_"<<joinTupleName<<"]){\n";
                                    *out << ind << "auxMap -> insert2(*insertResult.first);\n";
                                *out << --ind << "}\n";
                                addJoinTupleToSharedVariablesMap(ruleId,aggrIndex,"second",joinTupleName);
                            *out << --ind << "}\n";
                        *out << --ind << "}\n";

                        if (aggr.isNegated()){
                            *out << --ind << "}\n";
                        }
                    *out << --ind << "}\n";

                    
                }else{
                    //*out << ind << "std::cout<<\"False\"<<std::endl;\n";
                    *out << ind << "Tuple t(std::vector<int>({"<<joinTuple.substr(0,joinTuple.size()-1)<<"}),&_"<<joinTupleName<<");\n";
                    //*out << ind << "std::cout<<\"erase false join tuple \";t.print();std::cout<<std::endl;\n";

                    //se un letterale interno all'aggregato diventa falso elimino tutte le joinTuple che lo metchano sia dalle positive che dalle indefinite
                    *out << ind << "u" << joinTupleName <<".erase(t);\n";
                    *out << ind << "w" << joinTupleName <<".erase(t);\n";
                }

                
            }else{
                *out << ind << "Tuple tupleUndef(std::vector<int>({"<<joinTuple.substr(0,joinTuple.size()-1)<<"}),&_"<<joinTupleName<<");\n";
                //*out << ind << "tupleUndef.print();\n";
                //*out << ind << "std::cout<<\"buildig Undef\"<<std::endl;\n";
                *out << ind++ << "if(u"<<joinTupleName<<".find(Tuple(tupleUndef)) == NULL){\n";        
                    *out << ind << "const auto& insertResult = u" << joinTupleName <<".insert(Tuple(tupleUndef));\n";
                    //*out << ind << "tupleUndef.print();std::cout<<\"newUndef\"<<std::endl;\n";
                    *out << ind++ << "if (insertResult.second) {\n";
                        //*out << ind << "std::cout<<\"inserted\"<<std::endl;\n";
                        *out << ind++ << "for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_"<<joinTupleName<<"]){\n";
                            *out << ind << "auxMap -> insert2(*insertResult.first);\n";
                        *out << --ind << "}\n";
                        addJoinTupleToSharedVariablesMap(ruleId,aggrIndex,"second",joinTupleName);
                    *out << --ind << "}\n";
                *out << --ind << "}\n";
            }
            

        }
        buildIndex++;

    }
    for(unsigned i=0;i<aggr.getAggregate().getAggregateLiterals().size()-1;i++){
        *out << --ind << "}\n";
    }
    for(int i=0; i<pars;i++){
        *out << --ind << "}\n";
    }
        
}

void CompilationManager::declareAuxMap(std::string mapVariableName,std::vector<unsigned> keyIndexes,std::string predicateName,bool createFalseAuxMap){
    if(!declaredMaps.count(mapVariableName)){
        *out << ind << "AuxMap p" << mapVariableName << "({";
        for (unsigned k = 0; k < keyIndexes.size(); k++) {
            if (k > 0) {
                *out << ",";
            }
            *out << keyIndexes[k];
        }
        *out << "});\n";

        //TODO remove duplication
        *out << ind << "AuxMap u" << mapVariableName << "({";
        for (unsigned k = 0; k < keyIndexes.size(); k++) {
            if (k > 0) {
                *out << ",";
            }
            *out << keyIndexes[k];
        }
        *out << "});\n";

        if(createFalseAuxMap){
            *out << ind << "AuxMap f" << mapVariableName << "({";
            for (unsigned k = 0; k < keyIndexes.size(); k++) {
                if (k > 0) {
                    *out << ",";
                }
                *out << keyIndexes[k];
            }
            *out << "});\n";
        }
        predicateToAuxiliaryMaps[predicateName].insert(mapVariableName);
        if (mode == EAGER_MODE) {
            predicateToUndefAuxiliaryMaps[predicateName].insert(mapVariableName);
            if(createFalseAuxMap)
                predicateToFalseAuxiliaryMaps[predicateName].insert(mapVariableName);
        }
        declaredMaps.insert(mapVariableName);
    }
}
void CompilationManager::moveTrueToUndefJoinTuples(int ruleId,int aggrIndex,const aspc::Literal& li,int literalIndex){
    
    //prendo le joinTuple che metchano il letterale li
    std::string aggrIdentifier = std::to_string(ruleId)+":"+std::to_string(aggrIndex);
            
    *out << ind << "const std::vector<const Tuple* >& trueJoinTuples = p"<<aggregateLiteralToAuxiliaryMap[li.getPredicateName()+"_"+std::to_string(literalIndex)+"_"+aggrIdentifier]<<".getValues({";
    for(unsigned i=0;i<li.getAriety();i++){
        *out << "tuple["<<i<<"]";
        if(i < li.getAriety()-1){
            *out << ",";

        }
    }
    *out << "});\n";
    
    
    *out << ind++ << "for(const Tuple* t :trueJoinTuples){\n";
        //*out << ind << "t->print();std::cout<<\"moving\"<<std::endl;\n";
    
        //aggiungo la joinTuple vera a quelle indefinite
        *out << ind << "const auto insertResult = u" << aggregateLiteralToPredicateWSet[li.getPredicateName()+"_"+aggrIdentifier] << ".insert(Tuple(*t));\n";
        *out << ind++ << "if(insertResult.second){\n";
            //aggiorno le AuxMap
            *out << ind++ << "for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_"<< aggregateLiteralToPredicateWSet[li.getPredicateName()+"_"+aggrIdentifier]<<"]) {\n";
                    *out << ind << "auxMap -> insert2(*insertResult.first);\n";
            *out << --ind << "}\n";
            std::unordered_set<std::string> declaredVariables;

            for (std::string structureName : sharedVariablesMapForAggregateBody[aggregateLiteralToPredicateWSet[li.getPredicateName()+"_"+aggrIdentifier]]){
                int pos = structureName.find("_");
                if(pos!=std::string::npos)
                    structureName=structureName.substr(pos+1,structureName.length());
                pos = structureName.find("_");
                std::string aggrId =structureName.substr(0,pos)+":";
                ruleId = std::stoi(structureName.substr(0,pos));

                structureName=structureName.substr(pos+1,structureName.length());
                pos = structureName.find("_");
                aggrId+=structureName.substr(pos+1,structureName.length());
                std::string sharedVariables = sharedVariablesMap[aggrId];
                aggrIndex = std::stoi(structureName.substr(pos+1,structureName.length()));
                pos = sharedVariables.find(',');

                int i=0;
                if(sharedVariables!="" && pos == std::string::npos){
                    if(!declaredVariables.count(sharedVariables)){
                        *out << ind << "int " << sharedVariables<< " = t->at("<<sharedVariablesIndexesMap[aggrId][0]<<");\n";
                        declaredVariables.insert(sharedVariables);
                    }

                }

                while(pos!=std::string::npos ){
                    if(!declaredVariables.count(sharedVariables.substr(0,pos))){
                        *out << ind << "int " << sharedVariables.substr(0,pos)<< " = t->at("<<sharedVariablesIndexesMap[aggrId][i]<<");\n";
                        declaredVariables.insert(sharedVariables.substr(0,pos));
                    }
                    sharedVariables=sharedVariables.substr(pos+1,sharedVariables.length());
                    pos = sharedVariables.find(',');
                    if(pos == std::string::npos && sharedVariables!=""){
                        i++;
                        if(!declaredVariables.count(sharedVariables)){
                            *out << ind << "int " << sharedVariables<< " = t->at("<<sharedVariablesIndexesMap[aggrId][i]<<");\n";
                            declaredVariables.insert(sharedVariables);
                        }
                    }
                    i++;
                }
            }

            
            addJoinTupleToSharedVariablesMap(ruleId,aggrIndex,"second",aggregateLiteralToPredicateWSet[li.getPredicateName()+"_"+aggrIdentifier]);
        *out << --ind << "}\n";
        *out << ind << "w" << aggregateLiteralToPredicateWSet[li.getPredicateName()+"_"+aggrIdentifier] << ".erase(*t);\n";

    *out << --ind << "}\n";
}

void CompilationManager::updateTrueSharedVariablesMap(const aspc::Rule & r,aspc::Literal* li,int litIndex,std::vector<int> aggregateIndexes,std::vector<int> literalIndexes){
        
        std::unordered_set<std::string> boundVars;
        printVars(*li,"tuple.",boundVars);
        //*out << ind << "tuple.print();\n";
        //*out << ind << "std::cout<<\"updatesharedvariables\"<<std::endl;\n";
        
        li->addVariablesToSet(boundVars);
        int closingParenthesis=0;

        for(int litIndexBuild : literalIndexes){
            if(litIndex!=litIndexBuild){
                aspc::Literal* buildLit = (aspc::Literal*)r.getFormulas()[litIndexBuild];

                std::string mapVariableName = buildLit->getPredicateName() + "_";
                if(!buildLit->isBoundedLiteral(boundVars)){
                    for (unsigned tiIndex = 0; tiIndex < buildLit->getTerms().size(); tiIndex++) {
                        if ((buildLit->isVariableTermAt(tiIndex) && boundVars.count(buildLit->getTermAt(tiIndex))) || !buildLit->isVariableTermAt(tiIndex)) {
                            mapVariableName += std::to_string(tiIndex) + "_";
                        }
                    }
                    *out << ind << "const std::vector<const Tuple *>& tuples"<<litIndexBuild<<" = p"<<mapVariableName<<".getValues({";
                    printLiteralTuple(buildLit,boundVars);
                    *out << "});\n";
                    *out << ind << "const std::vector<const Tuple *>& tuplesU"<<litIndexBuild<<" = u"<<mapVariableName<<".getValues({";
                    printLiteralTuple(buildLit,boundVars);
                    *out << "});\n";
                }else{
                    *out << ind << "std::vector<const Tuple *> tuples"<<litIndexBuild<<";\n";
                    *out << ind << "const Tuple* t"<<litIndexBuild<<"= w"<<buildLit->getPredicateName()<<".find({";
                    printLiteralTuple(buildLit,boundVars);
                    *out << "});\n";
                    *out << ind << "if(t"<<litIndexBuild<<"!=NULL) tuples"<<litIndexBuild<<".push_back(t"<<litIndexBuild<<");\n";
                    
                    *out << ind << "std::vector<const Tuple *> tuplesU"<<litIndexBuild<<";\n";
                    *out << ind << "const Tuple* tU"<<litIndexBuild<<"= u"<<buildLit->getPredicateName()<<".find({";
                    printLiteralTuple(buildLit,boundVars);
                    *out << "});\n";
                    *out << ind << "if(tU"<<litIndexBuild<<"!=NULL) tuplesU"<<litIndexBuild<<".push_back(tU"<<litIndexBuild<<");\n";
                }

                //*out << ind << "std::cout<<\"Searching for X = \"<<X<<std::endl;\n";
                *out << ind << "const Tuple * tuple"<<litIndexBuild<<";\n";
                *out << ind++ << "for(int i=0; i< tuples"<<litIndexBuild<<".size() + tuplesU"<<litIndexBuild<<".size();i++){\n";
                *out << ind++ << "if(i<tuples"<<litIndexBuild<<".size())\n";
                *out << ind << "tuple"<<litIndexBuild<<"= tuples"<<litIndexBuild<<"[i];\n";
                *out << --ind << "else tuple"<<litIndexBuild<<" = tuplesU"<<litIndexBuild<<"[i-tuples"<<litIndexBuild<<".size()];\n";
                //*out << ind << "tuple"<<litIndexBuild<<"->print();\n";
                //*out << ind << "std::cout<<\"iterate\"<<std::endl;\n";

                closingParenthesis++;
                printVars(*buildLit,"tuple"+std::to_string(litIndexBuild)+"->",boundVars);
                buildLit->addVariablesToSet(boundVars);

            }

        }
        for(int aggrIndex : aggregateIndexes){
            std::string key(std::to_string(r.getRuleId())+":"+std::to_string(aggrIndex));


            std::string sharedVarsIndexesToString="";
            for(unsigned varIndex : sharedVariablesIndexesMap[key]){
                sharedVarsIndexesToString+=std::to_string(varIndex)+"_";
            }

            std::string sharedVars = sharedVariablesMap[key];
            if(sharedVars!=""){

                
                aspc::ArithmeticRelationWithAggregate* aggr = (aspc::ArithmeticRelationWithAggregate*)r.getFormulas()[aggrIndex];
                *out << ind++ << "if(var>0){\n";
                
                    checkExistsShareVariableMap(r.getRuleId(),aggrIndex,sharedVars,true);
                    
                        *out << ind << "const std::vector<const Tuple*>* trueJoinTuples = &p_"
                                <<aggregateLiteralToPredicateWSet[aggr->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+key]
                                        <<sharedVarsIndexesToString<<".getValues({"<<sharedVars<<"});\n"; 
                        *out << ind << "const std::vector<const Tuple*>* undefJoinTuples = &u_"
                                <<aggregateLiteralToPredicateWSet[aggr->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+key]
                                        <<sharedVarsIndexesToString<<".getValues({"<<sharedVars<<"});\n"; 
                        saveTuples("trueJoinTuples","w"+aggregateLiteralToPredicateWSet[aggr->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+key],aggrIndex,r.getRuleId());
                        //*out << ind << "std::cout<<\"true updated\"<<std::endl;\n";

                        saveTuples("undefJoinTuples","u"+aggregateLiteralToPredicateWSet[aggr->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+key],aggrIndex,r.getRuleId());

                        //*out << ind << "std::cout<<\"True size: \"<<sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<aggrIndex<<"[sharedTuple]->first->size()<<std::endl;\n";
                        //*out << ind << "std::cout<<\"Undef size: \"<<sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<aggrIndex<<"[sharedTuple]->second->size()<<std::endl;\n";
                    *out << --ind << "}\n";
                *out << --ind <<"}else{\n";
                    ind++;
                    
                    //checkExistsShareVariableMap(r.getRuleId(),aggrIndex,sharedVars,false);
                    //*out << ind << "sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<aggrIndex<<"[sharedTuple]->first->clear();\n";
                    //*out << ind << "delete sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<aggrIndex<<"[sharedTuple]->first;\n";
                    //*out << ind << "sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<aggrIndex<<"[sharedTuple]->second->clear();\n";
                    //*out << ind << "delete sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<aggrIndex<<"[sharedTuple]->second;\n";
                    //*out << ind << "delete sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<aggrIndex<<"[sharedTuple];\n";
                    //*out << ind << "sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<aggrIndex<<".erase(sharedTuple);\n";
                    //*out << --ind <<"}\n";
                    
                *out << --ind <<"}\n";
            }
        }
        for(int i=0;i<closingParenthesis;i++)
            *out << --ind <<"}\n";
        //*out << ind << "std::cout<<\"update\"<<std::endl;\n";
}
void CompilationManager::updateUndefinedSharedVariablesMap(aspc::Rule& r,int startLit){
    
    std::unordered_set<std::string> boundVars;
    printVars(r.getBodyLiterals()[startLit],"tuple.",boundVars);
    //*out << ind << "tuple.print();\n";
    //*out << ind << "std::cout<<\"updatesharedvariables\"<<std::endl;\n";
    r.getBodyLiterals()[startLit].addVariablesToSet(boundVars);
    int closingParenthesis=0;
    
    for(unsigned i=0;i<r.getBodyLiterals().size();i++){
    
        if(i!=startLit){
            
            std::string mapVariableName = r.getBodyLiterals()[i].getPredicateName() + "_";
            if(!r.getBodyLiterals()[i].isBoundedLiteral(boundVars)){
                for (unsigned tiIndex = 0; tiIndex < r.getBodyLiterals()[i].getAriety(); tiIndex++) {
                    if ((r.getBodyLiterals()[i].isVariableTermAt(tiIndex) && boundVars.count(r.getBodyLiterals()[i].getTermAt(tiIndex))) || !r.getBodyLiterals()[i].isVariableTermAt(tiIndex)) {
                        mapVariableName += std::to_string(tiIndex) + "_";
                    }
                }
                *out << ind << "const std::vector<const Tuple *>& undefTuples"<<i<<" = u"<<mapVariableName<<".getValues({";
                printLiteralTuple(&r.getBodyLiterals()[i],boundVars);
                *out << "});\n";
                *out << ind << "const std::vector<const Tuple*>& trueTuples"<<i<<" = p"<<mapVariableName<<".getValues({";
                printLiteralTuple(&r.getBodyLiterals()[i],boundVars);
                *out << "});\n";
                //*out << ind << "std::cout<<\"TrueSize = \"<<trueTuples"<<i<<".size()<<std::endl;\n";
                //*out << ind << "std::cout<<\"UndefSize = \"<<undefTuples"<<i<<".size()<<std::endl;\n";
                
            }else{
                *out << ind << "const Tuple* undefT = u"<<r.getBodyLiterals()[i].getPredicateName()<<".find({";
                printLiteralTuple(&r.getBodyLiterals()[i],boundVars);
                *out << "});\n";
                
                *out << ind << "const Tuple* trueT = w"<<r.getBodyLiterals()[i].getPredicateName()<<".find({";
                printLiteralTuple(&r.getBodyLiterals()[i],boundVars);
                *out << "});\n";
                
                *out << ind << "std::vector<const Tuple *> undefTuples"<<i<<";\n";
                *out << ind << "if(undefT != NULL) undefTuples"<<i<<".push_back(undefT);\n";
                
                *out << ind << "std::vector<const Tuple *> trueTuples"<<i<<";\n";
                *out << ind << "if(trueT != NULL) trueTuples"<<i<<".push_back(undefT);\n";
            }
            
            //*out << ind << "std::cout<<\"Searching for X = \"<<X<<std::endl;\n";
            //*out << ind << "std::cout<<\"UndefSize = \"<<undefTuples"<<i<<".size()<<std::endl;\n";
            
            *out << ind++ << "for(int i=0;i<undefTuples"<<i<<".size()+trueTuples"<<i<<".size();i++){\n";
            *out << ind << "const Tuple * tuple"<<i<<";\n";
            
            /**out << ind++ << "if(i<undefTuples"<<i<<".size())\n";
                *out << ind-- << "std::cout<<\"Undefined Tuple\"<<std::endl;\n";
            *out << ind++ << "else std::cout<<\"True Tuple\"<<std::endl;";*/  
            
            *out << ind++ << "if(i<undefTuples"<<i<<".size())\n";
            *out << ind << "tuple"<<i<<" = undefTuples"<<i<<"[i];\n";
            *out << --ind << "else tuple"<<i<<" = trueTuples"<<i<<"[i-undefTuples"<<i<<".size()];\n";
            //*out << ind << "tuple"<<i<<"->print();\n";
            //*out << ind << "std::cout<<\"iterate\"<<std::endl;\n";

            closingParenthesis++;
            printVars(r.getBodyLiterals()[i],"tuple"+std::to_string(i)+"->",boundVars);
            r.getBodyLiterals()[i].addVariablesToSet(boundVars);
        }
    }
    
    //join partendos dal letterale startLit costruito
    
    for(const aspc::ArithmeticRelationWithAggregate& ar: r.getArithmeticRelationsWithAggregate()){
        //vedo le variabili condivise tra i letterali del corpo ed ogni aggregato della regola
        std::string key(std::to_string(r.getRuleId())+":"+std::to_string(ar.getFormulaIndex()));
        
        
        std::string sharedVarsIndexesToString="";
        for(unsigned varIndex : sharedVariablesIndexesMap[key]){
            
            //salvo gli indici delle variabili di aggregazione
            sharedVarsIndexesToString+=std::to_string(varIndex)+"_";
        }
        
        std::string sharedVars = sharedVariablesMap[key];
        if(sharedVars!=""){
            *out << ind++ << "{\n";
                //popolo la mappa delle variabili condivise se non esiste la tupla di shared variable
                checkExistsShareVariableMap(r.getRuleId(),ar.getFormulaIndex(),sharedVars,true);
                    *out << ind << "const std::vector<const Tuple*>* undefJoinTuples = &u_"
                            <<aggregateLiteralToPredicateWSet[ar.getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+key]
                                    <<sharedVarsIndexesToString<<".getValues({"<<sharedVars<<"});\n"; 
                    *out << ind << "const std::vector<const Tuple*>* trueJoinTuples = &p_"
                            <<aggregateLiteralToPredicateWSet[ar.getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+key]
                                    <<sharedVarsIndexesToString<<".getValues({"<<sharedVars<<"});\n"; 

                    
                    saveTuples("undefJoinTuples","u"+aggregateLiteralToPredicateWSet[ar.getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+key],ar.getFormulaIndex(),r.getRuleId());
                    saveTuples("trueJoinTuples","w"+aggregateLiteralToPredicateWSet[ar.getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+key],ar.getFormulaIndex(),r.getRuleId());
                    
                *out << --ind << "}\n";
            *out << --ind << "}\n";
            //*out << ind << "std::cout<<\"True size: \"<<sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<ar.getFormulaIndex()<<"[sharedTuple]->first->size()<<std::endl;\n";
            //*out << ind << "std::cout<<\"Undef size: \"<<sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<ar.getFormulaIndex()<<"[sharedTuple]->second->size()<<std::endl;\n";
            
        }
    }

    for(int i=0;i<closingParenthesis;i++){
        *out << --ind << "}\n";
    }

    //comment
    //*out << ind << "std::cout<<\"saved for all aggregate\"<<std::endl;\n";

}
void CompilationManager::saveTuples(std::string collectionName,std::string predicateSetName,int aggrIndex,int ruleId){
    
    *out << ind++ << "for(int i=0;i<"<<collectionName<<"->size();i++){\n";

        //*out << ind << "t->print();\n";
        *out << ind << "const auto findResult = "<< predicateSetName << ".find(Tuple(*"<<collectionName<<"->at(i)));\n";
        std::string position = collectionName == "trueJoinTuples" ? "first" : "second"; 
        /*if(collectionName != "trueJoinTuples"){
            *out << ind << "std::cout<<\"insertUndef\"<<std::endl;\n";
        }*/
        *out << ind++ << "if(findResult!=NULL){\n";
            *out << ind << "sharedVariables_"<<ruleId<<"_ToAggregate_"<<aggrIndex<<"[sharedTuple]->"<<position<<"->insert2(*findResult);\n";
        *out << --ind << "}\n";
    *out << --ind << "}\n";
}

void CompilationManager::generateFindSharedValueInJoinTuple(aspc::Program & program){
    for(const aspc::Rule & r : program.getRules()){
        if(r.isConstraint() && r.containsAggregate()){
            for(const aspc::ArithmeticRelationWithAggregate & ar : r.getArithmeticRelationsWithAggregate()){
                
                *out << ind++ << " std::vector<const Tuple*> undefJoinTuple_"<<r.getRuleId()<<"_"<<ar.getFormulaIndex()<<"_ShareValue(int actualSize){\n";
                    *out << ind << "std::cout<<\"undefJoinTuple_function\"<<std::endl;\n";            

                    std::string aggrIdentifier = std::to_string(r.getRuleId())+":"+std::to_string(ar.getFormulaIndex());
                    std::string joinTupleName = aggregateLiteralToPredicateWSet[ar.getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier];
                    *out << ind << "const std::vector<const Tuple *> * tuplesU = &u_"<<joinTupleName<<".getValues({});\n";
                    *out << ind++ << "for(int i = 0 ;i < tuplesU->size(); i++){\n";
                    *out << ind << "std::cout<<\"iterate on i\"<<std::endl;\n";            

                    int index=0;
                        for(const aspc::Literal& l : ar.getAggregate().getAggregateLiterals()){
                            *out << ind << "int check_"<<l.getPredicateName()<<"=1;\n";
                            *out << ind << "std::vector<int> lit_"<<l.getPredicateName()<<";\n";
                            *out << ind << "std::vector<const Tuple*> joinTuple_"<<l.getPredicateName()<<";\n";
                            *out << ind++ << "for(int termI="<<index<<"; termI<"<<index+l.getAriety()<<";termI++)\n";
                            *out << ind-- << "lit_"<<l.getPredicateName()<<".push_back(tuplesU->at(i)->at(termI));\n";
                            index+=l.getAriety();
                        }
                        *out << ind++ << "for(int j = i+1 ;j < tuplesU->size(); j++){\n";
                            *out << ind << "std::cout<<\"iterate on j\"<<std::endl;\n";            
                            *out << ind << "int startTermIndex=0;\n";
                            *out << ind << "bool check = true;\n";
                            for(const aspc::Literal& l : ar.getAggregate().getAggregateLiterals()){
                                *out << ind << "check = true;\n";
                                *out << ind++ << "if(u"<<l.getPredicateName()<<".find({";
                                for(int i=0;i<l.getAriety();i++){
                                    if(i>0)
                                        *out << ",";
                                    *out << "lit_"<<l.getPredicateName()<<"["<<i<<"]";
                                }
                                *out << "})!=NULL){\n";
                                    *out << ind++ << "for(int term = startTermIndex; term < startTermIndex+"<<l.getAriety()<<"; term++){\n";
                                        *out << ind++ << "if(tuplesU->at(i)->at(term)!=tuplesU->at(j)->at(term)){\n";
                                            *out << ind << "check=false;\n";
                                            *out << ind << "break;\n";
                                        *out << --ind << "}\n";
                                    *out << --ind << "}\n";
                                    *out << ind++ << "if(check){\n";
                                        *out << ind << "check_"<<l.getPredicateName()<<"++;\n";
                                        *out << ind << "joinTuple_"<<l.getPredicateName()<<".push_back(tuplesU->at(j));\n";
                                    *out << --ind << "}\n";

                                    *out << ind++ << "if(check_"<<l.getPredicateName()<<"+actualSize"<<ar.getCompareTypeAsString()<<ar.getGuard().getStringRep()<<")\n";
                                        *out << ind << "joinTuple_"<<l.getPredicateName()<<".push_back(tuplesU->at(i));\n";
                                    *out << ind-- << "return joinTuple_"<<l.getPredicateName()<<";\n";
                                *out << --ind << "}\n";
                                *out << ind << "startTermIndex+="<<l.getAriety()<<";\n";


                            }                
                        *out << --ind << "}\n";
                        
                    *out << --ind << "}\n";
                    *out << ind << "return EMPTY_TUPLES;\n";
                *out << --ind << "}\n";
            }
        }
    }
    
}
void CompilationManager::declareDataStructureForAggregate(const aspc::Rule& r,const std::set< pair<std::string, unsigned> >& aggregatePredicates){
    
    //dichiaro le auxMap che vengono utilizzate per costruire le jointuple
    for(const aspc::ArithmeticRelationWithAggregate& aggr : r.getArithmeticRelationsWithAggregate()){
        
        std::string joinTupleName="";
        std::set<string> varAlreadyAdded;
        std::vector<std::vector<unsigned>> aggregateLiteralIndexes;
        int varIndex=0;
        int index=0;
        std::string aggrIdentifier = std::to_string(r.getRuleId())+":"+std::to_string(aggr.getFormulaIndex());
        
        for(const aspc::Literal& li: aggr.getAggregate().getAggregateLiterals()){
            
            joinTupleName=joinTupleName+li.getPredicateName()+"_";
            aggregateLiteralIndexes.push_back(std::vector<unsigned>());
            
            //creo una mappa per ogni letterale indicizzata su tutte le variabili
            std::string mapIndexedAllVars=li.getPredicateName()+"_";
            
            std::vector<unsigned> localIndex;
            for (unsigned tiIndex = 0; tiIndex < li.getAriety(); tiIndex++) {
                
                joinTupleName= joinTupleName+li.getTermAt(tiIndex)+"_";
                if(aggr.getAggregate().containsVar(li.getTermAt(tiIndex))&& !varAlreadyAdded.count(li.getTermAt(tiIndex))){

                    //salvo le variabili di aggregazione per la coppia regola aggregato
                    aggregateVariablesIndex[aggrIdentifier].push_back(varIndex);
                    varAlreadyAdded.insert(li.getTermAt(tiIndex));
                }
                
                mapIndexedAllVars+=std::to_string(tiIndex)+"_";
                aggregateLiteralIndexes[index].push_back(varIndex);
                localIndex.push_back(tiIndex);
                varIndex++;

            }
            declareAuxMap(mapIndexedAllVars,localIndex,li.getPredicateName(),true);
            
            //creo una mappa non indicizzata per iniziare il join
            declareAuxMap(li.getPredicateName() + "_",std::vector<unsigned>(),li.getPredicateName(),true);

            //creo una mappa per gli altri letterali indicizzata rispetto al letterale corrente
            std::unordered_set<std::string> boundVariables;
            li.addVariablesToSet(boundVariables);
            int buildIndex=0;
            for(const aspc::Literal& li_: aggr.getAggregate().getAggregateLiterals()){
                std::string mapVariableName = li_.getPredicateName() + "_";
                std::vector< unsigned > keyIndexes;
                if(buildIndex != index){
                    for (unsigned tiIndex = 0; tiIndex < li_.getAriety(); tiIndex++) {
                        if ((li_.isVariableTermAt(tiIndex) && boundVariables.count(li_.getTermAt(tiIndex))) || !li_.isVariableTermAt(tiIndex)) {
                            mapVariableName += std::to_string(tiIndex) + "_";
                            keyIndexes.push_back(tiIndex);
            
                        }
                    }
                    li_.addVariablesToSet(boundVariables);
                    declareAuxMap(mapVariableName,keyIndexes,li_.getPredicateName(),true);
                }
                buildIndex++;                    
            }
            index++;
        }
        sharedVariablesMapForAggregateBody[joinTupleName].push_back("sharedVariables_"+std::to_string(r.getRuleId())+"_ToAggregate_"+std::to_string(aggr.getFormulaIndex()));
        
        //dichiaro una mappa per le joinTuple indicizzata sulle variabili di aggregazione
        std::string aggregateVarsIndex="";
        for(unsigned index_ : aggregateVariablesIndex[aggrIdentifier])
            aggregateVarsIndex+=std::to_string(index_)+"_";
        declareAuxMap("_"+joinTupleName+aggregateVarsIndex,aggregateVariablesIndex[aggrIdentifier],joinTupleName,false);
        

        //dichiaro una mappa per le joinTuple non indicizzata
        declareAuxMap("_"+joinTupleName,std::vector<unsigned>(),joinTupleName,false);


        //dichiaro una mappa per le joinTuple indicizzata sulle variabili condivise con il corpo
        std::string sharedVarsIndexToString="";
        for(unsigned index_ : sharedVariablesIndexesMap[aggrIdentifier])
            sharedVarsIndexToString+=std::to_string(index_)+"_";
        std::vector<unsigned> sharedVarsIndex = sharedVariablesIndexesMap[aggrIdentifier];
        declareAuxMap("_"+joinTupleName+sharedVarsIndexToString,sharedVarsIndex,joinTupleName,false);


        //dichiaro una mappa per le join tuples indicizzata sull variabili di ogni letterale nell'aggregato
        index=0;
        for(std::vector<unsigned>& indexes : aggregateLiteralIndexes){
            std::string literalTermIndex="";
            for(unsigned var : indexes)
                literalTermIndex = literalTermIndex + std::to_string(var) + "_";
            
            //salvo per ogni letterale il nome dell'AuxMap delle join tuple indicizzata secondo il letterale
            aggregateLiteralToAuxiliaryMap[aggr.getAggregate().getAggregateLiterals()[index].getPredicateName()+"_"+std::to_string(index)+"_"+aggrIdentifier]=std::string("_"+joinTupleName+literalTermIndex);
            aggregateLiteralToPredicateWSet[aggr.getAggregate().getAggregateLiterals()[index].getPredicateName()+"_"+aggrIdentifier]=std::string(joinTupleName);
            
            declareAuxMap("_"+joinTupleName+literalTermIndex,indexes,joinTupleName,false);
            index++;
        }
        std::string sharedVariables = sharedVariablesMap[aggrIdentifier];
        
        if(sharedVariables!=""){
            
            //dichiaro una mappa per ogni letterale del corpo indicizzata sulle shared variable e costanti
            int literalIndex=0;
            //std::cout<<"Declaring map for external predicates"<<std::endl;
            for(const aspc::Literal& bodyLiteral : r.getBodyLiterals()){
                //std::cout<<"Start from ";
                //bodyLiteral.print();
                //std::cout<<std::endl;

                std::string auxMapName = bodyLiteral.getPredicateName()+"_";
                std::unordered_set<std::string> boundVars;
                std::vector<unsigned> indexes;
                for(int i=0;i<bodyLiteral.getAriety();i++){
                    if(sharedVariables.find(bodyLiteral.getTermAt(i))!=std::string::npos || !bodyLiteral.isVariableTermAt(i)){
                        auxMapName+=std::to_string(i)+"_";
                        indexes.push_back(i);
                    }
                    if(bodyLiteral.isVariableTermAt(i)){
                        boundVars.insert(bodyLiteral.getTermAt(i));
                    }
                }
                
                bool declareFalseAuxMap = aggregatePredicates.count(std::pair<std::string,unsigned>(bodyLiteral.getPredicateName(),bodyLiteral.getAriety()))!=0;
                declareAuxMap(auxMapName,indexes,bodyLiteral.getPredicateName(),declareFalseAuxMap);

                //join dei letterali del corpo considerando le sharedVariables bound
                int bodyLiteralIndex=0;
                for(const aspc::Literal& buildBodyLiteral : r.getBodyLiterals()){
                    if(bodyLiteralIndex!=literalIndex){
                        std::string buildAuxMapName = buildBodyLiteral.getPredicateName()+"_";
                        std::vector<unsigned> buildindexes;
                
                        for(int i=0;i<buildBodyLiteral.getAriety();i++){
                            if(sharedVariables.find(buildBodyLiteral.getTermAt(i))!=std::string::npos || !buildBodyLiteral.isVariableTermAt(i) || boundVars.count(buildBodyLiteral.getTermAt(i))){
                                buildAuxMapName+=std::to_string(i)+"_";
                                buildindexes.push_back(i);
                            }
                            if(buildBodyLiteral.isVariableTermAt(i)){
                                boundVars.insert(buildBodyLiteral.getTermAt(i));
                            }
                        }
                        bool declareFalseAuxMap = aggregatePredicates.count(std::pair<std::string,unsigned>(buildBodyLiteral.getPredicateName(),buildBodyLiteral.getAriety()))!=0;
                        //std::cout<<"Declare "<<buildAuxMapName<<std::endl;
                        declareAuxMap(buildAuxMapName,buildindexes,buildBodyLiteral.getPredicateName(),declareFalseAuxMap);

                    }
                    bodyLiteralIndex++;
                }
                literalIndex++;
            }
        }
    }
}
void CompilationManager::generateStratifiedCompilableProgram(aspc::Program & program, AspCore2ProgramBuilder* builder) {

    bool programHasConstraint = program.hasConstraint();

    *out << ind << "#include \"Executor.h\"\n\n";
    *out << ind << "#include \"utils/ConstantsManager.h\"\n\n";
    *out << ind << "#include \"DLV2libs/input/InputDirector.h\"\n\n";
    *out << ind << "#include \"parsing/AspCore2InstanceBuilder.h\"\n\n";
    *out << ind << "#include \"datastructures/PredicateSet.h\"\n\n";
    *out << ind << "#include<map>\n\n";
    *out << ind << "namespace aspc {\n";
    *out << ind++ << "extern \"C\" Executor* create_object() {\n";

    *out << ind << "return new Executor;\n";
    *out << --ind << "}\n";

    *out << ind++ << "extern \"C\" void destroy_object( Executor* object ) {\n";
    *out << ind << "delete object;\n";
    *out << --ind << "}\n";

    

    *out << ind << "Executor::Executor() {}\n\n";

    //typedefs

    if (programHasConstraint) {
        *out << ind << "typedef TupleWithReasons Tuple;\n";
    } else {
        *out << ind << "typedef TupleWithoutReasons Tuple;\n";
    }
    *out << ind << "typedef AuxiliaryMap<Tuple> AuxMap;\n";

    *out << ind << "typedef std::vector<const Tuple* > Tuples;\n";
    *out << ind << "using PredicateWSet = PredicateSet<Tuple, TuplesHash>;\n\n";

    if (mode == LAZY_MODE) {
        *out << ind << "std::unordered_map<std::string, PredicateWSet*> predicateWSetMap;\n";
        *out << ind << "std::unordered_map<std::string, PredicateWSet*> predicateFSetMap;\n";
    }

    if (mode == EAGER_MODE) {
        *out << ind << "std::unordered_map<const std::string*, PredicateWSet*> predicateWSetMap;\n";
        *out << ind << "std::unordered_map<const std::string*, PredicateWSet*> predicateFSetMap;\n";
        *out << ind << "std::unordered_map<const std::string*, PredicateWSet*> predicateUSetMap;\n";
        *out << ind << "std::unordered_map<const std::string*,int> maxPossibleSums;\n";
        *out << ind << "std::unordered_map<const std::string*,int> trueSums;\n";
        *out << ind << "std::unordered_map<const std::string*,std::vector<int>> aggregatePredicatesAndVars;\n";
    }

    //contiene predicati con la rispettiva arità
    const set< pair<std::string, unsigned> >& predicates = program.getPredicates();
    
    for (const pair<std::string, unsigned>& predicate : predicates) {
        //*out << ind << "const std::string & "<< predicate.first << " = ConstantsManager::getInstance().getPredicateName("<< predicate.first <<");\n";
        *out << ind << "const std::string _" << predicate.first << " = \"" << predicate.first << "\";\n";
        *out << ind << "PredicateWSet w" << predicate.first << "(" << predicate.second << ");\n";
        *out << ind << "PredicateWSet u" << predicate.first << "(" << predicate.second << ");\n";
        *out << ind << "PredicateWSet f" << predicate.first << "(" << predicate.second << ");\n";

    }
    const std::set< pair<std::string, unsigned> >& aggregatePredicates = program.getAggregatePredicates();
    
    for(const std::pair<std::string, unsigned>& predicate : aggregatePredicates){
        if(!predicates.count(predicate)){
            *out << ind << "const std::string _" << predicate.first << " = \"" << predicate.first << "\";\n";
            *out << ind << "PredicateWSet w" << predicate.first << "(" << predicate.second << ");\n";
            *out << ind << "PredicateWSet u" << predicate.first << "(" << predicate.second << ");\n";
            *out << ind << "PredicateWSet f" << predicate.first << "(" << predicate.second << ");\n";

        }
    }

    
    //dichiaro predicateWSet per joinTuple
    std::unordered_set<std::string> declaredJoinTupleSet;

    for(const aspc::Rule & r : program.getRules()){
        if(r.isConstraint() && r.containsAggregate()){
            int formulaIndex=0;
            for(const aspc::Formula* f : r.getFormulas()){
                if(f->containsAggregate()){
                    const aspc::ArithmeticRelationWithAggregate* ar = (const aspc::ArithmeticRelationWithAggregate*)f;
                    
                    std::string joinTupleName ="";
                    std::vector<pair<std::string,int>> bodyAggregateVars;
                    int ariety=0;
                    for(const aspc::Literal& l : ar->getAggregate().getAggregateLiterals()){
                        joinTupleName=joinTupleName+l.getPredicateName()+"_";
                        for(unsigned i=0;i<l.getAriety();i++){
                            joinTupleName = joinTupleName + l.getTermAt(i)+"_";

                            if(l.isVariableTermAt(i) && l.getTermAt(i)!="_"){
                                bodyAggregateVars.push_back(std::pair<std::string,int>(l.getTermAt(i),ariety+i));
                            }

                        }
                        ariety+=l.getAriety();

                    }
            
                    //dichiaro le shared variables per l'aggregato formulaIndex e la regola
                    std::string key(std::to_string(r.getRuleId())+":"+std::to_string(formulaIndex));
                    bool firstVarAdded=false;
                    std::unordered_set<std::string> varsAlreadyAdded;
                    for(auto pair : bodyAggregateVars){
                        for(const aspc::Literal& l : r.getBodyLiterals()){
                            if(l.getVariables().count(pair.first)!=0){
                                if (firstVarAdded){
                                    sharedVariablesMap[key]+=",";
                                }
                                //if(!varsAlreadyAdded.count(pair.first)){
                                    firstVarAdded=true;
                                    sharedVariablesMap[key]+=pair.first;
                                    varsAlreadyAdded.insert(pair.first);
                                    sharedVariablesIndexesMap[key].push_back(pair.second);
                                //}
                                break;
                            }
                        }
                    }
                    
                    
                    if(!declaredJoinTupleSet.count(joinTupleName)){
                        *out << ind << "const std::string _" << joinTupleName << " = \"" << joinTupleName << "\";\n";
                        *out << ind << "PredicateWSet w"<<joinTupleName<<"("<<ariety<<");\n";
                        *out << ind << "PredicateWSet u"<<joinTupleName<<"("<<ariety<<");\n";
                        declaredJoinTupleSet.insert(joinTupleName);
                    }
                    *out << ind << "std::map<std::vector<int>, std::pair< AuxMap*,AuxMap*>*> sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<formulaIndex<<";\n";
                    
                    //*out << ind << "std::map<std::vector<int>, std::pair< AuxMap*,AuxMap*>*> u_sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<formulaIndex<<";\n";
                    //*out << ind << "std::map<std::vector<int>, std::pair< AuxMap*,AuxMap*>*> f_sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<formulaIndex<<";\n";
                }
                formulaIndex++;
            }
        }
    }
    
    *out << ind++ << "Executor::~Executor() {\n";
    for(const aspc::Rule & r : program.getRules()){
        if(r.isConstraint() && r.containsAggregate()){
            for(const aspc::ArithmeticRelationWithAggregate& ar: r.getArithmeticRelationsWithAggregate()){
                *out << ind++ << "for(auto sharedVar : sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<ar.getFormulaIndex()<<"){\n";
                *out << ind << "delete sharedVar.second->first;\n";
                *out << ind << "delete sharedVar.second->second;\n";
                *out << ind << "delete sharedVar.second;\n";
                *out << --ind << "}\n";
            }
        }
    }
    *out << --ind << "}\n\n";

    *out << ind << "\n";
    *out << ind << "const std::vector<const Tuple* > EMPTY_TUPLES;\n";
    *out << ind << "std::unordered_map<std::string, const std::string * > stringToUniqueStringPointer;\n";

    *out << ind << "typedef void (*ExplainNegative)(const std::vector<int> & lit, std::unordered_set<std::string> & open_set, std::vector<const Tuple *> & output);\n\n";

    *out << ind << "std::vector<Tuple> atomsTable;\n\n";

    *out << ind << "std::unordered_map<Tuple, unsigned, TuplesHash> tupleToVar;\n\n";

    *out << ind << "std::unordered_map<const std::string*, ExplainNegative> explainNegativeFunctionsMap;\n\n";

    *out << ind++ << "Tuple parseTuple(const std::string & literalString) {\n";
    
    *out << ind << "std::string predicateName;\n";
    *out << ind << "unsigned i = 0;\n";
    *out << ind++ << "for (i = 0; i < literalString.size(); i++) {\n";
    *out << ind++ << "if (literalString[i] == '(') {\n";
    *out << ind << "predicateName = literalString.substr(0, i);\n";
    *out << ind << "break;\n";
    *out << --ind << "}\n";
    *out << ind++ << "if (i == literalString.size() - 1) {\n";
    *out << ind << "predicateName = literalString.substr(0);\n";
    *out << --ind << "}\n";
    *out << --ind << "}\n";
    *out << ind << "std::vector<int> terms;\n";
    *out << ind++ << "for (; i < literalString.size(); i++) {\n";
    *out << ind << "char c = literalString[i];\n";
    *out << ind++ << "if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {\n";
    *out << ind << "int start = i;\n";
    *out << ind << "int openBrackets = 0;\n";
    *out << ind++ << "while ((c != ' ' && c != ',' && c != ')') || openBrackets > 0) {\n";
    *out << ind++ << "if (c == '(') {\n";
    *out << ind << "openBrackets++;\n";
    *out << ind-- << "} else if (c == ')') {\n";
    ind++;
    *out << ind << "openBrackets--;\n";
    *out << ind-- << "}\n";
    *out << ind << "i++;\n";
    *out << ind << "c = literalString[i];\n";
    *out << --ind << "}\n";
    *out << ind << "terms.push_back(ConstantsManager::getInstance().mapConstant(literalString.substr(start, i - start)));\n";
    *out << --ind << "}\n";
    *out << --ind << "}\n";
    *out << ind << "return Tuple(terms, stringToUniqueStringPointer[predicateName]);\n";
    *out << --ind << "}\n";

    *out << ind << "//only ground lit function calls are not known a priori\n";

    *out << ind++ << "void explainNegativeLiteral(const Tuple * lit, std::unordered_set<std::string> & open_set, std::vector<const Tuple *> & output) {\n";
    *out << ind << "explainNegativeFunctionsMap[lit->getPredicateName()](*lit, open_set, output);\n";
    *out << --ind << "}\n\n";


    //perform join functions    


    GraphWithTarjanAlgorithm& graphWithTarjanAlgorithm = builder->getGraphWithTarjanAlgorithm();
    std::vector< std::vector <int> > sccs = graphWithTarjanAlgorithm.SCC();
    
    //print working set
    //     for (const pair<std::string, unsigned>& predicate : predicates) {
    //        *out << ind << "w" << predicate.first <<".printTuples(\""<<predicate.first<<"\");\n";
    //    }
    const std::unordered_map<int, Vertex>& vertexByID = builder->getVertexByIDMap();

    //compute levels of each predicate
    std::vector< unsigned > predicateLevels(vertexByID.size());
    for (int i = sccs.size() - 1; i >= 0; i--) {
        const std::vector<int>& scc = sccs[i];
        for (int c : scc) {
            predicateLevels[c] = sccs.size() - i - 1;
        }
    }


    if (mode == LAZY_MODE) {
        *out << ind << "std::unordered_map <std::string, std::vector <AuxMap*> > predicateToAuxiliaryMaps;\n";
        *out << ind << "std::unordered_map <std::string, std::vector <AuxMap*> > predicateToUndefAuxiliaryMaps;\n";
        *out << ind << "std::unordered_map <std::string, std::vector <AuxMap*> > predicateToFalseAuxiliaryMaps;\n";
    } else {
        *out << ind << "std::unordered_map <const std::string*, std::vector <AuxMap*> > predicateToAuxiliaryMaps;\n";
        *out << ind << "std::unordered_map <const std::string*, std::vector <AuxMap*> > predicateToUndefAuxiliaryMaps;\n";
        *out << ind << "std::unordered_map <const std::string*, std::vector <AuxMap*> > predicateToFalseAuxiliaryMaps;\n";
    }
    unsigned sccsSize = sccs.size();
    if (programHasConstraint) {
        sccsSize++;
    }
    std::vector< std::unordered_map<std::string, std::vector<unsigned>>> starterToExitRulesByComponent(sccsSize);
    std::vector < std::unordered_map < std::string, std::vector<pair<unsigned, unsigned> > >> starterToRecursiveRulesByComponent(sccsSize);
    std::vector<bool> exitRules(program.getRulesSize(), false);
    const std::unordered_map<std::string, int>& predicateIDs = builder->getPredicateIDsMap();


    for (aspc::Rule& r : program.getRules()) {
        //r is a constraint
        bool exitRule = true;
        unsigned lIndex = 0;
        unsigned headLevel = sccs.size();
        if (!r.isConstraint()) {
            std::vector<unsigned> starters;
            headLevel = predicateLevels[predicateIDs.at(r.getHead().at(0).getPredicateName())];
            for (const aspc::Formula* f : r.getFormulas()) {
                if (f->isLiteral()) {
                    const aspc::Literal * l = (const aspc::Literal*) f;
                    unsigned predicateID = predicateIDs.at(l->getPredicateName());
                    if (predicateLevels.at(predicateID) == headLevel) {
                        if (l->isNegated()) {
                            throw std::runtime_error("The input program is not stratified because of " + l->getPredicateName());
                        }
                        exitRule = false;
                        starterToRecursiveRulesByComponent[headLevel][l->getPredicateName()].push_back(pair<unsigned, unsigned>(r.getRuleId(), lIndex));
                        starters.push_back(lIndex);
                    }
                }
                lIndex++;
            }
            r.bodyReordering(starters);
        }
        if (exitRule || r.isConstraint()) {
            if (mode == LAZY_MODE) {
                r.bodyReordering();
                unsigned starter = r.getStarters()[0];
                aspc::Literal* starterL = (aspc::Literal*) r.getFormulas()[starter];
                starterToExitRulesByComponent[headLevel][starterL->getPredicateName()].push_back(r.getRuleId());
            } else {
                //mode == EAGER
                vector<unsigned> starters;
                for (unsigned i = 0; i < r.getBodySize(); i++) {
                    
                    if (r.getFormulas()[i]->isLiteral()) {
                        starters.push_back(i);
                    }else if(r.getFormulas()[i]->containsAggregate()){
                        starters.push_back(i);
                        
                    }
                }
                if (r.isConstraint()) {
                    starters.push_back(r.getBodySize());
                }
                r.bodyReordering(starters);
                for (unsigned i = 0; i < starters.size(); i++) {
                    unsigned starter = r.getStarters()[i];
                    if (starter != r.getBodySize()) {
                        string pred_name;
                        
                        if(r.getFormulas()[starter]->isLiteral()){
                            aspc::Literal* starterL = (aspc::Literal*) r.getFormulas()[starter];
                            pred_name=starterL->getPredicateName();
                        }else if(r.getFormulas()[starter]->containsAggregate()){
                            aspc::ArithmeticRelationWithAggregate* starterAggr = (aspc::ArithmeticRelationWithAggregate*) r.getFormulas()[starter];
                            pred_name="#"+std::to_string(r.getRuleId())+":"+std::to_string(starter);
                        }
                        auto & rules = starterToExitRulesByComponent[headLevel][pred_name];
                        bool alreadyAdded = false;
                        for (unsigned rule : rules) {
                            alreadyAdded = alreadyAdded | (rule == r.getRuleId());
                        }
                        if (!alreadyAdded) {
                            rules.push_back(r.getRuleId());
                        }    
                        
                        
                    }
                }

            }
            exitRules[r.getRuleId()] = true;
        }
        if(r.containsAggregate()){
            declareDataStructureForAggregate(r,aggregatePredicates);
        }
        
        for (unsigned starter : r.getStarters()) {
            declareDataStructures(r, starter,aggregatePredicates);
        }

    }

    declareDataStructuresForReasonsOfNegative(program);

    for (const std::string & predicate : modelGeneratorPredicatesInNegativeReasons) {
        //*out << ind << "const std::string & "<< predicate.first << " = ConstantsManager::getInstance().getPredicateName("<< predicate.first <<");\n";
        *out << ind << "PredicateWSet neg_w" << predicate << "(" << predicateArieties[predicate] << ");\n";
    }

    
    if (program.hasConstraint()) {
        writeNegativeReasonsFunctionsPrototypes(program);
        *out << ind << "void explainPositiveLiteral(const Tuple *, std::unordered_set<std::string> &, std::vector<const Tuple*> &);\n";
        writeNegativeReasonsFunctions(program);
    }

    //generateFindSharedValueInJoinTuple(program);

    //print tuples 
    *out << ind++ << "void printTuples(const std::string & predicateName, const Tuples & tuples) {\n";
    *out << ind++ << "for (const std::vector<int> * tuple : tuples) {\n";
    *out << ind << "std::cout <<predicateName<< \"(\";\n";
    *out << ind++ << "for (unsigned i = 0; i < tuple->size(); i++) {\n";
    *out << ind++ << "if (i > 0) {\n";
    *out << ind << "std::cout << \",\";\n";
    *out << --ind << "}\n";
    *out << ind << "std::cout << ConstantsManager::getInstance().unmapConstant((*tuple)[i]);\n";
    *out << --ind << "}\n";
    *out << ind << "std::cout << \").\\n\";\n";
    *out << --ind << "}\n";
    *out << --ind << "}\n";
    //handle arieties
    *out << ind++ << "void Executor::executeFromFile(const char* filename) {\n";
    *out << ind << "DLV2::InputDirector director;\n";
    *out << ind << "AspCore2InstanceBuilder* builder = new AspCore2InstanceBuilder();\n";
    *out << ind << "director.configureBuilder(builder);\n";
    *out << ind << "std::vector<const char*> fileNames;\n";
    *out << ind << "fileNames.push_back(filename);\n";
    *out << ind << "director.parse(fileNames);\n";
    *out << ind << "executeProgramOnFacts(builder->getProblemInstance());\n";
    *out << ind << "delete builder;\n";
    *out << --ind << "}\n\n";


    if (programHasConstraint) {
        *out << ind++ << "void explainPositiveLiteral(const Tuple * tuple, std::unordered_set<std::string> & open_set, std::vector<const Tuple*> & outputReasons) {\n";
        //*out << ind << "const std::vector<const Tuple*> & tupleReasons = predicateReasonsMap.at(*tuple->getPredicateName())->at(tuple->getId());\n";
        *out << ind << "const std::vector<const Tuple*> & tupleReasons = tuple->getPositiveReasons();\n";
        *out << ind++ << " if (tupleReasons.empty()) {\n";
        *out << ind << "outputReasons.push_back(tuple);\n";
        *out << --ind << "}\n";
        *out << ind++ << "else {\n";
        *out << ind++ << "for (const Tuple * reason : tupleReasons) {\n";
        *out << ind << "explainPositiveLiteral(reason, open_set, outputReasons);\n";
        *out << --ind << "}\n";


        *out << --ind << "}\n";
        *out << ind++ << "for (const Tuple & reason : tuple->getNegativeReasons()) {\n";
        *out << ind << "explainNegativeLiteral(&reason, open_set, outputReasons);\n";
        //*out << ind << "outputReasons.push_back(&reason);\n";
        *out << --ind << "}\n";
        *out << --ind << "}\n\n";

        *out << ind++ << "aspc::Literal tupleToLiteral(const Tuple & tuple) {\n";
        *out << ind << "aspc::Literal literal(*tuple.getPredicateName(), tuple.isNegated());\n";
        *out << ind++ << "for (int v : tuple) {\n";
        *out << ind << "literal.addTerm(ConstantsManager::getInstance().unmapConstant(v));\n";
        *out << --ind << "}\n";
        *out << ind << "return literal;\n";
        *out << --ind << "}\n";

    }


    string tupleType = "WithoutReasons";
    if (programHasConstraint) {
        tupleType = "WithReasons";
    }
    // ---------------------- onLiteralTrue(const aspc::Literal* l) --------------------------------------//

    *out << ind++ << "inline void Executor::onLiteralTrue(const aspc::Literal* l) {\n";
    if (mode == LAZY_MODE) {

        *out << ind++ << "if(!l->isNegated()) {\n";
        //*out << ind << "std::cout<<i<<\"\\n\";\n";
        *out << ind << "std::unordered_map<std::string,PredicateWSet*>::iterator it = predicateWSetMap.find(l->getPredicateName());\n";
        *out << ind++ << "if(it==predicateWSetMap.end()) {\n";
        if (!programHasConstraint) {
            *out << ind << "l->print();\n";
            *out << ind << "std::cout<<\".\\n\";\n";
        }
        *out << --ind << "}\n";
        *out << ind++ << "else {\n";

        *out << ind << "const auto& insertResult=it->second->insert(l->getTuple" << tupleType << "());\n";

        *out << ind++ << "if(insertResult.second){\n";
        //    *out << ind << "it->second->insert(tuple);\n";
        *out << ind++ << "for(AuxMap* auxMap:predicateToAuxiliaryMaps[it->first]){\n";
        *out << ind << "auxMap -> insert2(*insertResult.first);\n";
        *out << --ind << "}\n";
        *out << --ind << "}\n";
        *out << --ind << "}\n";
        *out << --ind << "}\n";
        *out << ind++ << "else {\n";
        *out << ind << "std::unordered_map<std::string,PredicateWSet*>::iterator it = predicateFSetMap.find(l->getPredicateName());\n";
        *out << ind++ << "if(it!=predicateFSetMap.end()) {\n";
        *out << ind << "const auto& insertResult=it->second->insert(l->getTuple" << tupleType << "());\n";
        *out << ind++ << "if(insertResult.second){\n";
        *out << ind++ << "for(AuxMap* auxMap:predicateToFalseAuxiliaryMaps[it->first]){\n";
        *out << ind << "auxMap -> insert2(*insertResult.first);\n";
        *out << --ind << "}\n";
        *out << --ind << "}\n";
        *out << --ind << "}\n";
        *out << --ind << "}\n";
    }
    *out << --ind << "}\n";
    // ---------------------- end onLiteralTrue(const aspc::Literal* l) --------------------------------------//
    
    // ---------------------- onLiteralUndef(const aspc::Literal* l) --------------------------------------//
    *out << ind++ << "inline void Executor::onLiteralUndef(const aspc::Literal* l) {\n";
    //*out << ind << "std::cout<<i<<\"\\n\";\n";

    //    if (mode == LAZY_MODE) {
    //        *out << ind << "std::unordered_map<std::string,PredicateWSet*>::iterator it = predicateUSetMap.find(l->getPredicateName());\n";
    //        *out << ind++ << "if(it==predicateUSetMap.end()) {\n";
    //        if (!programHasConstraint) {
    //            *out << ind << "l->print();\n";
    //            *out << ind << "std::cout<<\".\\n\";\n";
    //        }
    //        *out << --ind << "}\n";
    //        *out << ind++ << "else {\n";
    //        *out << ind << "const auto& insertResult=it->second->insert(l->getTuple" << tupleType << "());\n";
    //
    //        *out << ind++ << "if(insertResult.second){\n";
    //        //    *out << ind << "it->second->insert(tuple);\n";
    //        *out << ind++ << "for(AuxMap* auxMap:predicateToUndefAuxiliaryMaps[it->first]){\n";
    //        *out << ind << "auxMap -> insert2(*insertResult.first);\n";
    //        *out << --ind << "}\n";
    //        *out << --ind << "}\n";
    //        *out << --ind << "}\n";
    //    }
    *out << --ind << "}\n";
    // ---------------------- end onLiteralTrue() --------------------------------------//
    // ---------------------- onLiteralTrue(int var) --------------------------------------//
    *out << ind++ << "inline void Executor::onLiteralTrue(int var) {\n";
    if (mode == EAGER_MODE) {
        *out << ind << "unsigned uVar = var > 0 ? var : -var;\n";
        *out << ind << "const Tuple & tuple = atomsTable[uVar];\n";
        
        *out << ind << "std::unordered_map<const std::string*,int>::iterator sum_it;\n"; 
        *out << ind << "std::string minus = var < 0 ? \"-\" : \"\";\n";
        
        //*out << ind << "std::cout<<\"True \"<<minus;tuple.print();std::cout<<std::endl;\n";
        //*out << ind << "std::cout<<\"True \"<<var<<std::endl;\n";
        
        *out << ind << "std::unordered_map<const std::string*,PredicateWSet*>::iterator uSetIt = predicateUSetMap.find(tuple.getPredicateName());\n";
        *out << ind++ << "if(uSetIt!=predicateUSetMap.end()) {\n";
        *out << ind << "uSetIt->second->erase(tuple);\n";
        *out << --ind << "}\n";

        *out << ind << "std::unordered_map<const std::string*, PredicateWSet*>::iterator it = predicateWSetMap.find(tuple.getPredicateName());\n";
        *out << ind++ << "if (it == predicateWSetMap.end()) {\n";
        *out << ind << "} else {\n";
            *out << ind++ << "if (var > 0) {\n";
                *out << ind << "const auto& insertResult = it->second->insert(Tuple(tuple));\n";
                *out << ind++ << "if (insertResult.second) {\n";
                    *out << ind++ << "for (AuxMap* auxMap : predicateToAuxiliaryMaps[it->first]) {\n";
                        *out << ind << "auxMap -> insert2(*insertResult.first);\n";
                    *out << --ind << "}\n";
                *out << --ind << "}\n";
            *out << --ind << "}\n";    
        *out << --ind << "}\n";

        //salvo i letterali falsi dei predicati che appaiono nel corpo di qualche aggregato
        if(program.getAggregatePredicates().size() > 0){
            *out << ind++ << "if(var<0 && (";
            int predicateIndex=0;
            for(const auto &  predicate :program.getAggregatePredicates()){
                if(predicateIndex>0)
                    *out << " ||";
                *out << " tuple.getPredicateName() == &_"<<predicate.first;
                predicateIndex++;
            }
            *out << ")){\n";
                *out << ind << "std::unordered_map<const std::string*, PredicateWSet*>::iterator it_false = predicateFSetMap.find(tuple.getPredicateName());\n";
                *out << ind++ << "if (it_false == predicateFSetMap.end()) {\n";
                *out << ind << "} else {\n";
                    *out << ind++ << "if (var < 0) {\n";
                        *out << ind << "const auto& insertResult = it_false->second->insert(Tuple(tuple));\n";
                        *out << ind++ << "if (insertResult.second) {\n";
                            *out << ind++ << "for (AuxMap* auxMap : predicateToFalseAuxiliaryMaps[it_false->first]) {\n";
                                *out << ind << "auxMap -> insert2(*insertResult.first);\n";
                            *out << --ind << "}\n";
                        *out << --ind << "}\n";
                    *out << --ind << "}\n";
                *out << --ind << "}\n";
            *out << --ind << "}\n";

        }
        
        //*out << ind++ << "else {\n";
        //*out << ind << "it->second->erase(tuple);\n";
        //*out << --ind << "}\n";

        /**out << ind << "std::cout<<\"False size: \"<<farc_.getValues({}).size()<<std::endl;\n";
        *out << ind << "std::cout<<\"True size: \"<<parc_.getValues({}).size()<<std::endl;\n";
        *out << ind << "std::cout<<\"Undef size: \"<<uarc_.getValues({}).size()<<std::endl;\n";*/
                    
        //--------------------------------------------------Aggregate structure update--------------------------------------------------
        
        for(int ruleId=0; ruleId< program.getRules().size();ruleId++){
            aspc::Rule& r = program.getRules()[ruleId];
            if(r.isConstraint() && r.containsAggregate()){
                std::vector<int> aggregateIndexes;
                std::vector<int> literalIndexes;

                int formulaIndex=0;
                for(const aspc::Formula* f : r.getFormulas()){
                    if(f->containsAggregate()){

                        aggregateIndexes.push_back(formulaIndex);
                        int index=0;
                        const aspc::ArithmeticRelationWithAggregate* aggr = (aspc::ArithmeticRelationWithAggregate*)f;
                        for(const aspc::Literal& li: aggr->getAggregate().getAggregateLiterals()){
                            *out << ind++ << "if(tuple.getPredicateName() == &_"<<li.getPredicateName()<<"){\n";
                                *out << ind++ << "if(var > 0){\n";
                                    builJoinTupleStartingFromLiteral(r.getRuleId(),formulaIndex,li,*aggr,index,true);
                                *out << --ind << "}else{\n";
                                    ind++;
                                    //std::cout << "OnLiteral True aggregate update starting from"<<li.getPredicateName()<<" False"<<std::endl;

                                    builJoinTupleStartingFromLiteral(r.getRuleId(),formulaIndex,li,*aggr,index,true,true);
                                *out << --ind << "}\n";

                            *out << --ind << "}\n";
                            index++;

                        }

                    }else if(f->isLiteral()){
                        literalIndexes.push_back(formulaIndex);
                    }
                    formulaIndex++;
                }
                for(int litIndex : literalIndexes){
                    aspc::Literal* li = (aspc::Literal*) r.getFormulas()[litIndex];
                    *out << ind++ << "if(tuple.getPredicateName() == &_" << li->getPredicateName()<<"){\n";
                    if(sharedVariablesMap[std::to_string(r.getRuleId())+":"+std::to_string(r.getArithmeticRelationsWithAggregate()[0].getFormulaIndex())]!=""){
                        updateTrueSharedVariablesMap(r,li,litIndex,aggregateIndexes,literalIndexes);
                    }    
                    *out << --ind <<"}\n";

                }
            }
        }
        /**out << ind << "std::cout<<\"TrueKeySizeMap: \"<<p_aux_R_Y_member_Y_R_1_.size()<<std::endl;\n";
        *out << ind << "std::cout<<\"UndefKeySizeMap: \"<<u_aux_R_Y_member_Y_R_1_.size()<<std::endl;\n";
        *out << ind << "std::cout<<\"TrueSize: \"<<p_aux_R_Y_member_Y_R_.getValues({}).size()<<std::endl;\n";
        *out << ind << "std::cout<<\"UndefSize: \"<<u_aux_R_Y_member_Y_R_.getValues({}).size()<<std::endl;\n";*/

#ifdef EAGER_DEBUG
        *out << ind << "std::cout<<\"on literal true \";\n";
        *out << ind << "std::cout<<var<<\"\\n\";\n";
        *out << ind << "tuple.print();\n";
        *out << ind << "std::cout<<\"\\n\";\n";
#endif
        //*out << "std::cout<<\"End of On Literal True\"<<std::endl;\n";
    }
    *out << --ind << "}\n";

    // ---------------------- onLiteralUndef(int var) --------------------------------------//
    *out << ind++ << "inline void Executor::onLiteralUndef(int var) {\n";
    if (mode == EAGER_MODE) {
        //*out << ind << "std::cout<<\"undef \"<<var<<std::endl;\n";
        *out << ind << "unsigned uVar = var > 0 ? var : -var;\n";
        *out << ind << "const Tuple & tuple = atomsTable[uVar];\n";

        *out << ind << "std::unordered_map<const std::string*,int>::iterator sum_it;\n";
        *out << ind << "std::string minus = var < 0 ? \"-\" : \"\";\n";
        //*out << ind << "std::cout<<\"Undef \"<<minus;tuple.print();std::cout<<std::endl;\n";
        //*out << ind << "std::cout<<\"Undef \"<<var<<std::endl;\n";

        
#ifdef EAGER_DEBUG
        *out << ind << "std::cout<<\"on literal undef \";\n";
        *out << ind << "std::cout<<var<<\"\\n\";\n";
        *out << ind << "tuple.print();\n";
        *out << ind << "std::cout<<\"\\n\";\n";
#endif

        *out << ind++ << "if (var > 0) {\n";
            *out << ind << "std::unordered_map<const std::string*, PredicateWSet*>::iterator wSetIt = predicateWSetMap.find(tuple.getPredicateName());\n";
            *out << ind++ << "if (wSetIt != predicateWSetMap.end()) {\n";
                *out << ind << "wSetIt->second->erase(tuple);\n";
            *out << --ind << "}\n";
        *out << --ind << "}\n";

        if(program.getAggregatePredicates().size() > 0){
            *out << ind++ << "if(var<0 && (";
            int predicateIndex=0;
            for(const auto &  predicate :program.getAggregatePredicates()){
                if(predicateIndex>0)
                    *out << " ||";
                *out << " tuple.getPredicateName() == &_"<<predicate.first;
                predicateIndex++;
            }
            *out << ")){\n";
                *out << ind << "std::unordered_map<const std::string*, PredicateWSet*>::iterator falseSetIt = predicateFSetMap.find(tuple.getPredicateName());\n";
                *out << ind++ << "if (falseSetIt != predicateFSetMap.end()) {\n";
                *out << ind << "falseSetIt->second->erase(tuple);\n";
                *out << --ind << "}\n";
            *out << --ind << "}\n";
        }

        *out << ind << "std::unordered_map<const std::string*, PredicateWSet*>::iterator it = predicateUSetMap.find(tuple.getPredicateName());\n";
        *out << ind++ << "if (it == predicateUSetMap.end()) {\n";
        *out << ind << "} else {\n";
        *out << ind << "const auto& insertResult = it->second->insert(Tuple(tuple));\n";
        *out << ind++ << "if (insertResult.second) {\n";
        *out << ind++ << "for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[it->first]) {\n";
        *out << ind << "auxMap -> insert2(*insertResult.first);\n";
        *out << --ind << "}\n";
        *out << --ind << "}\n";
        *out << --ind << "}\n";
        

        /**out << ind << "std::cout<<\"False size: \"<<fp_1_.getValues({}).size()<<std::endl;\n";
        *out << ind << "std::cout<<\"True size: \"<<pp_1_.getValues({}).size()<<std::endl;\n";
        *out << ind << "std::cout<<\"Undef size: \"<<up_1_.getValues({}).size()<<std::endl;\n";*/
        
        
        //--------------------------------------------------Aggregate structure update--------------------------------------------------
        for(aspc::Rule& r :program.getRules()){
            if(r.isConstraint() && r.containsAggregate()){
                for(const aspc::ArithmeticRelationWithAggregate& aggr : r.getArithmeticRelationsWithAggregate()){
                    int index=0;
                    for(const aspc::Literal& li: aggr.getAggregate().getAggregateLiterals()){
                        *out << ind++ << "if(tuple.getPredicateName() == &_"<<li.getPredicateName()<<"){\n";
                            
                            *out << ind++ << "if(var > 0){\n";
                                moveTrueToUndefJoinTuples(r.getRuleId(),aggr.getFormulaIndex(),li,index);
                            *out << --ind << "}\n";

                            builJoinTupleStartingFromLiteral(r.getRuleId(),aggr.getFormulaIndex(),li,aggr,index,false);
                        *out << --ind << "}\n";
                        index++;
                    }
                }
                
                //aggiungo alla mappa delle shared variables le tuple di shared variables indefinite
                for(int i=0;i<r.getBodyLiterals().size();i++){
                    *out << ind++ << "if(tuple.getPredicateName() == &_" << r.getBodyLiterals()[i].getPredicateName()<<"){\n";
                    updateUndefinedSharedVariablesMap(r,i);
                    *out << --ind <<"}\n";
                }
            }
        }
    }
    /**out << ind << "std::cout<<\"TrueKeySizeMap: \"<<p_aux_R_Y_member_Y_R_1_.size()<<std::endl;\n";
    *out << ind << "std::cout<<\"UndefKeySizeMap: \"<<u_aux_R_Y_member_Y_R_1_.size()<<std::endl;\n";
    *out << ind << "std::cout<<\"TrueSize: \"<<p_aux_R_Y_member_Y_R_.getValues({}).size()<<std::endl;\n";
    *out << ind << "std::cout<<\"UndefSize: \"<<u_aux_R_Y_member_Y_R_.getValues({}).size()<<std::endl;\n";*/

    //*out << ind << "std::cout<<\"End of OnLiteralUndef\"<<std::endl;\n";
    *out << --ind << "}\n";
    // ---------------------- end onLiteralTrue(int var) --------------------------------------//

    // ---------------------- addedVarName(int var, const std::string & atom) --------------------------------------//

    *out << ind++ << "inline void Executor::addedVarName(int var, const std::string & atom) {\n";
    *out << ind << "atomsTable.resize(var+1);\n";
    *out << ind << "atomsTable.insert(atomsTable.begin()+var, parseTuple(atom));\n";
    *out << ind << "tupleToVar[atomsTable[var]]= var;\n";
    *out << --ind << "}\n";
    // ---------------------- end addedVarName(int var, const std::string & atom) --------------------------------------//

    // ---------------------- clearPropagatedLiterals() --------------------------------------//
    *out << ind++ << "void Executor::clearPropagations() {\n";
    *out << ind << "propagatedLiteralsAndReasons.clear();\n";
    //*out << ind << "reasonsForPropagatedLiterals.clear();\n";
    //*out << ind << "propagatedLiterals.clear();\n";
    *out << --ind << "}\n";

    // ---------------------- end clearPropagatedLiterals() --------------------------------------//

    // ---------------------- clear() --------------------------------------//
    *out << ind++ << "void Executor::clear() {\n";
    *out << ind << "failedConstraints.clear();\n";
    *out << ind << "predicateToAuxiliaryMaps.clear();\n";
    
    if (mode == LAZY_MODE) {
        *out << ind << "predicateToFalseAuxiliaryMaps.clear();\n";
    }

    for (const pair<std::string, unsigned>& predicate : predicates) {
        if (idbs.count(predicate.first) || headPredicates.count(predicate.first)) {
            *out << ind << "w" << predicate.first << ".clear();\n";
        }
    }


    for (const std::string & predicate : modelGeneratorPredicatesInNegativeReasons) {
        if (idbs.count(predicate) || headPredicates.count(predicate)) {
            *out << ind << "neg_w" << predicate << ".clear();\n";
        }
    }

    for (const auto & entry : predicateToAuxiliaryMaps) {
        for (const auto & auxSet : entry.second) {
            if (idbs.count(entry.first) || headPredicates.count(entry.first)) {
                *out << ind << "p" << auxSet << ".clear();\n";
            }
        }
    }

    for (const auto & entry : predicateToFalseAuxiliaryMaps) {
        for (const auto & auxSet : entry.second) {
            if (idbs.count(entry.first) || headPredicates.count(entry.first)) {
                *out << ind << auxSet << ".clear();\n";
            }
        }
    }

    *out << --ind << "}\n";

    // ---------------------- end clear() --------------------------------------//

    // ---------------------- init() --------------------------------------//
    *out << ind++ << "void Executor::init() {\n";
    if (program.hasConstraint()) {
        *out << ind << "createFunctionsMap();\n";
    }
    string reference = (mode == EAGER_MODE) ? "&" : "";

    for (const pair<std::string, unsigned>& predicate : predicates) {

        *out << ind << "predicateWSetMap[" << reference << "_" << predicate.first << "]=&w" << predicate.first << ";\n";
        if (mode == EAGER_MODE) {
            *out << ind << "predicateUSetMap[&_" << predicate.first << "]=&u" << predicate.first << ";\n";
        }
        *out << ind << "stringToUniqueStringPointer[\"" << predicate.first << "\"] = &_" << predicate.first << ";\n";
    }
    for(aspc::Rule& r :program.getRules()){
        if(r.isConstraint()){
            for(const aspc::ArithmeticRelationWithAggregate& aggr : r.getArithmeticRelationsWithAggregate()){
                for(const aspc::Literal& l: aggr.getAggregate().getAggregateLiterals()){
                    if(aggr.getAggregate().isSum()){
                        *out << ind << "maxPossibleSums[" << reference << "_" << l.getPredicateName() << "]=0;\n";
                        *out << ind << "trueSums[" << reference << "_" << l.getPredicateName() << "]=0;\n";
                    }
                    
                    *out << ind << "aggregatePredicatesAndVars [" << reference << "_" << l.getPredicateName() << "]= std::vector<int>({";
                    int countVars=0;
                    for(int i=0;i<l.getAriety();i++){
                        if(aggr.getAggregate().getAggregateVariables().count(l.getTermAt(i))){
                            *out << i;
                            countVars++;
                            if(countVars<aggr.getAggregate().getAggregateVariables().size()-1 && i<l.getAriety()-1)
                            *out <<",";
                        }
                        
                    }
                    *out << "});\n";
                    *out << ind << "predicateWSetMap[" << reference << "_" << l.getPredicateName() << "]=&w" << l.getPredicateName() << ";\n";
                    *out << ind << "predicateFSetMap[" << reference << "_" << l.getPredicateName() << "]=&f" << l.getPredicateName() << ";\n";
                    if (mode == EAGER_MODE) {
                        *out << ind << "predicateUSetMap[&_" << l.getPredicateName() << "]=&u" << l.getPredicateName() << ";\n";
                    }
                    *out << ind << "stringToUniqueStringPointer[\"" << l.getPredicateName() << "\"] = &_" << l.getPredicateName() << ";\n";
                }
            }
        }
    }
    


    for (const std::string & predicate : modelGeneratorPredicatesInNegativeReasons) {
        *out << ind << "predicateFSetMap[_" << predicate << "] = &neg_w" << predicate << ";\n";
    }

    for (const auto & entry : predicateToAuxiliaryMaps) {
        for (const auto & auxSet : entry.second) {
            *out << ind << "predicateToAuxiliaryMaps[" << reference << "_" << entry.first << "].push_back(&p" << auxSet << ");\n";
        }
    }

    for (const auto & entry : predicateToUndefAuxiliaryMaps) {
        for (const auto & auxSet : entry.second) {
            *out << ind << "predicateToUndefAuxiliaryMaps[" << reference << "_" << entry.first << "].push_back(&u" << auxSet << ");\n";
        }
    }

    for (const auto & entry : predicateToFalseAuxiliaryMaps) {
        for (const auto & auxSet : entry.second) {
            *out << ind << "predicateToFalseAuxiliaryMaps[" << reference << "_" << entry.first << "].push_back(&f" << auxSet << ");\n";
        }
    }

    *out << --ind << "}\n";
    /*
    *out << ind++ << "void searchPropagationForSharedValue(std::unordered_map<int, std::vector<int> >& propagatedLiteralsAndReasons, std::vector<int> reason,int actualSize){\n";
    for(const aspc::Rule& r : program.getRules()){
        if(r.isConstraint() && r.containsAggregate()){
            for(const aspc::ArithmeticRelationWithAggregate& aggregateRelation : r.getArithmeticRelationsWithAggregate()){
                std::string aggrIdentifier = std::to_string(r.getRuleId())+":"+std::to_string(aggregateRelation.getFormulaIndex());

                int index=0;
                for(const aspc::Literal& li : aggregateRelation.getAggregate().getAggregateLiterals()){
                    *out << ind++ << "for(const Tuple* t : u"<<li.getPredicateName()<<"_.getValues({})){\n";
                        *out << ind << "std::set<std::vector<int>> keySet;\n";
                        *out << ind++ << "for(const Tuple* joinTuple : u"<<aggregateLiteralToAuxiliaryMap[li.getPredicateName()+"_"+std::to_string(index)+"_"+aggrIdentifier]<<".getValues({";
                        for(int i=0;i<li.getAriety();i++){
                            if(i>0)
                                *out << ",";
                            *out << "t->at("<<i<<")";
                        }
                        *out << "})){\n";
                            *out << ind << "keySet.insert(std::vector<int>({";
                            int currentVar=0;
                            for(int varIndex : aggregateVariablesIndex[aggrIdentifier]){
                                if(currentVar>0)
                                    *out <<",";
                                *out << "joinTuple->at("<<varIndex<<")";
                                currentVar++;
                            }

                            *out << "}));\n";
                        *out << --ind << "}\n";
                        //*out << ind << "t->print();\n";
                        //*out << ind << "std::cout<<actualSize<<\" \"<<keySet.size()<<std::endl;\n";
                        *out << ind++ << "if(!(actualSize - keySet.size()"<<aggregateRelation.getCompareTypeAsString()<<aggregateRelation.getGuard().getStringRep()<<")){\n";
                            //*out << ind << "t->print();std::cout<<\"propagated\"<<std::endl;\n";
                            *out << ind << "const auto & it = tupleToVar.find(*t);\n";
                            *out << ind++ << "if(it != tupleToVar.end()) {\n";
                                *out << ind << "int sign = t->isNegated() ? -1 : 1;\n";
                                *out << ind << "auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;\n";
                            *out << --ind << "}\n";
                        *out << --ind << "}\n";
                    *out << --ind << "}\n";
                    index++;
                }
            }
        }
    }
    *out << --ind << "}\n";
    */
    // ---------------------- end init() --------------------------------------//


    // ---------------------- executeProgramOnFacts() --------------------------------------//
    if (mode == LAZY_MODE) {
        *out << ind << "void Executor::executeProgramOnFacts(const std::vector<int> & facts) {}\n";
        *out << ind++ << "void Executor::executeProgramOnFacts(const std::vector<aspc::Literal*> & facts) {\n";
    } else {
        *out << ind << "void Executor::executeProgramOnFacts(const std::vector<aspc::Literal*> & facts) {}\n";
        *out << ind++ << "void Executor::executeProgramOnFacts(const std::vector<int> & facts) {\n";
    }
    //data structure init

    if (mode == LAZY_MODE) {
        *out << ind << "init();\n";
        *out << ind << "clear();\n";
    } else {
        // mode == EAGER_MODE

        //facts[0] is the decision level for eager mode (-1 is facts level)
        *out << ind << "int decisionLevel = facts[0];\n";
        //*out << ind << "std::cout<<\"Execute program on facts: decision level \"<<decisionLevel<<std::endl;\n";
        
#ifdef EAGER_DEBUG
        *out << ind << "std::cout<<\"Execute program on facts: decision level \"<<decisionLevel<<std::endl;\n";
#endif
        //*out << ind++ << "if(decisionLevel > 0) {\n";
        *out << ind << "clearPropagations();\n";
        //*out << --ind << "}\n";
    }


    // *out << ind << "std::cout<<\"facts reading\"<<std::endl;\n";

    //feed facts
    //*out << ind << "std::cout<<\"facts\\n\";\n";
    if (mode == LAZY_MODE) {
        *out << ind++ << "for(unsigned i=0;i<facts.size();i++) {\n";
        *out << ind << "onLiteralTrue(facts[i]);\n";
        *out << --ind << "}\n";
    } else {
        // mode == EAGER_MODE
        *out << ind++ << "for(unsigned i=1;i<facts.size();i++) {\n";
        //*out << ind << "std::cout<<\"facts: \"<<facts[i]<<std::endl;\n";

        *out << ind << "onLiteralTrue(facts[i]);\n";

        *out << --ind << "}\n";
    }
    //*out << ind << "std::cout<<\"facts reading completed\"<<std::endl;\n";

    if (mode == LAZY_MODE) {
        //declare iterators
        for (const pair<std::string, unsigned>& predicate : predicates) {
            *out << ind << "unsigned index_" << predicate.first << "=0;\n";
        }

        for (unsigned i = 0; i < sccsSize; i++) {
            const std::unordered_map<std::string, std::vector<unsigned>>&startersExitRules = starterToExitRulesByComponent[i];
            for (const auto& rulesByPredicate : startersExitRules) {
                *out << ind << "index_" << rulesByPredicate.first << "=0;\n";
                *out << ind++ << "while(index_" << rulesByPredicate.first << "!=w" << rulesByPredicate.first << ".getTuples().size()){\n";
                *out << ind << "const Tuple * tuple0 = w" << rulesByPredicate.first << ".getTuples()[index_" << rulesByPredicate.first << "];\n";
                for (unsigned ruleId : rulesByPredicate.second) {
                    const aspc::Rule& r = program.getRule(ruleId);
                    *out << ind++ << "{\n";
                    compileRule(r, r.getStarters()[0], program);
                    *out << --ind << "}\n";

                }
                *out << ind << "index_" << rulesByPredicate.first << "++;\n";
                *out << --ind << "}\n";
            }

            const std::unordered_map<std::string, std::vector <pair<unsigned, unsigned>>>& recursiveRulesByStarter = starterToRecursiveRulesByComponent[i];
            if (recursiveRulesByStarter.size()) {
                *out << ind++ << "while(";
                unsigned index = 0;
                for (unsigned vertexId : sccs[sccs.size() - i - 1]) {
                    const Vertex& v = vertexByID.at(vertexId);
                    if (index > 0)
                        *out << " || ";
                    *out << "index_" << v.name << "!=w" << v.name << ".getTuples().size()";
                    index++;
                }
                *out << "){\n";
            }
            for (const auto& rulesByStarter : recursiveRulesByStarter) {
                *out << ind++ << "while(index_" << rulesByStarter.first << "!=w" << rulesByStarter.first << ".getTuples().size()){\n";
                *out << ind << "const Tuple * tuple0 = w" << rulesByStarter.first << ".getTuples()[index_" << rulesByStarter.first << "];\n";
                for (const auto& ruleAndStarterIndex : rulesByStarter.second) {
                    const aspc::Rule& r = program.getRule(ruleAndStarterIndex.first);
                    *out << ind++ << "{\n";
                    compileRule(r, ruleAndStarterIndex.second, program);
                    *out << --ind << "}\n";

                }
                *out << ind << "index_" << rulesByStarter.first << "++;\n";
                *out << --ind << "}\n";
            }
            if (recursiveRulesByStarter.size())
                *out << --ind << "}\n";

        }
        if (!programHasConstraint) {
            //*out << ind << "std::cout<<\"Propagator model:\"<<std::endl;\n";
            for (const pair<std::string, unsigned>& predicate : predicates) {
                *out << ind << "printTuples(\"" << predicate.first << "\",w" << predicate.first << ".getTuples());\n";

            }
        }
    } else {
        //mode == EAGER_MODE
        *out << ind++ << "if(decisionLevel==-1) {\n";
        *out << ind << "std::cout<<\"decisionLevelif\"<<std::endl;\n";
        for (const aspc::Rule& r : program.getRules()) {
            if (r.isConstraint() && !r.containsAggregate()) {
                *out << ind++ << "{\n";
                *out << ind << "const Tuple * tupleU = NULL;\n";
                *out << ind << "bool tupleUNegated = false;\n";

                compileRule(r, r.getBodySize(), program);
                

                *out << --ind << "}//close local scope\n";
            }else{
                *out << ind++ << "{\n";
                *out << ind << "const Tuple * tupleU = NULL;\n";
                *out << ind << "bool tupleUNegated = false;\n";
                compileConstrainWithAggregate(r, r.getBodySize(), program);

                *out << --ind << "}//close local scope\n";
            }
        }
        *out << --ind << "}//close decision level == -1\n";
        //*out << ind << "else std::cout<<\"outOfDecisionLevel\"<<std::endl;\n";
        //*out << ind << "std::cout<<\"start for on facts\"<<decisionLevel<<std::endl;\n";

        *out << ind++ << "for(unsigned i=1;i<facts.size();i++) {\n";

        *out << ind << "unsigned factVar = facts[i] > 0 ? facts[i] : -facts[i];\n";
        *out << ind << "Tuple starter = atomsTable[factVar];\n";
        *out << ind << "starter.setNegated(facts[i]<0);\n";

        //*out << ind << "starter.print();std::cout<<\" Starter\"<<std::endl;\n";
        //*out << ind << "std::cout<<\"Starting from \";starter.print();std::cout<<std::endl;\n";            
        /**out << ind << "std::cout<<\"False size: \"<<fp_1_.getValues({}).size()<<std::endl;\n";
        *out << ind << "std::cout<<\"True size: \"<<pp_1_.getValues({}).size()<<std::endl;\n";
        *out << ind << "std::cout<<\"Undef size: \"<<up_1_.getValues({}).size()<<std::endl;\n";*/
        for (unsigned i = 0; i < sccsSize; i++) {
            const std::unordered_map<std::string, std::vector<unsigned>>&startersExitRules = starterToExitRulesByComponent[i];
            for (const auto& rulesByPredicate : startersExitRules) {
                //*out << ind++ << "if(facts[i]->getPredicateName() == \"" << rulesByPredicate.first << "\") { \n";
                if(rulesByPredicate.first[0]=='#'){
                    
                    /*int start = rulesByPredicate.first.find(':');
                    *out << ind++ << "if(starter.getPredicateName() == &_" << rulesByPredicate.first.substr(start+1) << ") { \n";
                    *out << ind << "const Tuple * tuple0 = &starter;\n";*/
                    
                }else{
                    *out << ind++ << "if(starter.getPredicateName() == &_" << rulesByPredicate.first << ") { \n";
                    *out << ind << "const Tuple * tuple0 = &starter;\n";
                    
                }
                //std::cout<< "Predicate "<<rulesByPredicate.first<<std::endl;
                for (unsigned ruleId : rulesByPredicate.second) {
                    
                    const aspc::Rule& r = program.getRule(ruleId);
                    for (unsigned starter : r.getStarters()) {
                        if (starter != r.getBodySize()) {
                            //std::cout<<starter<<std::endl;
                            if(!r.containsAggregate()){
                                aspc::Literal* starterBodyLiteral = (aspc::Literal*)(r.getFormulas()[starter]);
                                string negationCheck = starterBodyLiteral->isNegated() ? "<" : ">";
                                if (rulesByPredicate.first == starterBodyLiteral->getPredicateName()) {
                                    *out << ind++ << "if(facts[i] " << negationCheck << " 0){\n";
                                    *out << ind++ << "{\n";
                                    *out << ind << "const Tuple * tupleU = NULL;\n";
                                    *out << ind << "bool tupleUNegated = false;\n";
                                    compileRule(r, starter, program);
                                    *out << --ind << "}//close loop nested join\n";
                                    *out << --ind << "}//close loop nested join\n";
                                }
                            }else{
                                if(r.getFormulas()[starter]->isLiteral()){
                                    aspc::Literal* starterBodyLiteral = (aspc::Literal*)(r.getFormulas()[starter]);
                                    string negationCheck = starterBodyLiteral->isNegated() ? "<" : ">";
                                    if (rulesByPredicate.first == starterBodyLiteral->getPredicateName()) {
                                        *out << ind++ << "if(facts[i] " << negationCheck << " 0){\n";
                                        *out << ind++ << "{\n";
                                        *out << ind << "const Tuple * tupleU = NULL;\n";
                                        *out << ind << "bool tupleUNegated = false;\n";
                                        compileConstrainWithAggregate(r, starter, program);
                                        *out << --ind << "}//close loop nested join\n";
                                        *out << --ind << "}//close loop nested join\n";
                                    }
                                }else if (r.getFormulas()[starter]->containsAggregate()){
                                    aspc::ArithmeticRelationWithAggregate* aggr = (aspc::ArithmeticRelationWithAggregate*)(r.getFormulas()[starter]);
                                    string negationCheck = aggr->isNegated() ? "<" : ">";

                                    //per constraint con multi aggregato controllare che il formulaIndex corrisponda
                                    if (rulesByPredicate.first == "#"+std::to_string(r.getRuleId())+":"+std::to_string(aggr->getFormulaIndex())) {
                                    
                                        *out << ind++ << "if(facts[i] "<<negationCheck<<" 0){\n";
                                        *out << ind << "bool tupleUNegated = false;\n";
                                        *out << ind << "const Tuple * tupleU = NULL;\n";

                                        compileConstrainWithAggregate(r, starter, program);
                                        *out << --ind << "}//close if\n";
                                    }

                                }
                            }
                        }
                    }

                }
                if(rulesByPredicate.first[0]!='#')
                    *out << --ind << "}//close predicate joins\n";
            }
        }
        *out << --ind << "}\n";

    }

    *out << --ind << "}\n";
    *out << ind << "}\n";


    //*out << ind << "w" << predicateIdPair.first << ".printTuples(\"" << predicateIdPair.first << "\");\n";
}

void CompilationManager::declareDataStructures(const aspc::Rule& r, unsigned start,const std::set< std::pair<std::string, unsigned> >& aggregatePredicates) {

    std::unordered_set<std::string> boundVariables;
    if (start != r.getBodySize()) {
        r.getFormulas().at(start)->addVariablesToSet(boundVariables);
        
    }
    
    const std::vector<unsigned> & joinOrder = r.getOrderedBodyIndexesByStarter(start);
    unsigned i = 1;
    if (start == r.getBodySize()) {
        i = 0;
    }
    for (; i < r.getFormulas().size(); i++) {
        const aspc::Formula* f = r.getFormulas()[joinOrder[i]];
        if (f->isLiteral()) {
            const aspc::Literal * li = (aspc::Literal *) f;
            if (li->isPositiveLiteral() && !li->isBoundedLiteral(boundVariables)) {
                
                std::vector< unsigned > keyIndexes;
                std::string mapVariableName = li->getPredicateName() + "_";
                for (unsigned tiIndex = 0; tiIndex < li->getTerms().size(); tiIndex++) {
                    if ((li->isVariableTermAt(tiIndex) && boundVariables.count(li->getTermAt(tiIndex))) || !li->isVariableTermAt(tiIndex)) {
                        mapVariableName += std::to_string(tiIndex) + "_";
                        keyIndexes.push_back(tiIndex);
                    }
                }
                
                if (!declaredMaps.count(mapVariableName)) {
                    *out << ind << "AuxMap p" << mapVariableName << "({";
                    for (unsigned k = 0; k < keyIndexes.size(); k++) {
                        if (k > 0) {
                            *out << ",";
                        }
                        *out << keyIndexes[k];
                    }
                    *out << "});\n";

                    //TODO remove duplication
                    *out << ind << "AuxMap u" << mapVariableName << "({";
                    for (unsigned k = 0; k < keyIndexes.size(); k++) {
                        if (k > 0) {
                            *out << ",";
                        }
                        *out << keyIndexes[k];
                    }
                    *out << "});\n";
                    if(aggregatePredicates.count(std::pair<std::string,unsigned>(li->getPredicateName(),li->getAriety()))!=0){

                        *out << ind << "AuxMap f" << mapVariableName << "({";
                        for (unsigned k = 0; k < keyIndexes.size(); k++) {
                            if (k > 0) {
                                *out << ",";
                            }
                            *out << keyIndexes[k];
                        }
                        *out << "});\n";
                        predicateToFalseAuxiliaryMaps[li->getPredicateName()].insert(mapVariableName);
                    
                    }
                    
                    predicateToAuxiliaryMaps[li->getPredicateName()].insert(mapVariableName);
                    if (mode == EAGER_MODE) {
                        predicateToUndefAuxiliaryMaps[li->getPredicateName()].insert(mapVariableName);
                    }
                    declaredMaps.insert(mapVariableName);
                }
            }
        }
        f->addVariablesToSet(boundVariables);
    }
}

bool literalPredicateAppearsBeforeSameSign(const std::vector<const aspc::Formula*>& body, vector<unsigned> joinOrder, unsigned i) {
    const aspc::Literal* l = (aspc::Literal*) body[joinOrder[i]];
    assert(f->isLiteral());

    for (unsigned p = 0; p < i; p++) {
        const aspc::Formula * f2 = body[joinOrder[p]];
        if (f2->isLiteral()) {
            const aspc::Literal* l2 = (aspc::Literal*) f2;
            if (l2->getPredicateName() == l->getPredicateName() && l->isNegated() == l2->isNegated()) {
                return true;
            }
            //find variables inequality?
        }
    }
    return false;
}
void CompilationManager::buildAggregateReason(){
    *out << ind << "std::vector<int> reason;\n"; 
    *out << ind++ << "if(tuplesU->size()>0){\n";
    *out << ind++ << "for(int j=0;j<tuples->size();j++){\n";
    *out << ind << "const auto & it = tupleToVar.find(*tuples->at(j));\n";
    *out << ind++ << "if(it!=tupleToVar.end()){\n";
    *out << ind << "reason.push_back(it->second * (tuples->at(j)->isNegated() ? -1:1));\n";
    *out << --ind <<"}//closing if\n";
    *out << --ind <<"}//closing for\n";
    *out << --ind <<"}//closing if\n";
    return;
}
void CompilationManager::propagate(aspc::ArithmeticRelationWithAggregate aggr,int index){
    
    if(aggr.getCompareTypeAsString()=="<" || aggr.getCompareTypeAsString()=="<="){
        
        *out << ++ind << "for(unsigned i=0;i<tuplesU"<<index<<"->size();i++){\n";
        *out << ++ind << "const auto & it = tupleToVar.find(*tuplesU"<<index<<"->at(i));\n";
        *out << ind++ << "if(it!=tupleToVar.end()){\n";
        *out << ind << "int sign = tuplesU"<<index<<"->at(i)->isNegated() ? -1 : 1;\n";
        
        //adding negative literal to reason
        /*
        *out << ind <<"PredicateWSet* falseSet = predicateFalseWSetMap[&_"<<aggr.getAggregate().getAggregateLiterals()[0].getPredicateName()<<"];\n";
        *out << ind++ << "for(int j = 0; j < falseSet->getTuples().size(); j++){\n";
        *out << ind << "const auto & it_false = tupleToVar.find(*(falseSet->getTuples()[j]));\n";
        *out << ind++ << "if(it_false!=tupleToVar.end()){\n";
        *out << ind << "reason.push_back(it_false->second * (falseSet->getTuples()[j]->isNegated() ? -1:1));\n";
        *out << --ind << "}//close if\n";
        *out << --ind <<"}//close for\n";
        */
        //-----------------------------------
        *out << ind << "auto & reas = propagatedLiteralsAndReasons.insert({it->second * sign, std::vector<int>(reason)}).first->second;\n";
        *out << --ind <<"}//closing if\n";
        *out << --ind <<"}//closing for\n";
        
    }else{
        *out << ++ind << "for(unsigned i=0;i<tuplesU"<<index<<"->size();i++){\n";
        *out << ++ind << "const auto & it = tupleToVar.find(*tuplesU"<<index<<"->at(i));\n";
        *out << ind++ << "if(it!=tupleToVar.end()){\n";
        *out << ind << "int sign = tuplesU"<<index<<"->at(i)->isNegated() ? -1 : 1;\n";
        *out << ind << "auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;\n";
        *out << --ind <<"}//closing if\n";
        *out << --ind <<"}//closing for\n";
    }
}
    
void CompilationManager::canPropagate(aspc::ArithmeticRelationWithAggregate aggr, int index){
    if(aggr.getCompareTypeAsString()=="<" || aggr.getCompareTypeAsString()=="<="){
        std::string one=aggr.getCompareTypeAsString()=="<=" ? "+1" : "";
        *out << "tuplesU"<<index<<"->size() + tuples"<<index<<"->size() =="<<aggr.getGuard().getTerm1()<<one<<" && tuplesU"<<index<<"->size()>0";
                                
    }else{
        std::string minusOne = aggr.getCompareTypeAsString()==">="? "-1" : "";
        *out<<"tuples"<<index<<"->size() == "<<aggr.getGuard().getTerm1()<<minusOne;
    }
}
    
void CompilationManager::evaluateAggregates(const std::vector<aspc::ArithmeticRelationWithAggregate>& aggrs,const std::vector<unsigned>& joinOrder, const std::unordered_set<std::string> & boundVariables,int & iterations){
    
    unsigned index = joinOrder[iterations];
    if(iterations==joinOrder.size()-1){
        *out << ind++ << "if(tuples"<<index<<"->size()"<<aggrs[index].getCompareTypeAsString()<<aggrs[index].getGuard().getTerm1()<<"){\n";
        *out << ind << "std::cout<<\"conflict detected in propagator\"<<std::endl;\n";
        *out << ind << "propagatedLiteralsAndReasons.insert({-1, std::vector<int>()});\n";
        *out << --ind << "}else if(";
        canPropagate(aggrs[index],index);
        *out << "){\n";
        propagate(aggrs[index],index);
        *out << --ind <<"}//close else if\n";
        
        return;
    }

    *out << ind++ << "if(tuples"<<index<<"->size()"<<aggrs[index].getCompareTypeAsString()<<aggrs[index].getGuard().getTerm1()<<"){\n";
    iterations++;
    evaluateAggregates(aggrs,joinOrder,boundVariables,iterations);
    *out << --ind << "}else{\n";
    for(int i=iterations;i<joinOrder.size();i++){
        *out << ++ind << "if(tuples"<<joinOrder[i]<<"->size()"<<aggrs[joinOrder[i]].getCompareTypeAsString()<<aggrs[joinOrder[i]].getGuard().getTerm1()<<"){\n";
    
    }
    *out << ++ind << "if(";
    canPropagate(aggrs[index],index);
    *out << "){\n";
    
    propagate(aggrs[index],index);
    *out << --ind << "}\n";
    
    for(int i=iterations;i<aggrs.size();i++){
        *out << --ind << "}\n";
    }
    *out << --ind << "}\n"; 
       
    
}
void CompilationManager::propagateUndef(bool asTrue, bool withReason,std::string mapVariableName){
    std::string reason = withReason ? "reason" : "";
    std::string sign = asTrue ? "1 : -1" : "-1 : 1";
        
    *out << ind++ << "for(unsigned i=0;i<tuplesUSize;i++){\n";
        
    *out << --ind << "} \n";
    return;
}
void CompilationManager::buildReason(std::string aggrIdentifier,const aspc::ArithmeticRelationWithAggregate* aggregateRelation,bool declareReason){

    
    int k=0;
    std::string tuple="";
    for(unsigned index : aggregateVariablesIndex[aggrIdentifier]){
        tuple+="trueTuples->at(reasonIndex)->at("+std::to_string(index)+")";
        if(k<aggregateVariablesIndex[aggrIdentifier].size()-1)
           tuple+=",";
        k++;
    }
    
    if(sharedVariablesMap[aggrIdentifier] != ""){
        if(!aggregateRelation->isNegated()){
            *out << ind << "const std::vector<const Tuple*>* trueTuples = &p_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier];
            for(unsigned index : sharedVariablesIndexesMap[aggrIdentifier])
                *out << index << "_";
            *out <<".getValues({"<<sharedVariablesMap[aggrIdentifier]<<"});\n";
        }        
    }else{

        if(!aggregateRelation->isNegated()){
            *out << ind << "const std::vector<const Tuple*>* trueTuples = &p_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier];
            *out <<".getValues({});\n";
        }
    }
    //*out << ind << "std::cout<<\"Truetuples size: \"<<trueTuples->size()<<std::endl;\n";
    if(declareReason)
        *out << ind << "std::vector<int> reason;\n";
    if(!aggregateRelation->isNegated()){
        *out << ind++ << "for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){\n";
            int varIndex=0;
            int literalIndex=0;
            for(const aspc::Literal& l : aggregateRelation->getAggregate().getAggregateLiterals()){
                *out << ind << "Tuple tuple"<<l.getPredicateName()<<"_"<<literalIndex<<"(std::vector<int>({";
                for(unsigned i=0;i<l.getAriety();i++){
                    *out << "trueTuples->at(reasonIndex)->at("<<varIndex<<")";
                    if(i<l.getAriety()-1)
                        *out << ",";
                    varIndex++;

                }
                *out << "}),&_"<<l.getPredicateName()<<");\n";
                //*out << ind << "tuple"<<l.getPredicateName()<<"_"<<literalIndex<<".print();\n";
                *out << ind << "const auto & it"<<l.getPredicateName()<<"_"<<literalIndex<<" = tupleToVar.find(tuple"<<l.getPredicateName()<<"_"<<literalIndex<<");\n";
                *out << ind++ << "if(it"<<l.getPredicateName()<<"_"<<literalIndex<<"!=tupleToVar.end()){\n";
                    //print tuple in reason to debug
                    //*out << ind << "tuple"<<l.getPredicateName()<<".print();\n";
                    //*out << ind << "std::cout<<it"<<l.getPredicateName()<<"->second<<std::endl;\n";
                    *out << ind << "reason.push_back(it"<<l.getPredicateName()<<"_"<<literalIndex<<"->second * (tuple"<<l.getPredicateName()<<"_"<<literalIndex<<".isNegated() ? -1:1));\n";
                *out << --ind <<"}//closing if\n";
                literalIndex++;
            }
            
        *out << --ind << "}\n";
    }else{
        for(int starter=0;starter<aggregateRelation->getAggregate().getAggregateLiterals().size();starter++){
            int parenthesis=0;
            const aspc::Literal* li = &(aggregateRelation->getAggregate().getAggregateLiterals()[starter]);
            
            *out << ind << "const std::vector<const Tuple*>* falseTuples"<<starter<<" = &f"<<li->getPredicateName()<<"_.getValues({});\n";
            *out << ind++ << "for(int i=0;i<falseTuples"<<starter<<"->size();i++){\n";
            std::string joiningTupleFound = aggregateRelation->getAggregate().getAggregateLiterals().size()<=1 ? "true":"false";
            *out << ind << "bool joiningTupleFound="<<joiningTupleFound<<";\n";
            std::unordered_set<std::string> boundVars;
            for(int i=0;i<li->getAriety();i++){
                if(li->isVariableTermAt(i) && !boundVars.count(li->getTermAt(i))){
                    *out << ind << "int "<<li->getTermAt(i)<<" = falseTuples"<<starter<<"->at(i)->at("<<i<<");\n";
                    boundVars.insert(li->getTermAt(i));
                }
            }
            
            for(int joiningLiteral=0;joiningLiteral < aggregateRelation->getAggregate().getAggregateLiterals().size();joiningLiteral++){
                if(joiningLiteral!=starter){
                    const aspc::Literal* liBuild = &(aggregateRelation->getAggregate().getAggregateLiterals()[joiningLiteral]);
                    std::string mapVariableName=liBuild->getPredicateName()+"_";
                    for(int i=0;i<liBuild->getAriety();i++){
                        if(boundVars.count(liBuild->getTermAt(i))||!liBuild->isVariableTermAt(i)){
                            mapVariableName+=std::to_string(i)+"_";
                        }
                    }

                    *out << ind << "const std::vector<const Tuple*>* trueTuples"<<joiningLiteral<<" = &p"<<mapVariableName<<".getValues({";
                    printLiteralTuple(liBuild,boundVars);
                    *out << "});\n";
                    *out << ind << "const std::vector<const Tuple*>* undefTuples"<<joiningLiteral<<" = &u"<<mapVariableName<<".getValues({";
                    printLiteralTuple(liBuild,boundVars);
                    *out << "});\n";
                    *out << ind << "const std::vector<const Tuple*>* falseTuples"<<joiningLiteral<<" = &f"<<mapVariableName<<".getValues({";
                    printLiteralTuple(liBuild,boundVars);
                    *out << "});\n";
                    

                    *out << ind++ << "for(int i_"<<joiningLiteral<<"=0;!joiningTupleFound && i_"<<joiningLiteral<<" < trueTuples"<<joiningLiteral<<"->size()+undefTuples"<<joiningLiteral<<"->size()+falseTuples"<<joiningLiteral<<"->size();i_"<<joiningLiteral<<"++){\n";
                    parenthesis++;     
                                   
                        *out << ind << "const Tuple * tuple"<<joiningLiteral<<"=NULL;\n";
                        *out << ind++ << "if(i_"<<joiningLiteral<<"<trueTuples"<<joiningLiteral<<"->size())\n";
                            *out << ind << "tuple"<<joiningLiteral<<"=trueTuples"<<joiningLiteral<<"->at(i_"<<joiningLiteral<<");\n";
                        *out << --ind << "else if(i_"<<joiningLiteral<<"<trueTuples"<<joiningLiteral<<"->size()+undefTuples"<<joiningLiteral<<"->size())\n";
                            *out << ++ind << "tuple"<<joiningLiteral<<"=undefTuples"<<joiningLiteral<<"->at(i_"<<joiningLiteral<<"-trueTuples"<<joiningLiteral<<"->size());\n";
                        *out << --ind << "else\n";
                            *out << ++ind << "tuple"<<joiningLiteral<<"=falseTuples"<<joiningLiteral<<"->at(i_"<<joiningLiteral<<"-trueTuples"<<joiningLiteral<<"->size()-undefTuples"<<joiningLiteral<<"->size());\n";
                        --ind;
                        int lastLiteral = aggregateRelation->getAggregate().getAggregateLiterals().size()-1;
                        if(starter==lastLiteral)
                            lastLiteral--;
                        if(joiningLiteral<lastLiteral)
                            for(int i=0;i<liBuild->getAriety();i++){
                                if(liBuild->isVariableTermAt(i) && !boundVars.count(liBuild->getTermAt(i))){
                                    *out << ind << "int "<<liBuild->getTermAt(i)<<" = tuple"<<joiningLiteral<<"->at("<<i<<");\n";
                                    boundVars.insert(liBuild->getTermAt(i));
                                }
                            }
                        else{
                            *out << ind << "joiningTupleFound=true;\n";
                        }
                    
                }
            }
            while(parenthesis>0){
                parenthesis--;
                *out << --ind << "}\n";
            }
            *out << ind++ << "if(joiningTupleFound){\n";
            //*out << ind << "falseTuples"<<starter<<"->at(i)->print();\n";
                *out << ind << "const auto & it = tupleToVar.find(*falseTuples"<<starter<<"->at(i));\n";
                *out << ind++ << "if(it!=tupleToVar.end()){\n";
                    *out << ind << "reason.push_back(it->second * (falseTuples"<<starter<<"->at(i)->isNegated() ? 1:-1));\n";
                *out << --ind <<"}//closing if\n";
            *out << --ind << "}\n";
            *out << --ind << "}\n";
        }
    }

}

void CompilationManager::propAggr(const aspc::ArithmeticRelationWithAggregate* aggregateRelation,std::string& aggregateIdentifier,bool withReason,std::string op){
    std::string reason = withReason ? "reason":"";
    std::string ruleId = aggregateIdentifier.substr(0,aggregateIdentifier.find(":"));
    std::string index = aggregateIdentifier.substr(aggregateIdentifier.find(":")+1,aggregateIdentifier.length());
    std::string joinTupleName = aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggregateIdentifier];
    
    if(sharedVariablesMap[aggregateIdentifier]!=""){
        *out << ind << "const std::vector<const Tuple*>* undefinedTuples = &u_"<<joinTupleName;
        for(unsigned index : sharedVariablesIndexesMap[aggregateIdentifier] )
            *out << index << "_";
        *out << ".getValues({"<<sharedVariablesMap[aggregateIdentifier]<<"});\n";
        
        

    }else{
        *out << ind << "const std::vector<const Tuple*>* undefinedTuples = &u_"<<joinTupleName<<".getValues({});\n";
        op=".";
    }
    *out << ind++ << "for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){\n";
    //*out << ind << "undefinedTuples->at(iUndef)->print();\n";            
    int k=0;
    std::string tuple="";
    for(unsigned index : aggregateVariablesIndex[aggregateIdentifier]){
        if(k>0)
           tuple+=",";
        tuple+="undefinedTuples->at(iUndef)->at("+std::to_string(index)+")";
        k++;
    }
    if(aggregateRelation->isNegated()){

        *out << ind++ << "if(sharedVarTuple"<<ruleId<<"_"<<index<<".second"<<op<<"first->getValues({"<<tuple<<"}).size() == 0 && ";
        *out << "sharedVarTuple"<<ruleId<<"_"<<index<<".second"<<op<<"second->getValues({" << tuple <<"}).size() == 1){\n";
        int lit_index=0;
        int ariety=0;
        //*out << ind << "undefinedTuples->at(iUndef)->print();\n";
        for(const aspc::Literal& l: aggregateRelation->getAggregate().getAggregateLiterals()){
            *out << ind++ << "{\n";
                std::string lit = "";
                bool checkVar=false;
                for(int i=0;i<l.getAriety();i++){
                    if(l.isVariableTermAt(i)){
                        if(checkVar)
                            lit+=",";
                        lit+="undefinedTuples->at(iUndef)->at("+std::to_string(ariety+i)+")";
                        checkVar=true;
                    }
                }
                
                *out << ind << "const Tuple* aggrTupleU = u"<<l.getPredicateName()<<".find({"<<lit<<"});\n";
                *out << ind++ << "if(aggrTupleU != NULL){\n";
                    *out << ind << "const auto & it = tupleToVar.find(*aggrTupleU);\n";
                    *out << ind++ << "if(it != tupleToVar.end()) {\n";
                        *out << ind << "int sign = aggrTupleU->isNegated() ? 1:-1;\n";
                        *out << ind << "auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>("<<reason<<")}).first->second;\n";
                    *out << --ind << "}\n";
                *out << --ind << "}\n";
            *out << --ind << "}\n";
            ariety+=l.getAriety();

        }
        *out << --ind << "}\n";
    }else{
        *out << ind++ << "if(sharedVarTuple"<<ruleId<<"_"<<index<<".second"<<op<<"first->getValues({"<<tuple<<"}).size() == 0){\n";
        //propago tutte le joinTuple che hanno 1 e 1 solo letterale indefinito
        int variableIndex=0;
        int parenthesis=0;
        *out << ind << "const Tuple* aggrTupleU=NULL;\n";
        
        for(const aspc::Literal& l: aggregateRelation->getAggregate().getAggregateLiterals()){
        
            *out << ind << "const std::vector<const Tuple*>* tuples"<<l.getPredicateName()<<";\n";
            *out << ind << "const std::vector<const Tuple*>* tuplesU"<<l.getPredicateName()<<"=&EMPTY_TUPLES;\n";
            
            std::string tuple="";
            std::string mapVariableName=l.getPredicateName()+"_";
            for(int j=0;j<l.getAriety();j++){
                mapVariableName=mapVariableName+std::to_string(j)+"_";
                tuple=tuple+"undefinedTuples->at(iUndef)->at("+std::to_string(variableIndex)+"),";
                variableIndex++;
            }
            *out << ind << "tuples"<<l.getPredicateName()<<"= &p"<<mapVariableName<<".getValues({"<<tuple.substr(0,tuple.size()-1)<<"});\n";
            *out << ind++ << "if(aggrTupleU==NULL)\n";
            *out << ind << "tuplesU"<<l.getPredicateName()<<"= &u"<<mapVariableName<<".getValues({"<<tuple.substr(0,tuple.size()-1)<<"});\n";
            
            //*out << ind << "std::cout<<\""<<l.getPredicateName()<<"true size : \"<<tuples"<<l.getPredicateName()<<"->size()<<std::endl;\n";
            //*out << ind << "std::cout<<\""<<l.getPredicateName()<<"undef size : \"<<tuplesU"<<l.getPredicateName()<<"->size()<<std::endl;\n";
            *out << --ind << "for(int j"<<variableIndex<<"=0;j"<<variableIndex<<"<tuples"<<l.getPredicateName()<<"->size()+tuplesU"<<l.getPredicateName()<<"->size();j"<<variableIndex<<"++){\n";
            parenthesis++;
                *out << ++ind <<"const Tuple* auxTuple"<<variableIndex<<"=NULL;\n";
                *out << ind <<"bool addedToReason=false;\n";

                *out << ind++ << "if(j"<<variableIndex<<" < tuples"<<l.getPredicateName()<<"->size()){\n";
                //ho un valore definito
                
                *out << ind << "auxTuple"<<variableIndex<<" = tuples"<<l.getPredicateName()<<"->at(j"<<variableIndex<<");\n";
                if(withReason){
                    *out << ind << "const auto & it = tupleToVar.find(*auxTuple"<<variableIndex<<");\n";
                    *out << ind++ << "if(it!=tupleToVar.end()){\n";

                        *out << ind << "addedToReason=true;\n";
                        *out << ind << "reason.push_back(it->second * (auxTuple"<<variableIndex<<"->isNegated() ? -1:1));\n";
                    *out << --ind <<"}//closing if\n";
                }
                *out << ind++ << "if(tuplesU"<<l.getPredicateName()<<" != &EMPTY_TUPLES) {\n";
                *out << ind << "aggrTupleU = NULL;\n";
                *out << --ind << "}\n";
                *out << --ind << "}else{\n";
                //fin'ora è il primo indefinito
                
                *out << ++ind << "auxTuple"<<variableIndex<<" = tuplesU"<<l.getPredicateName()<<"->at(j"<<variableIndex<<"-tuples"<<l.getPredicateName()<<"->size());\n";
                *out << ind << "aggrTupleU = auxTuple"<<variableIndex<<";\n";
                *out << --ind <<"}\n";

        }


        *out << ind++ << "if(aggrTupleU == NULL){\n";
            *out << ind << "std::cout<<\"Tuple undefined not well formed\"<<std::endl;\n";
        *out << --ind << "}else{\n";
            
            //*out << ind << "aggrTupleU->print();\nstd::cout<<\"Propagated\"<<std::endl;\n";
            *out << ++ind << "const auto & it = tupleToVar.find(*aggrTupleU);\n";
            *out << ind++ << "if(it != tupleToVar.end()) {\n";
                *out << ind << "int sign = aggrTupleU->isNegated() ? -1 : 1;\n";
                //*out << ind << "std::cout<<\"Propagating literal \"<<it->second*sign<<std::endl;\n";

                //if(reason != "")
                //*out << ind << "std::cout<<\"Reason size\"<<reason.size()<<std::endl;\n";
                //*out << ind << "std::cout<<\"Propagate aggregate literal \"<<it->second<<std::endl;\n";
        /*if(reason!=""){
            *out << ind++ << "for(int v : reason){\n";
            *out << ind << "unsigned uV = v > 0 ? v : -v; Tuple reasonTuple = atomsTable[uV]; reasonTuple.print(); std::cout<<std::endl;\n";
            *out << --ind << "}\n";
        }*/
                *out << ind << "auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>("<<reason<<")}).first->second;\n";

            *out << --ind <<"}\n";
        *out << --ind <<"}\n";
            
        for(int par = 0;par < parenthesis;par++){
            if(withReason)
                *out << ind << "if(addedToReason) reason.pop_back();\n";
            *out << --ind <<"}\n";
        }
        *out << --ind << "}\n";

    }
    *out << --ind << "}\n";

    
}
void CompilationManager::propagateAggregate(const aspc::ArithmeticRelationWithAggregate* aggregateRelation,std::string& aggregateIdentifier,bool withReason){
    int semiColonPos = aggregateIdentifier.find(":");
    
    std::string pairName = "joinTuples_"+aggregateIdentifier.substr(0,semiColonPos)+"_"+aggregateIdentifier.substr(semiColonPos+1,aggregateIdentifier.size());
    std::string reason = withReason ? "reason" : "";

    if(sharedVariablesMap[aggregateIdentifier]!=""){
        
        *out << ind << "const std::vector<const Tuple*>* undefinedTuples = &u_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggregateIdentifier];
        for(unsigned index : sharedVariablesIndexesMap[aggregateIdentifier] )
            *out << index << "_";
        *out << ".getValues({"<<sharedVariablesMap[aggregateIdentifier]<<"});\n";
        
        

    }else
        *out << ind << "const std::vector<const Tuple*>* undefinedTuples = &" << pairName <<".second->getValues({});\n";
    //*out << ind << "std::cout<<\"Undef size: \"<<undefinedTuples->size()<<std::endl;\n";
    *out << ind++ << "for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){\n";
    //*out << ind << "undefinedTuples->at(iUndef)->print();\n";            
    int k=0;
    std::string tuple="";
    for(unsigned index : aggregateVariablesIndex[aggregateIdentifier]){
        if(k>0)
           tuple+=",";
        tuple+="undefinedTuples->at(iUndef)->at("+std::to_string(index)+")";
        k++;
    }
        if(aggregateRelation->isNegated()){
            *out << ind++ << "if("<< pairName <<"SharedVariables.first->getValues({"<<tuple<<"}).size() == 0 && ";
            *out << pairName<<"SharedVariables.second->getValues({" << tuple <<"}).size() == 1){\n";
            int lit_index=0;
            int ariety=0;
            //*out << ind << "undefinedTuples->at(iUndef)->print();\n";
            for(const aspc::Literal& l: aggregateRelation->getAggregate().getAggregateLiterals()){
                *out << ind++ << "{\n";
                    std::string lit = "";
                    bool checkVar=false;
                    for(int i=0;i<l.getAriety();i++){
                        if(l.isVariableTermAt(i)){
                            if(checkVar)
                                lit+=",";
                            lit+="undefinedTuples->at(iUndef)->at("+std::to_string(ariety+i)+")";
                            checkVar=true;
                        }
                    }
                    
                    *out << ind << "const Tuple* aggrTupleU = u"<<l.getPredicateName()<<".find({"<<lit<<"});\n";
                    *out << ind++ << "if(aggrTupleU != NULL){\n";
                        //*out << ind << "std::cout<<\"Propagating literal\"<<std::endl;\n";
                        *out << ind << "const auto & it = tupleToVar.find(*aggrTupleU);\n";
                        *out << ind++ << "if(it != tupleToVar.end()) {\n";
                            *out << ind << "int sign = aggrTupleU->isNegated() ? 1:-1;\n";
                          //  *out << ind << "atomsTable[it->second].print();\n";
                            *out << ind << "auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>("<<reason<<")}).first->second;\n";
                        *out << --ind << "}\n";
                    *out << --ind << "}\n";
                *out << --ind << "}\n";
                ariety+=l.getAriety();

            }
            *out << --ind << "}\n";
        }else{
            *out << ind++ << "if("<< pairName <<"SharedVariables.first->getValues({"<<tuple<<"}).size() == 0){\n";
            //propago tutte le joinTuple che hanno 1 e 1 solo letterale indefinito
            int variableIndex=0;
            int parenthesis=0;
            *out << ind << "const Tuple* aggrTupleU=NULL;\n";
            
            for(const aspc::Literal& l: aggregateRelation->getAggregate().getAggregateLiterals()){
            
                *out << ind << "const std::vector<const Tuple*>* tuples"<<l.getPredicateName()<<";\n";
                *out << ind << "const std::vector<const Tuple*>* tuplesU"<<l.getPredicateName()<<"=&EMPTY_TUPLES;\n";
                
                std::string tuple="";
                std::string mapVariableName=l.getPredicateName()+"_";
                for(int j=0;j<l.getAriety();j++){
                    mapVariableName=mapVariableName+std::to_string(j)+"_";
                    tuple=tuple+"undefinedTuples->at(iUndef)->at("+std::to_string(variableIndex)+"),";
                    variableIndex++;
                }
                *out << ind << "tuples"<<l.getPredicateName()<<"= &p"<<mapVariableName<<".getValues({"<<tuple.substr(0,tuple.size()-1)<<"});\n";
                *out << ind++ << "if(aggrTupleU==NULL)\n";
                *out << ind << "tuplesU"<<l.getPredicateName()<<"= &u"<<mapVariableName<<".getValues({"<<tuple.substr(0,tuple.size()-1)<<"});\n";
                
                //*out << ind << "std::cout<<\""<<l.getPredicateName()<<"true size : \"<<tuples"<<l.getPredicateName()<<"->size()<<std::endl;\n";
                //*out << ind << "std::cout<<\""<<l.getPredicateName()<<"undef size : \"<<tuplesU"<<l.getPredicateName()<<"->size()<<std::endl;\n";
                *out << --ind << "for(int j"<<variableIndex<<"=0;j"<<variableIndex<<"<tuples"<<l.getPredicateName()<<"->size()+tuplesU"<<l.getPredicateName()<<"->size();j"<<variableIndex<<"++){\n";
                parenthesis++;
                    *out << ++ind <<"const Tuple* auxTuple"<<variableIndex<<"=NULL;\n";
                    *out << ind <<"bool addedToReason=false;\n";

                    *out << ind++ << "if(j"<<variableIndex<<" < tuples"<<l.getPredicateName()<<"->size()){\n";
                    //ho un valore definito
                    
                    *out << ind << "auxTuple"<<variableIndex<<" = tuples"<<l.getPredicateName()<<"->at(j"<<variableIndex<<");\n";
                    if(withReason){
                        *out << ind << "const auto & it = tupleToVar.find(*auxTuple"<<variableIndex<<");\n";
                        *out << ind++ << "if(it!=tupleToVar.end()){\n";

                            *out << ind << "addedToReason=true;\n";
                            *out << ind << "reason.push_back(it->second * (auxTuple"<<variableIndex<<"->isNegated() ? -1:1));\n";
                        *out << --ind <<"}//closing if\n";
                    }
                    *out << ind++ << "if(tuplesU"<<l.getPredicateName()<<" != &EMPTY_TUPLES) {\n";
                    //*out << ind << "aggrTupleU = NULL;\n";
                    *out << --ind << "}\n";
                    *out << --ind << "}else{\n";
                    //fin'ora è il primo indefinito
                    
                    *out << ++ind << "auxTuple"<<variableIndex<<" = tuplesU"<<l.getPredicateName()<<"->at(j"<<variableIndex<<"-tuples"<<l.getPredicateName()<<"->size());\n";
                    *out << ind << "aggrTupleU = auxTuple"<<variableIndex<<";\n";
                    *out << --ind <<"}\n";

            }


            *out << ind++ << "if(aggrTupleU == NULL){\n";
                *out << ind << "std::cout<<\"Tuple undefined not well formed\"<<std::endl;\n";
            *out << --ind << "}else{\n";
                
                //*out << ind << "aggrTupleU->print();\nstd::cout<<\"Propagated\"<<std::endl;\n";
                *out << ++ind << "const auto & it = tupleToVar.find(*aggrTupleU);\n";
                *out << ind++ << "if(it != tupleToVar.end()) {\n";
                    *out << ind << "int sign = aggrTupleU->isNegated() ? -1 : 1;\n";
                    //if(reason != "")
                    //*out << ind << "std::cout<<\"Reason size\"<<reason.size()<<std::endl;\n";
                    //*out << ind << "std::cout<<\"Propagate aggregate literal \"<<it->second<<std::endl;\n";
            /*if(reason!=""){
                *out << ind++ << "for(int v : reason){\n";
                *out << ind << "unsigned uV = v > 0 ? v : -v; Tuple reasonTuple = atomsTable[uV]; reasonTuple.print(); std::cout<<std::endl;\n";
                *out << --ind << "}\n";
            }*/
                    *out << ind << "auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>("<<reason<<")}).first->second;\n";

                *out << --ind <<"}\n";
            *out << --ind <<"}\n";
                
            for(int par = 0;par < parenthesis;par++){
                if(withReason)
                    *out << ind << "if(addedToReason) reason.pop_back();\n";
                *out << --ind <<"}\n";
            }
            *out << --ind << "}\n";

        }
            //*out << ind << "std::cout<<\"propagating\";undefinedTuples->at(iUndef)->print();std::cout<<std::endl;\n";
            
            

    *out << --ind << "}\n";
    
}
std::string CompilationManager::getSharedVariablesValues(std::string* sharedVariables)const{
    string value="";
    int comma = sharedVariables->find(",");
    int i=0;
    while(comma!=std::string::npos){
        if(i>0)
            value+="+\"_\"+";
        value+="std::to_string("+sharedVariables->substr(0,comma)+")";
        *sharedVariables=sharedVariables->substr(comma+1,sharedVariables->size());
        comma=sharedVariables->find(",");
    }
    return value;
}
void CompilationManager::countRemainingJoinTuples(std::string aggrIdentifier,std::string pairName,const aspc::ArithmeticRelationWithAggregate* aggregateRelation){
    std::string varProjection="";
    int currentVar=0;
    for(int varIndex : aggregateVariablesIndex[aggrIdentifier]){
        if(currentVar>0)
            varProjection+=",";
        varProjection+="joinTupleUndef->at("+std::to_string(varIndex)+")";
        currentVar++;
    }
    *out << ind << "int actualSize"<<aggregateRelation->getFormulaIndex()<<" = "<<pairName<<"SharedVariables.first->size();\n";
    //*out << ind << "std::cout<<\"True ActualSize: \"<<actualSize"<<aggregateRelation->getFormulaIndex()<<"<<std::endl;\n";
    *out << ind << "std::set<std::vector<int>> alreadyCounted"<<aggregateRelation->getFormulaIndex()<<";\n";
    std::string sharedVars = sharedVariablesMap[aggrIdentifier]; 
    
    if( sharedVars ==""){
        *out << ind++ << "for(const Tuple * joinTupleUndef : u_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<".getValues({})){\n";
    }else{
        std::string sharedVarProjection="";
        for(unsigned index : sharedVariablesIndexesMap[aggrIdentifier]){
            sharedVarProjection+=std::to_string(index)+"_";
        }
        *out << ind++ << "for(const Tuple * joinTupleUndef : u_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<sharedVarProjection<<".getValues({"<<sharedVars<<"})){\n";
    }

    *out << ind++ << "if("<<pairName<<"SharedVariables.first->getValues({"<<varProjection<<"}).size()==0){\n";
        *out << ind++ << "if("<<pairName<<"SharedVariables.second->getValues({"<<varProjection<<"}).size()>0){\n";
            *out << ind << "std::vector<int> key({"<<varProjection<<"});\n";
            *out << ind << "alreadyCounted"<<aggregateRelation->getFormulaIndex()<<".insert(key);\n";
        *out << --ind << "}\n";
    *out << --ind << "}\n";
        
    *out << --ind << "}\n";
    *out << ind << "actualSize"<<aggregateRelation->getFormulaIndex()<<"+=alreadyCounted"<<aggregateRelation->getFormulaIndex()<<".size();\n";
    //*out << ind << "std::cout<<\"True + Undef ActualSize: \"<<actualSize"<<aggregateRelation->getFormulaIndex()<<"<<std::endl;\n";

}
void CompilationManager::evaluateAggregateAsLast(bool withReason, const aspc::ArithmeticRelationWithAggregate* aggregateRelation, std::vector<unsigned>& joinOrder,int i,const aspc::Rule& r,bool propFirstAggr){
    bool starterAggregate = r.getFormulas()[joinOrder[0]]->containsAggregate();

    bool doubleAggregate = r.getArithmeticRelationsWithAggregate().size()>1 && (!starterAggregate || r.getBodyLiterals().size()<1);
    /*if (withReason){
        *out << ind << "std::cout<<\"For on Facts"<<r.getRuleId()<<"\"<<std::endl;\n";
    }else{
        *out << ind << "std::cout<<\"Decision Level If"<<r.getRuleId()<<"\"<<std::endl;\n";
    }*/
    if(r.getBodySize()<=1 && withReason){
        *out << ind++ << "if(";
        for(int i=0;i<aggregateRelation->getAggregate().getAggregateLiterals().size();i++){
            if(i>0)
                *out << " || ";
            *out << "starter.getPredicateName() == &_"<<aggregateRelation->getAggregate().getAggregateLiterals()[i].getPredicateName();
        }
        *out << "){\n";
    }

    
    std::string secondCondition="";

    if(doubleAggregate || propFirstAggr){
        //valuto il doppio aggregato per constraint di uguaglianza alla fine
        //*out << ind << "std::cout<<\"Evaluating double Aggregate ad the end\"<<std::endl;\n";

        std::string outSideAggrIdentifier = std::to_string(r.getRuleId())+":"+std::to_string(joinOrder[i]);
        std::string* sharedVariables = &sharedVariablesMap[outSideAggrIdentifier];
        std::string outSidePairName = "joinTuples_"+std::to_string(r.getRuleId())+"_"+std::to_string(joinOrder[i]);
        *out << ind << "std::pair<AuxMap*,AuxMap*> "<<outSidePairName<<";\n";
        *out << ind << outSidePairName<<".first = &p_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+outSideAggrIdentifier]<<";\n";
        *out << ind << outSidePairName<<".second = &u_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+outSideAggrIdentifier]<<";\n";

        if(*sharedVariables == ""){
            *out << ind << "std::pair<AuxMap*,AuxMap*> "<<outSidePairName<<"SharedVariables;\n";
            std::string aggregateVariableIndexToString="";
            for(unsigned index : aggregateVariablesIndex[outSideAggrIdentifier])
                aggregateVariableIndexToString+=std::to_string(index)+"_";

            
            *out << ind << outSidePairName << "SharedVariables.first = &p_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+outSideAggrIdentifier]<<aggregateVariableIndexToString<<";\n";
            *out << ind << outSidePairName << "SharedVariables.second = &u_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+outSideAggrIdentifier]<<aggregateVariableIndexToString<<";\n";
            
        }else{
            *out << ind++ << "{\n";
            checkExistsShareVariableMap(r.getRuleId(),aggregateRelation->getFormulaIndex(),*sharedVariables,true);
                //*out << ind << "std::cout<<\"Create sharedVarTuple\"<<std::endl;\n";

                std::string sharedVarToString="";
                for(int i=0;i<sharedVariablesIndexesMap[outSideAggrIdentifier].size();i++){
                    sharedVarToString+=std::to_string(sharedVariablesIndexesMap[outSideAggrIdentifier][i])+"_";
                }

                *out << ind << "const std::vector<const Tuple*>* trueJoinTuples = &p_"
                        <<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+outSideAggrIdentifier]
                                <<sharedVarToString<<".getValues({"<<*sharedVariables<<"});\n"; 
                *out << ind << "const std::vector<const Tuple*>* undefJoinTuples = &u_"
                        <<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+outSideAggrIdentifier]
                                <<sharedVarToString<<".getValues({"<<*sharedVariables<<"});\n"; 
                saveTuples("trueJoinTuples","w"+aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+outSideAggrIdentifier],aggregateRelation->getFormulaIndex(),r.getRuleId());
                //*out << ind << "std::cout<<\"true updated\"<<std::endl;\n";

                saveTuples("undefJoinTuples","u"+aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+outSideAggrIdentifier],aggregateRelation->getFormulaIndex(),r.getRuleId());

            *out << --ind << "}\n";
            *out << --ind << "}\n";

            *out << ind << "std::pair<AuxMap*,AuxMap*> "<<outSidePairName<<"SharedVariables = *sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<joinOrder[i]<<"[std::vector<int>({"<<*sharedVariables<<"})];\n";

        }
        if(aggregateRelation->isNegated()){
            countRemainingJoinTuples(outSideAggrIdentifier,outSidePairName,aggregateRelation);
        }

        *out << ind++ << "if(tupleU==NULL){\n";

        if(withReason){
            //aggiungo alla ragione le joinTuple del primo aggregato
            //*out << ind << "std::cout<<\"starting build reason\"<<std::endl;\n";
            buildReason(outSideAggrIdentifier,aggregateRelation,true);    
            //*out << ind << "std::cout<<\"reason size: \"<<reason.size()<<std::endl;\n";    
            
        }
        std::string plusOne = aggregateRelation->isPlusOne() ? "+1":"";

        if(aggregateRelation->isNegated()){
            *out << ind++ << "if(!(actualSize"<<joinOrder[i]<<aggregateRelation->getCompareTypeAsString()<<aggregateRelation->getGuard().getStringRep()<<plusOne<<")){\n";
        }else{
            *out << ind++ << "if("<<outSidePairName<<"SharedVariables.first->size()"<<aggregateRelation->getCompareTypeAsString()<<aggregateRelation->getGuard().getStringRep()<<plusOne<<"){\n";
        }
        if(propFirstAggr)
            return;
        if(aggregateRelation->isNegated())
            secondCondition = " && (!(actualSize"+std::to_string(joinOrder[i])+aggregateRelation->getCompareTypeAsString()+aggregateRelation->getGuard().getStringRep()+"+1))";
        else
            secondCondition = " && ("+outSidePairName+"SharedVariables.first->size()"+aggregateRelation->getCompareTypeAsString()+aggregateRelation->getGuard().getStringRep()+")";
        
        for(int j = 0; j<joinOrder.size();j++){
            if(j!=i && joinOrder[j]!=r.getBodySize() && r.getFormulas()[joinOrder[j]]->containsAggregate()){
                aggregateRelation = (aspc::ArithmeticRelationWithAggregate*) r.getFormulas()[joinOrder[j]];
                i=j;
                break;
            }
        }
        //primo aggregato vero avvio la valutazione del secondo 
            
    }

    //*out << ind << "std::cout<<\"ok\"<<std::endl;\n";
    std::string aggrIdentifier = std::to_string(r.getRuleId())+":"+std::to_string(joinOrder[i]);
    std::string* sharedVariables = &sharedVariablesMap[aggrIdentifier];
    std::string pairName = "joinTuples_"+std::to_string(r.getRuleId())+"_"+std::to_string(joinOrder[i]);
    *out << ind << "std::pair<AuxMap*,AuxMap*> "<<pairName<<";\n";
    *out << ind << pairName<<".first = &p_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<";\n";
    *out << ind << pairName<<".second = &u_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<";\n";
    
    if(*sharedVariables == ""){
        *out << ind << "std::pair<AuxMap*,AuxMap*> "<<pairName<<"SharedVariables;\n";
        std::string aggregateVariableIndexToString="";
        for(unsigned index : aggregateVariablesIndex[aggrIdentifier])
            aggregateVariableIndexToString+=std::to_string(index)+"_";

        
        *out << ind << pairName << "SharedVariables.first = &p_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<aggregateVariableIndexToString<<";\n";
        *out << ind << pairName << "SharedVariables.second = &u_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<aggregateVariableIndexToString<<";\n";
        
    }else{
        *out << ind++ << "{\n";
        checkExistsShareVariableMap(r.getRuleId(),aggregateRelation->getFormulaIndex(),*sharedVariables,true);
            std::string sharedVarToString="";
            for(int i=0;i<sharedVariablesIndexesMap[aggrIdentifier].size();i++){
                sharedVarToString+=std::to_string(sharedVariablesIndexesMap[aggrIdentifier][i])+"_";
            }

            *out << ind << "const std::vector<const Tuple*>* trueJoinTuples = &p_"
                    <<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]
                            <<sharedVarToString<<".getValues({"<<*sharedVariables<<"});\n"; 
            *out << ind << "const std::vector<const Tuple*>* undefJoinTuples = &u_"
                    <<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]
                            <<sharedVarToString<<".getValues({"<<*sharedVariables<<"});\n"; 
            saveTuples("trueJoinTuples","w"+aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier],aggregateRelation->getFormulaIndex(),r.getRuleId());
            //*out << ind << "std::cout<<\"true updated\"<<std::endl;\n";

            saveTuples("undefJoinTuples","u"+aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier],aggregateRelation->getFormulaIndex(),r.getRuleId());

        *out << --ind << "}\n";
        *out << --ind << "}\n";

        *out << ind << "std::pair<AuxMap*,AuxMap*> "<<pairName<<"SharedVariables = *sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<joinOrder[i]<<"[std::vector<int>({"<<*sharedVariables<<"})];\n";
        
    }    
    
    //l'aggregato viene valutato per ultimo quindi ho un bound per tutte le variabili condivise
    if(aggregateRelation->isNegated())
        countRemainingJoinTuples(aggrIdentifier,pairName,aggregateRelation);
    //*out<<ind<<"std::cout<<\"Evaluating tupleU\"<<std::endl;\n";
    if(!doubleAggregate)
        *out << ind++ << "if(tupleU==NULL){\n";

        //il corpo senza aggregato è vero
        if(withReason){
            //*out << ind << "std::cout<<\"starting build reason\"<<std::endl;\n";
            buildReason(aggrIdentifier,aggregateRelation,r.getArithmeticRelationsWithAggregate().size()<=1);    
            //*out << ind << "std::cout<<\"reason built\"<<std::endl;\n";  
            //*out << ind << "std::cout<<\"StarterForReason"<<r.getFormulas()[joinOrder[0]]->isLiteral()<<"\"<<std::endl;\n";
            for (unsigned j = 0; j < joinOrder.size()-1; j++) {
                if (r.getFormulas()[joinOrder[j]]->isLiteral()) {
                    *out << ind++ << "if(tuple"<<j<<"!=tupleU){\n";
                        //*out << ind << "tuple"<<j<<"->print();\n";
                        *out << ind << "const auto & it_reason"<<j<<" = tupleToVar.find(Tuple(*tuple"<<j<<"));\n";
                        *out << ind++ << "if(it_reason"<<j<<"!=tupleToVar.end())\n";
                            *out << ind-- << "reason.push_back(it_reason"<<j<<"->second * (tuple"<<j<<"->isNegated() ? -1:1));\n";
                    *out << --ind <<"}\n";
                }
            }
            *out << ind << "const auto & it_reason_starter = tupleToVar.find(Tuple(starter));\n";
            *out << ind++ << "if(it_reason_starter!=tupleToVar.end())\n";
                *out << ind-- << "reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));\n";
            //*out << ind << "std::cout<<\"reason size: \"<<reason.size()<<std::endl;\n";    
            
        }
        std::string reason = withReason ? "reason" : "";
        std::string plusOne = aggregateRelation->isPlusOne() ? "+1":""; 

        if(aggregateRelation->isNegated()){
            *out << ind++ << "if(!(actualSize"<<joinOrder[i]<<aggregateRelation->getCompareTypeAsString()<<aggregateRelation->getGuard().getStringRep()<<plusOne<<")){\n";
            //*out << ind << "std::cout<<\"Error size actual: \"<<R<<\" \"<<actualSize"<<joinOrder[i]<<"<<std::endl;\n";
        }else{
            *out << ind++ << "if("<<pairName<<"SharedVariables.first->size()"<<aggregateRelation->getCompareTypeAsString()<<aggregateRelation->getGuard().getStringRep()<<plusOne<<"){\n";
            //*out << ind << "std::cout<<\"Error size map: \"<<"<<pairName<<"SharedVariables.first->size()<<std::endl;\n";
        }
                std::string prop = r.getFormulas()[0]->containsAggregate() && !propFirstAggr ? "on last aggregate starting from aggregate":"";
                if(prop == "" && doubleAggregate){
                    prop = "on last aggregate starting from literal";
                }
                prop+=std::to_string(r.getRuleId());
                /*if(aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()=="asgn"){
                    *out<<ind << "std::cout<<X<<std::endl;\n";
                    *out << ind << "std::cout<<\"Error TrueKeySize: \"<<p_asgn_X_L_0_.getValues({X}).size()<<std::endl;\n";
                    *out << ind << "std::cout<<\"Error UndefKeySize: \"<<u_asgn_X_L_0_.getValues({X}).size()<<std::endl;\n";
                    
                }*/
                if(withReason)
                *out << ind << "starter.print();\n";
                *out << ind << "std::cout<<\"conflict detected in propagator"<<prop<<"\"<<std::endl;\n";
                *out << ind << "propagatedLiteralsAndReasons.insert({-1, std::vector<int>("<<reason<<")});\n";

            *out << --ind << "}else{\n";
            //std::string aggrIdentifier(std::to_string(r.getRuleId())+":"+std::to_string(aggregateRelation->getFormulaIndex()));
            ind++;
                //aggregato non vero
                
                if(aggregateRelation->isNegated()){
                    *out << ind++ << "if(actualSize"<<joinOrder[i]<<" == "<<aggregateRelation->getGuard().getStringRep()<<plusOne<<"){\n";
                }
                else{
                    std::string minusOne = aggregateRelation->getCompareTypeAsString() == ">=" ? "-1" : "";
                    *out << ind++ << "if("<<pairName<<"SharedVariables.first->size() == "<<aggregateRelation->getGuard().getStringRep()<<plusOne<<minusOne<<"){\n";
                }
                    //*out << ind << "std::cout<<\"Starting propagation last aggr\"<<std::endl;\n";            
                    //*out << ind << "std::cout<<\"total size: \"<<actualSize<<std::endl;\n";            
                    //*out << ind << "std::cout<<\"undef size: \"<<"<<pairName<<"SharedVariables.second->size()<<std::endl;\n";            

                    //se un'altra chiave verrà aggiunta l'aggregato diventa vero e si viola il constraint

                    propagateAggregate(aggregateRelation,aggrIdentifier,withReason);

                    *out << --ind << "}\n";
                *out << --ind << "}\n";
                if(doubleAggregate){
                    *out << --ind << "}//close ext aggregate if\n";
                }
                
            *out << --ind << "}else{\n";
            //*out << ind << "std::cout<<\"Undef not in aggregate\"<<std::endl;\n";
            //ho trovato un indefinito prima dell'aggregato
            if(withReason){
                buildReason(aggrIdentifier,aggregateRelation,!(starterAggregate && r.getArithmeticRelationsWithAggregate().size()>1));
                //l'aggregato viene valutato alla fine quindi prendo le tuple precedenti
                for (unsigned j = 0; j < joinOrder.size()-1; j++) {
                    if (r.getFormulas()[joinOrder[j]]->isLiteral()) {
                        *out << ind++ << "if(tuple"<<j<<"!=tupleU){\n";
                            *out << ind << "const auto & it_reason"<<j<<" = tupleToVar.find(Tuple(*tuple"<<j<<"));\n";
                            *out << ind++ << "if(it_reason"<<j<<"!=tupleToVar.end())\n";
                                *out << ind-- << "reason.push_back(it_reason"<<j<<"->second * (tuple"<<j<<"->isNegated() ? -1:1));\n";
                        *out << --ind <<"}\n";
                    }
                }
                if(r.getFormulas()[joinOrder[0]]->containsAggregate()){
                    *out << ind << "const auto & it_reason_starter = tupleToVar.find(Tuple(starter));\n";
                    *out << ind++ << "if(it_reason_starter!=tupleToVar.end())\n";
                        *out << ind-- << "reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));\n";
                }
                if(doubleAggregate){
                    for(const aspc::ArithmeticRelationWithAggregate& ar : r.getArithmeticRelationsWithAggregate()){
                        if(ar.getFormulaIndex()!=joinOrder[i]){
                            buildReason(std::to_string(r.getRuleId())+":"+std::to_string(ar.getFormulaIndex()),&ar,false);
                        }
                    }
                }
            }
            if(secondCondition!=""){
                *out << ind << "std::pair<AuxMap*,AuxMap*> "<<pairName<<";\n";
                *out << ind << pairName<<".first = &p_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<";\n";
                *out << ind << pairName<<".second = &u_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<";\n";
                
                if(*sharedVariables == ""){
                    *out << ind << "std::pair<AuxMap*,AuxMap*> "<<pairName<<"SharedVariables;\n";
                    std::string aggregateVariableIndexToString="";
                    for(unsigned index : aggregateVariablesIndex[aggrIdentifier])
                        aggregateVariableIndexToString+=std::to_string(index)+"_";

                    
                    *out << ind << pairName << "SharedVariables.first = &p_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<aggregateVariableIndexToString<<";\n";
                    *out << ind << pairName << "SharedVariables.second = &u_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<aggregateVariableIndexToString<<";\n";
                    
                }else{
                    *out << ind++ << "{\n";
                    checkExistsShareVariableMap(r.getRuleId(),aggregateRelation->getFormulaIndex(),*sharedVariables,true);
                        std::string sharedVarToString="";
                        for(int i=0;i<sharedVariablesIndexesMap[aggrIdentifier].size();i++){
                            sharedVarToString+=std::to_string(sharedVariablesIndexesMap[aggrIdentifier][i])+"_";
                        }

                        *out << ind << "const std::vector<const Tuple*>* trueJoinTuples = &p_"
                                <<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]
                                        <<sharedVarToString<<".getValues({"<<*sharedVariables<<"});\n"; 
                        *out << ind << "const std::vector<const Tuple*>* undefJoinTuples = &u_"
                                <<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]
                                        <<sharedVarToString<<".getValues({"<<*sharedVariables<<"});\n"; 
                        saveTuples("trueJoinTuples","w"+aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier],aggregateRelation->getFormulaIndex(),r.getRuleId());
                        //*out << ind << "std::cout<<\"true updated\"<<std::endl;\n";

                        saveTuples("undefJoinTuples","u"+aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier],aggregateRelation->getFormulaIndex(),r.getRuleId());

                    *out << --ind << "}\n";
                    *out << --ind << "}\n";

                    *out << ind << "std::pair<AuxMap*,AuxMap*> "<<pairName<<"SharedVariables = *sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<joinOrder[i]<<"[std::vector<int>({"<<*sharedVariables<<"})];\n";
                    
                }
                if(aggregateRelation->isNegated())
                    countRemainingJoinTuples(aggrIdentifier,pairName,aggregateRelation);
    
            }
            if(!aggregateRelation->isNegated()){
                *out << ++ind << "if("<<pairName<<"SharedVariables.first->size()"<<aggregateRelation->getCompareTypeAsString()<<aggregateRelation->getGuard().getStringRep()<<plusOne<<""<<secondCondition<<"){\n";

            }else{
                //std::string subUndefKey = withReason ? " - alreadyCounted.size()" : "";
                *out << ++ind << "if(!(actualSize"<<joinOrder[i]<<aggregateRelation->getCompareTypeAsString()<<aggregateRelation->getGuard().getStringRep()<<plusOne<<")"<<secondCondition<<"){\n";
            }

                *out << ++ind << "const auto & it = tupleToVar.find(*tupleU);\n";
                *out << ind++ << "if(it != tupleToVar.end()) {\n";
                    //*out << ind << "std::cout<<\"External prop\";tupleU->print();std::cout<<std::endl;\n";
                    *out << ind << "int sign = tupleU->isNegated() ? -1 : 1;\n";
                    *out << ind << "auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>("<<reason<<")}).first->second;\n";
                *out << --ind <<"}\n";
            *out << --ind << "}\n";
                
        *out << --ind << "}\n";

        if(r.getBodySize()<=1 && withReason){
            *out << --ind << "}\n";
        }
}
void CompilationManager::checkSharedVariablesOnUndefTuple(std::string sharedVars,std::string aggrIdentifier){
    int pos = sharedVars.find(",");
    unsigned currentVar=0;
    *out << ind++ << "if(";
    while(pos!=std::string::npos){
        if(currentVar>0)
            *out << " && ";
        *out << "joinTupleUndef->at(" <<sharedVariablesIndexesMap[aggrIdentifier][currentVar]<<") == "<<sharedVars.substr(0,pos);
        sharedVars=sharedVars.substr(pos+1,sharedVars.length());
        pos = sharedVars.find(",");
        currentVar++;
    }
    if(sharedVars!=""){
        if(currentVar>0)
            *out << " && ";
        *out << "joinTupleUndef->at(" <<sharedVariablesIndexesMap[aggrIdentifier][currentVar]<<") == "<<sharedVars.substr(0,sharedVars.length());
    }
    *out << " ){\n";
}
void CompilationManager::printCanPropagateIf(std::string aggrIdentifier,const aspc::ArithmeticRelationWithAggregate* aggr,std::string op){
    std::string ruleId = aggrIdentifier.substr(0,aggrIdentifier.find(":"));
    std::string index = aggrIdentifier.substr(aggrIdentifier.find(":")+1,aggrIdentifier.length());

        if(aggr->isNegated()){
            std::string plusOne = aggr->isPlusOne() ? "+1": "";
            *out << ind++ << "if(undefPlusTrue == "<<aggr->getGuard().getStringRep()<<plusOne<<"){\n";
        }else{
            std::string plusOne = aggr->isPlusOne() ? "": "-1";
            *out << ind++ << "if(sharedVarTuple"<<ruleId<<"_"<<index<<".second"<<op<<"first->size() == "<<aggr->getGuard().getStringRep()<<plusOne<<"){\n";
        }
}

void CompilationManager::printAggregateTrueIf(std::string aggrIdentifier,const aspc::ArithmeticRelationWithAggregate* aggr,std::string joinTupleName,std::string op,bool isBound){
    std::string sharedVars = sharedVariablesMap[aggrIdentifier];
    std::string ruleId = aggrIdentifier.substr(0,aggrIdentifier.find(":"));
    std::string index = aggrIdentifier.substr(aggrIdentifier.find(":")+1,aggrIdentifier.length());
    std::string plusOne = aggr->isPlusOne() ? "+1" : "";
    std::string aggregateVarProjection = "";
    std::string pair = "sharedVarTuple"+ruleId+"_"+index;
    for(unsigned var : aggregateVariablesIndex[aggrIdentifier]){
        aggregateVarProjection+=std::to_string(var)+"_";
    }
    if(!aggr->isNegated()){
        if(isBound)
            *out << ind++ << "if("<<pair<<".second"<<op<<"first->size()"<<aggr->getCompareTypeAsString()<<aggr->getGuard().getStringRep()<<plusOne<<"){\n";
        else {
            *out << ind << "int count"<<ruleId<<"_"<<aggr->getFormulaIndex()<<" = "<<pair<<".second"<<op<<"first->size();\n";
            *out << ind++ << "{\n";
        }

    }else{
        if(sharedVars=="")
            *out << ind << "int undefPlusTrue = p_"<<joinTupleName<<aggregateVarProjection<<".size();\n";
        else 
            *out << ind << "int undefPlusTrue = sharedVarTuple"<<ruleId<<"_"<<index<<".second"<<op<<"first->size();\n";
        std::string sharedVarsProjection = "";
        for(unsigned index : sharedVariablesIndexesMap[aggrIdentifier])
            sharedVarsProjection+=std::to_string(index)+"_";

        std::string tupleProjection="";
        for(unsigned index : aggregateVariablesIndex[aggrIdentifier])
            tupleProjection += "undefTuple->at("+std::to_string(index)+"),";
        tupleProjection = tupleProjection.substr(0,tupleProjection.size()-1);

        
        *out << ind << "std::set<std::vector<int>> distinctKeyNotTrue;\n"; 
        *out << ind++ << "for(const Tuple * undefTuple : u_"<<joinTupleName<<sharedVarsProjection<<".getValues({"<<sharedVars<<"})){\n";
            if(sharedVars=="")
                *out << ind++ << "if(p_"<<joinTupleName<<aggregateVarProjection<<".getValues({"<<tupleProjection<<"}).size()<=0)\n";
            else
                *out << ind++ << "if("<<pair<<".second"<<op<<"first->getValues({"<<tupleProjection<<"}).size()<=0)\n";
            *out << ind-- << "distinctKeyNotTrue.insert(std::vector<int>({"<<tupleProjection<<"}));\n";
        *out << --ind << "}\n";
        *out << ind << "undefPlusTrue+=distinctKeyNotTrue.size();\n";
        if(isBound){
            *out << ind << "//"<<aggr->getGuard().getStringRep()<<"\n";
            *out << ind++ << "if(!(undefPlusTrue"<<aggr->getCompareTypeAsString()<<aggr->getGuard().getStringRep()<<plusOne<<")){\n";
        }else{
            *out << ind << "int count"<<ruleId<<"_"<<aggr->getFormulaIndex()<<" = undefPlusTrue;\n";
            *out << ind++ << "{\n";
        }
    }
}
void CompilationManager::evaluationEndingWithAggregate(const aspc::Rule & r,std::vector<unsigned> joinOrder,unsigned start){
    const std::vector<const aspc::Formula*>& body = r.getFormulas();
    bool reason = start != joinOrder.size();
    std::string printReason = reason ? "reason":"";
    int closingParenthesis=0;
    std::unordered_set<std::string> boundVariables;
    for(int i=0;i<joinOrder.size();i++){
        const aspc::Formula* f = r.getFormulas()[joinOrder[i]];
        if(f->containsAggregate()){

        }else if (i != 0 || start == r.getBodySize()) {
            //std::cout<<"Evaluating literal"<<boundVariables.count("X")<<std::endl;
            if (f->isLiteral()) {
                aspc::Literal* l = (aspc::Literal*)f;
                if (l->isNegated() || l->isBoundedLiteral(boundVariables)) {
                    if (mode == LAZY_MODE) {
                        std::string negation = "";
                        if (l->isNegated()) {
                            negation = "!";
                        }
                        *out << ind << "const Tuple * tuple" << i << " = w" << l->getPredicateName() << ".find({";
                        printLiteralTuple(l);
                        *out << "});\n";
                        *out << ind++ << "if(" << negation << "tuple" << i << "){\n";
                        closingParenthesis++;
                    } else {
                        //mode == EAGER_MODE
                        bool appearsBefore = literalPredicateAppearsBeforeSameSign(body, joinOrder, i);
                        if(appearsBefore && l->isPositiveLiteral()) {
                            *out << ind << "const Tuple * tuple" << i << " = NULL;\n";
                            *out << ind++ << "if(tupleU && tupleU->getPredicateName() == &_"<<l->getPredicateName()<<"){\n"; 
                            *out << ind << "const Tuple * undefRepeatingTuple = (u" << l->getPredicateName() << ".find({";
                            printLiteralTuple(l);
                            *out << "}));\n";
                            *out << ind++ << "if(tupleU == undefRepeatingTuple){\n"; 
                            *out << ind << "tuple" << i << " = undefRepeatingTuple;\n";
                            *out << --ind << "}\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "if(!tuple"<<i<<"){\n;"; 
                            *out << ind << "tuple" << i << " = (w" << l->getPredicateName() << ".find({";
                            printLiteralTuple(l);
                            *out << "}));\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "if(!tuple"<<i<<" && !tupleU){\n;"; 
                            *out << ind << "tuple" << i << " = tupleU = (u" << l->getPredicateName() << ".find({";
                            printLiteralTuple(l);
                            *out << "}));\n";
                            *out << --ind << "}\n";
                            *out << ind << "tupleUNegated = false;\n";
                            *out << ind++ << "if(tuple" << i << "){\n";
                            closingParenthesis++;                      
                        }
                        else if(appearsBefore && l->isNegated()) {
                            *out << ind << "const Tuple * tuple" << i << " = NULL;\n";
                            *out << ind << "const Tuple negativeTuple = Tuple({";
                            printLiteralTuple(l);
                            *out << "}, &_" << l->getPredicateName() << ", true);\n";
                            *out << ind++ << "if(tupleU && tupleU->getPredicateName() == &_"<<l->getPredicateName()<<"){\n;"; 
                            *out << ind << "const Tuple * undefRepeatingTuple = (u" << l->getPredicateName() << ".find({";
                            printLiteralTuple(l);
                            *out << "}));\n";
                            *out << ind++ << "if(tupleU == undefRepeatingTuple){\n"; 
                            *out << ind << "tuple" << i << " = undefRepeatingTuple;\n";
                            *out << --ind << "}\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "if(!tuple"<<i<<"){\n;"; 
                            *out << ind++ << "if(!(w" << l->getPredicateName() << ".find({";
                            printLiteralTuple(l);
                            *out << "}))){\n";
                            *out << ind << "tuple" << i << " = &negativeTuple;\n";
                            *out << --ind << "}\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "if(tuple" << i << "){\n";
                            closingParenthesis++;     
                        }
                        else if (l->isNegated()) {
                            *out << ind << "const Tuple negativeTuple = Tuple({";
                            printLiteralTuple(l);
                            *out << "}, &_" << l->getPredicateName() << ", true);\n";
                            *out << ind << "const Tuple * tuple" << i << " = &negativeTuple;\n";
                            *out << ind << "bool lTrue = (w" << l->getPredicateName() << ".find(negativeTuple)!=NULL);\n";
                            *out << ind << "const Tuple * undefTuple = u" << l->getPredicateName() << ".find(negativeTuple);\n";
                            *out << ind++ << "if((!lTrue && undefTuple == NULL) || (undefTuple && tupleU == NULL)){\n";
                            *out << ind++ << "if(undefTuple){\n";
                            *out << ind << "tuple" << i << " = tupleU = undefTuple;\n";
                            *out << ind << "tupleU->print();\n";
                            *out << ind << "tupleUNegated = true;\n";
                            *out << --ind << "}\n";
                            closingParenthesis++;

                        } else {
                            *out << ind << "const Tuple * tuple" << i << " = (w" << l->getPredicateName() << ".find({";
                            printLiteralTuple(l);
                            *out << "}));\n";
                            *out << ind++ << "if(!tuple" << i << " && !tupleU ){\n";
                            *out << ind << "tuple" << i << " = tupleU = (u" << l->getPredicateName() << ".find({";
                            printLiteralTuple(l);
                            *out << "}));\n";
                            *out << ind << "tupleUNegated = false;\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "if(tuple" << i << "){\n";
                            closingParenthesis++;

                        }
                    }

                } else {
                    std::string mapVariableName = l->getPredicateName() + "_";
                    for (unsigned t = 0; t < l->getAriety(); t++) {
                        if ((l->isVariableTermAt(t) && boundVariables.count(l->getTermAt(t))) || !l->isVariableTermAt(t)) {
                            mapVariableName += std::to_string(t) + "_";
                        }
                    }
                    if (mode == LAZY_MODE) {
                        *out << ind << "const std::vector<const Tuple* >& tuples = p" << mapVariableName << ".getValues({";
                        printLiteralTuple(l, boundVariables);
                        *out << "});\n";
                        *out << ind++ << "for( unsigned i=0; i< tuples.size();i++){\n";
                        *out << ind << "const Tuple * tuple" << i << " = tuples[i];\n";
                        closingParenthesis++;

                    } else {
                        //mode == EAGER_MODE
                        *out << ind << "const std::vector<const Tuple* >* tuples;\n";
                        *out << ind << "tuples = &p" << mapVariableName << ".getValues({";
                        printLiteralTuple(l, boundVariables);
                        *out << "});\n";
                        *out << ind << "const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;\n";
                        bool appearsBefore = literalPredicateAppearsBeforeSameSign(body, joinOrder, i);
                        if (appearsBefore) {
                            *out << ind << "std::vector<const Tuple* > tupleUInVector;\n";
                        }
                        *out << ind++ << "if(tupleU == NULL){\n";
                        *out << ind << "tuplesU = &u" << mapVariableName << ".getValues({";
                        printLiteralTuple(l, boundVariables);
                        *out << "});\n";
                        *out << --ind << "}\n";
                        //repeating literal case

                        if (appearsBefore) {
                            *out << ind++ << "else {\n";
                            //handle constants and equal cards?
                            *out << ind++ << "if(tupleU && !tupleUNegated && tupleU->getPredicateName() == &_"<<l->getPredicateName()<<") {\n";
                            //check that bound variables have proper value
                            vector<unsigned> boundIndexes;
                            for(unsigned v = 0; v < l->getAriety(); v++) {
                                if(boundVariables.count(l->getTermAt(v))) {
                                    boundIndexes.push_back(v);
                                }
                            }
                            if(boundIndexes.size()) {
                                *out << ind++ << "if(";
                                    for(unsigned bi = 0; bi < boundIndexes.size(); bi++) {
                                        if(bi > 0) {
                                            *out << " && ";
                                        }
                                        *out << "tupleU->at(" << boundIndexes[bi] << ") == " << l->getTermAt(boundIndexes[bi]);
                                    }
                                    *out << "){\n";
                            }

                            *out << ind << "tupleUInVector.push_back(tupleU);\n";
                            if(boundIndexes.size()) {
                                    *out << --ind << "}\n";
                            }
                            *out << --ind << "}\n";
                            *out << --ind << "}\n";
                        }

                        if (!appearsBefore) {
                            *out << ind++ << "for( unsigned i=0; i< tuples->size() + tuplesU->size();i++){\n";
                            *out << ind << "const Tuple * tuple" << i << " = NULL;\n";
                            *out << ind++ << "if(i<tuples->size()){\n";
                            *out << ind << "tuple" << i << " = tuples->at(i);\n";
                            *out << ind++ << "if(tuplesU != &EMPTY_TUPLES) {\n";
                            *out << ind << "tupleU = NULL;\n";
                            *out << --ind << "}\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "else {\n";
                            *out << ind << "tuple" << i << " = tuplesU->at(i-tuples->size());\n";
                            *out << ind << "tupleU = tuple" << i << ";\n";
                            *out << ind << "tupleUNegated = false;\n";
                            *out << --ind << "}\n";
                        } else {
                            *out << ind++ << "for( unsigned i=0; i< tuples->size() + tuplesU->size() + tupleUInVector.size();i++){\n";
                            *out << ind << "const Tuple * tuple" << i << " = NULL;\n";
                            *out << ind++ << "if(i<tuples->size()){\n";
                            *out << ind << "tuple" << i << " = tuples->at(i);\n";
                            *out << ind++ << "if(tuplesU != &EMPTY_TUPLES) {\n";
                            *out << ind << "tupleU = NULL;\n";
                            *out << --ind << "}\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "else if(i<tuples->size()+tuplesU->size()) {\n";
                            *out << ind << "tuple" << i << " = tuplesU->at(i-tuples->size());\n";
                            *out << ind << "tupleU = tuple" << i << ";\n";
                            *out << ind << "tupleUNegated = false;\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "else {\n";
                            *out << ind << "tuple" << i << " = tupleU;\n";
                            *out << ind << "tupleUNegated = false;\n";
                            *out << --ind << "}\n";
                        }
                        closingParenthesis++;
                    }
                }
            } else if(f->containsAggregate()){


            }else{

                aspc::ArithmeticRelation* f = (aspc::ArithmeticRelation*) body[joinOrder[i]];
                if (f-> isBoundedRelation(boundVariables)) {
                    *out << ind++ << "if(" << f->getStringRep() << ") {\n";
                    closingParenthesis++;
                } else {
                    //bounded value assignment
                    *out << ind << "int " << f->getAssignmentStringRep(boundVariables) << ";\n";
                    boundVariables.insert(f->getAssignedVariable(boundVariables));

                }

            }

        }
        if (f->isPositiveLiteral() || (i == 0 && f->isLiteral())) {
            aspc::Literal* l = (aspc::Literal*)f;
            std::unordered_set<std::string> declaredVariables;
            for (unsigned t = 0; t < l->getAriety(); t++) {
                if (l->isVariableTermAt(t) && !boundVariables.count(l->getTermAt(t)) && !declaredVariables.count(l->getTermAt(t))) {
                    *out << ind << "int " << l->getTermAt(t) << " = (*tuple" << i << ")[" << t << "];\n";
                    declaredVariables.insert(l->getTermAt(t));
                }
            }
        }

        if (!r.getFormulas()[joinOrder[i]]->isBoundedLiteral(boundVariables) && !r.getFormulas()[joinOrder[i]]->isBoundedRelation(boundVariables) && r.getFormulas()[joinOrder[i]]->isLiteral() ) {
            r.getFormulas()[joinOrder[i]]->addVariablesToSet(boundVariables);
        }

        if (handleEqualCardsAndConstants(r, i, joinOrder)){
            closingParenthesis++;
        
        }

    }
    bool build = true;
    for(const aspc::ArithmeticRelationWithAggregate& aggr : r.getArithmeticRelationsWithAggregate()){

        std::string identifier = std::to_string(r.getRuleId())+":"+std::to_string(aggr.getFormulaIndex());
        std::string joinTupleName = aggregateLiteralToPredicateWSet[aggr.getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+identifier]; 

        std::string localSharedVars = sharedVariablesMap[identifier];
        std::string pair="sharedVarTuple"+std::to_string(r.getRuleId())+"_"+std::to_string(aggr.getFormulaIndex());
        *out << ind << "std::pair<std::vector<int>,std::pair<AuxMap*,AuxMap*> > "<<pair<<";\n";
        if(localSharedVars!=""){
                checkExistsShareVariableMap(r.getRuleId(),aggr.getFormulaIndex(),localSharedVars,true);
                    std::string sharedVarsIndexesToString="";
                    for(unsigned var : sharedVariablesIndexesMap[identifier]){
                        sharedVarsIndexesToString+=std::to_string(var)+"_";
                    }
                    *out << ind << "const std::vector<const Tuple*>* trueJoinTuples = &p_"
                                <<aggregateLiteralToPredicateWSet[aggr.getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+identifier]
                                        <<sharedVarsIndexesToString<<".getValues({"<<localSharedVars<<"});\n"; 
                        *out << ind << "const std::vector<const Tuple*>* undefJoinTuples = &u_"
                                <<aggregateLiteralToPredicateWSet[aggr.getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+identifier]
                                        <<sharedVarsIndexesToString<<".getValues({"<<localSharedVars<<"});\n"; 
                        saveTuples("trueJoinTuples","w"+aggregateLiteralToPredicateWSet[aggr.getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+identifier],aggr.getFormulaIndex(),r.getRuleId());
                        //*out << ind << "std::cout<<\"true updated\"<<std::endl;\n";

                        saveTuples("undefJoinTuples","u"+aggregateLiteralToPredicateWSet[aggr.getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+identifier],aggr.getFormulaIndex(),r.getRuleId());
            *out << --ind << "}\n";
            
            *out << ind << pair<<".second = *sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<aggr.getFormulaIndex()<<"[std::vector<int>({"<<localSharedVars<<"})];\n";
            
        }else{
            
            *out << ind << pair <<".second.first = &p_"<<joinTupleName<<";\n";
            *out << ind << pair <<".second.second = &u_"<<joinTupleName<<";\n";
        }
        if(reason){
            buildReason(identifier,&aggr,build);
            if(build){
                for(int j=1;j<joinOrder.size()-1;j++){
                    if(body[joinOrder[j]]->isLiteral()){
                        *out << ind++ << "if(tuple"<<j<<"!=tupleU){\n";
                            *out << ind << "const auto & it = tupleToVar.find(Tuple(*tuple"<<j<<"));\n";
                            *out << ind++ << "if(it!=tupleToVar.end()){\n";
                                *out << ind << "reason.push_back(it->second * (tuple"<<j<<"->isNegated() ? -1:1));\n";
                            *out << --ind <<"}\n";
                        *out << --ind <<"}\n";
                    }
                }
                *out << ind << "const auto & it = tupleToVar.find(Tuple(starter));\n";
                *out << ind++ << "if(it!=tupleToVar.end()){\n";
                    *out << ind << "reason.push_back(it->second * (starter.isNegated() ? -1:1));\n";
                *out << --ind <<"}\n";
            }

        }
        
        printAggregateTrueIf(identifier,&aggr,joinTupleName,".",aggr.isBoundedRelation(boundVariables));
        build=false;
    }
    *out << ind++ << "if(tupleU == NULL){\n";
        *out << ind << "std::cout<<\"propagation started from literal\"<<std::endl;\n";
        *out << ind << "std::cout<<\"conflict detected on propagator\"<<std::endl;\n";
        *out << ind << "propagatedLiteralsAndReasons.insert({-1, std::vector<int>("<<printReason<<")});\n";
    *out << --ind << "}else{\n";
        *out << ++ind << "const auto & it = tupleToVar.find(*tupleU);\n";
        *out << ind++ << "if(it != tupleToVar.end()) {\n";
            //*out << ind << "std::cout<<\"External propagation\";tupleU->print();std::cout<<std::endl;\n";
            *out << ind << "int sign = tupleU->isNegated() ? -1 : 1;\n";
            *out << ind << "propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>("<<printReason<<")}).first->second;\n";
        *out << --ind << "}\n";
    *out << --ind << "}\n";        
    for(int i=0;i<r.getArithmeticRelationsWithAggregate().size();i++)
        *out << --ind <<"}\n";
    *out << ind++ << "if(tupleU == NULL){\n";
        bool propFirst= false;
        bool propLast = false;

        while(!propFirst || !propLast){

            if(propLast || r.getArithmeticRelationsWithAggregate().size()<=1)
                propFirst=true;
            *out << ind++ << "{\n";
            int currentAggr=0;
            build = false;
            for(const aspc::ArithmeticRelationWithAggregate& aggr : r.getArithmeticRelationsWithAggregate()){
                std::string identifier = std::to_string(r.getRuleId())+":"+std::to_string(aggr.getFormulaIndex());
                std::string joinTupleName = aggregateLiteralToPredicateWSet[aggr.getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+identifier]; 
                std::string localSharedVars = sharedVariablesMap[identifier];
                std::string pair="sharedVarTuple"+std::to_string(r.getRuleId())+"_"+std::to_string(aggr.getFormulaIndex());
                *out << ind << "std::pair<std::vector<int>,std::pair<AuxMap*,AuxMap*> > "<<pair<<";\n";
                if(localSharedVars!=""){
                        checkExistsShareVariableMap(r.getRuleId(),aggr.getFormulaIndex(),localSharedVars,true);
                            std::string sharedVarsIndexesToString="";
                            for(unsigned var : sharedVariablesIndexesMap[identifier]){
                                sharedVarsIndexesToString+=std::to_string(var)+"_";
                            }
                            *out << ind << "const std::vector<const Tuple*>* trueJoinTuples = &p_"
                                        <<aggregateLiteralToPredicateWSet[aggr.getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+identifier]
                                                <<sharedVarsIndexesToString<<".getValues({"<<localSharedVars<<"});\n"; 
                                *out << ind << "const std::vector<const Tuple*>* undefJoinTuples = &u_"
                                        <<aggregateLiteralToPredicateWSet[aggr.getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+identifier]
                                                <<sharedVarsIndexesToString<<".getValues({"<<localSharedVars<<"});\n"; 
                                saveTuples("trueJoinTuples","w"+aggregateLiteralToPredicateWSet[aggr.getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+identifier],aggr.getFormulaIndex(),r.getRuleId());
                                //*out << ind << "std::cout<<\"true updated\"<<std::endl;\n";

                                saveTuples("undefJoinTuples","u"+aggregateLiteralToPredicateWSet[aggr.getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+identifier],aggr.getFormulaIndex(),r.getRuleId());
                    *out << --ind << "}\n";
                    *out << ind << pair<<".second = *sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<aggr.getFormulaIndex()<<"[std::vector<int>({"<<localSharedVars<<"})];\n";
                }else{
                    *out << ind << pair <<".second.first = &p_"<<joinTupleName<<";\n";
                    *out << ind << pair <<".second.second = &u_"<<joinTupleName<<";\n";
                }
                if(reason){
                    buildReason(identifier,&aggr,build);
                    if(build){
                        for(int j=1;j<joinOrder.size()-1;j++){
                            if(body[joinOrder[j]]->isLiteral()){
                                *out << ind++ << "if(tuple"<<j<<"!=tupleU){\n";
                                    *out << ind << "const auto & it = tupleToVar.find(Tuple(*tuple"<<j<<"));\n";
                                    *out << ind++ << "if(it!=tupleToVar.end()){\n";
                                        *out << ind << "reason.push_back(it->second * (tuple"<<j<<"->isNegated() ? -1:1));\n";
                                    *out << --ind <<"}\n";
                                *out << --ind <<"}\n";
                            }
                        }
                        *out << ind << "const auto & it = tupleToVar.find(Tuple(starter));\n";
                        *out << ind++ << "if(it!=tupleToVar.end()){\n";
                            *out << ind << "reason.push_back(it->second * (starter.isNegated() ? -1:1));\n";
                        *out << --ind <<"}\n";
                    }
                }
                if(currentAggr == 0)
                    if(!propFirst)
                        printAggregateTrueIf(identifier,&aggr,joinTupleName,".",aggr.isBoundedRelation(boundVariables));
                    else {
                        if(aggr.isNegated()){
                            std::string aggregateVarProjection = "";
                            for(unsigned var : aggregateVariablesIndex[identifier]){
                                aggregateVarProjection+=std::to_string(var)+"_";
                            }
                            if(localSharedVars=="")
                                *out << ind << "int undefPlusTrue = p_"<<joinTupleName<<aggregateVarProjection<<".size();\n";
                            else 
                                *out << ind << "int undefPlusTrue = "<<pair<<".second.first->size();\n";
        
                            std::string sharedVarsProjection = "";
                            for(unsigned index : sharedVariablesIndexesMap[identifier])
                                sharedVarsProjection+=std::to_string(index)+"_";

                            std::string tupleProjection="";
                            for(unsigned index : aggregateVariablesIndex[identifier])
                                tupleProjection += "undefTuple->at("+std::to_string(index)+"),";
                            tupleProjection = tupleProjection.substr(0,tupleProjection.size()-1);

                            
                            
                            *out << ind << "std::set<std::vector<int>> distinctKeyNotTrue;\n"; 
                            *out << ind++ << "for(const Tuple * undefTuple : u_"<<joinTupleName<<sharedVarsProjection<<".getValues({"<<localSharedVars<<"})){\n";
                                if(localSharedVars=="")
                                    *out << ind++ << "if(p_"<<joinTupleName<<aggregateVarProjection<<".getValues({"<<tupleProjection<<"}).size()<=0)\n";
                                else 
                                    *out << ind++ << "if("<<pair<<".second.first->getValues({"<<tupleProjection<<"}).size()<=0)\n";

                                *out << ind-- << "distinctKeyNotTrue.insert(std::vector<int>({"<<tupleProjection<<"}));\n";
                            *out << --ind << "}\n";
                            *out << ind << "undefPlusTrue+=distinctKeyNotTrue.size();\n";
                        }
                        printCanPropagateIf(identifier,&aggr,".");
                    }
                if(currentAggr == 1)
                    if(!propFirst){
                        if(aggr.isNegated()){
                            std::string aggregateVarProjection = "";
                            for(unsigned var : aggregateVariablesIndex[identifier]){
                                aggregateVarProjection+=std::to_string(var)+"_";
                            }
                            if(localSharedVars=="")
                                *out << ind << "int undefPlusTrue = p_"<<joinTupleName<<aggregateVarProjection<<".size();\n";
                            else 
                                *out << ind << "int undefPlusTrue = "<<pair<<".second.first->size();\n";

                            std::string sharedVarsProjection = "";
                            for(unsigned index : sharedVariablesIndexesMap[identifier])
                                sharedVarsProjection+=std::to_string(index)+"_";

                            std::string tupleProjection="";
                            for(unsigned index : aggregateVariablesIndex[identifier])
                                tupleProjection += "undefTuple->at("+std::to_string(index)+"),";
                            tupleProjection = tupleProjection.substr(0,tupleProjection.size()-1);
                            

                            *out << ind << "std::set<std::vector<int>> distinctKeyNotTrue;\n"; 
                            *out << ind++ << "for(const Tuple * undefTuple : u_"<<joinTupleName<<sharedVarsProjection<<".getValues({"<<localSharedVars<<"})){\n";
                                if(localSharedVars=="")
                                    *out << ind++ << "if(p_"<<joinTupleName<<aggregateVarProjection<<".getValues({"<<tupleProjection<<"}).size()<=0)\n";
                                else 
                                    *out << ind++ << "if("<<pair<<".second.first->getValues({"<<tupleProjection<<"}).size()<=0)\n";
                                *out << ind-- << "distinctKeyNotTrue.insert(std::vector<int>({"<<tupleProjection<<"}));\n";
                            *out << --ind << "}\n";
                            *out << ind << "undefPlusTrue+=distinctKeyNotTrue.size();\n";
                        }
                        printCanPropagateIf(identifier,&aggr,".");
                    }
                        
                    else 
                        printAggregateTrueIf(identifier,&aggr,joinTupleName,".",aggr.isBoundedRelation(boundVariables));
                        
                build=false;
                currentAggr++;
            }
            
            propLast=true;
            if(!propFirst){
                std::string identifier = std::to_string(r.getRuleId())+":"+std::to_string(r.getArithmeticRelationsWithAggregate()[1].getFormulaIndex());
                propAggr(&r.getArithmeticRelationsWithAggregate()[1],identifier,reason,".");
            }
            else{
                std::string identifier = std::to_string(r.getRuleId())+":"+std::to_string(r.getArithmeticRelationsWithAggregate()[0].getFormulaIndex());
                propAggr(&r.getArithmeticRelationsWithAggregate()[0],identifier,reason,".");
            }

            for(int i=0;i<r.getArithmeticRelationsWithAggregate().size();i++)
                *out << --ind << "}//aggr\n";
            *out << --ind << "}//local scope\n";
        }


    *out << --ind << "}//tupleU\n";        
    while(closingParenthesis>0){
        *out << --ind << "}\n";  
        closingParenthesis--;      
    }


}
void CompilationManager::evaluationStartingFromAggregate(const aspc::Rule & r,std::vector<unsigned> joinOrder,unsigned start){
    const std::vector<const aspc::Formula*>& body = r.getFormulas();
    const aspc::ArithmeticRelationWithAggregate* starter = (aspc::ArithmeticRelationWithAggregate*) body[joinOrder[0]];
    std::string aggrIdentifier = std::to_string(r.getRuleId())+":"+std::to_string(joinOrder[0]);
    std::string joinTupleName = aggregateLiteralToPredicateWSet[starter->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]; 
    bool reason = start != joinOrder.size();
    std::string printReason = reason ? "reason":"";
    unsigned closingParenthesis=0;
    if(reason){
        *out << ind++ << "if(";
        int aggrLitIndex=0;
        for(const aspc::Literal& aggrLit : starter->getAggregate().getAggregateLiterals()){

            if(aggrLitIndex > 0)
                *out << " || ";
            *out << "starter.getPredicateName()== &_" << aggrLit.getPredicateName();

            aggrLitIndex++;
        }
        *out << "){\n";
        closingParenthesis++;

    }
        std::string sharedVars = sharedVariablesMap[aggrIdentifier];
        
        std::unordered_set<std::string> boundVariables;
        std::unordered_set<std::string> sharedVarsSet;
        bool boundStarter = starter->isBoundedRelation(boundVariables);
        

        std::string op = "->";
        if(sharedVars!=""){
            *out << ind++ << "for(const auto sharedVarTuple"<<r.getRuleId()<<"_"<<joinOrder[0]<<" : sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<joinOrder[0]<<"){\n";
            //*out << ind << "std::cout<<\"true sharedVars \"<<sharedVarTuple.second->first->size()<<std::endl;\n";
            //*out << ind << "std::cout<<\"undef sharedVars \"<<sharedVarTuple.second->second->size()<<std::endl;\n";

            closingParenthesis++;
                int pos = sharedVars.find(',');
                int var=0;
                std::unordered_set<std::string> alreadyDeclared;

                // se ho solo una variabile condivisa entro qui e non nel while
                if(sharedVars!="" && pos == std::string::npos){
                    if(!alreadyDeclared.count(sharedVars)){
                        *out << ind << "int " << sharedVars<< " = sharedVarTuple"<<r.getRuleId()<<"_"<<joinOrder[0]<<".first["<<var<<"];\n";
                        alreadyDeclared.insert(sharedVars);
                    }
                    boundVariables.insert(sharedVars);
                    sharedVarsSet.insert(sharedVars);
                    
                }
                while(pos!=std::string::npos){
                    if(!alreadyDeclared.count(sharedVars.substr(0,pos))){
                        *out << ind << "int " << sharedVars.substr(0,pos)<< " = sharedVarTuple"<<r.getRuleId()<<"_"<<joinOrder[0]<<".first["<<var<<"];\n";
                        alreadyDeclared.insert(sharedVars.substr(0,pos));
                    }
                    boundVariables.insert(sharedVars.substr(0,pos));
                    sharedVarsSet.insert(sharedVars.substr(0,pos));
                    sharedVars=sharedVars.substr(pos+1,sharedVars.length());
                    pos = sharedVars.find(',');
                    var++;
                    if(sharedVars!="" && pos == std::string::npos){
                        if(!alreadyDeclared.count(sharedVars)){
                            *out << ind << "int " << sharedVars<< " = sharedVarTuple"<<r.getRuleId()<<"_"<<joinOrder[0]<<".first["<<var<<"];\n";
                            alreadyDeclared.insert(sharedVars);
                        }
                        boundVariables.insert(sharedVars);
                        sharedVarsSet.insert(sharedVars);
                    
                    }

                }
            
        }else{
            *out << ind++ << "{\n";
            closingParenthesis++;
            std::string pairName = "sharedVarTuple"+std::to_string(r.getRuleId())+"_"+std::to_string(joinOrder[0]);
            std::string aggregateVarProjection = "";
            for(unsigned index: aggregateVariablesIndex[aggrIdentifier])
                aggregateVarProjection+=std::to_string(index)+"_";
            *out << ind << "std::pair<std::vector<int>,std::pair<AuxMap*,AuxMap*> >"<<pairName<<";\n";
            *out << ind << pairName << ".second.first = &p_"<<joinTupleName<<aggregateVarProjection<<";\n";
            *out << ind << pairName << ".second.second = &u_"<<joinTupleName<<aggregateVarProjection<<";\n";
            op=".";
        }
        if(reason)
            buildReason(aggrIdentifier,starter,true);
        
        printAggregateTrueIf(aggrIdentifier,starter,joinTupleName,op,starter->isBoundedRelation(boundVariables));
        closingParenthesis++;
        *out << ind << "tupleU=NULL;\n";
        bool propLast=false;
        bool propFirst=false;

        do{
            if(propLast)
                propFirst=true;
            for(int i=1;i<body.size();i++){
                const aspc::Formula* f = body[joinOrder[i]];
                if(body[joinOrder[i]]->containsAggregate()){
                    //valuto l'ultimo aggregato
                    std::string identifier = std::to_string(r.getRuleId())+":"+std::to_string(joinOrder[i]);
                    std::string localSharedVars = sharedVariablesMap[identifier];
                    std::string pair="sharedVarTuple"+std::to_string(r.getRuleId())+"_"+std::to_string(joinOrder[i]);
                    *out << ind << "std::pair<std::vector<int>,std::pair<AuxMap*,AuxMap*> > "<<pair<<";\n";
                    const aspc::ArithmeticRelationWithAggregate* aggr = (aspc::ArithmeticRelationWithAggregate*)f;

                    if(localSharedVars!=""){
                            checkExistsShareVariableMap(r.getRuleId(),aggr->getFormulaIndex(),localSharedVars,true);
                                std::string sharedVarsIndexesToString="";
                                for(unsigned var : sharedVariablesIndexesMap[identifier]){
                                    sharedVarsIndexesToString+=std::to_string(var)+"_";
                                }
                                *out << ind << "const std::vector<const Tuple*>* trueJoinTuples = &p_"
                                            <<aggregateLiteralToPredicateWSet[aggr->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+identifier]
                                                    <<sharedVarsIndexesToString<<".getValues({"<<localSharedVars<<"});\n"; 
                                    *out << ind << "const std::vector<const Tuple*>* undefJoinTuples = &u_"
                                            <<aggregateLiteralToPredicateWSet[aggr->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+identifier]
                                                    <<sharedVarsIndexesToString<<".getValues({"<<localSharedVars<<"});\n"; 
                                    saveTuples("trueJoinTuples","w"+aggregateLiteralToPredicateWSet[aggr->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+identifier],aggr->getFormulaIndex(),r.getRuleId());
                                    //*out << ind << "std::cout<<\"true updated\"<<std::endl;\n";

                                    saveTuples("undefJoinTuples","u"+aggregateLiteralToPredicateWSet[aggr->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+identifier],aggr->getFormulaIndex(),r.getRuleId());
                            *out << --ind << "}\n";    
                        *out << ind << pair<<".second = *sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<joinOrder[i]<<"[std::vector<int>({"<<localSharedVars<<"})];\n";
                    }else{
                        //add aggregate var projection
                        std::string aggregateVarProjection = "";
                        for(unsigned index: aggregateVariablesIndex[aggrIdentifier])
                            aggregateVarProjection+=std::to_string(index)+"_";
                        
                        *out << ind << pair <<".second.first = &p_"<<joinTupleName<<aggregateVarProjection<<";\n";
                        *out << ind << pair <<".second.second = &u_"<<joinTupleName<<aggregateVarProjection<<";\n";
                    }
                    if(reason){
                        buildReason(identifier,(aspc::ArithmeticRelationWithAggregate*)f,false);
                        for(int j=1;j<joinOrder.size()-1;j++){
                            if(body[joinOrder[j]]->isLiteral()){
                                *out << ind++ << "if(tuple"<<j<<"!=tupleU){\n";
                                    *out << ind << "const auto & it = tupleToVar.find(Tuple(*tuple"<<j<<"));\n";
                                    *out << ind++ << "if(it!=tupleToVar.end()){\n";
                                        *out << ind << "reason.push_back(it->second * (tuple"<<j<<"->isNegated() ? -1:1));\n";
                                    *out << --ind <<"}\n";
                                *out << --ind <<"}\n";
                            }
                        }
                        *out << ind << "const auto & it = tupleToVar.find(Tuple(starter));\n";
                        *out << ind++ << "if(it!=tupleToVar.end()){\n";
                            *out << ind << "reason.push_back(it->second * (starter.isNegated() ? -1:1));\n";
                        *out << --ind <<"}\n";
                    }

                    // start aggregate is True
                    if(!boundStarter){
                        std::string plusOne = starter->isPlusOne() ? "+1":"";
                        if(!starter->isNegated()){
                            *out << ind << "if(count"<<r.getRuleId()<<"_"<<starter->getFormulaIndex()<<starter->getCompareTypeAsString()<<starter->getGuard().getStringRep()<<plusOne<<"){\n";
                        }else{
                            *out << ind << "if(!(count"<<r.getRuleId()<<"_"<<starter->getFormulaIndex()<<starter->getCompareTypeAsString()<<starter->getGuard().getStringRep()<<plusOne<<")){\n";
                        }
                    }
                    
                    *out << ind++ << "if(tupleU == NULL){\n";
                        printAggregateTrueIf(identifier,(aspc::ArithmeticRelationWithAggregate*)f,joinTupleName,".",f->isBoundedRelation(boundVariables));
                        const aspc::ArithmeticRelationWithAggregate* ar = (aspc::ArithmeticRelationWithAggregate*)f;

                        if(!propFirst){
                                *out << ind << "std::cout<<\"conflict detected on propagator\"<<std::endl;\n";
                                *out << ind << "propagatedLiteralsAndReasons.insert({-1, std::vector<int>("<<printReason<<")});\n";
                                
                            *out << --ind <<"}else{\n";
                                //second aggregate false
                                ind++;
                                printCanPropagateIf(identifier,(aspc::ArithmeticRelationWithAggregate*)f,".");
                                    propAggr((aspc::ArithmeticRelationWithAggregate*)f,identifier,true,".");
                                *out << --ind << "}//close can propagate if\n";
                            *out << --ind << "}//else aggr true if\n";
                            *out << --ind << "}else{\n";
                                //tupleU != NULL
                                ind++;
                                printAggregateTrueIf(identifier,(aspc::ArithmeticRelationWithAggregate*)f,joinTupleName,".",f->isBoundedRelation(boundVariables));
                                    *out << ind << "const auto & it = tupleToVar.find(*tupleU);\n";
                                    *out << ind++ << "if(it != tupleToVar.end()) {\n";
                                        //*out << ind << "std::cout<<\"External propagation\";tupleU->print();std::cout<<std::endl;\n";
                                        *out << ind << "int sign = tupleU->isNegated() ? -1 : 1;\n";
                                        *out << ind << "propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>("<<printReason<<")}).first->second;\n";
                                    *out << --ind << "}\n";
                                *out << --ind << "}\n";
                            *out << --ind << "}\n";
                            if(!boundStarter)
                                *out << --ind << "}\n";
                        }else{
                            //tupleU if and aggr true if
                            closingParenthesis+=2;
                        }
                
                    

                }else if (i != 0 || start == r.getBodySize()) {
                    //std::cout<<"Evaluating literal"<<boundVariables.count("X")<<std::endl;
                    if (f->isLiteral()) {
                        aspc::Literal* l = (aspc::Literal*)f;
                        if (l->isNegated() || l->isBoundedLiteral(boundVariables)) {
                            if (mode == LAZY_MODE) {
                                std::string negation = "";
                                if (l->isNegated()) {
                                    negation = "!";
                                }
                                *out << ind << "const Tuple * tuple" << i << " = w" << l->getPredicateName() << ".find({";
                                printLiteralTuple(l);
                                *out << "});\n";
                                *out << ind++ << "if(" << negation << "tuple" << i << "){\n";
                                closingParenthesis++;
                            } else {
                                //mode == EAGER_MODE
                                bool appearsBefore = literalPredicateAppearsBeforeSameSign(body, joinOrder, i);
                                if(appearsBefore && l->isPositiveLiteral()) {
                                    *out << ind << "const Tuple * tuple" << i << " = NULL;\n";
                                    *out << ind++ << "if(tupleU && tupleU->getPredicateName() == &_"<<l->getPredicateName()<<"){\n"; 
                                    *out << ind << "const Tuple * undefRepeatingTuple = (u" << l->getPredicateName() << ".find({";
                                    printLiteralTuple(l);
                                    *out << "}));\n";
                                    *out << ind++ << "if(tupleU == undefRepeatingTuple){\n"; 
                                    *out << ind << "tuple" << i << " = undefRepeatingTuple;\n";
                                    *out << --ind << "}\n";
                                    *out << --ind << "}\n";
                                    *out << ind++ << "if(!tuple"<<i<<"){\n;"; 
                                    *out << ind << "tuple" << i << " = (w" << l->getPredicateName() << ".find({";
                                    printLiteralTuple(l);
                                    *out << "}));\n";
                                    *out << --ind << "}\n";
                                    *out << ind++ << "if(!tuple"<<i<<" && !tupleU){\n;"; 
                                    *out << ind << "tuple" << i << " = tupleU = (u" << l->getPredicateName() << ".find({";
                                    printLiteralTuple(l);
                                    *out << "}));\n";
                                    *out << --ind << "}\n";
                                    *out << ind << "tupleUNegated = false;\n";
                                    *out << ind++ << "if(tuple" << i << "){\n";
                                    closingParenthesis++;                          
                                }
                                else if(appearsBefore && l->isNegated()) {
                                    *out << ind << "const Tuple * tuple" << i << " = NULL;\n";
                                    *out << ind << "const Tuple negativeTuple = Tuple({";
                                    printLiteralTuple(l);
                                    *out << "}, &_" << l->getPredicateName() << ", true);\n";
                                    *out << ind++ << "if(tupleU && tupleU->getPredicateName() == &_"<<l->getPredicateName()<<"){\n;"; 
                                    *out << ind << "const Tuple * undefRepeatingTuple = (u" << l->getPredicateName() << ".find({";
                                    printLiteralTuple(l);
                                    *out << "}));\n";
                                    *out << ind++ << "if(tupleU == undefRepeatingTuple){\n"; 
                                    *out << ind << "tuple" << i << " = undefRepeatingTuple;\n";
                                    *out << --ind << "}\n";
                                    *out << --ind << "}\n";
                                    *out << ind++ << "if(!tuple"<<i<<"){\n;"; 
                                    *out << ind++ << "if(!(w" << l->getPredicateName() << ".find({";
                                    printLiteralTuple(l);
                                    *out << "}))){\n";
                                    *out << ind << "tuple" << i << " = &negativeTuple;\n";
                                    *out << --ind << "}\n";
                                    *out << --ind << "}\n";
                                    *out << ind++ << "if(tuple" << i << "){\n";
                                    closingParenthesis++;                                                
                                }
                                else if (l->isNegated()) {
                                    *out << ind << "const Tuple negativeTuple = Tuple({";
                                    printLiteralTuple(l);
                                    *out << "}, &_" << l->getPredicateName() << ", true);\n";
                                    *out << ind << "const Tuple * tuple" << i << " = &negativeTuple;\n";
                                    *out << ind << "bool lTrue = (w" << l->getPredicateName() << ".find(negativeTuple)!=NULL);\n";
                                    *out << ind << "const Tuple * undefTuple = u" << l->getPredicateName() << ".find(negativeTuple);\n";
                                    *out << ind++ << "if((!lTrue && undefTuple == NULL) || (undefTuple && tupleU == NULL)){\n";
                                    *out << ind++ << "if(undefTuple){\n";
                                    *out << ind << "tuple" << i << " = tupleU = undefTuple;\n";
                                    *out << ind << "tupleU->print();\n";
                                    *out << ind << "tupleUNegated = true;\n";
                                    *out << --ind << "}\n";
                                    closingParenthesis++;

                                } else {
                                    *out << ind << "const Tuple * tuple" << i << " = (w" << l->getPredicateName() << ".find({";
                                    printLiteralTuple(l);
                                    *out << "}));\n";
                                    *out << ind++ << "if(!tuple" << i << " && !tupleU ){\n";
                                    *out << ind << "tuple" << i << " = tupleU = (u" << l->getPredicateName() << ".find({";
                                    printLiteralTuple(l);
                                    *out << "}));\n";
                                    *out << ind << "tupleUNegated = false;\n";
                                    *out << --ind << "}\n";
                                    *out << ind++ << "if(tuple" << i << "){\n";
                                    closingParenthesis++;
                                }
                            }

                        } else {
                            std::string mapVariableName = l->getPredicateName() + "_";
                            for (unsigned t = 0; t < l->getAriety(); t++) {
                                if ((l->isVariableTermAt(t) && boundVariables.count(l->getTermAt(t))) || !l->isVariableTermAt(t)) {
                                    mapVariableName += std::to_string(t) + "_";
                                }
                            }
                            if (mode == LAZY_MODE) {
                                *out << ind << "const std::vector<const Tuple* >& tuples = p" << mapVariableName << ".getValues({";
                                printLiteralTuple(l, boundVariables);
                                *out << "});\n";
                                *out << ind++ << "for( unsigned i=0; i< tuples.size();i++){\n";
                                *out << ind << "const Tuple * tuple" << i << " = tuples[i];\n";
                                closingParenthesis++;
                            } else {
                                //mode == EAGER_MODE
                                *out << ind << "const std::vector<const Tuple* >* tuples;\n";
                                *out << ind << "tuples = &p" << mapVariableName << ".getValues({";
                                printLiteralTuple(l, boundVariables);
                                *out << "});\n";
                                *out << ind << "const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;\n";
                                bool appearsBefore = literalPredicateAppearsBeforeSameSign(body, joinOrder, i);
                                if (appearsBefore) {
                                    *out << ind << "std::vector<const Tuple* > tupleUInVector;\n";
                                }
                                *out << ind++ << "if(tupleU == NULL){\n";
                                *out << ind << "tuplesU = &u" << mapVariableName << ".getValues({";
                                printLiteralTuple(l, boundVariables);
                                *out << "});\n";
                                *out << --ind << "}\n";
                                //repeating literal case

                                if (appearsBefore) {
                                    *out << ind++ << "else {\n";
                                    //handle constants and equal cards?
                                    *out << ind++ << "if(tupleU && !tupleUNegated && tupleU->getPredicateName() == &_"<<l->getPredicateName()<<") {\n";
                                    //check that bound variables have proper value
                                    vector<unsigned> boundIndexes;
                                    for(unsigned v = 0; v < l->getAriety(); v++) {
                                        if(boundVariables.count(l->getTermAt(v))) {
                                            boundIndexes.push_back(v);
                                        }
                                    }
                                    if(boundIndexes.size()) {
                                        *out << ind++ << "if(";
                                            for(unsigned bi = 0; bi < boundIndexes.size(); bi++) {
                                                if(bi > 0) {
                                                    *out << " && ";
                                                }
                                                *out << "tupleU->at(" << boundIndexes[bi] << ") == " << l->getTermAt(boundIndexes[bi]);
                                            }
                                            *out << "){\n";
                                    }

                                    *out << ind << "tupleUInVector.push_back(tupleU);\n";
                                    if(boundIndexes.size()) {
                                            *out << --ind << "}\n";
                                    }
                                    *out << --ind << "}\n";
                                    *out << --ind << "}\n";
                                }

                                if (!appearsBefore) {
                                    *out << ind++ << "for( unsigned i=0; i< tuples->size() + tuplesU->size();i++){\n";
                                    *out << ind << "const Tuple * tuple" << i << " = NULL;\n";
                                    *out << ind++ << "if(i<tuples->size()){\n";
                                    *out << ind << "tuple" << i << " = tuples->at(i);\n";
                                    *out << ind++ << "if(tuplesU != &EMPTY_TUPLES) {\n";
                                    *out << ind << "tupleU = NULL;\n";
                                    *out << --ind << "}\n";
                                    *out << --ind << "}\n";
                                    *out << ind++ << "else {\n";
                                    *out << ind << "tuple" << i << " = tuplesU->at(i-tuples->size());\n";
                                    *out << ind << "tupleU = tuple" << i << ";\n";
                                    *out << ind << "tupleUNegated = false;\n";
                                    *out << --ind << "}\n";
                                } else {
                                    *out << ind++ << "for( unsigned i=0; i< tuples->size() + tuplesU->size() + tupleUInVector.size();i++){\n";
                                    *out << ind << "const Tuple * tuple" << i << " = NULL;\n";
                                    *out << ind++ << "if(i<tuples->size()){\n";
                                    *out << ind << "tuple" << i << " = tuples->at(i);\n";
                                    *out << ind++ << "if(tuplesU != &EMPTY_TUPLES) {\n";
                                    *out << ind << "tupleU = NULL;\n";
                                    *out << --ind << "}\n";
                                    *out << --ind << "}\n";
                                    *out << ind++ << "else if(i<tuples->size()+tuplesU->size()) {\n";
                                    *out << ind << "tuple" << i << " = tuplesU->at(i-tuples->size());\n";
                                    *out << ind << "tupleU = tuple" << i << ";\n";
                                    *out << ind << "tupleUNegated = false;\n";
                                    *out << --ind << "}\n";
                                    *out << ind++ << "else {\n";
                                    *out << ind << "tuple" << i << " = tupleU;\n";
                                    *out << ind << "tupleUNegated = false;\n";
                                    *out << --ind << "}\n";
                                }
                                closingParenthesis++;
                            }
                        }
                    } else if(f->containsAggregate()){


                    }else{

                        aspc::ArithmeticRelation* f = (aspc::ArithmeticRelation*) body[joinOrder[i]];
                        if (f-> isBoundedRelation(boundVariables)) {
                            *out << ind++ << "if(" << f->getStringRep() << ") {\n";
                            closingParenthesis++;
                        } else {
                            //bounded value assignment
                            *out << ind << "int " << f->getAssignmentStringRep(boundVariables) << ";\n";
                            boundVariables.insert(f->getAssignedVariable(boundVariables));

                        }

                    }

                }
                if (f->isPositiveLiteral() || (i == 0 && f->isLiteral())) {
                    aspc::Literal* l = (aspc::Literal*)f;
                    std::unordered_set<std::string> declaredVariables;
                    for (unsigned t = 0; t < l->getAriety(); t++) {
                        if (l->isVariableTermAt(t) && !boundVariables.count(l->getTermAt(t)) && !declaredVariables.count(l->getTermAt(t))) {
                            *out << ind << "int " << l->getTermAt(t) << " = (*tuple" << i << ")[" << t << "];\n";
                            declaredVariables.insert(l->getTermAt(t));
                        }
                    }
                }

                if (!r.getFormulas()[joinOrder[i]]->isBoundedLiteral(boundVariables) && !r.getFormulas()[joinOrder[i]]->isBoundedRelation(boundVariables) && r.getFormulas()[joinOrder[i]]->isLiteral() ) {
                    r.getFormulas()[joinOrder[i]]->addVariablesToSet(boundVariables);
                }

                if (handleEqualCardsAndConstants(r, i, joinOrder))
                    closingParenthesis++;

            }

            if(r.getArithmeticRelationsWithAggregate().size()==1){
                
                if(!propFirst){
                    if(reason){

                        for(int j=1;j<joinOrder.size();j++){
                            if(body[joinOrder[j]]->isLiteral()){
                                *out << ind++ << "if(tuple"<<j<<"!=tupleU){\n";
                                    *out << ind << "const auto & it = tupleToVar.find(Tuple(*tuple"<<j<<"));\n";
                                    *out << ind++ << "if(it!=tupleToVar.end()){\n";
                                        *out << ind << "int sign = tuple"<<j<<"->isNegated() ? -1:1;\n";
                                        *out << ind << "reason.push_back(it->second * sign);\n";
                                    *out << --ind <<"}\n";
                                *out << --ind <<"}\n";
                            }
                        }
                        *out << ind << "const auto & it = tupleToVar.find(Tuple(starter));\n";
                        *out << ind++ << "if(it!=tupleToVar.end()){\n";
                            *out << ind << "int sign = starter.isNegated() ? -1:1;\n";
                            *out << ind << "reason.push_back(it->second * sign);\n";
                        *out << --ind <<"}\n";

                    }
                    if(!boundStarter){
                        std::string plusOne = starter->isPlusOne() ? "+1":"";
                        if(starter->isNegated())
                            *out << ind++ << "if(!(count"<<r.getRuleId()<<"_"<<starter->getFormulaIndex()<<starter->getCompareTypeAsString()<<starter->getGuard().getStringRep()<<plusOne<<")){\n";
                        else 
                            *out << ind++ << "if(count"<<r.getRuleId()<<"_"<<starter->getFormulaIndex()<<starter->getCompareTypeAsString()<<starter->getGuard().getStringRep()<<plusOne<<"){\n";
                    }
                    *out << ind++ << "if(tupleU == NULL){\n";
                        //*out << ind << "std::cout<<\"Prop starting from "<<aggrIdentifier<<" and both aggr true\"<<std::endl;\n";            
                        *out << ind << "std::cout<<\"conflict detected on propagator\"<<std::endl;\n";
                        *out << ind << "propagatedLiteralsAndReasons.insert({-1, std::vector<int>("<<printReason<<")});\n";
                    *out << --ind <<"}else{\n";
                        *out << ++ind << "const auto & it = tupleToVar.find(*tupleU);\n";
                        *out << ind++ << "if(it != tupleToVar.end()) {\n";
                            //*out << ind << "std::cout<<\"External propagation\";tupleU->print();std::cout<<std::endl;\n";
                            *out << ind << "int sign = tupleU->isNegated() ? -1 : 1;\n";
                            *out << ind << "propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>("<<printReason<<")}).first->second;\n";
                        *out << --ind << "}\n";
                    *out << --ind << "}\n";
                    if(!boundStarter){
                        *out << --ind << "}\n";
                    }
                }else{

                    *out << ind++ << "if(tupleU == NULL){\n";
                        if(reason){

                            for(int j=1;j<joinOrder.size();j++){
                                if(body[joinOrder[j]]->isLiteral()){
                                    *out << ind++ << "if(tuple"<<j<<"!=tupleU){\n";
                                        *out << ind << "const auto & it = tupleToVar.find(Tuple(*tuple"<<j<<"));\n";
                                        *out << ind++ << "if(it!=tupleToVar.end()){\n";
                                            *out << ind << "reason.push_back(it->second * (tuple"<<j<<"->isNegated() ? -1:1));\n";
                                        *out << --ind <<"}\n";
                                    *out << --ind <<"}\n";
                                }
                            }
                            *out << ind << "const auto & it = tupleToVar.find(Tuple(starter));\n";
                            *out << ind++ << "if(it!=tupleToVar.end()){\n";
                                *out << ind << "reason.push_back(it->second * (starter.isNegated() ? -1:1));\n";
                            *out << --ind <<"}\n";
                        }
                        std::string op = sharedVars!="" ? "->":".";
                        if(!boundStarter){
                            if(!starter->isNegated() ){
                                std::string plusOne = starter->isPlusOne() ? "": "-1";
                                *out << ind++ << "if(count"<<r.getRuleId()<<"_"<<starter->getFormulaIndex()<<" == "<<starter->getGuard().getStringRep()<<plusOne<<"){\n";
                            }else{
                                std::string plusOne = starter->isPlusOne() ? "+1":"";
                                *out << ind++ << "if(count"<<r.getRuleId()<<"_"<<starter->getFormulaIndex()<<" == "<<starter->getGuard().getStringRep()<<plusOne<<"){\n";
                            }
                        }
                        propAggr(starter,aggrIdentifier,reason,op);
                        if(!boundStarter)
                            *out << --ind << "}\n";
    
                    *out << --ind << "}\n";
                }
            }
            if(propFirst && r.getArithmeticRelationsWithAggregate().size()>1){
                if(!boundStarter){
                    if(!starter->isNegated()){
                        std::string plusOne = starter->isPlusOne() ? "": "-1";
                        *out << ind++ << "if(count"<<r.getRuleId()<<"_"<<starter->getFormulaIndex()<<" == "<<starter->getGuard().getStringRep()<<plusOne<<"){\n";
                    }else{
                        std::string plusOne = starter->isPlusOne() && starter->isNegated() ? "+1":"";
                        *out << ind++ << "if(count"<<r.getRuleId()<<"_"<<starter->getFormulaIndex()<<" == "<<starter->getGuard().getStringRep()<<plusOne<<"){\n";
                    }

                }
                propAggr(starter,aggrIdentifier,reason,"->");
            }
            int leaveOpenParenthesis = reason ? 2:1;
            
            while (closingParenthesis>leaveOpenParenthesis){
                *out << --ind <<"}\n";
                closingParenthesis--;
            }
            boundVariables.clear();
            for(std::string var : sharedVarsSet){
                boundVariables.insert(var);
            }
            if(!propFirst){
                std::string printElse = boundStarter ? "else":"";
                *out << ind++ << printElse <<"{\n";
                    std::string op = sharedVars!="" ? "->":".";
                    if(boundStarter){
                        printCanPropagateIf(aggrIdentifier,starter,op);
                        closingParenthesis++;
                    }else{
                        *out << ind << "tupleU=NULL;\n";
                    }
                    
                closingParenthesis++;
            }
            propLast=true;
            
        }while(!propFirst);

        
        for(int i=0;i<closingParenthesis;i++){
            *out << --ind <<"}\n";
        }
    \
}
void CompilationManager::compileConstraintWithAggregate(const aspc::Rule & r, unsigned start, const aspc::Program & p){

    std::vector<unsigned> joinOrder = r.getOrderedBodyIndexesByStarter(start);
    unsigned closingParenthesis = 0;
    std::unordered_set<std::string> boundVariables;

    if(r.getFormulas()[joinOrder[0]]->containsAggregate()){
        evaluationStartingFromAggregate(r,joinOrder,start);
    }else{
        evaluationEndingWithAggregate(r,joinOrder,start);
    }
}

void CompilationManager::compileConstrainWithAggregate(const aspc::Rule & r, unsigned start, const aspc::Program & p){
    std::vector<unsigned> joinOrder = r.getOrderedBodyIndexesByStarter(start);
    /*
    std::cout<<start<<std::endl;
    for(int i=0;i<joinOrder.size();i++)
        std::cout<<joinOrder[i]<<" ";
    std::cout<<std::endl;
    */
    const std::vector<const aspc::Formula*>& body = r.getFormulas();
    
    unsigned closingParenthesis = 0;
    
    std::unordered_set<std::string> boundVariables;
    
    int aggregateIndex=-1;
    bool again=true;
    unsigned startIndex=0;
    bool aggregateNotBound = false;
    
    //join loops, for each body formula
    do{
        again=false;
        for (unsigned i = startIndex; i < body.size(); i++) {
            const aspc::Formula* f = body[joinOrder[i]];
            if(f->containsAggregate()){
                if(aggregateIndex==-1)
                    aggregateIndex=i;
                
                aspc::ArithmeticRelationWithAggregate* aggregateRelation = (aspc::ArithmeticRelationWithAggregate*)f;
                bool decisionLevelCheck = start != body.size();

                if(i==0 && r.getBodyLiterals().size()>0){

                    //valutazione constraint parte dell'aggregato ed ho almeno un letterale esterno

                    std::string plusOne = aggregateRelation->isPlusOne() ? "+1":"";
                    
                    //*out << ind << "std::cout<<\"Evaluating starting from Aggregate\"<<std::endl;\n";            
                    *out << ind++ << "if(";
                    int aggrLitIndex=0;
                    for(const aspc::Literal& aggrLit : aggregateRelation->getAggregate().getAggregateLiterals()){

                        if(aggrLitIndex > 0)
                            *out << " || ";
                        *out << "starter.getPredicateName()== &_" << aggrLit.getPredicateName();

                        aggrLitIndex++;
                    }
                    *out << "){\n";


                    closingParenthesis++;
                    std::string aggrIdentifier = std::to_string(r.getRuleId())+":"+std::to_string(joinOrder[0]);
                    std::string sharedVars = sharedVariablesMap[aggrIdentifier];
                    std::string pairName = "joinTuples_"+std::to_string(r.getRuleId())+"_"+std::to_string(joinOrder[0]);
                    if(sharedVars!=""){
                        *out << ind++ << "for(const auto sharedVarTuple : sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<joinOrder[0]<<"){\n";
                        *out << ind << "tupleU=NULL;\n";
                        //*out << ind << "std::cout<<\"true sharedVars \"<<sharedVarTuple.second->first->size()<<std::endl;\n";
                        //*out << ind << "std::cout<<\"undef sharedVars \"<<sharedVarTuple.second->second->size()<<std::endl;\n";

                        closingParenthesis++;
                            int pos = sharedVars.find(',');
                            int var=0;
                            std::unordered_set<std::string> alreadyDeclared;

                            // se ho solo una variabile condivisa entro qui e non nel while
                            if(sharedVars!="" && pos == std::string::npos){
                                if(!alreadyDeclared.count(sharedVars)){
                                    *out << ind << "int " << sharedVars<< " = sharedVarTuple.first["<<var<<"];\n";
                                    alreadyDeclared.insert(sharedVars);
                                }
                                boundVariables.insert(sharedVars);
                                
                            }
                            while(pos!=std::string::npos){
                                if(!alreadyDeclared.count(sharedVars.substr(0,pos))){
                                    *out << ind << "int " << sharedVars.substr(0,pos)<< " = sharedVarTuple.first["<<var<<"];\n";
                                    alreadyDeclared.insert(sharedVars.substr(0,pos));
                                }
                                boundVariables.insert(sharedVars.substr(0,pos));
                                sharedVars=sharedVars.substr(pos+1,sharedVars.length());
                                pos = sharedVars.find(',');
                                var++;
                                if(sharedVars!="" && pos == std::string::npos){
                                    if(!alreadyDeclared.count(sharedVars)){
                                        *out << ind << "int " << sharedVars<< " = sharedVarTuple.first["<<var<<"];\n";
                                        alreadyDeclared.insert(sharedVars);
                                    }
                                    boundVariables.insert(sharedVars);
                                
                                }

                            }
                            
                        /*if(aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()=="asgn")
                        *out << ind << "std::cout<<\"sharedVars \"<<X<<std::endl;\n";
                        */
                            *out << ind << "std::pair<AuxMap*,AuxMap*> "<<pairName<<";\n";
                            *out << ind << pairName << ".first=sharedVarTuple.second->first;\n";
                            *out << ind << pairName << ".second=sharedVarTuple.second->second;\n";


                    }else{

                        std::string joinTupleName(aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]);
                        for(unsigned index: aggregateVariablesIndex[aggrIdentifier])
                            joinTupleName+=std::to_string(index)+"_";
                        *out << ind << "std::pair<AuxMap*,AuxMap*> "<<pairName<<";\n";
                        *out << ind << pairName << ".first = &p_"<<joinTupleName<<";\n";
                        *out << ind << pairName << ".second = &u_"<<joinTupleName<<";\n";
                        //*out << ind << "std::cout<<\"Size: \"<<joinTuples_0_1.first->size()<<std::endl;\n";


                    }
                    /**out << ind << "std::cout<<X_1<<std::endl;\n";
                    *out << ind << "std::cout<<\"trueSize map \"<<sharedVarTuple.second->first->size()<<std::endl;\n";
                    *out << ind << "std::cout<<\"undefSize map \"<<sharedVarTuple.second->second->size()<<std::endl;\n";*/
                    
                    if(aggregateRelation->isBoundedRelation(boundVariables)){
                        
                        //la guardia è una costante 

                        if(aggregateRelation->isNegated()){
                            std::string aggVarProjection="";
                            int currentVar=0;
                            for(int varIndex : aggregateVariablesIndex[aggrIdentifier]){
                                if(currentVar>0)
                                    aggVarProjection+=",";
                                aggVarProjection+="joinTupleUndef->at("+std::to_string(varIndex)+")";
                                currentVar++;
                            }

                            
                            *out << ind << "int actualSize"<<joinOrder[i]<<" = "<<pairName<<".first->size();\n";
                            *out << ind << "std::set<std::vector<int>> alreadyCounted;\n";
                            sharedVars = sharedVariablesMap[aggrIdentifier]; 
                            if( sharedVars ==""){
                                *out << ind++ << "for(const Tuple * joinTupleUndef : u_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<".getValues({})){\n";
                            }else{
                                std::string sharedVarProjection="";
                                for(unsigned index : sharedVariablesIndexesMap[aggrIdentifier]){
                                    sharedVarProjection+=std::to_string(index)+"_";
                                }
                                *out << ind++ << "for(const Tuple * joinTupleUndef : u_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<sharedVarProjection<<".getValues({"<<sharedVars<<"})){\n";
                            }
                                *out << ind++ << "if("<<pairName<<".first->getValues({"<<aggVarProjection<<"}).size()==0){\n";
                                    *out << ind++ << "if("<<pairName<<".second->getValues({"<<aggVarProjection<<"}).size()>0){\n";
                                        *out << ind << "std::vector<int> key({"<<aggVarProjection<<"});\n";
                                        *out << ind << "alreadyCounted.insert(key);\n";
                                    *out << --ind << "}\n";
                                *out << --ind << "}\n";
                            *out << --ind << "}\n";
                            *out << ind << "actualSize"<<joinOrder[i]<<"+=alreadyCounted.size();\n";
                            //*out << ind << "std::cout<<\"External propagation Total size: \"<<actualSize"<<joinOrder[i]<<"<<std::endl;\n";
                            *out << ind++ << "if(!(actualSize"<<joinOrder[i]<<aggregateRelation->getCompareTypeAsString()<<aggregateRelation->getGuard().getStringRep()<<plusOne<<")){\n";
                            //*out << ind << "std::cout<<\"aggr true\"<<std::endl;\n";
                            closingParenthesis++;
                        }else{
                            *out << ind++ << "if("<<pairName<<".first->size()"<<aggregateRelation->getCompareTypeAsString()<<aggregateRelation->getGuard().getStringRep()<<plusOne<<"){\n";
                            closingParenthesis++;    
                        }
                        
                        
                            //*out << ind << "std::cout<<\"Aggregate is True\"<<std::endl;\n";
                    }else if (aggregateRelation->isBoundedValueAssignment(boundVariables)){
                        /*isBoundedValueAssignment=true;
                        *out << ind << "int ";
                        for(std::string term : aggregateRelation->getGuard().getAllTerms()){
                            if(isVariable(term)){
                                boundVariables.insert(term);
                                *out << term;
                                break;
                            }
                        }
                        *out << " = joinTuples.first->size();\n";*/
                        
                    }else{
                        // la guardia non è bound
                        aggregateNotBound=true;
                        if(aggregateRelation->isNegated()){
                            std::string varProjection="";
                            int currentVar=0;
                            for(int varIndex : aggregateVariablesIndex[aggrIdentifier]){
                                if(currentVar>0)
                                    varProjection+=",";
                                varProjection+="joinTupleUndef->at("+std::to_string(varIndex)+")";
                                currentVar++;
                            }
                            *out << ind << "int count = "<<pairName<<".first->size();\n";
                            *out << ind << "std::set<std::vector<int>> alreadyCounted;\n";

                            sharedVars = sharedVariablesMap[aggrIdentifier]; 
    
                            if( sharedVars ==""){
                                *out << ind++ << "for(const Tuple * joinTupleUndef : u_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<".getValues({})){\n";
                            }else{
                                std::string sharedVarProjection="";
                                for(unsigned index : sharedVariablesIndexesMap[aggrIdentifier]){
                                    sharedVarProjection+=std::to_string(index)+"_";
                                }
                                *out << ind++ << "for(const Tuple * joinTupleUndef : u_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<sharedVarProjection<<".getValues({"<<sharedVars<<"})){\n";
                            }
                                *out << ind++ << "if("<<pairName<<".first->getValues({"<<varProjection<<"}).size()==0){\n";
                                    *out << ind++ << "if("<<pairName<<".second->getValues({"<<varProjection<<"}).size()>0){\n";
                                        *out << ind << "std::vector<int> key({"<<varProjection<<"});\n";
                                        *out << ind << "alreadyCounted.insert(key);\n";
                                    *out << --ind << "}\n";
                                *out << --ind << "}\n";
                            *out << --ind << "}\n";
                            *out << ind << "count+=alreadyCounted.size();\n";

                        }else{
                            *out << ind << "int count = "<<pairName<<".first->size();\n";

                        }
                        //*out << ind << "std::cout<<\"Count: \"<<count<<std::endl;\n";
                        *out << ind++ << "{\n";
                        closingParenthesis++;
                    }
                    if(r.getArithmeticRelationsWithAggregate().size()>1)
                        buildReason(aggrIdentifier,(aspc::ArithmeticRelationWithAggregate*)r.getFormulas()[joinOrder[0]],true);
                    //std::cout<<"End Evaluating starting from Aggregate "<<boundVariables.count("X")<<std::endl;   
                    /*if(aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()=="asgn"){
                        *out << ind << "if(!tupleU) std::cout<<\"Tuple is NULL\"<<std::endl;\n";
                        *out<<ind << "std::cout<<\"Check hasChild11\"<<std::endl;\n";         
                        *out << ind << "const Tuple * child = (whaschild.find({X}));\n";
                        *out << ind++ << "if(!child && !tupleU){\n";
                            *out << ind << "child = (uhaschild.find({X}));\n";
                            *out << ind++ << "if(!child){\n";
                                *out << ind << "std::cout<<\"not found\"<<std::endl;\n";
                            *out << --ind << "}else std::cout<<\"undef\"<<std::endl;\n";
                        *out << --ind << "}else std::cout<<\"true\"<<std::endl;\n";
                        
                    }*/
                        
                }else{
                    //se mi trovo a livello decisionale -1 oppure valuto l'aggregato come ultimo 

                    //*out << ind << "std::cout<<\"Evaluate Aggregate as Last" << decisionLevelCheck << "\"<<std::endl;\n";
                    evaluateAggregateAsLast(decisionLevelCheck,aggregateRelation,joinOrder,i,r,aggregateIndex==-2);
                }
                //std::cout<<"Starter : "<<start<<" Aggregate "<<i<<std::endl;
            }else if (i != 0 || start == r.getBodySize()) {
                //std::cout<<"Evaluating literal"<<boundVariables.count("X")<<std::endl;
                if (f->isLiteral()) {
                    aspc::Literal* l = (aspc::Literal*)f;
                    if (l->isNegated() || l->isBoundedLiteral(boundVariables)) {
                        if (mode == LAZY_MODE) {
                            std::string negation = "";
                            if (l->isNegated()) {
                                negation = "!";
                            }
                            *out << ind << "const Tuple * tuple" << i << " = w" << l->getPredicateName() << ".find({";
                            printLiteralTuple(l);
                            *out << "});\n";
                            *out << ind++ << "if(" << negation << "tuple" << i << "){\n";
                            closingParenthesis++;
                        } else {
                            //mode == EAGER_MODE
                            bool appearsBefore = literalPredicateAppearsBeforeSameSign(body, joinOrder, i);
                            if(appearsBefore && l->isPositiveLiteral()) {
                                *out << ind << "const Tuple * tuple" << i << " = NULL;\n";
                                *out << ind++ << "if(tupleU && tupleU->getPredicateName() == &_"<<l->getPredicateName()<<"){\n"; 
                                *out << ind << "const Tuple * undefRepeatingTuple = (u" << l->getPredicateName() << ".find({";
                                printLiteralTuple(l);
                                *out << "}));\n";
                                *out << ind++ << "if(tupleU == undefRepeatingTuple){\n"; 
                                *out << ind << "tuple" << i << " = undefRepeatingTuple;\n";
                                *out << --ind << "}\n";
                                *out << --ind << "}\n";
                                *out << ind++ << "if(!tuple"<<i<<"){\n;"; 
                                *out << ind << "tuple" << i << " = (w" << l->getPredicateName() << ".find({";
                                printLiteralTuple(l);
                                *out << "}));\n";
                                *out << --ind << "}\n";
                                *out << ind++ << "if(!tuple"<<i<<" && !tupleU){\n;"; 
                                *out << ind << "tuple" << i << " = tupleU = (u" << l->getPredicateName() << ".find({";
                                printLiteralTuple(l);
                                *out << "}));\n";
                                *out << --ind << "}\n";
                                *out << ind << "tupleUNegated = false;\n";
                                *out << ind++ << "if(tuple" << i << "){\n";
                                closingParenthesis++;                          
                            }
                            else if(appearsBefore && l->isNegated()) {
                                *out << ind << "const Tuple * tuple" << i << " = NULL;\n";
                                *out << ind << "const Tuple negativeTuple = Tuple({";
                                printLiteralTuple(l);
                                *out << "}, &_" << l->getPredicateName() << ", true);\n";
                                *out << ind++ << "if(tupleU && tupleU->getPredicateName() == &_"<<l->getPredicateName()<<"){\n;"; 
                                *out << ind << "const Tuple * undefRepeatingTuple = (u" << l->getPredicateName() << ".find({";
                                printLiteralTuple(l);
                                *out << "}));\n";
                                *out << ind++ << "if(tupleU == undefRepeatingTuple){\n"; 
                                *out << ind << "tuple" << i << " = undefRepeatingTuple;\n";
                                *out << --ind << "}\n";
                                *out << --ind << "}\n";
                                *out << ind++ << "if(!tuple"<<i<<"){\n;"; 
                                *out << ind++ << "if(!(w" << l->getPredicateName() << ".find({";
                                printLiteralTuple(l);
                                *out << "}))){\n";
                                *out << ind << "tuple" << i << " = &negativeTuple;\n";
                                *out << --ind << "}\n";
                                *out << --ind << "}\n";
                                *out << ind++ << "if(tuple" << i << "){\n";
                                closingParenthesis++;                                                
                            }
                            else if (l->isNegated()) {
                                *out << ind << "const Tuple negativeTuple = Tuple({";
                                printLiteralTuple(l);
                                *out << "}, &_" << l->getPredicateName() << ", true);\n";
                                *out << ind << "const Tuple * tuple" << i << " = &negativeTuple;\n";
                                *out << ind << "bool lTrue = (w" << l->getPredicateName() << ".find(negativeTuple)!=NULL);\n";
                                *out << ind << "const Tuple * undefTuple = u" << l->getPredicateName() << ".find(negativeTuple);\n";
                                *out << ind++ << "if((!lTrue && undefTuple == NULL) || (undefTuple && tupleU == NULL)){\n";
                                *out << ind++ << "if(undefTuple){\n";
                                *out << ind << "tuple" << i << " = tupleU = undefTuple;\n";
                                *out << ind << "tupleU->print();\n";
                                *out << ind << "tupleUNegated = true;\n";
                                *out << --ind << "}\n";
                                closingParenthesis++;

                            } else {
                                *out << ind << "const Tuple * tuple" << i << " = (w" << l->getPredicateName() << ".find({";
                                printLiteralTuple(l);
                                *out << "}));\n";
                                *out << ind++ << "if(!tuple" << i << " && !tupleU ){\n";
                                *out << ind << "tuple" << i << " = tupleU = (u" << l->getPredicateName() << ".find({";
                                printLiteralTuple(l);
                                *out << "}));\n";
                                *out << ind << "tupleUNegated = false;\n";
                                *out << --ind << "}\n";
                                *out << ind++ << "if(tuple" << i << "){\n";
                                closingParenthesis++;
                            }
                        }

                    } else {
                        std::string mapVariableName = l->getPredicateName() + "_";
                        for (unsigned t = 0; t < l->getAriety(); t++) {
                            if ((l->isVariableTermAt(t) && boundVariables.count(l->getTermAt(t))) || !l->isVariableTermAt(t)) {
                                mapVariableName += std::to_string(t) + "_";
                            }
                        }
                        if (mode == LAZY_MODE) {
                            *out << ind << "const std::vector<const Tuple* >& tuples = p" << mapVariableName << ".getValues({";
                            printLiteralTuple(l, boundVariables);
                            *out << "});\n";
                            *out << ind++ << "for( unsigned i=0; i< tuples.size();i++){\n";
                            *out << ind << "const Tuple * tuple" << i << " = tuples[i];\n";
                            closingParenthesis++;
                        } else {
                            //mode == EAGER_MODE
                            *out << ind << "const std::vector<const Tuple* >* tuples;\n";
                            *out << ind << "tuples = &p" << mapVariableName << ".getValues({";
                            printLiteralTuple(l, boundVariables);
                            *out << "});\n";
                            *out << ind << "const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;\n";
                            bool appearsBefore = literalPredicateAppearsBeforeSameSign(body, joinOrder, i);
                            if (appearsBefore) {
                                *out << ind << "std::vector<const Tuple* > tupleUInVector;\n";
                            }
                            *out << ind++ << "if(tupleU == NULL){\n";
                            *out << ind << "tuplesU = &u" << mapVariableName << ".getValues({";
                            printLiteralTuple(l, boundVariables);
                            *out << "});\n";
                            *out << --ind << "}\n";
                            //repeating literal case

                            if (appearsBefore) {
                                *out << ind++ << "else {\n";
                                //handle constants and equal cards?
                                *out << ind++ << "if(tupleU && !tupleUNegated && tupleU->getPredicateName() == &_"<<l->getPredicateName()<<") {\n";
                                //check that bound variables have proper value
                                vector<unsigned> boundIndexes;
                                for(unsigned v = 0; v < l->getAriety(); v++) {
                                    if(boundVariables.count(l->getTermAt(v))) {
                                        boundIndexes.push_back(v);
                                    }
                                }
                                if(boundIndexes.size()) {
                                    *out << ind++ << "if(";
                                     for(unsigned bi = 0; bi < boundIndexes.size(); bi++) {
                                         if(bi > 0) {
                                             *out << " && ";
                                         }
                                         *out << "tupleU->at(" << boundIndexes[bi] << ") == " << l->getTermAt(boundIndexes[bi]);
                                     }
                                     *out << "){\n";
                                }

                                *out << ind << "tupleUInVector.push_back(tupleU);\n";
                                if(boundIndexes.size()) {
                                     *out << --ind << "}\n";
                                }
                                *out << --ind << "}\n";
                                *out << --ind << "}\n";
                            }

                            if (!appearsBefore) {
                                *out << ind++ << "for( unsigned i=0; i< tuples->size() + tuplesU->size();i++){\n";
                                *out << ind << "const Tuple * tuple" << i << " = NULL;\n";
                                *out << ind++ << "if(i<tuples->size()){\n";
                                *out << ind << "tuple" << i << " = tuples->at(i);\n";
                                *out << ind++ << "if(tuplesU != &EMPTY_TUPLES) {\n";
                                *out << ind << "tupleU = NULL;\n";
                                *out << --ind << "}\n";
                                *out << --ind << "}\n";
                                *out << ind++ << "else {\n";
                                *out << ind << "tuple" << i << " = tuplesU->at(i-tuples->size());\n";
                                *out << ind << "tupleU = tuple" << i << ";\n";
                                *out << ind << "tupleUNegated = false;\n";
                                *out << --ind << "}\n";
                            } else {
                                *out << ind++ << "for( unsigned i=0; i< tuples->size() + tuplesU->size() + tupleUInVector.size();i++){\n";
                                *out << ind << "const Tuple * tuple" << i << " = NULL;\n";
                                *out << ind++ << "if(i<tuples->size()){\n";
                                *out << ind << "tuple" << i << " = tuples->at(i);\n";
                                *out << ind++ << "if(tuplesU != &EMPTY_TUPLES) {\n";
                                *out << ind << "tupleU = NULL;\n";
                                *out << --ind << "}\n";
                                *out << --ind << "}\n";
                                *out << ind++ << "else if(i<tuples->size()+tuplesU->size()) {\n";
                                *out << ind << "tuple" << i << " = tuplesU->at(i-tuples->size());\n";
                                *out << ind << "tupleU = tuple" << i << ";\n";
                                *out << ind << "tupleUNegated = false;\n";
                                *out << --ind << "}\n";
                                *out << ind++ << "else {\n";
                                *out << ind << "tuple" << i << " = tupleU;\n";
                                *out << ind << "tupleUNegated = false;\n";
                                *out << --ind << "}\n";
                            }
                            closingParenthesis++;
                        }
                    }
                } else if(f->containsAggregate()){


                }else{

                    aspc::ArithmeticRelation* f = (aspc::ArithmeticRelation*) body[joinOrder[i]];
                    if (f-> isBoundedRelation(boundVariables)) {
                        *out << ind++ << "if(" << f->getStringRep() << ") {\n";
                        closingParenthesis++;
                    } else {
                        //bounded value assignment
                        *out << ind << "int " << f->getAssignmentStringRep(boundVariables) << ";\n";
                        boundVariables.insert(f->getAssignedVariable(boundVariables));

                    }

                }

            }
            
            if (f->isPositiveLiteral() || (i == 0 && f->isLiteral())) {
                aspc::Literal* l = (aspc::Literal*)f;
                std::unordered_set<std::string> declaredVariables;
                for (unsigned t = 0; t < l->getAriety(); t++) {
                    if (l->isVariableTermAt(t) && !boundVariables.count(l->getTermAt(t)) && !declaredVariables.count(l->getTermAt(t))) {
                        *out << ind << "int " << l->getTermAt(t) << " = (*tuple" << i << ")[" << t << "];\n";
                        declaredVariables.insert(l->getTermAt(t));
                    }
                }
            }

            if (!r.getFormulas()[joinOrder[i]]->isBoundedLiteral(boundVariables) && !r.getFormulas()[joinOrder[i]]->isBoundedRelation(boundVariables) && r.getFormulas()[joinOrder[i]]->isLiteral() ) {
                r.getFormulas()[joinOrder[i]]->addVariablesToSet(boundVariables);
            }

            if (handleEqualCardsAndConstants(r, i, joinOrder))
                closingParenthesis++;

            //rule fires
            if (i == body.size() - 1) {

                if (!r.isConstraint()) {

                    //a rule is firing
                    string tupleType = "TupleWithoutReasons";
                    if (p.hasConstraint()) {
                        tupleType = "TupleWithReasons";
                    }
                    *out << ind << "const auto & insertResult = w" << r.getHead().front().getPredicateName() << ".insert(" << tupleType << "({";

                    for (unsigned th = 0; th < r.getHead().front().getTermsSize(); th++) {
                        if (!r.getHead().front().isVariableTermAt(th)) {
                            if (isInteger(r.getHead().front().getTermAt(th))) {
                                *out << r.getHead().front().getTermAt(th);
                            } else {
                                *out << "ConstantsManager::getInstance().mapConstant(\"" << escapeDoubleQuotes(r.getHead().front().getTermAt(th)) << "\")";
                            }
                        } else {
                            *out << r.getHead().front().getTermAt(th);
                        }
                        if (th < r.getHead().front().getTermsSize() - 1) {
                            *out << ",";
                        }
                    }


                    *out << "}, &_" << r.getHead().front().getPredicateName() << "));\n";
                    *out << ind++ << "if(insertResult.second){\n";

                    if (p.hasConstraint()) {
                        for (unsigned i = 0; i < body.size(); i++) {
                            if (body[joinOrder[i]]->isPositiveLiteral()) {
                                *out << ind << "insertResult.first->addPositiveReason(tuple" << i << ");\n";
                            } else if (body[joinOrder[i]]->isLiteral()) {
                                aspc::Literal* l = (aspc::Literal*) body[joinOrder[i]];
                                *out << ind << "insertResult.first->addNegativeReason(Tuple({";
                                printLiteralTuple(l);
                                *out << "}, &_" << l->getPredicateName() << ", true));\n";
                            }
                        }
                    }

                    for (const std::string& auxMapName : predicateToAuxiliaryMaps[r.getHead().front().getPredicateName()]) {
                        *out << ind << "p" << auxMapName << ".insert2(*w" << r.getHead().front().getPredicateName() << ".getTuples().back());\n";
                    }

                    *out << --ind << "}\n";
                } else {
                    //*out << ind << "std::cout<<\"constraint failed\"<<std::endl;\n";
                    //we are handling a constraint
                    if (mode == LAZY_MODE) {
                        *out << ind << "failedConstraints.push_back(std::vector<aspc::Literal>());\n";

                        *out << ind << "std::vector<const Tuple *> reasons;\n";

                        for (unsigned i = 0; i < body.size(); i++) {
                            if (body[joinOrder[i]]->isLiteral()) {
                                aspc::Literal* l = (aspc::Literal*) body[joinOrder[i]];
                                if (idbs.count(l->getPredicateName()) || headPredicates.count(l->getPredicateName())) {
                                    *out << ind << "std::unordered_set<std::string> open_set" << i << ";\n";
                                    if (l->isPositiveLiteral()) {
                                        *out << ind << "explainPositiveLiteral(tuple" << i << ", open_set" << i << ", reasons);\n";
                                    } else {
                                        *out << ind << "Tuple tuple" << i << " = Tuple({";
                                        printLiteralTuple(l);
                                        *out << "}, &_" << l->getPredicateName() << ", true);\n";
                                        *out << ind << "explainNegativeLiteral(&tuple" << i << ", open_set" << i << ", reasons);\n";
                                        //*out << ind << "failedConstraints.back().push_back(tupleToLiteral(Tuple({";
                                        //writeNegativeTuple(r, joinOrder, start, i);
                                        //*out << "}, 0, &ConstantsManager::getInstance().getPredicateName(\"" << l->getPredicateName() << "\"), true)));\n";
                                    }
                                }
                            }
                        }
                        *out << ind++ << "for(const Tuple * reason: reasons) {\n";
                        *out << ind << "failedConstraints.back().push_back(tupleToLiteral(*reason));\n";
                        *out << --ind << "}\n";


                        
                    } else {

                        //mode == EAGER_MODE
                        //*out << ind << "aspc::Literal propagatedLiteral = (tupleToLiteral(*tupleU));\n";
                        //*out << ind << "propagatedLiteral.setNegated(tupleUNegated);\n";
                        //TODO maybe negate literal
                        if(aggregateIndex == -2){
                            aspc::ArithmeticRelationWithAggregate* aggregateRelation = (aspc::ArithmeticRelationWithAggregate*)body[joinOrder[0]];

                            std::string minusOne = aggregateRelation->getCompareTypeAsString() == ">=" ? "-1" : "";
                            std::string plusOne = aggregateRelation->isPlusOne() ? "+1" : "";

                            if(aggregateNotBound){
                                //*out << ind << "std::cout<<X<<\" \"<<count<<std::endl;\n";
                                //*out << ind << "if(tupleU==NULL) tuple1->print();std::cout<<std::endl;\n";
                                if(aggregateRelation->isNegated()){
                                    minusOne = aggregateRelation->getCompareTypeAsString() == ">=" ? "":"+1";
                                }
                                *out << ind++ << "if(tupleU ==  NULL && count == "<<aggregateRelation->getGuard().getStringRep()<<plusOne<<minusOne<<") {\n";
                                //*out << ind << "std::cout<<\"propagate aggregateLiteral\"<<std::endl;\n";
                            }else{
                                *out << ind++ << "if(tupleU ==  NULL) {\n";
                            }
                            
                            //*out << ind << "std::cout<<\"can propagate\"<<std::endl;\n";
                            std::string aggrIdentifier = std::to_string(r.getRuleId())+":"+std::to_string(joinOrder[0]);
                            std::string* sharedVariables = &sharedVariablesMap[aggrIdentifier];
                            std::string pairName = "joinTuples_"+std::to_string(r.getRuleId())+"_"+std::to_string(joinOrder[0]);

                            *out << ind << "std::pair<AuxMap*,AuxMap*> "<<pairName<<";\n";
                            *out << ind << pairName << ".first = &p_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<";\n";
                            *out << ind << pairName << ".second = &u_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<";\n";
                            if(*sharedVariables == ""){
                                *out << ind << "std::pair<AuxMap*,AuxMap*> "<< pairName << "SharedVariables;\n";
                                std::string name="";
                                for(unsigned index : aggregateVariablesIndex[aggrIdentifier])
                                    name+=std::to_string(index)+"_";


                                *out << ind << pairName << "SharedVariables.first = &p_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<name<<";\n";
                                *out << ind << pairName << "SharedVariables.second = &u_"<<aggregateLiteralToPredicateWSet[aggregateRelation->getAggregate().getAggregateLiterals()[0].getPredicateName()+"_"+aggrIdentifier]<<name<<";\n";

                            }else{
                                *out << ind << "std::pair<AuxMap*,AuxMap*> "<< pairName << "SharedVariables = *sharedVariables_"<<r.getRuleId()<<"_ToAggregate_"<<joinOrder[0]<<"[std::vector<int>({"<<*sharedVariables<<"})];\n";
                            }    
                            // if r.getFormulas()[joinOrder[joinOrder.size()-1]]->containsAggregate() then reason already built for double aggregate 
                            buildReason(aggrIdentifier,aggregateRelation,!r.getFormulas()[joinOrder[joinOrder.size()-1]]->containsAggregate());
                            for (unsigned i = 1; i < body.size(); i++) {
                                if (body[joinOrder[i]]->isLiteral()) {
                                    *out << ind << "const auto & it_reason"<<i<<" = tupleToVar.find(Tuple(*tuple"<<i<<"));\n";
                                    *out << ind++ << "if(it_reason"<<i<<"!=tupleToVar.end())\n";
                                        *out << ind-- << "reason.push_back(it_reason"<<i<<"->second * (tuple"<<i<<"->isNegated() ? -1:1));\n";
                                }
                            }
                            *out << ind << "const auto & it_reason_starter = tupleToVar.find(Tuple(starter));\n";
                            *out << ind++ << "if(it_reason_starter!=tupleToVar.end())\n";
                                *out << ind-- << "reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));\n";
                            //*out << ind << "std::cout<<\"propagating Aggregate starting from aggregate\"<<std::endl;\n";
                            propagateAggregate(aggregateRelation,aggrIdentifier,true);
                            *out << --ind << "}\n";
                            if(r.getFormulas()[joinOrder[body.size()-1]]->containsAggregate()){
                                *out << --ind << "}//close aggregate if\n";
                                *out << --ind << "}//close tupleU if\n";

                            }
                            
                        }else if(!r.containsAggregate()){
                            //needed for propagations at level 0.. constraint may fail, then return incoherence (value 1)
                            *out << ind++ << "if(!tupleU) {\n";
                            *out << ind << "std::cout<<\"conflict detected in propagator\"<<std::endl;\n";
                            *out << ind << "propagatedLiteralsAndReasons.insert({-1, std::vector<int>("/*<<reason*/<<")});\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "else {\n";
                            *out << ind << "const auto & it = tupleToVar.find(*tupleU);\n";

            #ifdef EAGER_DEBUG
                                *out << ind << "std::cout<<\"propagating \";\n";
                                *out << ind << "std::cout<<(-1 * sign* ((int) (it->second)))<<\" \";\n";
                                *out << ind++ << "if(sign > 0) {\n";
                                *out << ind << "std::cout<<\"not \";\n";
                                *out << --ind << "}\n";
                                *out << ind << "tupleU->print();\n";
                                *out << ind << "std::cout<<\"\\n\";\n";
            #endif

                            *out << ind++ << "if(it != tupleToVar.end()) {\n";
                            *out << ind << "int sign = tupleU->isNegated() ? -1 : 1;\n";
                            *out << ind << "auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;\n";
                            //*out << ind << "propagatedLiteralsAndReasons[tupleToVar[*tupleU]] = std::vector<int>();\n";
                            //*out << ind << "std::cout<<\"constraint failed\"<<std::endl;\n";

                            //*out << ind << "reasonsForPropagatedLiterals.push_back(std::vector<aspc::Literal>());\n";

                            *out << ind << "std::vector<const Tuple *> reasons;\n";
                            for (unsigned i = 0; i < body.size(); i++) {
                                if (body[joinOrder[i]]->isLiteral()) {
                                    aspc::Literal* l = (aspc::Literal*) body[joinOrder[i]];
                                    //if (idbs.count(l->getPredicateName()) || headPredicates.count(l->getPredicateName())) {
                                    *out << ind++ << "if(tuple" << i << " != tupleU){\n";
                                    *out << ind << "std::unordered_set<std::string> open_set" << i << ";\n";
        #ifdef EAGER_DEBUG
                                    *out << ind << "tuple" << i << "->print();\n";
                                    *out << ind << "std::cout<<\"\\n\";\n";
        #endif
                                    if (l->isPositiveLiteral()) {
                                        //*out << ind << "reasons.push_back(tuple" << i << ");\n";
                                        *out << ind << "explainPositiveLiteral(tuple" << i << ", open_set" << i << ", reasons);\n";

                                    } else {
                                        //                                *out << ind << "Tuple tuple" << i << " = Tuple({";
                                        //                                printLiteralTuple(l);
                                        //                                *out << "}, &_" << l->getPredicateName() << ", true);\n";
                                        *out << ind << "reasons.push_back(tuple" << i << ");\n";
                                        //*out << ind << "explainNegativeLiteral(tuple" << i << ", open_set" << i << ", reasons);\n";
                                    }
                                    *out << --ind << "}\n";
                                    //}
                                }
                            }
                            *out << ind << "reas.reserve(reasons.size());\n";
                            *out << ind++ << "for(const Tuple * reason: reasons) {\n";
                            *out << ind << "const auto & it = tupleToVar.find(*reason);\n";
                            *out << ind++ << "if(it != tupleToVar.end()) {\n";
                            *out << ind << "reas.push_back(it->second * (reason->isNegated()? -1:1));\n";
                            *out << --ind << "}\n";
                            *out << --ind << "}\n";
                            //*out << ind++ << "if(decisionLevel == -1) {\n";
                            //*out << ind << "executeProgramOnFacts({-1,it->second*sign*-1});\n";
                            //*out << --ind << "}\n";
                            *out << --ind << "}\n";
                            *out << --ind << "}\n";
                        }else if(aggregateIndex==0 && r.getBodyLiterals().size()>0){
                                
                                if(!r.getFormulas()[joinOrder[body.size()-1]]->containsAggregate()){
                                    //propagazione partita dall'aggregato
                                    aspc::ArithmeticRelationWithAggregate* aggregateRelation = (aspc::ArithmeticRelationWithAggregate*)body[joinOrder[0]];
                                    std::string plusOne = aggregateRelation->isPlusOne() ? "+1":"";
                                    if(aggregateNotBound){
                                        std::string negation = aggregateRelation->isNegated() ? "!":"";
                                        *out << ind++ << "if("<<negation<<"(count " <<aggregateRelation->getCompareTypeAsString() <<aggregateRelation->getGuard().getStringRep()<<plusOne << ")){\n";
                                        //*out << ind << "std::cout<<\"Count: \"<<count<<\" X: \"<<X<<std::endl;\n";
                                    }
                                    std::string aggrIdentifier(std::to_string(r.getRuleId())+":"+std::to_string(joinOrder[0]));
                                    //*out << ind << "std::cout<<\"Building reason\"<<std::endl;\n";
                                    buildReason(aggrIdentifier,aggregateRelation,true);
                                    //*out << ind << "std::cout<<\"Reason built\"<<std::endl;\n";
                                    //*out << ind << "bool extJoin = true;\n";
                                    for (unsigned j = 1;j <= joinOrder[0]; j++) {
                                        if (r.getFormulas()[joinOrder[j]]->isLiteral()) {

                                            *out << ind++ << "if(tuple"<<j<<"!=tupleU){\n";
                                                *out << ind << "const auto & it_reason"<<j<<" = tupleToVar.find(Tuple(*tuple"<<j<<"));\n";
                                                *out << ind++ << "if(it_reason"<<j<<"!=tupleToVar.end())\n";
                                                    *out << ind-- << "reason.push_back(it_reason"<<j<<"->second * (tuple"<<j<<"->isNegated() ? -1:1));\n";
                                            *out << --ind <<"}\n";
                                            //*out << ind << "std::cout<<\"Tuple "<<j<<"\";tuple"<<j<<"->print();std::cout<<std::endl;\n";
                                            //*out << ind++ << "if(!tuple"<<j<<")\n";
                                            //*out << ind-- << "extJoin = false;\n";
                                        }
                                    }
                                    *out << ind << "const auto & it_reason_starter = tupleToVar.find(Tuple(starter));\n";
                                    *out << ind++ << "if(it_reason_starter!=tupleToVar.end())\n";
                                        *out << ind-- << "reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));\n";

                                    *out << ind++ << "if(tupleU == NULL) {\n";
                                        
                                        *out << ind << "std::cout<<\"conflict detected in propagator External Propagation"<<aggregateRelation->getFormulaIndex()<<"\"<<std::endl;\n";
                                        *out << ind << "propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});\n";
                                    *out << --ind << "}\n";
                                    *out << ind++ << "else {\n";
                                    
                                        

                                        *out << ind << "const auto & it = tupleToVar.find(*tupleU);\n";
                                        *out << ind++ << "if(it != tupleToVar.end()) {\n";
                                            //*out << ind << "std::cout<<\"External propagation\";tupleU->print();std::cout<<std::endl;\n";
                                            *out << ind << "int sign = tupleU->isNegated() ? -1 : 1;\n";
                                            *out << ind << "propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;\n";
                                        *out << --ind << "}\n";
                                    *out << --ind << "}\n";
                                    if(aggregateNotBound){
                                        *out << --ind << "}\n";
                                    }

                                }
                                
                        }
                    }
                    //TESTING FEATURE, LIMIT NUMBER OF FAILED CONSTRAINTS
                    //                *out << ind++ << "if(failedConstraints.size() >= 1000) {\n";
                    //                *out << ind << "return;\n";
                    //                *out << --ind << "}\n";
                }

            }

            
            //std::cout<<"Next formula"<<boundVariables.count("X")<<std::endl;
        }
        for (unsigned i = 0; i < closingParenthesis; i++){
            if(aggregateIndex == 0 && r.getBodyLiterals().size()>0){
                if(sharedVariablesMap[std::to_string(r.getRuleId())+":"+std::to_string(joinOrder[0])]!=""){
                    //se ho delle variabili condivise l'if sul predicateName e il for sulle sharedVariables si chiudono dopo
                    if(i == closingParenthesis-2)
                        break;
                }else{

                    //se non ho delle variabili condivise l'if sul predicateName si chiude dopo
                    if(i == closingParenthesis-1)
                        break;
                }
            }    

            *out << --ind << "}//close par\n";
        }
        if(aggregateIndex == 0 && r.getBodyLiterals().size()>0){
            //itero i letterali del corpo senza considerare l'aggregato
            again=true;
            startIndex=1;
            aggregateIndex=-2;
            boundVariables.clear();
            std::string sharedVars = sharedVariablesMap[std::to_string(r.getRuleId())+":"+std::to_string(joinOrder[0])];
            
            if(sharedVars!=""){
                closingParenthesis=2;
                //se ho delle variabili condivise allora mi trovo dentro il for dove queste sono fissate
                int pos = sharedVars.find(',');
                if(pos == std::string::npos){
                    boundVariables.insert(sharedVars);
                }
                while(pos!=std::string::npos){
                    boundVariables.insert(sharedVars.substr(0,pos));
                    sharedVars = sharedVars.substr(pos+1,sharedVars.size());
                    pos = sharedVars.find(',');
                    if(pos == std::string::npos){
                        boundVariables.insert(sharedVars);
                }
                }
            }else
                closingParenthesis=1;
            aspc::ArithmeticRelationWithAggregate* ar = (aspc::ArithmeticRelationWithAggregate*)body[joinOrder[0]];
            std::string pairName = "joinTuples_"+std::to_string(r.getRuleId())+"_"+std::to_string(joinOrder[0]);

            if(!aggregateNotBound){
                
                //la guardia è bound 
                *out << ind++ << "else{\n";
                    closingParenthesis++;
                    std::string plusOne = ar->isPlusOne() ? "+1" : "";

                    if(ar->isNegated()){

                        *out << ind++ << "if(actualSize"<<ar->getFormulaIndex()<<"=="<<ar->getGuard().getStringRep()<<plusOne<<"){\n";

                    }else{
                        std::string minusOne = ar->getCompareTypeAsString() == ">=" ? "-1" : "";
                        *out << ind++ << "if("<< pairName << ".first->size()=="<<ar->getGuard().getStringRep()<<plusOne<<minusOne<<"){\n";

                    }
                //*out << ind << "std::cout<<\"Aggregate is False starting propagation\"<<std::endl;\n";
                
                closingParenthesis++;
            }else{
                *out << ind++ << "{\n";
                *out << ind << "tupleU=NULL;\n";
                //*out << ind << "std::cout<<\"aggregate not true\"<<std::endl;\n";
                closingParenthesis++;
            }

            //*out << ind << "std::cout<<\"starting propagation \"<<std::endl;\n";


        }
    }while(again);
}
void CompilationManager::compileRule(const aspc::Rule & r, unsigned start, const aspc::Program & p) {
        
    //Iterate over starting workingset
    std::vector<unsigned> joinOrder = r.getOrderedBodyIndexesByStarter(start);
    const std::vector<const aspc::Formula*>& body = r.getFormulas();
    unsigned closingParenthesis = 0;
    std::unordered_set<std::string> boundVariables;


    //join loops, for each body formula
    for (unsigned i = 0; i < body.size(); i++) {
        const aspc::Formula* f = body[joinOrder[i]];
        if (i != 0 || start == r.getBodySize()) {
            if (f->isLiteral()) {
                aspc::Literal* l = (aspc::Literal*)f;
                if (l->isNegated() || l->isBoundedLiteral(boundVariables)) {

                    if (mode == LAZY_MODE) {
                        std::string negation = "";
                        if (l->isNegated()) {
                            negation = "!";
                        }
                        *out << ind << "const Tuple * tuple" << i << " = w" << l->getPredicateName() << ".find({";
                        printLiteralTuple(l);
                        *out << "});\n";
                        *out << ind++ << "if(" << negation << "tuple" << i << "){\n";
                        closingParenthesis++;
                    } else {
                        //mode == EAGER_MODE
                        bool appearsBefore = literalPredicateAppearsBeforeSameSign(body, joinOrder, i);
                        if(appearsBefore && l->isPositiveLiteral()) {
                            *out << ind << "const Tuple * tuple" << i << " = NULL;\n";
                            *out << ind++ << "if(tupleU && tupleU->getPredicateName() == &_"<<l->getPredicateName()<<"){\n;"; 
                            *out << ind << "const Tuple * undefRepeatingTuple = (u" << l->getPredicateName() << ".find({";
                            printLiteralTuple(l);
                            *out << "}));\n";
                            *out << ind++ << "if(tupleU == undefRepeatingTuple){\n"; 
                            *out << ind << "tuple" << i << " = undefRepeatingTuple;\n";
                            *out << --ind << "}\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "if(!tuple"<<i<<"){\n;"; 
                            *out << ind << "tuple" << i << " = (w" << l->getPredicateName() << ".find({";
                            printLiteralTuple(l);
                            *out << "}));\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "if(!tuple"<<i<<" && !tupleU){\n;"; 
                            *out << ind << "tuple" << i << " = tupleU = (u" << l->getPredicateName() << ".find({";
                            printLiteralTuple(l);
                            *out << "}));\n";
                            *out << --ind << "}\n";
                            *out << ind << "tupleUNegated = false;\n";
                            *out << ind++ << "if(tuple" << i << "){\n";
                            closingParenthesis++;                          
                        }
                        else if(appearsBefore && l->isNegated()) {
                            *out << ind << "const Tuple * tuple" << i << " = NULL;\n";
                            *out << ind << "const Tuple negativeTuple = Tuple({";
                            printLiteralTuple(l);
                            *out << "}, &_" << l->getPredicateName() << ", true);\n";
                            *out << ind++ << "if(tupleU && tupleU->getPredicateName() == &_"<<l->getPredicateName()<<"){\n;"; 
                            *out << ind << "const Tuple * undefRepeatingTuple = (u" << l->getPredicateName() << ".find({";
                            printLiteralTuple(l);
                            *out << "}));\n";
                            *out << ind++ << "if(tupleU == undefRepeatingTuple){\n"; 
                            *out << ind << "tuple" << i << " = undefRepeatingTuple;\n";
                            *out << --ind << "}\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "if(!tuple"<<i<<"){\n;"; 
                            *out << ind++ << "if(!(w" << l->getPredicateName() << ".find({";
                            printLiteralTuple(l);
                            *out << "}))){\n";
                            *out << ind << "tuple" << i << " = &negativeTuple;\n";
                            *out << --ind << "}\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "if(tuple" << i << "){\n";
                            closingParenthesis++;                                                
                        }
                        else if (l->isNegated()) {
                            *out << ind << "const Tuple negativeTuple = Tuple({";
                            printLiteralTuple(l);
                            *out << "}, &_" << l->getPredicateName() << ", true);\n";
                            *out << ind << "const Tuple * tuple" << i << " = &negativeTuple;\n";
                            *out << ind << "bool lTrue = (w" << l->getPredicateName() << ".find(negativeTuple)!=NULL);\n";
                            *out << ind << "const Tuple * undefTuple = u" << l->getPredicateName() << ".find(negativeTuple);\n";
                            *out << ind++ << "if((!lTrue && undefTuple == NULL) || (undefTuple && tupleU == NULL)){\n";
                            *out << ind++ << "if(undefTuple){\n";
                            *out << ind << "tuple" << i << " = tupleU = undefTuple;\n";
                            *out << ind << "tupleUNegated = true;\n";
                            *out << --ind << "}\n";
                            closingParenthesis++;

                        } else {
                            *out << ind << "const Tuple * tuple" << i << " = (w" << l->getPredicateName() << ".find({";
                            printLiteralTuple(l);
                            *out << "}));\n";
                            *out << ind++ << "if(!tuple" << i << " && !tupleU ){\n";
                            *out << ind << "tuple" << i << " = tupleU = (u" << l->getPredicateName() << ".find({";
                            printLiteralTuple(l);
                            *out << "}));\n";
                            *out << ind << "tupleUNegated = false;\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "if(tuple" << i << "){\n";
                            closingParenthesis++;
                        }
                    }

                } else {
                    std::string mapVariableName = l->getPredicateName() + "_";
                    for (unsigned t = 0; t < l->getAriety(); t++) {
                        if ((l->isVariableTermAt(t) && boundVariables.count(l->getTermAt(t))) || !l->isVariableTermAt(t)) {
                            mapVariableName += std::to_string(t) + "_";
                        }
                    }
                    if (mode == LAZY_MODE) {
                        *out << ind << "const std::vector<const Tuple* >& tuples = p" << mapVariableName << ".getValues({";
                        printLiteralTuple(l, boundVariables);
                        *out << "});\n";
                        *out << ind++ << "for( unsigned i=0; i< tuples.size();i++){\n";
                        *out << ind << "const Tuple * tuple" << i << " = tuples[i];\n";
                        closingParenthesis++;
                    } else {
                        //mode == EAGER_MODE
                        *out << ind << "const std::vector<const Tuple* >* tuples;\n";
                        *out << ind << "tuples = &p" << mapVariableName << ".getValues({";
                        printLiteralTuple(l, boundVariables);
                        *out << "});\n";
                        *out << ind << "const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;\n";
                        bool appearsBefore = literalPredicateAppearsBeforeSameSign(body, joinOrder, i);
                        if (appearsBefore) {
                            *out << ind << "std::vector<const Tuple* > tupleUInVector;\n";
                        }
                        *out << ind++ << "if(tupleU == NULL){\n";
                        *out << ind << "tuplesU = &u" << mapVariableName << ".getValues({";
                        printLiteralTuple(l, boundVariables);
                        *out << "});\n";
                        *out << --ind << "}\n";
                        //repeating literal case

                        if (appearsBefore) {
                            *out << ind++ << "else {\n";
                            //handle constants and equal cards?
                            *out << ind++ << "if(tupleU && !tupleUNegated && tupleU->getPredicateName() == &_"<<l->getPredicateName()<<") {\n";
                            //check that bound variables have proper value
                            vector<unsigned> boundIndexes;
                            for(unsigned v = 0; v < l->getAriety(); v++) {
                                if(boundVariables.count(l->getTermAt(v))) {
                                    boundIndexes.push_back(v);
                                }
                            }
                            if(boundIndexes.size()) {
                                *out << ind++ << "if(";
                                 for(unsigned bi = 0; bi < boundIndexes.size(); bi++) {
                                     if(bi > 0) {
                                         *out << " && ";
                                     }
                                     *out << "tupleU->at(" << boundIndexes[bi] << ") == " << l->getTermAt(boundIndexes[bi]);
                                 }
                                 *out << "){\n";
                            }
                            
                            *out << ind << "tupleUInVector.push_back(tupleU);\n";
                            if(boundIndexes.size()) {
                                 *out << --ind << "}\n";
                            }
                            *out << --ind << "}\n";
                            *out << --ind << "}\n";
                        }

                        if (!appearsBefore) {
                            *out << ind++ << "for( unsigned i=0; i< tuples->size() + tuplesU->size();i++){\n";
                            *out << ind << "const Tuple * tuple" << i << " = NULL;\n";
                            *out << ind++ << "if(i<tuples->size()){\n";
                            *out << ind << "tuple" << i << " = tuples->at(i);\n";
                            *out << ind++ << "if(tuplesU != &EMPTY_TUPLES) {\n";
                            *out << ind << "tupleU = NULL;\n";
                            *out << --ind << "}\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "else {\n";
                            *out << ind << "tuple" << i << " = tuplesU->at(i-tuples->size());\n";
                            *out << ind << "tupleU = tuple" << i << ";\n";
                            *out << ind << "tupleUNegated = false;\n";
                            *out << --ind << "}\n";
                        } else {
                            *out << ind++ << "for( unsigned i=0; i< tuples->size() + tuplesU->size() + tupleUInVector.size();i++){\n";
                            *out << ind << "const Tuple * tuple" << i << " = NULL;\n";
                            *out << ind++ << "if(i<tuples->size()){\n";
                            *out << ind << "tuple" << i << " = tuples->at(i);\n";
                            *out << ind++ << "if(tuplesU != &EMPTY_TUPLES) {\n";
                            *out << ind << "tupleU = NULL;\n";
                            *out << --ind << "}\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "else if(i<tuples->size()+tuplesU->size()) {\n";
                            *out << ind << "tuple" << i << " = tuplesU->at(i-tuples->size());\n";
                            *out << ind << "tupleU = tuple" << i << ";\n";
                            *out << ind << "tupleUNegated = false;\n";
                            *out << --ind << "}\n";
                            *out << ind++ << "else {\n";
                            *out << ind << "tuple" << i << " = tupleU;\n";
                            *out << ind << "tupleUNegated = false;\n";
                            *out << --ind << "}\n";
                        }
                        closingParenthesis++;
                    }
                }
            } else {
                aspc::ArithmeticRelation* f = (aspc::ArithmeticRelation*) body[joinOrder[i]];
                if (f-> isBoundedRelation(boundVariables)) {
                    *out << ind++ << "if(" << f->getStringRep() << ") {\n";
                    closingParenthesis++;
                } else {
                    //bounded value assignment
                    *out << ind << "int " << f->getAssignmentStringRep(boundVariables) << ";\n";
                    boundVariables.insert(f->getAssignedVariable(boundVariables));
                    
                }

            }

        }
        if (f->isPositiveLiteral() || (i == 0 && f->isLiteral())) {
            aspc::Literal* l = (aspc::Literal*)f;
            std::unordered_set<std::string> declaredVariables;
            for (unsigned t = 0; t < l->getAriety(); t++) {
                if (l->isVariableTermAt(t) && !boundVariables.count(l->getTermAt(t)) && !declaredVariables.count(l->getTermAt(t))) {
                    *out << ind << "int " << l->getTermAt(t) << " = (*tuple" << i << ")[" << t << "];\n";
                    declaredVariables.insert(l->getTermAt(t));
                }
            }
        }
        if (!r.getFormulas()[joinOrder[i]]->isBoundedLiteral(boundVariables) && !r.getFormulas()[joinOrder[i]]->isBoundedRelation(boundVariables)) {
            r.getFormulas()[joinOrder[i]]->addVariablesToSet(boundVariables);
        }

        if (handleEqualCardsAndConstants(r, i, joinOrder))
            closingParenthesis++;

        //rule fires
        if (i == body.size() - 1) {

            if (!r.isConstraint()) {

                //a rule is firing
                string tupleType = "TupleWithoutReasons";
                if (p.hasConstraint()) {
                    tupleType = "TupleWithReasons";
                }
                *out << ind << "const auto & insertResult = w" << r.getHead().front().getPredicateName() << ".insert(" << tupleType << "({";

                for (unsigned th = 0; th < r.getHead().front().getTermsSize(); th++) {
                    if (!r.getHead().front().isVariableTermAt(th)) {
                        if (isInteger(r.getHead().front().getTermAt(th))) {
                            *out << r.getHead().front().getTermAt(th);
                        } else {
                            *out << "ConstantsManager::getInstance().mapConstant(\"" << escapeDoubleQuotes(r.getHead().front().getTermAt(th)) << "\")";
                        }
                    } else {
                        *out << r.getHead().front().getTermAt(th);
                    }
                    if (th < r.getHead().front().getTermsSize() - 1) {
                        *out << ",";
                    }
                }


                *out << "}, &_" << r.getHead().front().getPredicateName() << "));\n";
                *out << ind++ << "if(insertResult.second){\n";

                if (p.hasConstraint()) {
                    for (unsigned i = 0; i < body.size(); i++) {
                        if (body[joinOrder[i]]->isPositiveLiteral()) {
                            *out << ind << "insertResult.first->addPositiveReason(tuple" << i << ");\n";
                        } else if (body[joinOrder[i]]->isLiteral()) {
                            aspc::Literal* l = (aspc::Literal*) body[joinOrder[i]];
                            *out << ind << "insertResult.first->addNegativeReason(Tuple({";
                            printLiteralTuple(l);
                            *out << "}, &_" << l->getPredicateName() << ", true));\n";
                        }
                    }
                }

                for (const std::string& auxMapName : predicateToAuxiliaryMaps[r.getHead().front().getPredicateName()]) {
                    *out << ind << "p" << auxMapName << ".insert2(*w" << r.getHead().front().getPredicateName() << ".getTuples().back());\n";
                }

                *out << --ind << "}\n";
            } else {
                //*out << ind << "std::cout<<\"constraint failed\"<<std::endl;\n";
                //we are handling a constraint
                if (mode == LAZY_MODE) {
                    *out << ind << "failedConstraints.push_back(std::vector<aspc::Literal>());\n";

                    *out << ind << "std::vector<const Tuple *> reasons;\n";

                    for (unsigned i = 0; i < body.size(); i++) {
                        if (body[joinOrder[i]]->isLiteral()) {
                            aspc::Literal* l = (aspc::Literal*) body[joinOrder[i]];
                            if (idbs.count(l->getPredicateName()) || headPredicates.count(l->getPredicateName())) {
                                *out << ind << "std::unordered_set<std::string> open_set" << i << ";\n";
                                if (l->isPositiveLiteral()) {
                                    *out << ind << "explainPositiveLiteral(tuple" << i << ", open_set" << i << ", reasons);\n";
                                } else {
                                    *out << ind << "Tuple tuple" << i << " = Tuple({";
                                    printLiteralTuple(l);
                                    *out << "}, &_" << l->getPredicateName() << ", true);\n";
                                    *out << ind << "explainNegativeLiteral(&tuple" << i << ", open_set" << i << ", reasons);\n";
                                    //*out << ind << "failedConstraints.back().push_back(tupleToLiteral(Tuple({";
                                    //writeNegativeTuple(r, joinOrder, start, i);
                                    //*out << "}, 0, &ConstantsManager::getInstance().getPredicateName(\"" << l->getPredicateName() << "\"), true)));\n";
                                }
                            }
                        }
                    }
                    *out << ind++ << "for(const Tuple * reason: reasons) {\n";
                    *out << ind << "failedConstraints.back().push_back(tupleToLiteral(*reason));\n";
                    *out << --ind << "}\n";
                } else {
                    //mode == EAGER_MODE
                    //*out << ind << "aspc::Literal propagatedLiteral = (tupleToLiteral(*tupleU));\n";
                    //*out << ind << "propagatedLiteral.setNegated(tupleUNegated);\n";
                    //TODO maybe negate literal
                    //*out << ind << "propagatedLiteral.print();\n";
                    *out << ind << "int sign = tupleUNegated ? -1 : 1;\n";

                    //needed for propagations at level 0.. constraint may fail, then return incoherence (value 1)
                    *out << ind++ << "if(!tupleU) {\n";
                    *out << ind << "std::cout<<\"conflict detected in propagator\"<<std::endl;\n";
                    *out << ind << "propagatedLiteralsAndReasons.insert({-1, std::vector<int>()});\n";
                    *out << --ind << "}\n";

                    *out << ind++ << "else {\n";
                    *out << ind << "const auto & it = tupleToVar.find(*tupleU);\n";
#ifdef EAGER_DEBUG
                    *out << ind << "std::cout<<\"propagating \";\n";
                    *out << ind << "std::cout<<(-1 * sign* ((int) (it->second)))<<\" \";\n";
                    *out << ind++ << "if(sign > 0) {\n";
                    *out << ind << "std::cout<<\"not \";\n";
                    *out << --ind << "}\n";
                    *out << ind << "tupleU->print();\n";
                    *out << ind << "std::cout<<\"\\n\";\n";
#endif
                    *out << ind++ << "if(it != tupleToVar.end()) {\n";
                    *out << ind << "auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;\n";
                    //*out << ind << "propagatedLiteralsAndReasons[tupleToVar[*tupleU]] = std::vector<int>();\n";
                    //*out << ind << "std::cout<<\"constraint failed\"<<std::endl;\n";

                    //*out << ind << "reasonsForPropagatedLiterals.push_back(std::vector<aspc::Literal>());\n";

                    *out << ind << "std::vector<const Tuple *> reasons;\n";

                    for (unsigned i = 0; i < body.size(); i++) {
                        if (body[joinOrder[i]]->isLiteral()) {
                            aspc::Literal* l = (aspc::Literal*) body[joinOrder[i]];
                            //if (idbs.count(l->getPredicateName()) || headPredicates.count(l->getPredicateName())) {
                            *out << ind++ << "if(tuple" << i << " != tupleU){\n";
                            *out << ind << "std::unordered_set<std::string> open_set" << i << ";\n";
#ifdef EAGER_DEBUG
                            *out << ind << "tuple" << i << "->print();\n";
                            *out << ind << "std::cout<<\"\\n\";\n";
#endif
                            if (l->isPositiveLiteral()) {
                                //*out << ind << "reasons.push_back(tuple" << i << ");\n";
                                *out << ind << "explainPositiveLiteral(tuple" << i << ", open_set" << i << ", reasons);\n";

                            } else {
                                //                                *out << ind << "Tuple tuple" << i << " = Tuple({";
                                //                                printLiteralTuple(l);
                                //                                *out << "}, &_" << l->getPredicateName() << ", true);\n";
                                *out << ind << "reasons.push_back(tuple" << i << ");\n";
                                //*out << ind << "explainNegativeLiteral(tuple" << i << ", open_set" << i << ", reasons);\n";
                            }
                            *out << --ind << "}\n";
                            //}
                        }
                    }
                    *out << ind << "reas.reserve(reasons.size());\n";
                    *out << ind++ << "for(const Tuple * reason: reasons) {\n";
                    *out << ind << "const auto & it = tupleToVar.find(*reason);\n";
                    *out << ind++ << "if(it != tupleToVar.end()) {\n";
                    *out << ind << "reas.push_back(it->second * (reason->isNegated()? -1:1));\n";
                    *out << --ind << "}\n";
                    *out << --ind << "}\n";
                    //*out << ind++ << "if(decisionLevel == -1) {\n";
                    //*out << ind << "executeProgramOnFacts({-1,it->second*sign*-1});\n";
                    //*out << --ind << "}\n";
                    *out << --ind << "}\n";
                    *out << --ind << "}\n";


                }
                //TESTING FEATURE, LIMIT NUMBER OF FAILED CONSTRAINTS
                //                *out << ind++ << "if(failedConstraints.size() >= 1000) {\n";
                //                *out << ind << "return;\n";
                //                *out << --ind << "}\n";
            }

        }


    }
    for (unsigned i = 0; i < closingParenthesis; i++) {
        *out << --ind << "}//close par\n";
    }
    
        /*unsigned i = 1;
        if(start == r.getBodySize()) {
            i=0;
        }
        for (unsigned i = 1; i < body.size(); i++) {
            if (body[i]->isLiteral()) {
         *out << --ind << "}//close lit\n";
            }
        }*/
    //}
    
    
}

void CompilationManager::printLiteralTuple(const aspc::Literal* l, const std::vector<bool> & coveredMask) {

    bool first = true;
    for (unsigned term = 0; term < l->getAriety(); term++) {
        if (!l->isVariableTermAt(term) || coveredMask[term]) {
            if (!first) {
                *out << ", ";
            }
            if (!l->isVariableTermAt(term) && !isInteger(l->getTermAt(term))) {
                *out << "ConstantsManager::getInstance().mapConstant(\"" << escapeDoubleQuotes(l->getTermAt(term)) << "\")";
            } else {
                *out << l->getTermAt(term);
            }
            first = false;
        }
    }

}

void CompilationManager::printLiteralTuple(const aspc::Literal* l) {
    for (unsigned t = 0; t < l->getAriety(); t++) {
        if (t > 0) {
            *out << ", ";
        }
        if (!l->isVariableTermAt(t) && !isInteger(l->getTermAt(t))) {
            *out << "ConstantsManager::getInstance().mapConstant(\"" << escapeDoubleQuotes(l->getTermAt(t)) << "\")";
        } else {
            *out << l->getTermAt(t);
        }
    }


}

void CompilationManager::printLiteralTuple(const aspc::Literal* l, const std::unordered_set<std::string> & boundVariables) {
    bool first = true;
    for (unsigned t = 0; t < l->getAriety(); t++) {
        if (!l->isVariableTermAt(t) || boundVariables.count(l->getTermAt(t))) {
            if (!first) {
                *out << ", ";
            }
            if (!l->isVariableTermAt(t) && !isInteger(l->getTermAt(t))) {
                *out << "ConstantsManager::getInstance().mapConstant(\"" << escapeDoubleQuotes(l->getTermAt(t)) << "\")";
            } else {
                *out << l->getTermAt(t);
            }
            first = false;
        }
    }


}

void initRuleBoundVariablesAux(std::unordered_set<std::string> & output, const aspc::Literal & lit, const std::unordered_set<std::string> & litBoundVariables, const aspc::Atom & head) {
    for (unsigned i = 0; i < lit.getAriety(); i++) {
        if (litBoundVariables.count(lit.getTermAt(i))) {
            output.insert(head.getTermAt(i));
        }
    }
}

void CompilationManager::declareDataStructuresForReasonsOfNegative(const aspc::Program & program, const aspc::Literal & lit, std::unordered_set<std::string> & boundVariables, std::unordered_set<std::string> & openSet) {


    std::string canonicalRep = lit.getCanonicalRepresentation(boundVariables);
    if (openSet.count(canonicalRep)) {
        return;
    }

    if (lit.isNegated() && modelGeneratorPredicates.count(lit.getPredicateName())) {
        modelGeneratorPredicatesInNegativeReasons.insert(lit.getPredicateName());
        predicateArieties.insert(std::make_pair(lit.getPredicateName(), lit.getAriety()));
    }

    openSet.insert(canonicalRep);

    for (const aspc::Rule & r : program.getRules()) {
        if (!r.isConstraint() && lit.unifies(r.getHead()[0])) {
            std::unordered_set<std::string> ruleBoundVariables;
            const aspc::Atom & head = r.getHead()[0];
            initRuleBoundVariablesAux(ruleBoundVariables, lit, boundVariables, head);
            std::vector<const aspc::Formula*> orderedFormulas = r.getOrderedBodyForReasons(ruleBoundVariables);
            for (unsigned i = 0; i < r.getBodySize(); i++) {
                const aspc::Formula * f = orderedFormulas[i];
                if (f -> isLiteral()) {
                    const aspc::Literal * bodyLit = (const aspc::Literal *) f;
                    if (lit.isNegated()) {
                        if (!bodyLit->isNegated()) {
                            std::vector<unsigned> coveredVariableIndexes;
                            bodyLit->getAtom().getCoveredVariables(ruleBoundVariables, coveredVariableIndexes);
                            if (coveredVariableIndexes.size() < bodyLit->getAriety()) {
                                std::string mapVariableName = bodyLit->getPredicateName() + "_";
                                for (unsigned index : coveredVariableIndexes) {
                                    mapVariableName += std::to_string(index) + "_";
                                }
                                if (!declaredMaps.count(mapVariableName)) {
                                    *out << ind << "AuxMap p" << mapVariableName << "({";
                                    for (unsigned k = 0; k < coveredVariableIndexes.size(); k++) {
                                        if (k > 0) {
                                            *out << ",";
                                        }
                                        *out << coveredVariableIndexes[k];
                                    }
                                    *out << "});\n";
                                    predicateToAuxiliaryMaps[bodyLit->getPredicateName()].insert(mapVariableName);
                                    //                                    *out << ind << "predicateToAuxiliaryMaps[" << bodyLit->getPredicateName() << "].push_back(&p" << mapVariableName << ");\n";
                                    //*out << ind << "std::string " << mapVariableName << ";\n";
                                    declaredMaps.insert(mapVariableName);
                                }
                                mapVariableName = "false_p" + mapVariableName;
                                if (!declaredMaps.count(mapVariableName) && modelGeneratorPredicates.count(bodyLit -> getPredicateName())) {
                                    *out << ind << "AuxMap " << mapVariableName << "({";
                                    for (unsigned k = 0; k < coveredVariableIndexes.size(); k++) {
                                        if (k > 0) {
                                            *out << ",";
                                        }
                                        *out << coveredVariableIndexes[k];
                                    }
                                    *out << "});\n";
                                    predicateToFalseAuxiliaryMaps[bodyLit->getPredicateName()].insert(mapVariableName);
                                    declaredMaps.insert(mapVariableName);
                                }

                            }
                            aspc::Literal temp = *bodyLit;
                            temp.setNegated(true);
                            declareDataStructuresForReasonsOfNegative(program, temp, ruleBoundVariables, openSet);
                        } else {
                            aspc::Literal temp = *bodyLit;
                            temp.setNegated(false);
                            declareDataStructuresForReasonsOfNegative(program, temp, ruleBoundVariables, openSet);
                        }
                    } else if (!modelGeneratorPredicates.count(bodyLit -> getPredicateName()) || bodyLit->isNegated()) {
                        std::unordered_set<std::string> bodyLitVariables = bodyLit->getVariables();
                        declareDataStructuresForReasonsOfNegative(program, *bodyLit, bodyLitVariables, openSet);
                    }
                    for (unsigned t = 0; t < bodyLit->getAriety(); t++) {
                        if (bodyLit -> isVariableTermAt(t)) {
                            ruleBoundVariables.insert(bodyLit->getTermAt(t));
                        }
                    }
                }
            }
        }
    }
}

void CompilationManager::declareDataStructuresForReasonsOfNegative(const aspc::Program & program) {

    *out << ind << "//printing aux maps needed for reasons of negative literals;\n";
    std::unordered_set<std::string> open_set;

    for (const aspc::Rule & r : program.getRules()) {
        if (r.isConstraint()) {
            for (const aspc::Formula * f : r.getFormulas()) {
                if (f->isLiteral()) {
                    const aspc::Literal & lit = (const aspc::Literal &) * f;
                    std::unordered_set<std::string> litVariables = lit.getVariables();
                    declareDataStructuresForReasonsOfNegative(program, lit, litVariables, open_set);
                }
            }
        }
    }
}

bool CompilationManager::handleEqualCardsAndConstants(const aspc::Rule& r, unsigned i, const std::vector<unsigned>& joinOrder) {

    if (!r.getFormulas()[joinOrder[i]]->isLiteral()) {
        return false;
    }

    bool hasCondition = false;
    const aspc::Literal * l = (aspc::Literal *) r.getFormulas()[joinOrder[i]];
    if (l->isNegated() && i != 0) {
        return false;
    }
    for (unsigned t1 = 0; t1 < l->getAriety(); t1++) {
        if (!l->isVariableTermAt(t1)) {
            if (!hasCondition) {
                *out << ind++ << "if( ";
                hasCondition = true;
            } else
                *out << " && ";

            *out << "(*tuple" << i << ")[" << t1 << "] == ";
            if (isInteger(l->getTermAt(t1))) {
                *out << l->getTermAt(t1);
            } else {
                *out << "ConstantsManager::getInstance().mapConstant(\"" << escapeDoubleQuotes(l->getTermAt(t1)) << "\")";
            }

        }
        for (unsigned t2 = t1 + 1; t2 < l->getAriety(); t2++)
            if (l->isVariableTermAt(t1) && l->getTermAt(t1) == l->getTermAt(t2)) {
                if (!hasCondition) {
                    *out << ind++ << "if( ";
                    hasCondition = true;
                } else
                    *out << " && ";
                *out << "(*tuple" << i << ")[" << t1 << "] == (*tuple" << i << ")[" << t2 << "]";
            }
    }
    if (hasCondition)
        *out << "){\n";
    return hasCondition;
}

const std::set<std::string>& CompilationManager::getBodyPredicates() {
    return bodyPredicates;
}


