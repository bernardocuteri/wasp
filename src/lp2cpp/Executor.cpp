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
const std::string _b = "b";
PredicateWSet wb(1);
PredicateWSet ub(1);
PredicateWSet fb(1);
const std::string _a = "a";
PredicateWSet wa(1);
PredicateWSet ua(1);
PredicateWSet fa(1);
const std::string _a_X_ = "a_X_";
PredicateWSet wa_X_(1);
PredicateWSet ua_X_(1);
std::map<std::vector<int>, std::pair< AuxMap*,AuxMap*>*> sharedVariables_0_ToAggregate_1;
Executor::~Executor() {
    for(auto sharedVar : sharedVariables_0_ToAggregate_1){
        delete sharedVar.second->first;
        delete sharedVar.second->second;
        delete sharedVar.second;
    }
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
AuxMap pa_0_({0});
AuxMap ua_0_({0});
AuxMap fa_0_({0});
AuxMap pa_({});
AuxMap ua_({});
AuxMap fa_({});
AuxMap p_a_X_0_({0});
AuxMap u_a_X_0_({0});
AuxMap p_a_X_({});
AuxMap u_a_X_({});
AuxMap pb_({});
AuxMap ub_({});
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
    if(var<0 && ( tuple.getPredicateName() == &_a)){
        std::unordered_map<const std::string*, PredicateWSet*>::iterator it_false = predicateFSetMap.find(tuple.getPredicateName());
        if (it_false == predicateFSetMap.end()) {
            } else {
            if (var < 0) {
                const auto& insertResult = it_false->second->insert(Tuple(tuple));
                if (insertResult.second) {
                    for (AuxMap* auxMap : predicateToFalseAuxiliaryMaps[it_false->first]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_a){
        if(var > 0){
            int X = tuple.at(0);
            bool buildUndef=false;
            bool buildFalse=false;
            Tuple t(std::vector<int>({X}),&_a_X_);
            if(!buildUndef){
                if(wa_X_.find(t)==NULL){
                    const auto& insertResult = wa_X_.insert(Tuple(t));
                    if (insertResult.second) {
                        for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_a_X_]){
                            auxMap -> insert2(*insertResult.first);
                        }
                        ua_X_.erase(t);
                    }
                }else{
                }
            }else{
                if(ua_X_.find(t)==NULL){
                    const auto& insertResult = ua_X_.insert(Tuple(t));
                    if (insertResult.second) {
                        for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_a_X_]){
                            auxMap -> insert2(*insertResult.first);
                        }
                    }
                }
            }
        }else{
            int X = tuple.at(0);
            bool buildUndef=false;
            bool buildFalse=false;
            Tuple t(std::vector<int>({X}),&_a_X_);
            ua_X_.erase(t);
            wa_X_.erase(t);
        }
    }
    if(tuple.getPredicateName() == &_b){
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
    if(var<0 && ( tuple.getPredicateName() == &_a)){
        std::unordered_map<const std::string*, PredicateWSet*>::iterator falseSetIt = predicateFSetMap.find(tuple.getPredicateName());
        if (falseSetIt != predicateFSetMap.end()) {
            falseSetIt->second->erase(tuple);
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
    if(tuple.getPredicateName() == &_a){
        if(var > 0){
            const std::vector<const Tuple* >& trueJoinTuples = p_a_X_0_.getValues({tuple[0]});
            for(const Tuple* t :trueJoinTuples){
                const auto insertResult = ua_X_.insert(Tuple(*t));
                if(insertResult.second){
                    for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_a_X_]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                }
                wa_X_.erase(*t);
            }
        }
        int X = tuple.at(0);
        bool buildUndef=false;
        bool buildFalse=false;
        Tuple tupleUndef(std::vector<int>({X}),&_a_X_);
        if(ua_X_.find(Tuple(tupleUndef)) == NULL){
            const auto& insertResult = ua_X_.insert(Tuple(tupleUndef));
            if (insertResult.second) {
                for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_a_X_]){
                    auxMap -> insert2(*insertResult.first);
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_b){
        int Y = tuple.at(0);
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
    predicateWSetMap[&_b]=&wb;
    predicateUSetMap[&_b]=&ub;
    stringToUniqueStringPointer["b"] = &_b;
    aggregatePredicatesAndVars [&_a]= std::vector<int>({0});
    predicateWSetMap[&_a]=&wa;
    predicateFSetMap[&_a]=&fa;
    predicateUSetMap[&_a]=&ua;
    stringToUniqueStringPointer["a"] = &_a;
    predicateToAuxiliaryMaps[&_b].push_back(&pb_);
    predicateToAuxiliaryMaps[&_a].push_back(&pa_);
    predicateToAuxiliaryMaps[&_a].push_back(&pa_0_);
    predicateToAuxiliaryMaps[&_a_X_].push_back(&p_a_X_);
    predicateToAuxiliaryMaps[&_a_X_].push_back(&p_a_X_0_);
    predicateToUndefAuxiliaryMaps[&_b].push_back(&ub_);
    predicateToUndefAuxiliaryMaps[&_a].push_back(&ua_);
    predicateToUndefAuxiliaryMaps[&_a].push_back(&ua_0_);
    predicateToUndefAuxiliaryMaps[&_a_X_].push_back(&u_a_X_);
    predicateToUndefAuxiliaryMaps[&_a_X_].push_back(&u_a_X_0_);
    predicateToFalseAuxiliaryMaps[&_a].push_back(&fa_);
    predicateToFalseAuxiliaryMaps[&_a].push_back(&fa_0_);
}
void Executor::executeProgramOnFacts(const std::vector<aspc::Literal*> & facts) {}
void Executor::executeProgramOnFacts(const std::vector<int> & facts) {
    int decisionLevel = facts[0];
    clearPropagations();
    for(unsigned i=1;i<facts.size();i++) {
        onLiteralTrue(facts[i]);
    }
    if(decisionLevel==-1) {
        std::cout<<"decisionLevelif"<<std::endl;
        {
            const Tuple * tupleU = NULL;
            bool tupleUNegated = false;
            const std::vector<const Tuple* >* tuples;
            tuples = &pb_.getValues({});
            const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
            if(tupleU == NULL){
                tuplesU = &ub_.getValues({});
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
                int Y = (*tuple0)[0];
                std::pair<AuxMap*,AuxMap*> joinTuples_0_1;
                joinTuples_0_1.first = &p_a_X_;
                joinTuples_0_1.second = &u_a_X_;
                std::pair<AuxMap*,AuxMap*> joinTuples_0_1SharedVariables;
                joinTuples_0_1SharedVariables.first = &p_a_X_0_;
                joinTuples_0_1SharedVariables.second = &u_a_X_0_;
                if(tupleU==NULL){
                    if(joinTuples_0_1SharedVariables.first->size()>=Y+1){
                        std::cout<<"conflict detected in propagator0"<<std::endl;
                        propagatedLiteralsAndReasons.insert({-1, std::vector<int>()});
                    }else{
                        if(joinTuples_0_1SharedVariables.first->size() == Y+1-1){
                            const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_1.second->getValues({});
                            for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                if(joinTuples_0_1SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0){
                                    const Tuple* aggrTupleU=NULL;
                                    const std::vector<const Tuple*>* tuplesa;
                                    const std::vector<const Tuple*>* tuplesUa=&EMPTY_TUPLES;
                                    tuplesa= &pa_0_.getValues({undefinedTuples->at(iUndef)->at(0)});
                                    if(aggrTupleU==NULL)
                                        tuplesUa= &ua_0_.getValues({undefinedTuples->at(iUndef)->at(0)});
                                    for(int j1=0;j1<tuplesa->size()+tuplesUa->size();j1++){
                                        const Tuple* auxTuple1=NULL;
                                        bool addedToReason=false;
                                        if(j1 < tuplesa->size()){
                                            auxTuple1 = tuplesa->at(j1);
                                            if(tuplesUa != &EMPTY_TUPLES) {
                                            }
                                        }else{
                                            auxTuple1 = tuplesUa->at(j1-tuplesa->size());
                                            aggrTupleU = auxTuple1;
                                        }
                                        if(aggrTupleU == NULL){
                                            std::cout<<"Tuple undefined not well formed"<<std::endl;
                                        }else{
                                            const auto & it = tupleToVar.find(*aggrTupleU);
                                            if(it != tupleToVar.end()) {
                                                int sign = aggrTupleU->isNegated() ? -1 : 1;
                                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }else{
                    if(joinTuples_0_1SharedVariables.first->size()>=Y+1){
                        const auto & it = tupleToVar.find(*tupleU);
                        if(it != tupleToVar.end()) {
                            int sign = tupleU->isNegated() ? -1 : 1;
                            auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;
                        }
                    }
                }
            }//close par
        }//close local scope
    }//close decision level == -1
    for(unsigned i=1;i<facts.size();i++) {
        unsigned factVar = facts[i] > 0 ? facts[i] : -facts[i];
        Tuple starter = atomsTable[factVar];
        starter.setNegated(facts[i]<0);
        if(facts[i] > 0){
            bool tupleUNegated = false;
            const Tuple * tupleU = NULL;
            if(starter.getPredicateName()== &_a){
                std::pair<AuxMap*,AuxMap*> joinTuples_0_1;
                joinTuples_0_1.first = &p_a_X_0_;
                joinTuples_0_1.second = &u_a_X_0_;
                int count = joinTuples_0_1.first->size();
                {
                    const std::vector<const Tuple* >* tuples;
                    tuples = &pb_.getValues({});
                    const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                    if(tupleU == NULL){
                        tuplesU = &ub_.getValues({});
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
                        int Y = (*tuple1)[0];
                        if((count >=Y+1)){
                            const std::vector<const Tuple*>* trueTuples = &p_a_X_.getValues({});
                            std::vector<int> reason;
                            for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                Tuple tuplea_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0)}),&_a);
                                const auto & ita_0 = tupleToVar.find(tuplea_0);
                                if(ita_0!=tupleToVar.end()){
                                    reason.push_back(ita_0->second * (tuplea_0.isNegated() ? -1:1));
                                }//closing if
                            }
                            if(tuple1!=tupleU){
                                const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                                if(it_reason1!=tupleToVar.end())
                                    reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                            }
                            const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                            if(it_reason_starter!=tupleToVar.end())
                                reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                            if(tupleU == NULL) {
                                std::cout<<"conflict detected in propagator External Propagation1"<<std::endl;
                                propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                            }
                            else {
                                const auto & it = tupleToVar.find(*tupleU);
                                if(it != tupleToVar.end()) {
                                    int sign = tupleU->isNegated() ? -1 : 1;
                                    propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                }
                            }
                        }
                    }//close par
                }//close par
                {
                    tupleU=NULL;
                    const std::vector<const Tuple* >* tuples;
                    tuples = &pb_.getValues({});
                    const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                    if(tupleU == NULL){
                        tuplesU = &ub_.getValues({});
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
                        int Y = (*tuple1)[0];
                        if(tupleU ==  NULL && count == Y+1-1) {
                            std::pair<AuxMap*,AuxMap*> joinTuples_0_1;
                            joinTuples_0_1.first = &p_a_X_;
                            joinTuples_0_1.second = &u_a_X_;
                            std::pair<AuxMap*,AuxMap*> joinTuples_0_1SharedVariables;
                            joinTuples_0_1SharedVariables.first = &p_a_X_0_;
                            joinTuples_0_1SharedVariables.second = &u_a_X_0_;
                            const std::vector<const Tuple*>* trueTuples = &p_a_X_.getValues({});
                            std::vector<int> reason;
                            for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                Tuple tuplea_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0)}),&_a);
                                const auto & ita_0 = tupleToVar.find(tuplea_0);
                                if(ita_0!=tupleToVar.end()){
                                    reason.push_back(ita_0->second * (tuplea_0.isNegated() ? -1:1));
                                }//closing if
                            }
                            const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                            if(it_reason1!=tupleToVar.end())
                                reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                            const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                            if(it_reason_starter!=tupleToVar.end())
                                reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                            const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_1.second->getValues({});
                            for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                if(joinTuples_0_1SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0){
                                    const Tuple* aggrTupleU=NULL;
                                    const std::vector<const Tuple*>* tuplesa;
                                    const std::vector<const Tuple*>* tuplesUa=&EMPTY_TUPLES;
                                    tuplesa= &pa_0_.getValues({undefinedTuples->at(iUndef)->at(0)});
                                    if(aggrTupleU==NULL)
                                        tuplesUa= &ua_0_.getValues({undefinedTuples->at(iUndef)->at(0)});
                                    for(int j1=0;j1<tuplesa->size()+tuplesUa->size();j1++){
                                        const Tuple* auxTuple1=NULL;
                                        bool addedToReason=false;
                                        if(j1 < tuplesa->size()){
                                            auxTuple1 = tuplesa->at(j1);
                                            const auto & it = tupleToVar.find(*auxTuple1);
                                            if(it!=tupleToVar.end()){
                                                addedToReason=true;
                                                reason.push_back(it->second * (auxTuple1->isNegated() ? -1:1));
                                            }//closing if
                                            if(tuplesUa != &EMPTY_TUPLES) {
                                            }
                                        }else{
                                            auxTuple1 = tuplesUa->at(j1-tuplesa->size());
                                            aggrTupleU = auxTuple1;
                                        }
                                        if(aggrTupleU == NULL){
                                            std::cout<<"Tuple undefined not well formed"<<std::endl;
                                        }else{
                                            const auto & it = tupleToVar.find(*aggrTupleU);
                                            if(it != tupleToVar.end()) {
                                                int sign = aggrTupleU->isNegated() ? -1 : 1;
                                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                            }
                                        }
                                        if(addedToReason) reason.pop_back();
                                    }
                                }
                            }
                        }
                    }//close par
                }//close par
            }//close par
        }//close if
        if(starter.getPredicateName() == &_b) { 
            const Tuple * tuple0 = &starter;
            if(facts[i] > 0){
                {
                    const Tuple * tupleU = NULL;
                    bool tupleUNegated = false;
                    int Y = (*tuple0)[0];
                    std::pair<AuxMap*,AuxMap*> joinTuples_0_1;
                    joinTuples_0_1.first = &p_a_X_;
                    joinTuples_0_1.second = &u_a_X_;
                    std::pair<AuxMap*,AuxMap*> joinTuples_0_1SharedVariables;
                    joinTuples_0_1SharedVariables.first = &p_a_X_0_;
                    joinTuples_0_1SharedVariables.second = &u_a_X_0_;
                    if(tupleU==NULL){
                        const std::vector<const Tuple*>* trueTuples = &p_a_X_.getValues({});
                        std::vector<int> reason;
                        for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                            Tuple tuplea_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0)}),&_a);
                            const auto & ita_0 = tupleToVar.find(tuplea_0);
                            if(ita_0!=tupleToVar.end()){
                                reason.push_back(ita_0->second * (tuplea_0.isNegated() ? -1:1));
                            }//closing if
                        }
                        if(tuple0!=tupleU){
                            const auto & it_reason0 = tupleToVar.find(Tuple(*tuple0));
                            if(it_reason0!=tupleToVar.end())
                                reason.push_back(it_reason0->second * (tuple0->isNegated() ? -1:1));
                        }
                        const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                        if(it_reason_starter!=tupleToVar.end())
                            reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                        if(joinTuples_0_1SharedVariables.first->size()>=Y+1){
                            starter.print();
                            std::cout<<"conflict detected in propagator0"<<std::endl;
                            propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                        }else{
                            if(joinTuples_0_1SharedVariables.first->size() == Y+1-1){
                                const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_1.second->getValues({});
                                for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                    if(joinTuples_0_1SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0){
                                        const Tuple* aggrTupleU=NULL;
                                        const std::vector<const Tuple*>* tuplesa;
                                        const std::vector<const Tuple*>* tuplesUa=&EMPTY_TUPLES;
                                        tuplesa= &pa_0_.getValues({undefinedTuples->at(iUndef)->at(0)});
                                        if(aggrTupleU==NULL)
                                            tuplesUa= &ua_0_.getValues({undefinedTuples->at(iUndef)->at(0)});
                                        for(int j1=0;j1<tuplesa->size()+tuplesUa->size();j1++){
                                            const Tuple* auxTuple1=NULL;
                                            bool addedToReason=false;
                                            if(j1 < tuplesa->size()){
                                                auxTuple1 = tuplesa->at(j1);
                                                const auto & it = tupleToVar.find(*auxTuple1);
                                                if(it!=tupleToVar.end()){
                                                    addedToReason=true;
                                                    reason.push_back(it->second * (auxTuple1->isNegated() ? -1:1));
                                                }//closing if
                                                if(tuplesUa != &EMPTY_TUPLES) {
                                                }
                                            }else{
                                                auxTuple1 = tuplesUa->at(j1-tuplesa->size());
                                                aggrTupleU = auxTuple1;
                                            }
                                            if(aggrTupleU == NULL){
                                                std::cout<<"Tuple undefined not well formed"<<std::endl;
                                            }else{
                                                const auto & it = tupleToVar.find(*aggrTupleU);
                                                if(it != tupleToVar.end()) {
                                                    int sign = aggrTupleU->isNegated() ? -1 : 1;
                                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                }
                                            }
                                            if(addedToReason) reason.pop_back();
                                        }
                                    }
                                }
                            }
                        }
                    }else{
                    const std::vector<const Tuple*>* trueTuples = &p_a_X_.getValues({});
                    std::vector<int> reason;
                    for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                        Tuple tuplea_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0)}),&_a);
                        const auto & ita_0 = tupleToVar.find(tuplea_0);
                        if(ita_0!=tupleToVar.end()){
                            reason.push_back(ita_0->second * (tuplea_0.isNegated() ? -1:1));
                        }//closing if
                    }
                    if(tuple0!=tupleU){
                        const auto & it_reason0 = tupleToVar.find(Tuple(*tuple0));
                        if(it_reason0!=tupleToVar.end())
                            reason.push_back(it_reason0->second * (tuple0->isNegated() ? -1:1));
                    }
                        if(joinTuples_0_1SharedVariables.first->size()>=Y+1){
                            const auto & it = tupleToVar.find(*tupleU);
                            if(it != tupleToVar.end()) {
                                int sign = tupleU->isNegated() ? -1 : 1;
                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                            }
                        }
                    }
                }//close loop nested join
            }//close loop nested join
        }//close predicate joins
    }
}
}
