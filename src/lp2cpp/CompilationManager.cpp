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

#include "CompilationManager.h"
#include "utils/Indentation.h"
#include "utils/SharedFunctions.h"
#include <ostream>
#include <fstream>
#include <cassert>
#include <map>
#include <set>
#include <list>
#include <bits/unordered_map.h>
#include "DLV2libs/input/InputDirector.h"
#include "parsing/AspCore2ProgramBuilder.h"
#include "language/ArithmeticExpression.h"
#include "language/ArithmeticRelation.h"
#include "datastructures/BoundAnnotatedLiteral.h"
#include <algorithm>
using namespace std;


const string tab = "    ";

CompilationManager::CompilationManager() : out(&std::cout), ind(0) {
}

void CompilationManager::setOutStream(std::ostream* outputTarget) {
    this->out = outputTarget;
}

void CompilationManager::lp2cpp(const string &filename) {
    DLV2::InputDirector director;
    AspCore2ProgramBuilder* builder = new AspCore2ProgramBuilder();
    director.configureBuilder(builder);
    vector<const char*> fileNames;
    fileNames.push_back(filename.c_str());
    director.parse(fileNames);
    //    generateCompilableProgram(builder->getProgram(), builder);
    //builder->getProgram().print();
    generateStratifiedCompilableProgram(builder->getProgram(), builder);
    delete builder;

}

