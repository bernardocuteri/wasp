#include "Executor.h"

#include "utils/ConstantsManager.h"

#include "DLV2libs/input/InputDirector.h"

#include "parsing/AspCore2InstanceBuilder.h"

#include "datastructures/PredicateSet.h"

namespace aspc {
extern "C" Executor* create_object() {
    return new Executor;
}
extern "C" void destroy_object( Executor* object ) {
    delete object;
}
Executor::~Executor() {}

Executor::Executor() {}

typedef TupleWithReasons Tuple;
typedef AuxiliaryMap<Tuple> AuxMap;
typedef std::vector<const Tuple* > Tuples;
using PredicateWSet = PredicateSet<Tuple, TuplesHash>;

std::unordered_map<const std::string*, PredicateWSet*> predicateWSetMap;
std::unordered_map<const std::string*, PredicateWSet*> predicateFalseWSetMap;
std::unordered_map<const std::string*, PredicateWSet*> predicateUSetMap;
const std::string _p_1 = "p_1";
PredicateWSet wp_1(2);
PredicateWSet up_1(2);
const std::string _p_2 = "p_2";
PredicateWSet wp_2(1);
PredicateWSet up_2(1);

const std::vector<const Tuple* > EMPTY_TUPLES;
std::unordered_map<std::string, const std::string * > stringToUniqueStringPointer;
typedef void (*ExplainNegative)(const std::vector<unsigned> & lit, std::unordered_set<std::string> & open_set, std::vector<const Tuple *> & output);

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
    std::vector<unsigned> terms;
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
AuxMap pp_2_({});
AuxMap up_2_({});
AuxMap pp_1_0_({0});
AuxMap up_1_0_({0});
//printing aux maps needed for reasons of negative literals;
//printing functions prototypes for reasons of negative literals;
void explain_not_p_2_0_(const std::vector<unsigned> &, std::unordered_set<std::string> &, std::vector<const Tuple *> &);
void explainPositiveLiteral(const Tuple *, std::unordered_set<std::string> &, std::vector<const Tuple*> &);
//printing functions for reasons of negative literals;
void explain_not_p_2_0_(const std::vector<unsigned> & lit, std::unordered_set<std::string> & open_set, std::vector<const Tuple *> & output){
    std::string canonicalRep = _p_2;
    canonicalRep += std::to_string(lit[0]);
    if(open_set.find(canonicalRep)!=open_set.end()){
        return;
    }
    open_set.insert(canonicalRep);
    open_set.erase(canonicalRep);
}
void createFunctionsMap() {
    explainNegativeFunctionsMap[&_p_2] = explain_not_p_2_0_;
}
void printTuples(const std::string & predicateName, const Tuples & tuples) {
    for (const std::vector<unsigned> * tuple : tuples) {
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
    for (unsigned v : tuple) {
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
    predicateWSetMap[&_p_1]=&wp_1;
    predicateUSetMap[&_p_1]=&up_1;
    stringToUniqueStringPointer["p_1"] = &_p_1;
    predicateWSetMap[&_p_2]=&wp_2;
    predicateUSetMap[&_p_2]=&up_2;
    stringToUniqueStringPointer["p_2"] = &_p_2;
    predicateToAuxiliaryMaps[&_p_1].push_back(&pp_1_0_);
    predicateToAuxiliaryMaps[&_p_2].push_back(&pp_2_);
    predicateToUndefAuxiliaryMaps[&_p_1].push_back(&up_1_0_);
    predicateToUndefAuxiliaryMaps[&_p_2].push_back(&up_2_);
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
            const std::vector<const Tuple* >* tuples;
            tuples = &pp_2_.getValues({});
            const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
            if(tupleU == NULL){
                tuplesU = &up_2_.getValues({});
            }
            for( unsigned i=0; i< tuples->size() + tuplesU->size();i++){
                const Tuple * tuple0 = NULL;
                if(i<tuples->size()){
                    tuple0 = tuples->at(i);
                    if(tuplesU != &EMPTY_TUPLES) {
                        tupleU = NULL;
                    }
                }
                else {
                    tuple0 = tuplesU->at(i-tuples->size());
                    tupleU = tuple0;
                    tupleUNegated = false;
                }
                unsigned X_0 = (*tuple0)[0];
                const Tuple negativeTuple = Tuple({X_0}, &_p_2, true);
                const Tuple * tuple1 = &negativeTuple;
                bool lTrue = (wp_2.find(negativeTuple)!=NULL);
                const Tuple * undefTuple = up_2.find(negativeTuple);
                if((!lTrue && undefTuple == NULL) || (undefTuple && tupleU == NULL)){
                    if(undefTuple){
                        tuple1 = tupleU = undefTuple;
                        tupleUNegated = true;
                    }
                    const std::vector<const Tuple* >* tuples;
                    tuples = &pp_1_0_.getValues({2});
                    const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                    if(tupleU == NULL){
                        tuplesU = &up_1_0_.getValues({2});
                    }
                    for( unsigned i=0; i< tuples->size() + tuplesU->size();i++){
                        const Tuple * tuple2 = NULL;
                        if(i<tuples->size()){
                            tuple2 = tuples->at(i);
                            if(tuplesU != &EMPTY_TUPLES) {
                                tupleU = NULL;
                            }
                        }
                        else {
                            tuple2 = tuplesU->at(i-tuples->size());
                            tupleU = tuple2;
                            tupleUNegated = false;
                        }
                        unsigned X_1 = (*tuple2)[1];
                        if( (*tuple2)[0] == 2){
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
                                        reasons.push_back(tuple1);
                                    }
                                    if(tuple2 != tupleU){
                                        std::unordered_set<std::string> open_set2;
                                        explainPositiveLiteral(tuple2, open_set2, reasons);
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
            }//close par
        }//close local scope
    }//close decision level == -1
    for(unsigned i=1;i<facts.size();i++) {
        unsigned factVar = facts[i] > 0 ? facts[i] : -facts[i];
        Tuple starter = atomsTable[factVar];
        starter.setNegated(facts[i]<0);
        if(starter.getPredicateName() == &_p_1) { 
            const Tuple * tuple0 = &starter;
            if(facts[i] > 0){
                {
                    const Tuple * tupleU = NULL;
                    bool tupleUNegated = false;
                    unsigned X_1 = (*tuple0)[1];
                    if( (*tuple0)[0] == 2){
                        const std::vector<const Tuple* >* tuples;
                        tuples = &pp_2_.getValues({});
                        const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                        if(tupleU == NULL){
                            tuplesU = &up_2_.getValues({});
                        }
                        for( unsigned i=0; i< tuples->size() + tuplesU->size();i++){
                            const Tuple * tuple1 = NULL;
                            if(i<tuples->size()){
                                tuple1 = tuples->at(i);
                                if(tuplesU != &EMPTY_TUPLES) {
                                    tupleU = NULL;
                                }
                            }
                            else {
                                tuple1 = tuplesU->at(i-tuples->size());
                                tupleU = tuple1;
                                tupleUNegated = false;
                            }
                            unsigned X_0 = (*tuple1)[0];
                            const Tuple negativeTuple = Tuple({X_0}, &_p_2, true);
                            const Tuple * tuple2 = &negativeTuple;
                            bool lTrue = (wp_2.find(negativeTuple)!=NULL);
                            const Tuple * undefTuple = up_2.find(negativeTuple);
                            if((!lTrue && undefTuple == NULL) || (undefTuple && tupleU == NULL)){
                                if(undefTuple){
                                    tuple2 = tupleU = undefTuple;
                                    tupleUNegated = true;
                                }
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
                                        if(tuple2 != tupleU){
                                            std::unordered_set<std::string> open_set2;
                                            reasons.push_back(tuple2);
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
                }//close loop nested join
            }//close loop nested join
        }//close predicate joins
        if(starter.getPredicateName() == &_p_2) { 
            const Tuple * tuple0 = &starter;
            if(facts[i] < 0){
                {
                    const Tuple * tupleU = NULL;
                    bool tupleUNegated = false;
                    unsigned X_0 = (*tuple0)[0];
                    const Tuple * tuple1 = (wp_2.find({X_0}));
                    if(!tuple1 && !tupleU ){
                        tuple1 = tupleU = (up_2.find({X_0}));
                        tupleUNegated = false;
                    }
                    if(tuple1){
                        const std::vector<const Tuple* >* tuples;
                        tuples = &pp_1_0_.getValues({2});
                        const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                        if(tupleU == NULL){
                            tuplesU = &up_1_0_.getValues({2});
                        }
                        for( unsigned i=0; i< tuples->size() + tuplesU->size();i++){
                            const Tuple * tuple2 = NULL;
                            if(i<tuples->size()){
                                tuple2 = tuples->at(i);
                                if(tuplesU != &EMPTY_TUPLES) {
                                    tupleU = NULL;
                                }
                            }
                            else {
                                tuple2 = tuplesU->at(i-tuples->size());
                                tupleU = tuple2;
                                tupleUNegated = false;
                            }
                            unsigned X_1 = (*tuple2)[1];
                            if( (*tuple2)[0] == 2){
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
                                            reasons.push_back(tuple0);
                                        }
                                        if(tuple1 != tupleU){
                                            std::unordered_set<std::string> open_set1;
                                            explainPositiveLiteral(tuple1, open_set1, reasons);
                                        }
                                        if(tuple2 != tupleU){
                                            std::unordered_set<std::string> open_set2;
                                            explainPositiveLiteral(tuple2, open_set2, reasons);
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
                }//close loop nested join
            }//close loop nested join
            if(facts[i] > 0){
                {
                    const Tuple * tupleU = NULL;
                    bool tupleUNegated = false;
                    unsigned X_0 = (*tuple0)[0];
                    const Tuple negativeTuple = Tuple({X_0}, &_p_2, true);
                    const Tuple * tuple1 = &negativeTuple;
                    bool lTrue = (wp_2.find(negativeTuple)!=NULL);
                    const Tuple * undefTuple = up_2.find(negativeTuple);
                    if((!lTrue && undefTuple == NULL) || (undefTuple && tupleU == NULL)){
                        if(undefTuple){
                            tuple1 = tupleU = undefTuple;
                            tupleUNegated = true;
                        }
                        const std::vector<const Tuple* >* tuples;
                        tuples = &pp_1_0_.getValues({2});
                        const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                        if(tupleU == NULL){
                            tuplesU = &up_1_0_.getValues({2});
                        }
                        for( unsigned i=0; i< tuples->size() + tuplesU->size();i++){
                            const Tuple * tuple2 = NULL;
                            if(i<tuples->size()){
                                tuple2 = tuples->at(i);
                                if(tuplesU != &EMPTY_TUPLES) {
                                    tupleU = NULL;
                                }
                            }
                            else {
                                tuple2 = tuplesU->at(i-tuples->size());
                                tupleU = tuple2;
                                tupleUNegated = false;
                            }
                            unsigned X_1 = (*tuple2)[1];
                            if( (*tuple2)[0] == 2){
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
                                            reasons.push_back(tuple1);
                                        }
                                        if(tuple2 != tupleU){
                                            std::unordered_set<std::string> open_set2;
                                            explainPositiveLiteral(tuple2, open_set2, reasons);
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
                }//close loop nested join
            }//close loop nested join
        }//close predicate joins
    }
}
}
