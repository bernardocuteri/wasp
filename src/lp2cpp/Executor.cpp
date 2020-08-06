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
const std::string _fraction_user = "fraction_user";
PredicateWSet wfraction_user(1);
PredicateWSet ufraction_user(1);
PredicateWSet ffraction_user(1);
const std::string _assign = "assign";
PredicateWSet wassign(5);
PredicateWSet uassign(5);
PredicateWSet fassign(5);
const std::string _assign_USER_ID_X_Y_Z_W_ = "assign_USER_ID_X_Y_Z_W_";
PredicateWSet wassign_USER_ID_X_Y_Z_W_(5);
PredicateWSet uassign_USER_ID_X_Y_Z_W_(5);
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
AuxMap passign_0_1_2_3_4_({0,1,2,3,4});
AuxMap uassign_0_1_2_3_4_({0,1,2,3,4});
AuxMap fassign_0_1_2_3_4_({0,1,2,3,4});
AuxMap passign_({});
AuxMap uassign_({});
AuxMap fassign_({});
AuxMap p_assign_USER_ID_X_Y_Z_W_0_({0});
AuxMap u_assign_USER_ID_X_Y_Z_W_0_({0});
AuxMap p_assign_USER_ID_X_Y_Z_W_({});
AuxMap u_assign_USER_ID_X_Y_Z_W_({});
AuxMap p_assign_USER_ID_X_Y_Z_W_0_1_2_3_4_({0,1,2,3,4});
AuxMap u_assign_USER_ID_X_Y_Z_W_0_1_2_3_4_({0,1,2,3,4});
AuxMap pfraction_user_({});
AuxMap ufraction_user_({});
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
    if(var<0 && ( tuple.getPredicateName() == &_assign)){
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
    if(tuple.getPredicateName() == &_assign){
        if(var > 0){
            int USER_ID = tuple.at(0);
            int X = tuple.at(1);
            int Y = tuple.at(2);
            int Z = tuple.at(3);
            int W = tuple.at(4);
            bool buildUndef=false;
            bool buildFalse=false;
            Tuple t(std::vector<int>({USER_ID,X,Y,Z,W}),&_assign_USER_ID_X_Y_Z_W_);
            if(!buildUndef && !buildFalse){
                if(wassign_USER_ID_X_Y_Z_W_.find(t)==NULL){
                    const auto& insertResult = wassign_USER_ID_X_Y_Z_W_.insert(Tuple(t));
                    if (insertResult.second) {
                        for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_assign_USER_ID_X_Y_Z_W_]){
                            auxMap -> insert2(*insertResult.first);
                        }
                        uassign_USER_ID_X_Y_Z_W_.erase(t);
                    }
                }
            }else{
            if(!buildFalse){
                if(uassign_USER_ID_X_Y_Z_W_.find(t)==NULL){
                        const auto& insertResult = uassign_USER_ID_X_Y_Z_W_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_assign_USER_ID_X_Y_Z_W_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                        }
                    }
                }
            }
        }else{
            int USER_ID = tuple.at(0);
            int X = tuple.at(1);
            int Y = tuple.at(2);
            int Z = tuple.at(3);
            int W = tuple.at(4);
            bool buildUndef=false;
            bool buildFalse=false;
            Tuple t(std::vector<int>({USER_ID,X,Y,Z,W}),&_assign_USER_ID_X_Y_Z_W_);
            uassign_USER_ID_X_Y_Z_W_.erase(t);
            wassign_USER_ID_X_Y_Z_W_.erase(t);
        }
    }
    if(tuple.getPredicateName() == &_fraction_user){
        int F = tuple.at(0);
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
    if(var<0 && ( tuple.getPredicateName() == &_assign)){
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
    if(tuple.getPredicateName() == &_assign){
        if(var > 0){
            const std::vector<const Tuple* >& trueJoinTuples = p_assign_USER_ID_X_Y_Z_W_0_1_2_3_4_.getValues({tuple[0],tuple[1],tuple[2],tuple[3],tuple[4]});
            for(const Tuple* t :trueJoinTuples){
                const auto insertResult = uassign_USER_ID_X_Y_Z_W_.insert(Tuple(*t));
                if(insertResult.second){
                    for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_assign_USER_ID_X_Y_Z_W_]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                }
                wassign_USER_ID_X_Y_Z_W_.erase(*t);
            }
        }
        int USER_ID = tuple.at(0);
        int X = tuple.at(1);
        int Y = tuple.at(2);
        int Z = tuple.at(3);
        int W = tuple.at(4);
        bool buildUndef=false;
        bool buildFalse=false;
        Tuple tupleUndef(std::vector<int>({USER_ID,X,Y,Z,W}),&_assign_USER_ID_X_Y_Z_W_);
        const auto& insertResult = uassign_USER_ID_X_Y_Z_W_.insert(Tuple(tupleUndef));
        if (insertResult.second) {
            for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_assign_USER_ID_X_Y_Z_W_]){
                auxMap -> insert2(*insertResult.first);
            }
        }
    }
    if(tuple.getPredicateName() == &_fraction_user){
        int F = tuple.at(0);
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
    predicateWSetMap[&_fraction_user]=&wfraction_user;
    predicateUSetMap[&_fraction_user]=&ufraction_user;
    stringToUniqueStringPointer["fraction_user"] = &_fraction_user;
    aggregatePredicatesAndVars [&_assign]= std::vector<int>({0});
    predicateWSetMap[&_assign]=&wassign;
    predicateFSetMap[&_assign]=&fassign;
    predicateUSetMap[&_assign]=&uassign;
    stringToUniqueStringPointer["assign"] = &_assign;
    predicateToAuxiliaryMaps[&_fraction_user].push_back(&pfraction_user_);
    predicateToAuxiliaryMaps[&_assign].push_back(&passign_);
    predicateToAuxiliaryMaps[&_assign].push_back(&passign_0_1_2_3_4_);
    predicateToAuxiliaryMaps[&_assign_USER_ID_X_Y_Z_W_].push_back(&p_assign_USER_ID_X_Y_Z_W_);
    predicateToAuxiliaryMaps[&_assign_USER_ID_X_Y_Z_W_].push_back(&p_assign_USER_ID_X_Y_Z_W_0_);
    predicateToAuxiliaryMaps[&_assign_USER_ID_X_Y_Z_W_].push_back(&p_assign_USER_ID_X_Y_Z_W_0_1_2_3_4_);
    predicateToUndefAuxiliaryMaps[&_fraction_user].push_back(&ufraction_user_);
    predicateToUndefAuxiliaryMaps[&_assign].push_back(&uassign_);
    predicateToUndefAuxiliaryMaps[&_assign].push_back(&uassign_0_1_2_3_4_);
    predicateToUndefAuxiliaryMaps[&_assign_USER_ID_X_Y_Z_W_].push_back(&u_assign_USER_ID_X_Y_Z_W_);
    predicateToUndefAuxiliaryMaps[&_assign_USER_ID_X_Y_Z_W_].push_back(&u_assign_USER_ID_X_Y_Z_W_0_);
    predicateToUndefAuxiliaryMaps[&_assign_USER_ID_X_Y_Z_W_].push_back(&u_assign_USER_ID_X_Y_Z_W_0_1_2_3_4_);
    predicateToFalseAuxiliaryMaps[&_assign].push_back(&fassign_);
    predicateToFalseAuxiliaryMaps[&_assign].push_back(&fassign_0_1_2_3_4_);
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
            tuples = &pfraction_user_.getValues({});
            const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
            if(tupleU == NULL){
                tuplesU = &ufraction_user_.getValues({});
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
                int F = (*tuple0)[0];
                std::pair<AuxMap*,AuxMap*> joinTuples_0_1;
                joinTuples_0_1.first = &p_assign_USER_ID_X_Y_Z_W_;
                joinTuples_0_1.second = &u_assign_USER_ID_X_Y_Z_W_;
                std::pair<AuxMap*,AuxMap*> joinTuples_0_1SharedVariables;
                joinTuples_0_1SharedVariables.first = &p_assign_USER_ID_X_Y_Z_W_0_;
                joinTuples_0_1SharedVariables.second = &u_assign_USER_ID_X_Y_Z_W_0_;
                int actualSize1 = joinTuples_0_1SharedVariables.first->size();
                std::set<std::vector<int>> alreadyCounted1;
                for(const Tuple * joinTupleUndef : u_assign_USER_ID_X_Y_Z_W_.getValues({})){
                    if(joinTuples_0_1SharedVariables.first->getValues({joinTupleUndef->at(0)}).size()==0){
                        if(joinTuples_0_1SharedVariables.second->getValues({joinTupleUndef->at(0)}).size()>0){
                            std::vector<int> key({joinTupleUndef->at(0)});
                            alreadyCounted1.insert(key);
                        }
                    }
                }
                actualSize1+=alreadyCounted1.size();
                if(tupleU==NULL){
                    if(!(actualSize1>=F)){
                        std::cout<<"conflict detected in propagator"<<std::endl;
                        propagatedLiteralsAndReasons.insert({-1, std::vector<int>()});
                    }else{
                        if(actualSize1 == F){
                            const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_1.second->getValues({});
                            for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                if(joinTuples_0_1SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0 && joinTuples_0_1SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 1){
                                    {
                                        const Tuple* aggrTupleU = uassign.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1),undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3),undefinedTuples->at(iUndef)->at(4)});
                                        if(aggrTupleU != NULL){
                                            const auto & it = tupleToVar.find(*aggrTupleU);
                                            if(it != tupleToVar.end()) {
                                                int sign = aggrTupleU->isNegated() ? 1:-1;
                                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }else{
                    if(!(actualSize1>=F)){
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
        if(facts[i] < 0){
            bool tupleUNegated = false;
            const Tuple * tupleU = NULL;
            if(starter.getPredicateName()== &_assign){
                std::pair<AuxMap*,AuxMap*> joinTuples_0_1;
                joinTuples_0_1.first = &p_assign_USER_ID_X_Y_Z_W_0_;
                joinTuples_0_1.second = &u_assign_USER_ID_X_Y_Z_W_0_;
                int count = joinTuples_0_1.first->size();
                std::set<std::vector<int>> alreadyCounted;
                for(const Tuple * joinTupleUndef : u_assign_USER_ID_X_Y_Z_W_.getValues({})){
                    if(joinTuples_0_1.first->getValues({joinTupleUndef->at(0)}).size()==0){
                        if(joinTuples_0_1.second->getValues({joinTupleUndef->at(0)}).size()>0){
                            std::vector<int> key({joinTupleUndef->at(0)});
                            alreadyCounted.insert(key);
                        }
                    }
                }
                count+=alreadyCounted.size();
                {
                    const std::vector<const Tuple* >* tuples;
                    tuples = &pfraction_user_.getValues({});
                    const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                    if(tupleU == NULL){
                        tuplesU = &ufraction_user_.getValues({});
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
                        int F = (*tuple1)[0];
                        if(!(count >=F)){
                            std::vector<int> reason;
                            const std::vector<const Tuple*>* falseTuples0 = &fassign_.getValues({});
                            for(int i=0;i<falseTuples0->size();i++){
                                bool joiningTupleFound=true;
                                int USER_ID = falseTuples0->at(i)->at(0);
                                int X = falseTuples0->at(i)->at(1);
                                int Y = falseTuples0->at(i)->at(2);
                                int Z = falseTuples0->at(i)->at(3);
                                int W = falseTuples0->at(i)->at(4);
                                if(joiningTupleFound){
                                    const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                    if(it!=tupleToVar.end()){
                                        reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                    }//closing if
                                }
                            }
                            if(tuple1!=tupleU){
                                const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                                if(it_reason1!=tupleToVar.end())
                                    reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                            }
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
                    tuples = &pfraction_user_.getValues({});
                    const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                    if(tupleU == NULL){
                        tuplesU = &ufraction_user_.getValues({});
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
                        int F = (*tuple1)[0];
                        if(tupleU ==  NULL && count == F) {
                            std::pair<AuxMap*,AuxMap*> joinTuples_0_1;
                            joinTuples_0_1.first = &p_assign_USER_ID_X_Y_Z_W_;
                            joinTuples_0_1.second = &u_assign_USER_ID_X_Y_Z_W_;
                            std::pair<AuxMap*,AuxMap*> joinTuples_0_1SharedVariables;
                            joinTuples_0_1SharedVariables.first = &p_assign_USER_ID_X_Y_Z_W_0_;
                            joinTuples_0_1SharedVariables.second = &u_assign_USER_ID_X_Y_Z_W_0_;
                            std::vector<int> reason;
                            const std::vector<const Tuple*>* falseTuples0 = &fassign_.getValues({});
                            for(int i=0;i<falseTuples0->size();i++){
                                bool joiningTupleFound=true;
                                int USER_ID = falseTuples0->at(i)->at(0);
                                int X = falseTuples0->at(i)->at(1);
                                int Y = falseTuples0->at(i)->at(2);
                                int Z = falseTuples0->at(i)->at(3);
                                int W = falseTuples0->at(i)->at(4);
                                if(joiningTupleFound){
                                    const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                    if(it!=tupleToVar.end()){
                                        reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                    }//closing if
                                }
                            }
                            const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                            if(it_reason1!=tupleToVar.end())
                                reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                            const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_1.second->getValues({});
                            for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                if(joinTuples_0_1SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0 && joinTuples_0_1SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 1){
                                    {
                                        const Tuple* aggrTupleU = uassign.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1),undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3),undefinedTuples->at(iUndef)->at(4)});
                                        if(aggrTupleU != NULL){
                                            const auto & it = tupleToVar.find(*aggrTupleU);
                                            if(it != tupleToVar.end()) {
                                                int sign = aggrTupleU->isNegated() ? 1:-1;
                                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }//close par
                }//close par
            }//close par
        }//close if
        if(starter.getPredicateName() == &_fraction_user) { 
            const Tuple * tuple0 = &starter;
            if(facts[i] > 0){
                {
                    const Tuple * tupleU = NULL;
                    bool tupleUNegated = false;
                    int F = (*tuple0)[0];
                    std::pair<AuxMap*,AuxMap*> joinTuples_0_1;
                    joinTuples_0_1.first = &p_assign_USER_ID_X_Y_Z_W_;
                    joinTuples_0_1.second = &u_assign_USER_ID_X_Y_Z_W_;
                    std::pair<AuxMap*,AuxMap*> joinTuples_0_1SharedVariables;
                    joinTuples_0_1SharedVariables.first = &p_assign_USER_ID_X_Y_Z_W_0_;
                    joinTuples_0_1SharedVariables.second = &u_assign_USER_ID_X_Y_Z_W_0_;
                    int actualSize1 = joinTuples_0_1SharedVariables.first->size();
                    std::set<std::vector<int>> alreadyCounted1;
                    for(const Tuple * joinTupleUndef : u_assign_USER_ID_X_Y_Z_W_.getValues({})){
                        if(joinTuples_0_1SharedVariables.first->getValues({joinTupleUndef->at(0)}).size()==0){
                            if(joinTuples_0_1SharedVariables.second->getValues({joinTupleUndef->at(0)}).size()>0){
                                std::vector<int> key({joinTupleUndef->at(0)});
                                alreadyCounted1.insert(key);
                            }
                        }
                    }
                    actualSize1+=alreadyCounted1.size();
                    if(tupleU==NULL){
                        std::vector<int> reason;
                        const std::vector<const Tuple*>* falseTuples0 = &fassign_.getValues({});
                        for(int i=0;i<falseTuples0->size();i++){
                            bool joiningTupleFound=true;
                            int USER_ID = falseTuples0->at(i)->at(0);
                            int X = falseTuples0->at(i)->at(1);
                            int Y = falseTuples0->at(i)->at(2);
                            int Z = falseTuples0->at(i)->at(3);
                            int W = falseTuples0->at(i)->at(4);
                            if(joiningTupleFound){
                                const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                if(it!=tupleToVar.end()){
                                    reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                }//closing if
                            }
                        }
                        if(tuple0!=tupleU){
                            const auto & it_reason0 = tupleToVar.find(Tuple(*tuple0));
                            if(it_reason0!=tupleToVar.end())
                                reason.push_back(it_reason0->second * (tuple0->isNegated() ? -1:1));
                        }
                        if(!(actualSize1>=F)){
                            std::cout<<"conflict detected in propagator"<<std::endl;
                            propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                        }else{
                            if(actualSize1 == F){
                                const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_1.second->getValues({});
                                for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                    if(joinTuples_0_1SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0 && joinTuples_0_1SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 1){
                                        {
                                            const Tuple* aggrTupleU = uassign.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1),undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3),undefinedTuples->at(iUndef)->at(4)});
                                            if(aggrTupleU != NULL){
                                                const auto & it = tupleToVar.find(*aggrTupleU);
                                                if(it != tupleToVar.end()) {
                                                    int sign = aggrTupleU->isNegated() ? 1:-1;
                                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }else{
                    std::vector<int> reason;
                    const std::vector<const Tuple*>* falseTuples0 = &fassign_.getValues({});
                    for(int i=0;i<falseTuples0->size();i++){
                        bool joiningTupleFound=true;
                        int USER_ID = falseTuples0->at(i)->at(0);
                        int X = falseTuples0->at(i)->at(1);
                        int Y = falseTuples0->at(i)->at(2);
                        int Z = falseTuples0->at(i)->at(3);
                        int W = falseTuples0->at(i)->at(4);
                        if(joiningTupleFound){
                            const auto & it = tupleToVar.find(*falseTuples0->at(i));
                            if(it!=tupleToVar.end()){
                                reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                            }//closing if
                        }
                    }
                    if(tuple0!=tupleU){
                        const auto & it_reason0 = tupleToVar.find(Tuple(*tuple0));
                        if(it_reason0!=tupleToVar.end())
                            reason.push_back(it_reason0->second * (tuple0->isNegated() ? -1:1));
                    }
                        if(!(actualSize1>=F)){
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
