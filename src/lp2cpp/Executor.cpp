#include "Executor.h"

#include "utils/ConstantsManager.h"

#include "DLV2libs/input/InputDirector.h"

#include "parsing/AspCore2InstanceBuilder.h"

#include "datastructures/PredicateSet.h"

#include<map>

namespace aspc {
extern "C" Executor* create_object() {
    return new Executor;
}
extern "C" void destroy_object( Executor* object ) {
    delete object;
}
Executor::Executor() {}

typedef TupleWithReasons Tuple;
typedef AuxiliaryMap<Tuple> AuxMap;
typedef std::vector<const Tuple* > Tuples;
using PredicateWSet = PredicateSet<Tuple, TuplesHash>;

std::unordered_map<const std::string*, PredicateWSet*> predicateWSetMap;
std::unordered_map<const std::string*, PredicateWSet*> predicateFSetMap;
std::unordered_map<const std::string*, PredicateWSet*> predicateUSetMap;
std::unordered_map<const std::string*,int> maxPossibleSums;
std::unordered_map<const std::string*,int> trueSums;
std::unordered_map<const std::string*,std::vector<int>> aggregatePredicatesAndVars;
const std::string _p_0 = "p_0";
PredicateWSet wp_0(1);
PredicateWSet up_0(1);
PredicateWSet fp_0(1);
Executor::~Executor() {
}


const std::vector<const Tuple* > EMPTY_TUPLES;
std::unordered_map<std::string, const std::string * > stringToUniqueStringPointer;
typedef void (*ExplainNegative)(const std::vector<int> & lit, std::unordered_set<std::string> & open_set, std::vector<const Tuple *> & output);

std::vector<Tuple> atomsTable;

std::unordered_map<Tuple, unsigned, TuplesHash> tupleToVar;

std::unordered_map<const std::string*, ExplainNegative> explainNegativeFunctionsMap;

Tuple parseTuple(const std::string & literalString) {
    std::string predicateName;
    unsigned i = 0;
    for (i = 0; i < literalString.size(); i++) {
        if (literalString[i] == '(') {
            predicateName = literalString.substr(0, i);
            break;
        }
        if (i == literalString.size() - 1) {
            predicateName = literalString.substr(0);
        }
    }
    std::vector<int> terms;
    for (; i < literalString.size(); i++) {
        char c = literalString[i];
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
            int start = i;
            int openBrackets = 0;
            while ((c != ' ' && c != ',' && c != ')') || openBrackets > 0) {
                if (c == '(') {
                    openBrackets++;
                    } else if (c == ')') {
                    openBrackets--;
                    }
                i++;
                c = literalString[i];
            }
            terms.push_back(ConstantsManager::getInstance().mapConstant(literalString.substr(start, i - start)));
        }
    }
    return Tuple(terms, stringToUniqueStringPointer[predicateName]);
}
//only ground lit function calls are not known a priori
void explainNegativeLiteral(const Tuple * lit, std::unordered_set<std::string> & open_set, std::vector<const Tuple *> & output) {
    explainNegativeFunctionsMap[lit->getPredicateName()](*lit, open_set, output);
}

std::unordered_map <const std::string*, std::vector <AuxMap*> > predicateToAuxiliaryMaps;
std::unordered_map <const std::string*, std::vector <AuxMap*> > predicateToUndefAuxiliaryMaps;
std::unordered_map <const std::string*, std::vector <AuxMap*> > predicateToFalseAuxiliaryMaps;
AuxMap pp_0_({});
AuxMap up_0_({});
//printing aux maps needed for reasons of negative literals;
//printing functions prototypes for reasons of negative literals;
void explainPositiveLiteral(const Tuple *, std::unordered_set<std::string> &, std::vector<const Tuple*> &);
//printing functions for reasons of negative literals;
void createFunctionsMap() {
}
void printTuples(const std::string & predicateName, const Tuples & tuples) {
    for (const std::vector<int> * tuple : tuples) {
        std::cout <<predicateName<< "(";
        for (unsigned i = 0; i < tuple->size(); i++) {
            if (i > 0) {
                std::cout << ",";
            }
            std::cout << ConstantsManager::getInstance().unmapConstant((*tuple)[i]);
        }
        std::cout << ").\n";
    }
}
void Executor::executeFromFile(const char* filename) {
    DLV2::InputDirector director;
    AspCore2InstanceBuilder* builder = new AspCore2InstanceBuilder();
    director.configureBuilder(builder);
    std::vector<const char*> fileNames;
    fileNames.push_back(filename);
    director.parse(fileNames);
    executeProgramOnFacts(builder->getProblemInstance());
    delete builder;
}