void initRuleBoundVariables(unordered_set<string> & ruleBoundVariables, const BoundAnnotatedLiteral & lit, const aspc::Atom & head) {
    for (unsigned i = 0; i < lit.getBoundIndexes().size(); i++) {
        if (lit.getBoundIndexes().at(i) && head.isVariableTermAt(i)) {
            ruleBoundVariables.insert(head.getTermAt(i));
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

void CompilationManager::writeNegativeReasonsFunctions(const aspc::Program & program, const BoundAnnotatedLiteral & lit, bool negationMet,
        list<BoundAnnotatedLiteral> & toProcessLiterals, list<BoundAnnotatedLiteral> & processedLiterals, unordered_map <string, string> & functionsMap) {


    //TODO use real MG predicates 
    if (lit.isNegated()) {
        *out << ind++ << "void explain_" << lit.getStringRep() << "(const Tuple * lit, unordered_set<string> & open_set, vector<const Tuple *> & output){\n";
        if (lit.isGround()) {
            functionsMap[lit.getPredicateName()] = "explain_"+lit.getStringRep();
        }
        if (modelGeneratorPredicates.count(lit.getPredicateName())) {
            *out << ind << "output.push_back(lit);\n";
            *out << --ind << "}\n";
            return;
        }
    }


    //TODO lit.predicate is in MG predicates
    for (const aspc::Rule & r : program.getRules()) {
        //TODO runtime unification
        if (!r.isConstraint() && lit.getPredicateName() == r.getHead()[0].getPredicateName()) {
            unsigned forCounter = 0;
            unordered_set<string> ruleBoundVariables;
            const aspc::Atom & head = r.getHead()[0];
            initRuleBoundVariables(ruleBoundVariables, lit, head);
            for (unsigned i = 0; i < r.getBodySize(); i++) {
                const aspc::Formula * f = r.getFormulas()[i];
                if (f -> isLiteral()) {
                    const aspc::Literal * bodyLit = (const aspc::Literal *) f;
                    if (negationMet) {
                        if (!bodyLit->isNegated()) {

                            vector<bool> coveredMask;
                            bodyLit->getAtom().getBoundTermsMask(ruleBoundVariables, coveredMask);
                            BoundAnnotatedLiteral bodyBoundLit = BoundAnnotatedLiteral(bodyLit->getPredicateName(), coveredMask, true);
                            possiblyAddToProcessLiteral(bodyBoundLit, toProcessLiterals, processedLiterals);
                            *out << ind << "explain_" << bodyBoundLit.getStringRep() << "(lit, open_set, output);\n";
                            if (i < r.getBodySize() - 1) {
                                string mapVariableName = bodyLit->getPredicateName() + "_";
                                for (unsigned index = 0; index < coveredMask.size(); index++) {
                                    if (bodyBoundLit.getBoundIndexes()[index]) {
                                        mapVariableName += to_string(index) + "_";
                                    }
                                }
                                *out << ind++ << "for(v: " << mapVariableName << "){\n";
                                forCounter++;
                            }

                            //declareDataStructuresForReasonsOfNegative(program, *bodyLit, true, ruleBoundVariables, openSet);
                        } else {
                            BoundAnnotatedLiteral bodyBoundLit = BoundAnnotatedLiteral(bodyLit->getPredicateName(), vector<bool>(bodyLit->getAriety(), true), false);
                            possiblyAddToProcessLiteral(bodyBoundLit, toProcessLiterals, processedLiterals);
                            *out << ind << "explain_positive(lit, open_set, output);\n";
                            //*out << ind << "explain_" << bodyBoundLit.getStringRep() << "(lit, result, open);\n";
                        }
                    } else {
                        //if (!modelGeneratorPredicates.count(bodyLit -> getPredicateName()))
                        if (bodyLit->isNegated()) {
                            BoundAnnotatedLiteral bodyBoundLit = BoundAnnotatedLiteral(bodyLit->getPredicateName(), vector<bool>(bodyLit->getAriety(), true), true);
                            possiblyAddToProcessLiteral(bodyBoundLit, toProcessLiterals, processedLiterals);
                            //                            unordered_set<string> bodyLitVariables = bodyLit->getVariables();
                            //                            declareDataStructuresForReasonsOfNegative(program, *bodyLit, bodyLit -> isNegated(), bodyLitVariables, openSet);
                        }
                    }
                    for (unsigned t = 0; t < bodyLit->getAriety(); t++) {
                        if (bodyLit -> isVariableTermAt(t)) {
                            ruleBoundVariables.insert(bodyLit->getTermAt(t));
                        }
                    }
                }
            }
            for (unsigned i = 0; i < forCounter; i++) {
                *out << --ind << "}\n";
            }
        }
    }
    if (lit.isNegated()) {
        *out << --ind << "}\n";
    }
}

BoundAnnotatedLiteral literalToBoundAnnotatedLiteral(const aspc::Literal & lit) {

    return BoundAnnotatedLiteral(lit.getPredicateName(), vector<bool>(lit.getAriety(), true), lit.isNegated());

}

BoundAnnotatedLiteral literalToBoundAnnotatedLiteral(const aspc::Literal & lit, unordered_set<string> & boundVariables) {

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

void CompilationManager::writeNegativeReasonsFunctions(aspc::Program & program) {

    *out << ind << "//printing functions for reasons of negative literals;\n";

    list<BoundAnnotatedLiteral> toProcessLiterals;
    list<BoundAnnotatedLiteral> processedLiterals;
    unordered_map <string, string> functionsMap;

    for (const aspc::Rule & r : program.getRules()) {
        if (r.isConstraint()) {
            for (const aspc::Formula * f : r.getFormulas()) {
                if (f->isLiteral()) {
                    const aspc::Literal & lit = (const aspc::Literal &) * f;
                    toProcessLiterals.push_back(literalToBoundAnnotatedLiteral(lit));
                }
            }
        }
    }
    while (!toProcessLiterals.empty()) {
        BoundAnnotatedLiteral next = toProcessLiterals.back();
        toProcessLiterals.pop_back();
        processedLiterals.push_back(next);
        writeNegativeReasonsFunctions(program, next, next.isNegated(), toProcessLiterals, processedLiterals, functionsMap);
    }

    *out << ind++ << "void createFunctionsMap() {\n";
    for (const auto & entry : functionsMap) {
        *out << ind << "explainNegativeFunctionsMap[&" << entry.first << "] = " << entry.second << ";\n";
    }
    *out << --ind << "}\n";
}

void CompilationManager::generateStratifiedCompilableProgram(aspc::Program & program, AspCore2ProgramBuilder* builder) {

    bool programHasConstraint = program.hasConstraint();

    *out << ind << "#include \"Executor.h\"\n\n";
    *out << ind << "#include \"utils/ConstantsManager.h\"\n\n";
    *out << ind << "#include \"DLV2libs/input/InputDirector.h\"\n\n";
    *out << ind << "#include \"parsing/AspCore2InstanceBuilder.h\"\n\n";
    *out << ind++ << "extern \"C\" Executor* create_object() {\n";

    *out << ind << "return new Executor;\n";
    *out << --ind << "}\n";

    *out << ind++ << "extern \"C\" void destroy_object( Executor* object ) {\n";
    *out << ind << "delete object;\n";
    *out << --ind << "}\n";

    *out << ind << "Executor::~Executor() {}\n\n";

    *out << ind << "Executor::Executor() {}\n\n";

    const set< pair<string, unsigned> >& predicates = program.getPredicates();
    for (const pair<string, unsigned>& predicate : predicates) {
        //*out << ind << "const string & "<< predicate.first << " = ConstantsManager::getInstance().getPredicateName("<< predicate.first <<");\n";
        *out << ind << "const string " << predicate.first << " = \"" << predicate.first << "\";\n";
    }

    *out << ind << "\n";
    *out << ind << "typedef void (*ExplainNegative)(const Tuple * lit, unordered_set<string> & open_set, vector<const Tuple *> & output);\n\n";

    *out << ind << "map<const string*, ExplainNegative> explainNegativeFunctionsMap;\n\n";

    *out << ind << "//only ground lit function calls are not known a priori\n";

    *out << ind++ << "void explainNegativeLiteral(const Tuple * lit, unordered_set<string> & open_set, vector<const Tuple *> & output) {\n";
    *out << ind << "explainNegativeFunctionsMap[lit->getPredicateName()](lit, open_set, output);\n";
    *out << --ind << "}\n\n";


    //perform join functions    

    //TODO continue from here
    if (program.hasConstraint()) {
        writeNegativeReasonsFunctions(program);
    }

    //typedefs

    *out << ind << "typedef vector<const Tuple* > Tuples;\n";

    //print tuples 
    *out << ind++ << "void printTuples(const string & predicateName, const Tuples & tuples) {\n";
    *out << ind++ << "for (const vector<unsigned> * tuple : tuples) {\n";
    *out << ind << "cout <<predicateName<< \"(\";\n";
    *out << ind++ << "for (unsigned i = 0; i < tuple->size(); i++) {\n";
    *out << ind++ << "if (i > 0) {\n";
    *out << ind << "cout << \",\";\n";
    *out << --ind << "}\n";
    *out << ind << "cout << ConstantsManager::getInstance().unmapConstant((*tuple)[i]);\n";
    *out << --ind << "}\n";
    *out << ind << "cout << \").\\n\";\n";
    *out << --ind << "}\n";
    *out << --ind << "}\n";
    //handle arieties
    *out << ind++ << "void Executor::executeFromFile(const char* filename) {\n";
    *out << ind << "DLV2::InputDirector director;\n";
    *out << ind << "AspCore2InstanceBuilder* builder = new AspCore2InstanceBuilder();\n";
    *out << ind << "director.configureBuilder(builder);\n";
    *out << ind << "vector<const char*> fileNames;\n";
    *out << ind << "fileNames.push_back(filename);\n";
    *out << ind << "director.parse(fileNames);\n";
    *out << ind << "executeProgramOnFacts(builder->getProblemInstance());\n";
    *out << ind << "delete builder;\n";
    *out << --ind << "}\n\n";
    *out << ind << "using PredicateWSet = std::unordered_set<Tuple, TupleHash>;\n\n";

    *out << ind++ << "void explainPositiveLiteral(const Tuple * tuple, unordered_set<string> & open_set, vector<const Tuple*> & outputReasons) {\n";
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
    *out << ind++ << "for (unsigned v : tuple) {\n";
    *out << ind << "literal.addTerm(ConstantsManager::getInstance().unmapConstant(v));\n";
    *out << --ind << "}\n";
    *out << ind << "return literal;\n";
    *out << --ind << "}\n";


    *out << ind++ << "void Executor::executeProgramOnFacts(const vector<aspc::Literal*> & facts) {\n";
    //data structure init

    if(program.hasConstraint()) {
        *out << ind << "createFunctionsMap();\n";
    }

    *out << ind << "failedConstraints.clear();\n";

    *out << ind << "map<string, PredicateWSet*> predicateWSetMap;\n";
    *out << ind << "map<string, PredicateWSet*> predicateFalseWSetMap;\n";
    *out << ind << "map<string, Tuples* > predicateTuplesMap;\n";
    *out << ind << "map<string, Tuples* > predicateFalseTuplesMap;\n";

    for (const pair<string, unsigned>& predicate : predicates) {
        *out << ind << "Tuples tuples_" << predicate.first << ";\n";
        *out << ind << "Tuples negativeTuples_" << predicate.first << ";\n";
        *out << ind << "PredicateWSet w" << predicate.first << ";\n";
        *out << ind << "predicateWSetMap[" << predicate.first << "]=&w" << predicate.first << ";\n";
        *out << ind << "predicateFalseTuplesMap[" << predicate.first << "]=&negativeTuples_" << predicate.first << ";\n";
        *out << ind << "predicateTuplesMap[" << predicate.first << "]=&tuples_" << predicate.first << ";\n";
    }


    bodyPredicates = program.getBodyPredicates();

    GraphWithTarjanAlgorithm& graphWithTarjanAlgorithm = builder->getGraphWithTarjanAlgorithm();
    vector< vector <int> > sccs = graphWithTarjanAlgorithm.SCC();

    //print working set
    //     for (const pair<string, unsigned>& predicate : predicates) {
    //        *out << ind << "w" << predicate.first <<".printTuples(\""<<predicate.first<<"\");\n";
    //    }
    joinKeys.resize(program.getRulesSize());
    auxiliaryMapsNameByRuleAndStartIndex.resize(program.getRulesSize());
    for (unsigned i = 0; i < program.getRulesSize(); i++) {
        auxiliaryMapsNameByRuleAndStartIndex[i].resize(program.getRule(i).getBodySize());
    }

    const unordered_map<int, Vertex>& vertexByID = builder->getVertexByIDMap();

    //compute levels of each predicate
    vector< unsigned > predicateLevels(vertexByID.size());
    for (int i = sccs.size() - 1; i >= 0; i--) {
        const vector<int>& scc = sccs[i];
        for (int c : scc) {
            predicateLevels[c] = sccs.size() - i - 1;
        }
    }

    *out << ind << "unordered_map <string, vector <AuxiliaryMap*> > predicateToAuxiliaryMaps;\n";
    *out << ind << "unordered_map <string, vector <AuxiliaryMap*> > predicateToFalseAuxiliaryMaps;\n";
    unsigned sccsSize = sccs.size();
    if (programHasConstraint) {
        sccsSize++;
    }
    vector< unordered_map<string, vector<unsigned>>> starterToExitRulesByComponent(sccsSize);
    vector < unordered_map<string, vector<pair<unsigned, unsigned> > >> starterToRecursiveRulesByComponent(sccsSize);
    vector<bool> exitRules(program.getRulesSize(), false);
    const unordered_map<string, int>& predicateIDs = builder->getPredicateIDsMap();


    for (aspc::Rule& r : program.getRules()) {
        //r is a constraint
        bool exitRule = true;
        unsigned lIndex = 0;
        unsigned headLevel = sccs.size();
        if (!r.isConstraint()) {
            vector<unsigned> starters;
            headLevel = predicateLevels[predicateIDs.at(r.getHead().at(0).getPredicateName())];
            for (const aspc::Formula* f : r.getFormulas()) {
                if (f->isLiteral()) {
                    const aspc::Literal * l = (const aspc::Literal*) f;
                    unsigned predicateID = predicateIDs.at(l->getPredicateName());
                    if (predicateLevels.at(predicateID) == headLevel) {
                        exitRule = false;
                        joinKeys[r.getRuleId()][lIndex].resize(r.getBodySize() - 1);
                        starterToRecursiveRulesByComponent[headLevel][l->getPredicateName()].push_back(pair<unsigned, unsigned>(r.getRuleId(), lIndex));
                        starters.push_back(lIndex);
                    }
                }
                lIndex++;
            }
            r.bodyReordering(starters);
        }
        if (exitRule || r.isConstraint()) {
            r.bodyReordering();
            unsigned starter = r.getStarters()[0];
            aspc::Literal* starterL = (aspc::Literal*) r.getFormulas()[starter];
            starterToExitRulesByComponent[headLevel][starterL->getPredicateName()].push_back(r.getRuleId());
            joinKeys[r.getRuleId()][starter].resize(r.getBodySize() - 1);
            exitRules[r.getRuleId()] = true;
        }
        for (unsigned starter : r.getStarters()) {
            declareDataStructures(r, starter);
        }



    }

    declareDataStructuresForReasonsOfNegative(program);





    //feed facts
    //*out << ind << "cout<<\"facts\\n\";\n";
    *out << ind++ << "for(unsigned i=0;i<facts.size();i++) {\n";
    *out << ind++ << "if(!facts[i]->isNegated()) {\n";
    //*out << ind << "cout<<i<<\"\\n\";\n";
    *out << ind << "map<string,PredicateWSet*>::iterator it = predicateWSetMap.find(facts[i]->getPredicateName());\n";
    *out << ind++ << "if(it==predicateWSetMap.end()) {\n";
    if (!programHasConstraint) {
        *out << ind << "facts[i]->print();\n";
        *out << ind << "cout<<\".\\n\";\n";
    }
    *out << --ind << "}\n";
    *out << ind++ << "else {\n";
    *out << ind << "const auto& insertResult=it->second->insert(facts[i]->getTuple(it->second->size()));\n";

    *out << ind++ << "if(insertResult.second){\n";
    //    *out << ind << "it->second->insert(tuple);\n";
    *out << ind << "Tuples & tuples = *predicateTuplesMap[facts[i]->getPredicateName()];\n";
    *out << ind << "tuples.push_back(&(*(insertResult.first)));\n";
    *out << ind++ << "for(AuxiliaryMap* auxMap:predicateToAuxiliaryMaps[it->first]){\n";
    *out << ind << "auxMap -> insert2(*tuples.back());\n";
    *out << --ind << "}\n";
    *out << --ind << "}\n";
    *out << --ind << "}\n";
    *out << --ind << "}\n";
    *out << ind++ << "else {\n";
    *out << ind << "map<string,PredicateWSet*>::iterator it = predicateFalseWSetMap.find(facts[i]->getPredicateName());\n";
    *out << ind++ << "if(it!=predicateFalseWSetMap.end()) {\n";
    *out << ind << "const auto& insertResult=it->second->insert(facts[i]->getTuple(it->second->size()));\n";
    *out << ind++ << "if(insertResult.second){\n";
    *out << ind << "Tuples & tuples = *predicateFalseTuplesMap[facts[i]->getPredicateName()];\n";
    *out << ind++ << "for(AuxiliaryMap* auxMap:predicateToFalseAuxiliaryMaps[it->first]){\n";
    *out << ind << "auxMap -> insert2(*tuples.back());\n";
    *out << --ind << "}\n";
    *out << --ind << "}\n";
    *out << --ind << "}\n";
    *out << --ind << "}\n";
    *out << --ind << "}\n";

    //declare iterators
    for (const pair<string, unsigned>& predicate : predicates) {
        *out << ind << "unsigned index_" << predicate.first << "=0;\n";
    }

    for (unsigned i = 0; i < sccsSize; i++) {
        const unordered_map<string, vector<unsigned>>&startersExitRules = starterToExitRulesByComponent[i];
        for (const auto& rulesByPredicate : startersExitRules) {
            *out << ind << "index_" << rulesByPredicate.first << "=0;\n";
            *out << ind++ << "while(index_" << rulesByPredicate.first << "!=tuples_" << rulesByPredicate.first << ".size()){\n";
            *out << ind << "const Tuple * tuple0 = tuples_" << rulesByPredicate.first << "[index_" << rulesByPredicate.first << "];\n";
            for (unsigned ruleId : rulesByPredicate.second) {
                const aspc::Rule& r = program.getRule(ruleId);
                *out << ind++ << "{\n";
                compileRule(r, r.getStarters()[0]);
                *out << --ind << "}\n";

            }
            *out << ind << "index_" << rulesByPredicate.first << "++;\n";
            *out << --ind << "}\n";
        }

        const unordered_map<string, vector <pair<unsigned, unsigned>>>& recursiveRulesByStarter = starterToRecursiveRulesByComponent[i];
        if (recursiveRulesByStarter.size()) {
            *out << ind++ << "while(";
            unsigned index = 0;
            for (unsigned vertexId : sccs[sccs.size() - i - 1]) {
                const Vertex& v = vertexByID.at(vertexId);
                if (index > 0)
                    *out << " || ";
                *out << "index_" << v.name << "!=tuples_" << v.name << ".size()";
                index++;
            }
            *out << "){\n";
        }
        for (const auto& rulesByStarter : recursiveRulesByStarter) {
            *out << ind++ << "while(index_" << rulesByStarter.first << "!=tuples_" << rulesByStarter.first << ".size()){\n";
            *out << ind << "const Tuple * tuple0 = tuples_" << rulesByStarter.first << "[index_" << rulesByStarter.first << "];\n";
            for (const auto& ruleAndStarterIndex : rulesByStarter.second) {
                const aspc::Rule& r = program.getRule(ruleAndStarterIndex.first);
                *out << ind++ << "{\n";
                compileRule(r, ruleAndStarterIndex.second);
                *out << --ind << "}\n";

            }
            *out << ind << "index_" << rulesByStarter.first << "++;\n";
            *out << --ind << "}\n";
        }
        if (recursiveRulesByStarter.size())
            *out << --ind << "}\n";

    }
    if (!programHasConstraint) {
        for (const pair<string, unsigned>& predicate : predicates) {
            *out << ind << "printTuples(\"" << predicate.first << "\",tuples_" << predicate.first << ");\n";

        }
    }
    *out << --ind << "}\n";


    //*out << ind << "w" << predicateIdPair.first << ".printTuples(\"" << predicateIdPair.first << "\");\n";
}

void CompilationManager::declareDataStructures(const aspc::Rule& r, unsigned start) {

    const vector<unsigned> & joinOrder = r.getOrderedBodyIndexesByStarter(start);


    for (unsigned i = 1; i < r.getFormulas().size(); i++) {
        if (r.getFormulas()[joinOrder[i]]->isLiteral()) {
            const aspc::Literal * li = (aspc::Literal *) r.getFormulas().at(joinOrder[i]);

            string mapVariableName = "";
            vector< unsigned > keyIndexes;
            mapVariableName += li->getPredicateName() + "_";
            for (unsigned tiIndex = 0; tiIndex < li->getTerms().size(); tiIndex++) {
                bool found = false;
                const string& ti = li->getTermAt(tiIndex);
                for (unsigned j = 0; j < i && !found; j++) {
                    if (r.getFormulas()[joinOrder[j]]->isLiteral()) {
                        const aspc::Literal * lj = (aspc::Literal *) r.getFormulas().at(joinOrder[j]);
                        unsigned termIndex = 0;
                        for (string tj : lj->getTerms()) {
                            if (ti == tj) {
                                joinKeys[r.getRuleId()][start][i - 1].push_back(pair<unsigned, unsigned>(j, termIndex));
                                mapVariableName += to_string(tiIndex) + "_";
                                keyIndexes.push_back(tiIndex);
                                found = true;
                                break;
                            }
                            termIndex++;
                        }
                    }
                }
            }
            auxiliaryMapsNameByRuleAndStartIndex[r.getRuleId()][start].push_back(mapVariableName);
            if (!declaredMaps.count(mapVariableName)) {
                *out << ind << "vector<unsigned> keyIndexes" << mapVariableName << ";\n";
                for (unsigned keyIndex : keyIndexes) {
                    *out << ind << "keyIndexes" << mapVariableName << ".push_back(" << keyIndex << ");\n";
                }

                *out << ind << "AuxiliaryMap p" << mapVariableName << "(&keyIndexes" << mapVariableName << ");\n";
                predicateToAuxiliaryMaps[li->getPredicateName()].insert(mapVariableName);
                *out << ind << "predicateToAuxiliaryMaps[" << li->getPredicateName() << "].push_back(&p" << mapVariableName << ");\n";
                declaredMaps.insert(mapVariableName);
            }

        } else {
            auxiliaryMapsNameByRuleAndStartIndex[r.getRuleId()][start].push_back("");
        }
    }
}

void CompilationManager::compileRule(const aspc::Rule & r, unsigned start) {
    //Iterate over starting workingset
    vector<unsigned> joinOrder = r.getOrderedBodyIndexesByStarter(start);
    const vector<const aspc::Formula*>& body = r.getFormulas();

    unsigned closingParenthesis = 0;
    set<string> boundVariables;
    //join loops, for each body formula
    for (unsigned i = 0; i < body.size(); i++) {

        if (i != 0) {
            if (body[joinOrder[i]]->isLiteral()) {

                aspc::Literal* l = (aspc::Literal*)body[joinOrder[i]];
                if (l->isNegated()) {
                    auto it = joinKeys[r.getRuleId()][start][i - 1].begin();

                    *out << ind++ << "if(w" << l->getPredicateName() << ".find({";

                    for (unsigned j = 0; j < l->getAriety(); j++) {
                        if (!l->isVariableTermAt(j)) {
                            *out << l->getTermAt(j);
                        } else {
                            *out << "(*tuple" << it->first << ")[" << it->second << "]";
                            it++;
                        }
                        if (j < l->getAriety() - 1) {
                            *out << ",";
                        }
                    }

                    *out << "})==w" << l->getPredicateName() << ".end()){\n";

                } else {
                    unsigned counter = 0;
                    *out << ind << "const vector<const Tuple* >& tuples = p" << auxiliaryMapsNameByRuleAndStartIndex[r.getRuleId()][start][i - 1] << ".getValues({";
                    for (const auto& keyPair : joinKeys[r.getRuleId()][start][i - 1]) {
                        *out << "(*tuple" << keyPair.first << ")[" << keyPair.second << "]";
                        if (counter < joinKeys[r.getRuleId()][start][i - 1].size() - 1) {
                            *out << ",";
                        }
                        counter++;
                    }

                    *out << "});\n";
                    *out << ind++ << "for( unsigned i=0; i< tuples.size();i++){\n";
                    *out << ind << "const Tuple * tuple" << i << " = tuples[i];\n";
                    //                *out << ind++ << "for( const vector<unsigned>& tuple" << i << ": tuples){\n";
                }
            } else {
                aspc::ArithmeticRelation* f = (aspc::ArithmeticRelation*) body[joinOrder[i]];

                if (handleExpression(r, *f, start, boundVariables)) {
                    closingParenthesis++;
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
                *out << ind << "const auto & insertResult = w" << r.getHead().front().getPredicateName() << ".insert({{";

                for (unsigned th = 0; th < r.getHead().front().getTermsSize(); th++) {
                    if (!r.getHead().front().isVariableTermAt(th)) {
                        if (isUnsignedInteger(r.getHead().front().getTermAt(th))) {
                            *out << r.getHead().front().getTermAt(th);
                        } else {
                            *out << "ConstantsManager::getInstance().mapConstant(\"" << escapeDoubleQuotes(r.getHead().front().getTermAt(th)) << "\"";
                        }
                    } else {
                        unsigned k;
                        if (r.getBodyToHeadVariablesMap().count(th)) {
                            for (k = 0; k < joinOrder.size(); k++) {
                                if (joinOrder[k] == r.getBodyToHeadVariablesMap()[th].first)
                                    break;
                            }
                            *out << "(*tuple" << k << ")[" << r.getBodyToHeadVariablesMap()[th].second << "]";
                        } else {
                            *out << r.getHead().front().getTermAt(th);
                        }
                    }
                    if (th < r.getHead().front().getTermsSize() - 1) {
                        *out << ",";
                    }
                }


                *out << "},w" << r.getHead().front().getPredicateName() << ".size(), &" << r.getHead().front().getPredicateName() << "});\n";
                *out << ind++ << "if(insertResult.second){\n";
                *out << ind << "tuples_" << r.getHead().front().getPredicateName() << ".push_back(&(*insertResult.first));\n";

                for (unsigned i = 0; i < body.size(); i++) {
                    if (body[joinOrder[i]]->isPositiveLiteral()) {
                        *out << ind << "insertResult.first->addPositiveReason(tuple" << i << ");\n";
                    } else if (body[joinOrder[i]]->isLiteral()) {
                        *out << ind << "insertResult.first->addNegativeReason({";
                        writeNegativeTuple(r, joinOrder, start, i);
                        *out << "});\n";
                    }
                }

                for (const string& auxMapName : predicateToAuxiliaryMaps[r.getHead().front().getPredicateName()]) {
                    *out << ind << "p" << auxMapName << ".insert2(*tuples_" << r.getHead().front().getPredicateName() << ".back());\n";
                    //                *out << ind << "cout<<\"" << auxMapName << "\"<<endl;\n";
                }
                //            *out << ind << "cout<<\"join succeded\"<<endl;\n";
                *out << --ind << "}\n";
            } else {

                //we are handling a constraint
                *out << ind << "failedConstraints.push_back(vector<aspc::Literal>());\n";

                *out << ind << "vector<const Tuple *> reasons;\n";

                for (unsigned i = 0; i < body.size(); i++) {
                    if (body[joinOrder[i]]->isLiteral()) {
                        aspc::Literal* l = (aspc::Literal*) body[joinOrder[i]];
                        *out << ind << "unordered_set<string> open_set" << i << ";\n";
                        if (l->isPositiveLiteral()) {
                            *out << ind << "explainPositiveLiteral(tuple" << i << ", open_set" << i << ", reasons);\n";
                        } else {
                            *out << ind << "Tuple tuple" << i << " = Tuple({";
                            writeNegativeTuple(r, joinOrder, start, i);
                            *out << "}, 0, &" << l->getPredicateName() << ", true);\n";
                            *out << ind << "explainNegativeLiteral(&tuple" << i << ", open_set" << i << ", reasons);\n";
                            //*out << ind << "failedConstraints.back().push_back(tupleToLiteral(Tuple({";
                            //writeNegativeTuple(r, joinOrder, start, i);
                            //*out << "}, 0, &ConstantsManager::getInstance().getPredicateName(\"" << l->getPredicateName() << "\"), true)));\n";
                        }
                    }
                }
                *out << ind++ << "for(const Tuple * reason: reasons) {\n";
                *out << ind << "failedConstraints.back().push_back(tupleToLiteral(*reason));\n";
                *out << --ind << "}\n";

            }

        }


    }
    for (unsigned i = 0; i < closingParenthesis; i++) {
        *out << --ind << "}\n";
    }
    for (unsigned i = 1; i < body.size(); i++) {
        if (body[i]->isLiteral()) {
            *out << --ind << "}\n";
        }
    }
}

void initRuleBoundVariables(unordered_set<string> & output, const aspc::Literal & lit, const unordered_set<string> & litBoundVariables, const aspc::Atom & head) {
    for (unsigned i = 0; i < lit.getAriety(); i++) {
        if (litBoundVariables.count(lit.getTermAt(i))) {
            output.insert(head.getTermAt(i));
        }
    }
}

void CompilationManager::declareDataStructuresForReasonsOfNegative(const aspc::Program & program, const aspc::Literal & lit, bool negationMet, unordered_set<string> & litBoundVariables, unordered_set<string> & openSet) {

    //TODO use real MG predicates 


    string canonicalRep = lit.getCanonicalRepresentation();
    if (openSet.count(canonicalRep)) {
        return;
    }
    openSet.insert(canonicalRep);

    for (const aspc::Rule & r : program.getRules()) {
        if (!r.isConstraint() && lit.unifies(r.getHead()[0])) {
            unordered_set<string> ruleBoundVariables;
            const aspc::Atom & head = r.getHead()[0];
            initRuleBoundVariables(ruleBoundVariables, lit, litBoundVariables, head);
            for (unsigned i = 0; i < r.getBodySize(); i++) {
                const aspc::Formula * f = r.getFormulas()[i];
                if (f -> isLiteral()) {
                    const aspc::Literal * bodyLit = (const aspc::Literal *) f;
                    if (negationMet) {
                        if (!bodyLit->isNegated()) {
                            vector<unsigned> coveredVariableIndexes;
                            bodyLit->getAtom().getCoveredVariables(ruleBoundVariables, coveredVariableIndexes);
                            if (coveredVariableIndexes.size() < bodyLit->getAriety()) {
                                string mapVariableName = bodyLit->getPredicateName() + "_";
                                for (unsigned index : coveredVariableIndexes) {
                                    mapVariableName += to_string(index) + "_";
                                }
                                if (!declaredMaps.count(mapVariableName)) {
                                    *out << ind << "vector<unsigned> keyIndexes" << mapVariableName << ";\n";
                                    for (unsigned index : coveredVariableIndexes) {
                                        *out << ind << "keyIndexes" << mapVariableName << ".push_back(" << index << ");\n";
                                    }
                                    *out << ind << "AuxiliaryMap p" << mapVariableName << "(&keyIndexes" << mapVariableName << ");\n";
                                    predicateToAuxiliaryMaps[bodyLit->getPredicateName()].insert(mapVariableName);
                                    *out << ind << "predicateToAuxiliaryMaps[" << bodyLit->getPredicateName() << "].push_back(&p" << mapVariableName << ");\n";
                                    //*out << ind << "string " << mapVariableName << ";\n";
                                    declaredMaps.insert(mapVariableName);
                                    if (modelGeneratorPredicates.count(bodyLit -> getPredicateName())) {
                                        *out << ind << "AuxiliaryMap false_p" << mapVariableName << "(&keyIndexes" << mapVariableName << ");\n";
                                        predicateToFalseAuxiliaryMaps[bodyLit->getPredicateName()].insert(mapVariableName);
                                        *out << ind << "predicateToFalseAuxiliaryMaps[" << bodyLit->getPredicateName() << "].push_back(&false_p" << mapVariableName << ");\n";
                                        modelGeneratorPredicatesInNegativeReasons.insert(bodyLit -> getPredicateName());
                                    }
                                }
                            }
                            declareDataStructuresForReasonsOfNegative(program, *bodyLit, true, ruleBoundVariables, openSet);
                        }
                    } else if (!modelGeneratorPredicates.count(bodyLit -> getPredicateName())) {
                        if (!negationMet) {
                            unordered_set<string> bodyLitVariables = bodyLit->getVariables();
                            declareDataStructuresForReasonsOfNegative(program, *bodyLit, bodyLit -> isNegated(), bodyLitVariables, openSet);
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

void CompilationManager::declareDataStructuresForReasonsOfNegative(const aspc::Program & program) {

    *out << ind << "//printing aux maps needed for reasons of negative literals;\n";
    unordered_set<string> open_set;

    for (const aspc::Rule & r : program.getRules()) {
        if (r.isConstraint()) {
            for (const aspc::Formula * f : r.getFormulas()) {
                if (f->isLiteral()) {
                    const aspc::Literal & lit = (const aspc::Literal &) * f;
                    unordered_set<string> litVariables = lit.getVariables();
                    declareDataStructuresForReasonsOfNegative(program, lit, lit.isNegated(), litVariables, open_set);
                }
            }
        }
    }
}

void CompilationManager::writeNegativeTuple(const aspc::Rule & rule, vector<unsigned> & joinOrder, unsigned start, unsigned i) {

    unsigned ruleId = rule.getRuleId();
    aspc::Literal* l = (aspc::Literal*)rule.getFormulas()[joinOrder[i]];
    auto it = joinKeys[ruleId][start][i - 1].begin();
    for (unsigned j = 0; j < l->getAriety(); j++) {
        if (!l->isVariableTermAt(j)) {
            *out << l->getTermAt(j);
        } else {
            *out << "(*tuple" << it->first << ")[" << it->second << "]";
            it++;
        }
        if (j < l->getAriety() - 1) {
            *out << ",";
        }
    }

}

string evalExpression(const aspc::ArithmeticExpression & expr, const aspc::Rule& r, unsigned start) {
    std::string res = "";
    std::pair<int, int> p = r.findFirstOccurrenceOfVariableByStarter(expr.getTerm1(), start);
    if (p.first != -1) {
        res += "(*tuple" + std::to_string(p.first) + ")[" + std::to_string(p.second) + "]";
    } else {
        res += expr.getTerm1();
    }
    if (!expr.isSingleTerm()) {
        res += expr.getOperation();
        p = r.findFirstOccurrenceOfVariableByStarter(expr.getTerm2(), start);
        if (p.first != -1) {
            res += "(*tuple" + std::to_string(p.first) + ")[" + std::to_string(p.second) + "]";
        } else {
            res += expr.getTerm2();
        }
    }
    return res;

}

bool CompilationManager::handleExpression(const aspc::Rule& r, const aspc::ArithmeticRelation& f, unsigned start, const set<string> & boundVariables) {
    if (f.isBoundedRelation(boundVariables)) {
        *out << ind++ << "if(" << evalExpression(f.getLeft(), r, start) + aspc::ArithmeticRelation::comparisonType2String[f.getComparisonType()] + evalExpression(f.getRight(), r, start) << ") {\n";
        return true;
    } else if (f.isBoundedValueAssignment(boundVariables)) {
        *out << ind << "unsigned " << f.getLeft().getTerm1() << " = " << evalExpression(f.getRight(), r, start) << ";\n";
        return false;
    }
    assert(false);
    return false;

}

bool CompilationManager::handleEqualCardsAndConstants(const aspc::Rule& r, unsigned i, const vector<unsigned>& joinOrder) {

    if (!r.getFormulas()[joinOrder[i]]->isLiteral()) {
        return false;
    }

    bool hasCondition = false;
    const aspc::Literal * l = (aspc::Literal *) r.getFormulas()[joinOrder[i]];
    if (l->isNegated()) {
        return false;
    }
    for (unsigned t1 = 0; t1 < l->getAriety(); t1++) {
        if (!l->isVariableTermAt(t1)) {
            if (!hasCondition) {
                *out << ind++ << "if( ";
                hasCondition = true;
            } else
                *out << " && ";
            *out << "(*tuple" << i << ")[" << t1 << "] == " << l->getTermAt(t1);

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