void explainPositiveLiteral(const Tuple * tuple, std::unordered_set<std::string> & open_set, std::vector<const Tuple*> & outputReasons) {
    const std::vector<const Tuple*> & tupleReasons = tuple->getPositiveReasons();
     if (tupleReasons.empty()) {
        outputReasons.push_back(tuple);
    }
    else {
        for (const Tuple * reason : tupleReasons) {
            explainPositiveLiteral(reason, open_set, outputReasons);
        }
    }
    for (const Tuple & reason : tuple->getNegativeReasons()) {
        explainNegativeLiteral(&reason, open_set, outputReasons);
    }
}

aspc::Literal tupleToLiteral(const Tuple & tuple) {
    aspc::Literal literal(*tuple.getPredicateName(), tuple.isNegated());
    for (int v : tuple) {
        literal.addTerm(ConstantsManager::getInstance().unmapConstant(v));
    }
    return literal;
}
inline void Executor::onLiteralTrue(const aspc::Literal* l) {
}
inline void Executor::onLiteralUndef(const aspc::Literal* l) {
}
inline void Executor::onLiteralTrue(int var) {
    unsigned uVar = var > 0 ? var : -var;
    const Tuple & tuple = atomsTable[uVar];
    std::unordered_map<const std::string*,int>::iterator sum_it;
    std::string minus = var < 0 ? "-" : "";
    std::unordered_map<const std::string*,PredicateWSet*>::iterator uSetIt = predicateUSetMap.find(tuple.getPredicateName());
    if(uSetIt!=predicateUSetMap.end()) {
        uSetIt->second->erase(tuple);
    }
    std::unordered_map<const std::string*, PredicateWSet*>::iterator it = predicateWSetMap.find(tuple.getPredicateName());
    if (it == predicateWSetMap.end()) {
        } else {
        if (var > 0) {
            const auto& insertResult = it->second->insert(Tuple(tuple));
            if (insertResult.second) {
                for (AuxMap* auxMap : predicateToAuxiliaryMaps[it->first]) {
                    auxMap -> insert2(*insertResult.first);
                }
            }
        }
    }
}
inline void Executor::onLiteralUndef(int var) {
    unsigned uVar = var > 0 ? var : -var;
    const Tuple & tuple = atomsTable[uVar];
    std::unordered_map<const std::string*,int>::iterator sum_it;
    std::string minus = var < 0 ? "-" : "";
    if (var > 0) {
        std::unordered_map<const std::string*, PredicateWSet*>::iterator wSetIt = predicateWSetMap.find(tuple.getPredicateName());
        if (wSetIt != predicateWSetMap.end()) {
            wSetIt->second->erase(tuple);
        }
    }
    std::unordered_map<const std::string*, PredicateWSet*>::iterator it = predicateUSetMap.find(tuple.getPredicateName());
    if (it == predicateUSetMap.end()) {
        } else {
        const auto& insertResult = it->second->insert(Tuple(tuple));
        if (insertResult.second) {
            for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[it->first]) {
                auxMap -> insert2(*insertResult.first);
            }
        }
    }
}
inline void Executor::addedVarName(int var, const std::string & atom) {
    atomsTable.resize(var+1);
    atomsTable.insert(atomsTable.begin()+var, parseTuple(atom));
    tupleToVar[atomsTable[var]]= var;
}
void Executor::clearPropagations() {
    propagatedLiteralsAndReasons.clear();
}
void Executor::clear() {
    failedConstraints.clear();
    predicateToAuxiliaryMaps.clear();
}
void Executor::init() {
    createFunctionsMap();
    predicateWSetMap[&_p_0]=&wp_0;
    predicateUSetMap[&_p_0]=&up_0;
    stringToUniqueStringPointer["p_0"] = &_p_0;
    predicateToAuxiliaryMaps[&_p_0].push_back(&pp_0_);
    predicateToUndefAuxiliaryMaps[&_p_0].push_back(&up_0_);
}
void Executor::executeProgramOnFacts(const std::vector<aspc::Literal*> & facts) {}
void Executor::executeProgramOnFacts(const std::vector<int> & facts) {
    int decisionLevel = facts[0];
    clearPropagations();
    for(unsigned i=1;i<facts.size();i++) {
        onLiteralTrue(facts[i]);
    }
    if(decisionLevel==-1) {
        {
            const Tuple * tupleU = NULL;
            bool tupleUNegated = false;
            const Tuple * tuple0 = (wp_0.find({0}));
            if(!tuple0 && !tupleU ){
                tuple0 = tupleU = (up_0.find({0}));
                tupleUNegated = false;
            }
            if(tuple0){
                if( (*tuple0)[0] == 0){
                    const std::vector<const Tuple* >* tuples;
                    tuples = &pp_0_.getValues({});
                    const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                    std::vector<const Tuple* > tupleUInVector;
                    if(tupleU == NULL){
                        tuplesU = &up_0_.getValues({});
                    }
                    else {
                        if(tupleU && !tupleUNegated && tupleU->getPredicateName() == &_p_0) {
                            tupleUInVector.push_back(tupleU);
                        }
                    }
                    for( unsigned i=0; i< tuples->size() + tuplesU->size() + tupleUInVector.size();i++){
                        const Tuple * tuple1 = NULL;
                        if(i<tuples->size()){
                            tuple1 = tuples->at(i);
                            if(tuplesU != &EMPTY_TUPLES) {
                                tupleU = NULL;
                            }
                        }
                        else if(i<tuples->size()+tuplesU->size()) {
                            tuple1 = tuplesU->at(i-tuples->size());
                            tupleU = tuple1;
                            tupleUNegated = false;
                        }
                        else {
                            tuple1 = tupleU;
                            tupleUNegated = false;
                        }
                        int X_1 = (*tuple1)[0];
                        int sign = tupleUNegated ? -1 : 1;
                        if(!tupleU) {
                            std::cout<<"conflict detected in propagator"<<std::endl;
                            propagatedLiteralsAndReasons.insert({-1, std::vector<int>()});
                        }
                        else {
                            const auto & it = tupleToVar.find(*tupleU);
                            if(it != tupleToVar.end()) {
                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;
                                std::vector<const Tuple *> reasons;
                                if(tuple0 != tupleU){
                                    std::unordered_set<std::string> open_set0;
                                    explainPositiveLiteral(tuple0, open_set0, reasons);
                                }
                                if(tuple1 != tupleU){
                                    std::unordered_set<std::string> open_set1;
                                    explainPositiveLiteral(tuple1, open_set1, reasons);
                                }
                                reas.reserve(reasons.size());
                                for(const Tuple * reason: reasons) {
                                    const auto & it = tupleToVar.find(*reason);
                                    if(it != tupleToVar.end()) {
                                        reas.push_back(it->second * (reason->isNegated()? -1:1));
                                    }
                                }
                            }
                        }
                    }//close par
                }//close par
            }//close par
        }//close local scope
    }//close decision level == -1
    for(unsigned i=1;i<facts.size();i++) {
        unsigned factVar = facts[i] > 0 ? facts[i] : -facts[i];
        Tuple starter = atomsTable[factVar];
        starter.setNegated(facts[i]<0);
        if(starter.getPredicateName() == &_p_0) { 
            const Tuple * tuple0 = &starter;
            if(facts[i] > 0){
                {
                    const Tuple * tupleU = NULL;
                    bool tupleUNegated = false;
                    if( (*tuple0)[0] == 0){
                        const std::vector<const Tuple* >* tuples;
                        tuples = &pp_0_.getValues({});
                        const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                        std::vector<const Tuple* > tupleUInVector;
                        if(tupleU == NULL){
                            tuplesU = &up_0_.getValues({});
                        }
                        else {
                            if(tupleU && !tupleUNegated && tupleU->getPredicateName() == &_p_0) {
                                tupleUInVector.push_back(tupleU);
                            }
                        }
                        for( unsigned i=0; i< tuples->size() + tuplesU->size() + tupleUInVector.size();i++){
                            const Tuple * tuple1 = NULL;
                            if(i<tuples->size()){
                                tuple1 = tuples->at(i);
                                if(tuplesU != &EMPTY_TUPLES) {
                                    tupleU = NULL;
                                }
                            }
                            else if(i<tuples->size()+tuplesU->size()) {
                                tuple1 = tuplesU->at(i-tuples->size());
                                tupleU = tuple1;
                                tupleUNegated = false;
                            }
                            else {
                                tuple1 = tupleU;
                                tupleUNegated = false;
                            }
                            int X_1 = (*tuple1)[0];
                            int sign = tupleUNegated ? -1 : 1;
                            if(!tupleU) {
                                std::cout<<"conflict detected in propagator"<<std::endl;
                                propagatedLiteralsAndReasons.insert({-1, std::vector<int>()});
                            }
                            else {
                                const auto & it = tupleToVar.find(*tupleU);
                                if(it != tupleToVar.end()) {
                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;
                                    std::vector<const Tuple *> reasons;
                                    if(tuple0 != tupleU){
                                        std::unordered_set<std::string> open_set0;
                                        explainPositiveLiteral(tuple0, open_set0, reasons);
                                    }
                                    if(tuple1 != tupleU){
                                        std::unordered_set<std::string> open_set1;
                                        explainPositiveLiteral(tuple1, open_set1, reasons);
                                    }
                                    reas.reserve(reasons.size());
                                    for(const Tuple * reason: reasons) {
                                        const auto & it = tupleToVar.find(*reason);
                                        if(it != tupleToVar.end()) {
                                            reas.push_back(it->second * (reason->isNegated()? -1:1));
                                        }
                                    }
                                }
                            }
                        }//close par
                    }//close par
                }//close loop nested join
            }//close loop nested join
            if(facts[i] > 0){
                {
                    const Tuple * tupleU = NULL;
                    bool tupleUNegated = false;
                    int X_1 = (*tuple0)[0];
                    const Tuple * tuple1 = NULL;
                    if(tupleU && tupleU->getPredicateName() == &_p_0){
;                        const Tuple * undefRepeatingTuple = (up_0.find({0}));
                        if(tupleU == undefRepeatingTuple){
                            tuple1 = undefRepeatingTuple;
                        }
                    }
                    if(!tuple1){
;                        tuple1 = (wp_0.find({0}));
                    }
                    if(!tuple1 && !tupleU){
;                        tuple1 = tupleU = (up_0.find({0}));
                    }
                    tupleUNegated = false;
                    if(tuple1){
                        if( (*tuple1)[0] == 0){
                            int sign = tupleUNegated ? -1 : 1;
                            if(!tupleU) {
                                std::cout<<"conflict detected in propagator"<<std::endl;
                                propagatedLiteralsAndReasons.insert({-1, std::vector<int>()});
                            }
                            else {
                                const auto & it = tupleToVar.find(*tupleU);
                                if(it != tupleToVar.end()) {
                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;
                                    std::vector<const Tuple *> reasons;
                                    if(tuple0 != tupleU){
                                        std::unordered_set<std::string> open_set0;
                                        explainPositiveLiteral(tuple0, open_set0, reasons);
                                    }
                                    if(tuple1 != tupleU){
                                        std::unordered_set<std::string> open_set1;
                                        explainPositiveLiteral(tuple1, open_set1, reasons);
                                    }
                                    reas.reserve(reasons.size());
                                    for(const Tuple * reason: reasons) {
                                        const auto & it = tupleToVar.find(*reason);
                                        if(it != tupleToVar.end()) {
                                            reas.push_back(it->second * (reason->isNegated()? -1:1));
                                        }
                                    }
                                }
                            }
                        }//close par
                    }//close par
                }//close loop nested join
            }//close loop nested join
        }//close predicate joins
    }
}
}
