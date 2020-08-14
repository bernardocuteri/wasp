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
const std::string _c = "c";
PredicateWSet wc(2);
PredicateWSet uc(2);
PredicateWSet fc(2);
const std::string _d = "d";
PredicateWSet wd(2);
PredicateWSet ud(2);
PredicateWSet fd(2);
const std::string _e = "e";
PredicateWSet we(3);
PredicateWSet ue(3);
PredicateWSet fe(3);
const std::string _f = "f";
PredicateWSet wf(2);
PredicateWSet uf(2);
PredicateWSet ff(2);
const std::string _e_X_A_Y_f_B_Y_ = "e_X_A_Y_f_B_Y_";
PredicateWSet we_X_A_Y_f_B_Y_(5);
PredicateWSet ue_X_A_Y_f_B_Y_(5);
std::map<std::vector<int>, std::pair< AuxMap*,AuxMap*>*> sharedVariables_0_ToAggregate_2;
Executor::~Executor() {
    for(auto sharedVar : sharedVariables_0_ToAggregate_2){
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
AuxMap pe_0_1_2_({0,1,2});
AuxMap ue_0_1_2_({0,1,2});
AuxMap fe_0_1_2_({0,1,2});
AuxMap pe_({});
AuxMap ue_({});
AuxMap fe_({});
AuxMap pf_1_({1});
AuxMap uf_1_({1});
AuxMap ff_1_({1});
AuxMap pf_0_1_({0,1});
AuxMap uf_0_1_({0,1});
AuxMap ff_0_1_({0,1});
AuxMap pf_({});
AuxMap uf_({});
AuxMap ff_({});
AuxMap pe_2_({2});
AuxMap ue_2_({2});
AuxMap fe_2_({2});
AuxMap p_e_X_A_Y_f_B_Y_1_3_({1,3});
AuxMap u_e_X_A_Y_f_B_Y_1_3_({1,3});
AuxMap p_e_X_A_Y_f_B_Y_({});
AuxMap u_e_X_A_Y_f_B_Y_({});
AuxMap p_e_X_A_Y_f_B_Y_0_2_4_({0,2,4});
AuxMap u_e_X_A_Y_f_B_Y_0_2_4_({0,2,4});
AuxMap p_e_X_A_Y_f_B_Y_0_1_2_({0,1,2});
AuxMap u_e_X_A_Y_f_B_Y_0_1_2_({0,1,2});
AuxMap p_e_X_A_Y_f_B_Y_3_4_({3,4});
AuxMap u_e_X_A_Y_f_B_Y_3_4_({3,4});
AuxMap pc_0_1_({0,1});
AuxMap uc_0_1_({0,1});
AuxMap pd_0_({0});
AuxMap ud_0_({0});
AuxMap pc_({});
AuxMap uc_({});
AuxMap pc_1_({1});
AuxMap uc_1_({1});
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
    if(var<0 && ( tuple.getPredicateName() == &_e || tuple.getPredicateName() == &_f)){
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
    if(tuple.getPredicateName() == &_e){
        if(var > 0){
            int X = tuple.at(0);
            int A = tuple.at(1);
            int Y = tuple.at(2);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuples1 = pf_1_.getValues({Y});
            const std::vector<const Tuple*>& tuplesU1 = uf_1_.getValues({Y});
            for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
                bool buildUndef=false;
                const Tuple* tuple1;
                if(i<tuples1.size())
                    tuple1=tuples1[i];
                else{
                    buildUndef=true;
                    tuple1=tuplesU1[i-tuples1.size()];
                }
                int B = tuple1->at(0);
                Tuple t(std::vector<int>({X,A,Y,B,Y}),&_e_X_A_Y_f_B_Y_);
                if(!buildUndef){
                    if(we_X_A_Y_f_B_Y_.find(t)==NULL){
                        const auto& insertResult = we_X_A_Y_f_B_Y_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_e_X_A_Y_f_B_Y_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({X,Y,Y}));
                                if(sharedVariables_0_ToAggregate_2.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_0_ToAggregate_2[sharedTuple];
                                    joinTuples->first->insert2(*insertResult.first);
                                }
                            }
                            ue_X_A_Y_f_B_Y_.erase(t);
                        }
                    }else{
                    }
                }else{
                    if(ue_X_A_Y_f_B_Y_.find(t)==NULL){
                        const auto& insertResult = ue_X_A_Y_f_B_Y_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_e_X_A_Y_f_B_Y_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({X,Y,Y}));
                                if(sharedVariables_0_ToAggregate_2.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_0_ToAggregate_2[sharedTuple];
                                    joinTuples->second->insert2(*insertResult.first);
                                }
                            }
                        }
                    }
                }
            }
        }else{
            int X = tuple.at(0);
            int A = tuple.at(1);
            int Y = tuple.at(2);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuplesU1 = uf_1_.getValues({Y});
            const std::vector<const Tuple*>& tuples1 = pf_1_.getValues({Y});
            for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
                bool buildUndef=false;
                const Tuple* tuple1;
                if(i<tuples1.size())
                    tuple1=tuples1[i];
                else{
                    buildUndef=true;
                    tuple1=tuplesU1[i-tuples1.size()];
                }
                int B = tuple1->at(0);
                Tuple t(std::vector<int>({X,A,Y,B,Y}),&_e_X_A_Y_f_B_Y_);
                ue_X_A_Y_f_B_Y_.erase(t);
                we_X_A_Y_f_B_Y_.erase(t);
            }
        }
    }
    if(tuple.getPredicateName() == &_f){
        if(var > 0){
            int B = tuple.at(0);
            int Y = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuples0 = pe_2_.getValues({Y});
            const std::vector<const Tuple*>& tuplesU0 = ue_2_.getValues({Y});
            for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
                bool buildUndef=false;
                const Tuple* tuple0;
                if(i<tuples0.size())
                    tuple0=tuples0[i];
                else{
                    buildUndef=true;
                    tuple0=tuplesU0[i-tuples0.size()];
                }
                int X = tuple0->at(0);
                int A = tuple0->at(1);
                Tuple t(std::vector<int>({X,A,Y,B,Y}),&_e_X_A_Y_f_B_Y_);
                if(!buildUndef){
                    if(we_X_A_Y_f_B_Y_.find(t)==NULL){
                        const auto& insertResult = we_X_A_Y_f_B_Y_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_e_X_A_Y_f_B_Y_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({X,Y,Y}));
                                if(sharedVariables_0_ToAggregate_2.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_0_ToAggregate_2[sharedTuple];
                                    joinTuples->first->insert2(*insertResult.first);
                                }
                            }
                            ue_X_A_Y_f_B_Y_.erase(t);
                        }
                    }else{
                    }
                }else{
                    if(ue_X_A_Y_f_B_Y_.find(t)==NULL){
                        const auto& insertResult = ue_X_A_Y_f_B_Y_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_e_X_A_Y_f_B_Y_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({X,Y,Y}));
                                if(sharedVariables_0_ToAggregate_2.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_0_ToAggregate_2[sharedTuple];
                                    joinTuples->second->insert2(*insertResult.first);
                                }
                            }
                        }
                    }
                }
            }
        }else{
            int B = tuple.at(0);
            int Y = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuplesU0 = ue_2_.getValues({Y});
            const std::vector<const Tuple*>& tuples0 = pe_2_.getValues({Y});
            for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
                bool buildUndef=false;
                const Tuple* tuple0;
                if(i<tuples0.size())
                    tuple0=tuples0[i];
                else{
                    buildUndef=true;
                    tuple0=tuplesU0[i-tuples0.size()];
                }
                int X = tuple0->at(0);
                int A = tuple0->at(1);
                Tuple t(std::vector<int>({X,A,Y,B,Y}),&_e_X_A_Y_f_B_Y_);
                ue_X_A_Y_f_B_Y_.erase(t);
                we_X_A_Y_f_B_Y_.erase(t);
            }
        }
    }
    if(tuple.getPredicateName() == &_c){
        int X = tuple.at(0);
        int Y = tuple.at(1);
        const std::vector<const Tuple *>& tuples1 = pd_0_.getValues({Y});
        const std::vector<const Tuple *>& tuplesU1 = ud_0_.getValues({Y});
        const Tuple * tuple1;
        for(int i=0; i< tuples1.size() + tuplesU1.size();i++){
            if(i<tuples1.size())
                tuple1= tuples1[i];
            else tuple1 = tuplesU1[i-tuples1.size()];
            int Z = tuple1->at(1);
            if(var>0){
                Tuple sharedTuple(std::vector<int>({X,Y,Y}));
                if(!sharedVariables_0_ToAggregate_2.count(sharedTuple)){
                    sharedVariables_0_ToAggregate_2[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1,3})),new AuxMap(std::vector<unsigned>({1,3})));
                    const std::vector<const Tuple*>* trueJoinTuples = &p_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                    const std::vector<const Tuple*>* undefJoinTuples = &u_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                    for(int i=0;i<trueJoinTuples->size();i++){
                        const auto findResult = we_X_A_Y_f_B_Y_.find(Tuple(*trueJoinTuples->at(i)));
                        if(findResult!=NULL){
                            sharedVariables_0_ToAggregate_2[sharedTuple]->first->insert2(*findResult);
                        }
                    }
                    for(int i=0;i<undefJoinTuples->size();i++){
                        const auto findResult = ue_X_A_Y_f_B_Y_.find(Tuple(*undefJoinTuples->at(i)));
                        if(findResult!=NULL){
                            sharedVariables_0_ToAggregate_2[sharedTuple]->second->insert2(*findResult);
                        }
                    }
                }
            }else{
            }
        }
    }
    if(tuple.getPredicateName() == &_d){
        int Y = tuple.at(0);
        int Z = tuple.at(1);
        const std::vector<const Tuple *>& tuples0 = pc_1_.getValues({Y});
        const std::vector<const Tuple *>& tuplesU0 = uc_1_.getValues({Y});
        const Tuple * tuple0;
        for(int i=0; i< tuples0.size() + tuplesU0.size();i++){
            if(i<tuples0.size())
                tuple0= tuples0[i];
            else tuple0 = tuplesU0[i-tuples0.size()];
            int X = tuple0->at(0);
            if(var>0){
                Tuple sharedTuple(std::vector<int>({X,Y,Y}));
                if(!sharedVariables_0_ToAggregate_2.count(sharedTuple)){
                    sharedVariables_0_ToAggregate_2[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1,3})),new AuxMap(std::vector<unsigned>({1,3})));
                    const std::vector<const Tuple*>* trueJoinTuples = &p_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                    const std::vector<const Tuple*>* undefJoinTuples = &u_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                    for(int i=0;i<trueJoinTuples->size();i++){
                        const auto findResult = we_X_A_Y_f_B_Y_.find(Tuple(*trueJoinTuples->at(i)));
                        if(findResult!=NULL){
                            sharedVariables_0_ToAggregate_2[sharedTuple]->first->insert2(*findResult);
                        }
                    }
                    for(int i=0;i<undefJoinTuples->size();i++){
                        const auto findResult = ue_X_A_Y_f_B_Y_.find(Tuple(*undefJoinTuples->at(i)));
                        if(findResult!=NULL){
                            sharedVariables_0_ToAggregate_2[sharedTuple]->second->insert2(*findResult);
                        }
                    }
                }
            }else{
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
    if(var<0 && ( tuple.getPredicateName() == &_e || tuple.getPredicateName() == &_f)){
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
    if(tuple.getPredicateName() == &_e){
        if(var > 0){
            const std::vector<const Tuple* >& trueJoinTuples = p_e_X_A_Y_f_B_Y_0_1_2_.getValues({tuple[0],tuple[1],tuple[2]});
            for(const Tuple* t :trueJoinTuples){
                const auto insertResult = ue_X_A_Y_f_B_Y_.insert(Tuple(*t));
                if(insertResult.second){
                    for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_e_X_A_Y_f_B_Y_]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                    int X = t->at(0);
                    int Y = t->at(2);
                    {
                        Tuple sharedTuple(std::vector<int>({X,Y,Y}));
                        if(sharedVariables_0_ToAggregate_2.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_0_ToAggregate_2[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                }
                we_X_A_Y_f_B_Y_.erase(*t);
            }
        }
        int X = tuple.at(0);
        int A = tuple.at(1);
        int Y = tuple.at(2);
        bool buildUndef=false;
        bool buildFalse=false;
        const std::vector<const Tuple*>& tuplesU1 = pf_1_.getValues({Y});
        const std::vector<const Tuple*>& tuples1 = uf_1_.getValues({Y});
        for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
            const Tuple* tuple1;
            if(i<tuples1.size())
                tuple1=tuples1[i];
            else{
                tuple1=tuplesU1[i-tuples1.size()];
            }
            int B = tuple1->at(0);
            Tuple tupleUndef(std::vector<int>({X,A,Y,B,Y}),&_e_X_A_Y_f_B_Y_);
            const auto& insertResult = ue_X_A_Y_f_B_Y_.insert(Tuple(tupleUndef));
            if (insertResult.second) {
                for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_e_X_A_Y_f_B_Y_]){
                    auxMap -> insert2(*insertResult.first);
                }
                {
                    Tuple sharedTuple(std::vector<int>({X,Y,Y}));
                    if(sharedVariables_0_ToAggregate_2.count(sharedTuple)!=0){
                        auto joinTuples = sharedVariables_0_ToAggregate_2[sharedTuple];
                        joinTuples->second->insert2(*insertResult.first);
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_f){
        if(var > 0){
            const std::vector<const Tuple* >& trueJoinTuples = p_e_X_A_Y_f_B_Y_3_4_.getValues({tuple[0],tuple[1]});
            for(const Tuple* t :trueJoinTuples){
                const auto insertResult = ue_X_A_Y_f_B_Y_.insert(Tuple(*t));
                if(insertResult.second){
                    for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_e_X_A_Y_f_B_Y_]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                    int X = t->at(0);
                    int Y = t->at(2);
                    {
                        Tuple sharedTuple(std::vector<int>({X,Y,Y}));
                        if(sharedVariables_0_ToAggregate_2.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_0_ToAggregate_2[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                }
                we_X_A_Y_f_B_Y_.erase(*t);
            }
        }
        int B = tuple.at(0);
        int Y = tuple.at(1);
        bool buildUndef=false;
        bool buildFalse=false;
        const std::vector<const Tuple*>& tuplesU0 = pe_2_.getValues({Y});
        const std::vector<const Tuple*>& tuples0 = ue_2_.getValues({Y});
        for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
            const Tuple* tuple0;
            if(i<tuples0.size())
                tuple0=tuples0[i];
            else{
                tuple0=tuplesU0[i-tuples0.size()];
            }
            int X = tuple0->at(0);
            int A = tuple0->at(1);
            Tuple tupleUndef(std::vector<int>({X,A,Y,B,Y}),&_e_X_A_Y_f_B_Y_);
            const auto& insertResult = ue_X_A_Y_f_B_Y_.insert(Tuple(tupleUndef));
            if (insertResult.second) {
                for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_e_X_A_Y_f_B_Y_]){
                    auxMap -> insert2(*insertResult.first);
                }
                {
                    Tuple sharedTuple(std::vector<int>({X,Y,Y}));
                    if(sharedVariables_0_ToAggregate_2.count(sharedTuple)!=0){
                        auto joinTuples = sharedVariables_0_ToAggregate_2[sharedTuple];
                        joinTuples->second->insert2(*insertResult.first);
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_c){
        int X = tuple.at(0);
        int Y = tuple.at(1);
        const std::vector<const Tuple *>& undefTuples1 = ud_0_.getValues({Y});
        const std::vector<const Tuple*>& trueTuples1 = pd_0_.getValues({Y});
        for(int i=0;i<undefTuples1.size()+trueTuples1.size();i++){
            const Tuple * tuple1;
            if(i<undefTuples1.size())
                tuple1 = undefTuples1[i];
            else tuple1 = trueTuples1[i-undefTuples1.size()];
            int Z = tuple1->at(1);
            Tuple sharedTuple(std::vector<int>({X,Y,Y}));
            if(!sharedVariables_0_ToAggregate_2.count(sharedTuple)){
                sharedVariables_0_ToAggregate_2[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1,3})),new AuxMap(std::vector<unsigned>({1,3})));
                const std::vector<const Tuple*>* undefJoinTuples = &u_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                const std::vector<const Tuple*>* trueJoinTuples = &p_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                for(int i=0;i<undefJoinTuples->size();i++){
                    const auto findResult = ue_X_A_Y_f_B_Y_.find(Tuple(*undefJoinTuples->at(i)));
                    if(findResult!=NULL){
                        sharedVariables_0_ToAggregate_2[sharedTuple]->second->insert2(*findResult);
                    }
                }
                for(int i=0;i<trueJoinTuples->size();i++){
                    const auto findResult = we_X_A_Y_f_B_Y_.find(Tuple(*trueJoinTuples->at(i)));
                    if(findResult!=NULL){
                        sharedVariables_0_ToAggregate_2[sharedTuple]->first->insert2(*findResult);
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_d){
        int Y = tuple.at(0);
        int Z = tuple.at(1);
        const std::vector<const Tuple *>& undefTuples0 = uc_1_.getValues({Y});
        const std::vector<const Tuple*>& trueTuples0 = pc_1_.getValues({Y});
        for(int i=0;i<undefTuples0.size()+trueTuples0.size();i++){
            const Tuple * tuple0;
            if(i<undefTuples0.size())
                tuple0 = undefTuples0[i];
            else tuple0 = trueTuples0[i-undefTuples0.size()];
            int X = tuple0->at(0);
            Tuple sharedTuple(std::vector<int>({X,Y,Y}));
            if(!sharedVariables_0_ToAggregate_2.count(sharedTuple)){
                sharedVariables_0_ToAggregate_2[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1,3})),new AuxMap(std::vector<unsigned>({1,3})));
                const std::vector<const Tuple*>* undefJoinTuples = &u_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                const std::vector<const Tuple*>* trueJoinTuples = &p_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                for(int i=0;i<undefJoinTuples->size();i++){
                    const auto findResult = ue_X_A_Y_f_B_Y_.find(Tuple(*undefJoinTuples->at(i)));
                    if(findResult!=NULL){
                        sharedVariables_0_ToAggregate_2[sharedTuple]->second->insert2(*findResult);
                    }
                }
                for(int i=0;i<trueJoinTuples->size();i++){
                    const auto findResult = we_X_A_Y_f_B_Y_.find(Tuple(*trueJoinTuples->at(i)));
                    if(findResult!=NULL){
                        sharedVariables_0_ToAggregate_2[sharedTuple]->first->insert2(*findResult);
                    }
                }
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
    predicateWSetMap[&_c]=&wc;
    predicateUSetMap[&_c]=&uc;
    stringToUniqueStringPointer["c"] = &_c;
    predicateWSetMap[&_d]=&wd;
    predicateUSetMap[&_d]=&ud;
    stringToUniqueStringPointer["d"] = &_d;
    aggregatePredicatesAndVars [&_e]= std::vector<int>({1});
    predicateWSetMap[&_e]=&we;
    predicateFSetMap[&_e]=&fe;
    predicateUSetMap[&_e]=&ue;
    stringToUniqueStringPointer["e"] = &_e;
    aggregatePredicatesAndVars [&_f]= std::vector<int>({0});
    predicateWSetMap[&_f]=&wf;
    predicateFSetMap[&_f]=&ff;
    predicateUSetMap[&_f]=&uf;
    stringToUniqueStringPointer["f"] = &_f;
    predicateToAuxiliaryMaps[&_d].push_back(&pd_0_);
    predicateToAuxiliaryMaps[&_c].push_back(&pc_);
    predicateToAuxiliaryMaps[&_c].push_back(&pc_0_1_);
    predicateToAuxiliaryMaps[&_c].push_back(&pc_1_);
    predicateToAuxiliaryMaps[&_e_X_A_Y_f_B_Y_].push_back(&p_e_X_A_Y_f_B_Y_);
    predicateToAuxiliaryMaps[&_e_X_A_Y_f_B_Y_].push_back(&p_e_X_A_Y_f_B_Y_0_1_2_);
    predicateToAuxiliaryMaps[&_e_X_A_Y_f_B_Y_].push_back(&p_e_X_A_Y_f_B_Y_0_2_4_);
    predicateToAuxiliaryMaps[&_e_X_A_Y_f_B_Y_].push_back(&p_e_X_A_Y_f_B_Y_1_3_);
    predicateToAuxiliaryMaps[&_e_X_A_Y_f_B_Y_].push_back(&p_e_X_A_Y_f_B_Y_3_4_);
    predicateToAuxiliaryMaps[&_e].push_back(&pe_);
    predicateToAuxiliaryMaps[&_e].push_back(&pe_0_1_2_);
    predicateToAuxiliaryMaps[&_e].push_back(&pe_2_);
    predicateToAuxiliaryMaps[&_f].push_back(&pf_);
    predicateToAuxiliaryMaps[&_f].push_back(&pf_0_1_);
    predicateToAuxiliaryMaps[&_f].push_back(&pf_1_);
    predicateToUndefAuxiliaryMaps[&_d].push_back(&ud_0_);
    predicateToUndefAuxiliaryMaps[&_c].push_back(&uc_);
    predicateToUndefAuxiliaryMaps[&_c].push_back(&uc_0_1_);
    predicateToUndefAuxiliaryMaps[&_c].push_back(&uc_1_);
    predicateToUndefAuxiliaryMaps[&_e_X_A_Y_f_B_Y_].push_back(&u_e_X_A_Y_f_B_Y_);
    predicateToUndefAuxiliaryMaps[&_e_X_A_Y_f_B_Y_].push_back(&u_e_X_A_Y_f_B_Y_0_1_2_);
    predicateToUndefAuxiliaryMaps[&_e_X_A_Y_f_B_Y_].push_back(&u_e_X_A_Y_f_B_Y_0_2_4_);
    predicateToUndefAuxiliaryMaps[&_e_X_A_Y_f_B_Y_].push_back(&u_e_X_A_Y_f_B_Y_1_3_);
    predicateToUndefAuxiliaryMaps[&_e_X_A_Y_f_B_Y_].push_back(&u_e_X_A_Y_f_B_Y_3_4_);
    predicateToUndefAuxiliaryMaps[&_e].push_back(&ue_);
    predicateToUndefAuxiliaryMaps[&_e].push_back(&ue_0_1_2_);
    predicateToUndefAuxiliaryMaps[&_e].push_back(&ue_2_);
    predicateToUndefAuxiliaryMaps[&_f].push_back(&uf_);
    predicateToUndefAuxiliaryMaps[&_f].push_back(&uf_0_1_);
    predicateToUndefAuxiliaryMaps[&_f].push_back(&uf_1_);
    predicateToFalseAuxiliaryMaps[&_f].push_back(&ff_);
    predicateToFalseAuxiliaryMaps[&_f].push_back(&ff_0_1_);
    predicateToFalseAuxiliaryMaps[&_f].push_back(&ff_1_);
    predicateToFalseAuxiliaryMaps[&_e].push_back(&fe_);
    predicateToFalseAuxiliaryMaps[&_e].push_back(&fe_0_1_2_);
    predicateToFalseAuxiliaryMaps[&_e].push_back(&fe_2_);
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
            tuples = &pc_.getValues({});
            const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
            if(tupleU == NULL){
                tuplesU = &uc_.getValues({});
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
                int X = (*tuple0)[0];
                int Y = (*tuple0)[1];
                const std::vector<const Tuple* >* tuples;
                tuples = &pd_0_.getValues({Y});
                const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                if(tupleU == NULL){
                    tuplesU = &ud_0_.getValues({Y});
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
                    int Z = (*tuple1)[1];
                    std::pair<AuxMap*,AuxMap*> joinTuples_0_2;
                    joinTuples_0_2.first = &p_e_X_A_Y_f_B_Y_;
                    joinTuples_0_2.second = &u_e_X_A_Y_f_B_Y_;
                    std::pair<AuxMap*,AuxMap*> joinTuples_0_2SharedVariables = *sharedVariables_0_ToAggregate_2[std::vector<int>({X,Y,Y})];
                    if(tupleU==NULL){
                        if(joinTuples_0_2SharedVariables.first->size()>=3){
                            std::cout<<"conflict detected in propagator"<<std::endl;
                            propagatedLiteralsAndReasons.insert({-1, std::vector<int>()});
                        }else{
                            if(joinTuples_0_2SharedVariables.first->size() == 3-1){
                                const std::vector<const Tuple*>* undefinedTuples = &u_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                                for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                    if(joinTuples_0_2SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1),undefinedTuples->at(iUndef)->at(3)}).size() == 0){
                                        const Tuple* aggrTupleU=NULL;
                                        const std::vector<const Tuple*>* tuplese;
                                        const std::vector<const Tuple*>* tuplesUe=&EMPTY_TUPLES;
                                        tuplese= &pe_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1),undefinedTuples->at(iUndef)->at(2)});
                                        if(aggrTupleU==NULL)
                                            tuplesUe= &ue_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1),undefinedTuples->at(iUndef)->at(2)});
                                        for(int j3=0;j3<tuplese->size()+tuplesUe->size();j3++){
                                            const Tuple* auxTuple3=NULL;
                                            bool addedToReason=false;
                                            if(j3 < tuplese->size()){
                                                auxTuple3 = tuplese->at(j3);
                                                if(tuplesUe != &EMPTY_TUPLES) {
                                                }
                                            }else{
                                                auxTuple3 = tuplesUe->at(j3-tuplese->size());
                                                aggrTupleU = auxTuple3;
                                            }
                                            const std::vector<const Tuple*>* tuplesf;
                                            const std::vector<const Tuple*>* tuplesUf=&EMPTY_TUPLES;
                                            tuplesf= &pf_0_1_.getValues({undefinedTuples->at(iUndef)->at(3),undefinedTuples->at(iUndef)->at(4)});
                                            if(aggrTupleU==NULL)
                                                tuplesUf= &uf_0_1_.getValues({undefinedTuples->at(iUndef)->at(3),undefinedTuples->at(iUndef)->at(4)});
                                            for(int j5=0;j5<tuplesf->size()+tuplesUf->size();j5++){
                                                const Tuple* auxTuple5=NULL;
                                                bool addedToReason=false;
                                                if(j5 < tuplesf->size()){
                                                    auxTuple5 = tuplesf->at(j5);
                                                    if(tuplesUf != &EMPTY_TUPLES) {
                                                    }
                                                }else{
                                                    auxTuple5 = tuplesUf->at(j5-tuplesf->size());
                                                    aggrTupleU = auxTuple5;
                                                }
                                                if(aggrTupleU == NULL){
                                                    std::cout<<"Tuple undefined not well formed"<<std::endl;
                                                    propagatedLiteralsAndReasons.insert({-1, std::vector<int>()});
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
                        }
                    }else{
                        if(joinTuples_0_2SharedVariables.first->size()>=3){
                            const auto & it = tupleToVar.find(*tupleU);
                            if(it != tupleToVar.end()) {
                                int sign = tupleU->isNegated() ? -1 : 1;
                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;
                            }
                        }
                    }
                }//close par
            }//close par
        }//close local scope
    }//close decision level == -1
    for(unsigned i=1;i<facts.size();i++) {
        unsigned factVar = facts[i] > 0 ? facts[i] : -facts[i];
        Tuple starter = atomsTable[factVar];
        starter.setNegated(facts[i]<0);
        if(starter.getPredicateName() == &_d) { 
            const Tuple * tuple0 = &starter;
            if(facts[i] > 0){
                {
                    const Tuple * tupleU = NULL;
                    bool tupleUNegated = false;
                    int Y = (*tuple0)[0];
                    int Z = (*tuple0)[1];
                    const std::vector<const Tuple* >* tuples;
                    tuples = &pc_1_.getValues({Y});
                    const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                    if(tupleU == NULL){
                        tuplesU = &uc_1_.getValues({Y});
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
                        int X = (*tuple1)[0];
                        std::pair<AuxMap*,AuxMap*> joinTuples_0_2;
                        joinTuples_0_2.first = &p_e_X_A_Y_f_B_Y_;
                        joinTuples_0_2.second = &u_e_X_A_Y_f_B_Y_;
                        std::pair<AuxMap*,AuxMap*> joinTuples_0_2SharedVariables = *sharedVariables_0_ToAggregate_2[std::vector<int>({X,Y,Y})];
                        if(tupleU==NULL){
                            const std::vector<const Tuple*>* trueTuples = &p_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                            std::vector<int> reason;
                            for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                Tuple tuplee_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1),trueTuples->at(reasonIndex)->at(2)}),&_e);
                                const auto & ite_0 = tupleToVar.find(tuplee_0);
                                if(ite_0!=tupleToVar.end()){
                                    reason.push_back(ite_0->second * (tuplee_0.isNegated() ? -1:1));
                                }//closing if
                                Tuple tuplef_1(std::vector<int>({trueTuples->at(reasonIndex)->at(3),trueTuples->at(reasonIndex)->at(4)}),&_f);
                                const auto & itf_1 = tupleToVar.find(tuplef_1);
                                if(itf_1!=tupleToVar.end()){
                                    reason.push_back(itf_1->second * (tuplef_1.isNegated() ? -1:1));
                                }//closing if
                            }
                            if(tuple0!=tupleU){
                                const auto & it_reason0 = tupleToVar.find(Tuple(*tuple0));
                                if(it_reason0!=tupleToVar.end())
                                    reason.push_back(it_reason0->second * (tuple0->isNegated() ? -1:1));
                            }
                            if(tuple1!=tupleU){
                                const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                                if(it_reason1!=tupleToVar.end())
                                    reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                            }
                            if(joinTuples_0_2SharedVariables.first->size()>=3){
                                std::cout<<"conflict detected in propagator"<<std::endl;
                                propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                            }else{
                                if(joinTuples_0_2SharedVariables.first->size() == 3-1){
                                    const std::vector<const Tuple*>* undefinedTuples = &u_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                                    for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                        if(joinTuples_0_2SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1),undefinedTuples->at(iUndef)->at(3)}).size() == 0){
                                            const Tuple* aggrTupleU=NULL;
                                            const std::vector<const Tuple*>* tuplese;
                                            const std::vector<const Tuple*>* tuplesUe=&EMPTY_TUPLES;
                                            tuplese= &pe_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1),undefinedTuples->at(iUndef)->at(2)});
                                            if(aggrTupleU==NULL)
                                                tuplesUe= &ue_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1),undefinedTuples->at(iUndef)->at(2)});
                                            for(int j3=0;j3<tuplese->size()+tuplesUe->size();j3++){
                                                const Tuple* auxTuple3=NULL;
                                                bool addedToReason=false;
                                                if(j3 < tuplese->size()){
                                                    auxTuple3 = tuplese->at(j3);
                                                    const auto & it = tupleToVar.find(*auxTuple3);
                                                    if(it!=tupleToVar.end()){
                                                        addedToReason=true;
                                                        reason.push_back(it->second * (auxTuple3->isNegated() ? -1:1));
                                                    }//closing if
                                                    if(tuplesUe != &EMPTY_TUPLES) {
                                                    }
                                                }else{
                                                    auxTuple3 = tuplesUe->at(j3-tuplese->size());
                                                    aggrTupleU = auxTuple3;
                                                }
                                                const std::vector<const Tuple*>* tuplesf;
                                                const std::vector<const Tuple*>* tuplesUf=&EMPTY_TUPLES;
                                                tuplesf= &pf_0_1_.getValues({undefinedTuples->at(iUndef)->at(3),undefinedTuples->at(iUndef)->at(4)});
                                                if(aggrTupleU==NULL)
                                                    tuplesUf= &uf_0_1_.getValues({undefinedTuples->at(iUndef)->at(3),undefinedTuples->at(iUndef)->at(4)});
                                                for(int j5=0;j5<tuplesf->size()+tuplesUf->size();j5++){
                                                    const Tuple* auxTuple5=NULL;
                                                    bool addedToReason=false;
                                                    if(j5 < tuplesf->size()){
                                                        auxTuple5 = tuplesf->at(j5);
                                                        const auto & it = tupleToVar.find(*auxTuple5);
                                                        if(it!=tupleToVar.end()){
                                                            addedToReason=true;
                                                            reason.push_back(it->second * (auxTuple5->isNegated() ? -1:1));
                                                        }//closing if
                                                        if(tuplesUf != &EMPTY_TUPLES) {
                                                        }
                                                    }else{
                                                        auxTuple5 = tuplesUf->at(j5-tuplesf->size());
                                                        aggrTupleU = auxTuple5;
                                                    }
                                                    if(aggrTupleU == NULL){
                                                        std::cout<<"Tuple undefined not well formed"<<std::endl;
                                                        propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                                    }else{
                                                        const auto & it = tupleToVar.find(*aggrTupleU);
                                                        if(it != tupleToVar.end()) {
                                                            int sign = aggrTupleU->isNegated() ? -1 : 1;
                                                            auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                        }
                                                    }
                                                    if(addedToReason) reason.pop_back();
                                                }
                                                if(addedToReason) reason.pop_back();
                                            }
                                        }
                                    }
                                }
                            }
                        }else{
                        const std::vector<const Tuple*>* trueTuples = &p_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                        std::vector<int> reason;
                        for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                            Tuple tuplee_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1),trueTuples->at(reasonIndex)->at(2)}),&_e);
                            const auto & ite_0 = tupleToVar.find(tuplee_0);
                            if(ite_0!=tupleToVar.end()){
                                reason.push_back(ite_0->second * (tuplee_0.isNegated() ? -1:1));
                            }//closing if
                            Tuple tuplef_1(std::vector<int>({trueTuples->at(reasonIndex)->at(3),trueTuples->at(reasonIndex)->at(4)}),&_f);
                            const auto & itf_1 = tupleToVar.find(tuplef_1);
                            if(itf_1!=tupleToVar.end()){
                                reason.push_back(itf_1->second * (tuplef_1.isNegated() ? -1:1));
                            }//closing if
                        }
                        if(tuple0!=tupleU){
                            const auto & it_reason0 = tupleToVar.find(Tuple(*tuple0));
                            if(it_reason0!=tupleToVar.end())
                                reason.push_back(it_reason0->second * (tuple0->isNegated() ? -1:1));
                        }
                        if(tuple1!=tupleU){
                            const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                            if(it_reason1!=tupleToVar.end())
                                reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                        }
                            if(joinTuples_0_2SharedVariables.first->size()>=3){
                                const auto & it = tupleToVar.find(*tupleU);
                                if(it != tupleToVar.end()) {
                                    int sign = tupleU->isNegated() ? -1 : 1;
                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                }
                            }
                        }
                    }//close par
                }//close loop nested join
            }//close loop nested join
        }//close predicate joins
        if(facts[i] > 0){
            bool tupleUNegated = false;
            const Tuple * tupleU = NULL;
            if(starter.getPredicateName()== &_e || starter.getPredicateName()== &_f){
                for(const auto sharedVarTuple : sharedVariables_0_ToAggregate_2){
                    int X = sharedVarTuple.first[0];
                    int Y = sharedVarTuple.first[1];
                    std::pair<AuxMap*,AuxMap*> joinTuples_0_2;
                    joinTuples_0_2.first=sharedVarTuple.second->first;
                    joinTuples_0_2.second=sharedVarTuple.second->second;
                    if(joinTuples_0_2.first->size()>=3){
                        const Tuple * tuple1 = (wc.find({X, Y}));
                        if(!tuple1 && !tupleU ){
                            tuple1 = tupleU = (uc.find({X, Y}));
                            tupleUNegated = false;
                        }
                        if(tuple1){
                            const std::vector<const Tuple* >* tuples;
                            tuples = &pd_0_.getValues({Y});
                            const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                            if(tupleU == NULL){
                                tuplesU = &ud_0_.getValues({Y});
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
                                int Z = (*tuple2)[1];
                                const std::vector<const Tuple*>* trueTuples = &p_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                                std::vector<int> reason;
                                for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                    Tuple tuplee_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1),trueTuples->at(reasonIndex)->at(2)}),&_e);
                                    const auto & ite_0 = tupleToVar.find(tuplee_0);
                                    if(ite_0!=tupleToVar.end()){
                                        reason.push_back(ite_0->second * (tuplee_0.isNegated() ? -1:1));
                                    }//closing if
                                    Tuple tuplef_1(std::vector<int>({trueTuples->at(reasonIndex)->at(3),trueTuples->at(reasonIndex)->at(4)}),&_f);
                                    const auto & itf_1 = tupleToVar.find(tuplef_1);
                                    if(itf_1!=tupleToVar.end()){
                                        reason.push_back(itf_1->second * (tuplef_1.isNegated() ? -1:1));
                                    }//closing if
                                }
                                if(tuple1!=tupleU){
                                    const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                                    if(it_reason1!=tupleToVar.end())
                                        reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                                }
                                if(tuple2!=tupleU){
                                    const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                    if(it_reason2!=tupleToVar.end())
                                        reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                }
                                const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                                if(it_reason_starter!=tupleToVar.end())
                                    reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                                if(tupleU == NULL) {
                                    std::cout<<"shared var: "<<X<<" "<<Y<<std::endl;
                                    std::cout<<"tuple1: ";tuple1->print();std::cout<<std::endl;
                                    std::cout<<"tuple2: ";tuple2->print();std::cout<<std::endl;
                                    std::cout<<"conflict detected in propagator External Propagation2"<<std::endl;
                                    propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                }
                                else {
                                    const auto & it = tupleToVar.find(*tupleU);
                                    if(it != tupleToVar.end()) {
                                        int sign = tupleU->isNegated() ? -1 : 1;
                                        propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                    }
                                }
                            }//close par
                        }//close par
                    }//close par
                    else{
                        if(joinTuples_0_2.first->size()==3-1){
                            const Tuple * tuple1 = (wc.find({X, Y}));
                            if(!tuple1 && !tupleU ){
                                tuple1 = tupleU = (uc.find({X, Y}));
                                tupleUNegated = false;
                            }
                            if(tuple1){
                                const std::vector<const Tuple* >* tuples;
                                tuples = &pd_0_.getValues({Y});
                                const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                                if(tupleU == NULL){
                                    tuplesU = &ud_0_.getValues({Y});
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
                                    int Z = (*tuple2)[1];
                                    if(tupleU ==  NULL) {
                                        std::pair<AuxMap*,AuxMap*> joinTuples_0_2;
                                        joinTuples_0_2.first = &p_e_X_A_Y_f_B_Y_;
                                        joinTuples_0_2.second = &u_e_X_A_Y_f_B_Y_;
                                        std::pair<AuxMap*,AuxMap*> joinTuples_0_2SharedVariables = *sharedVariables_0_ToAggregate_2[std::vector<int>({X,Y,Y})];
                                        const std::vector<const Tuple*>* trueTuples = &p_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                                        std::vector<int> reason;
                                        for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                            Tuple tuplee_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1),trueTuples->at(reasonIndex)->at(2)}),&_e);
                                            const auto & ite_0 = tupleToVar.find(tuplee_0);
                                            if(ite_0!=tupleToVar.end()){
                                                reason.push_back(ite_0->second * (tuplee_0.isNegated() ? -1:1));
                                            }//closing if
                                            Tuple tuplef_1(std::vector<int>({trueTuples->at(reasonIndex)->at(3),trueTuples->at(reasonIndex)->at(4)}),&_f);
                                            const auto & itf_1 = tupleToVar.find(tuplef_1);
                                            if(itf_1!=tupleToVar.end()){
                                                reason.push_back(itf_1->second * (tuplef_1.isNegated() ? -1:1));
                                            }//closing if
                                        }
                                        const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                                        if(it_reason1!=tupleToVar.end())
                                            reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                                        const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                        if(it_reason2!=tupleToVar.end())
                                            reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                        const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                                        if(it_reason_starter!=tupleToVar.end())
                                            reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                                        const std::vector<const Tuple*>* undefinedTuples = &u_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                                        for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                            if(joinTuples_0_2SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1),undefinedTuples->at(iUndef)->at(3)}).size() == 0){
                                                const Tuple* aggrTupleU=NULL;
                                                const std::vector<const Tuple*>* tuplese;
                                                const std::vector<const Tuple*>* tuplesUe=&EMPTY_TUPLES;
                                                tuplese= &pe_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1),undefinedTuples->at(iUndef)->at(2)});
                                                if(aggrTupleU==NULL)
                                                    tuplesUe= &ue_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1),undefinedTuples->at(iUndef)->at(2)});
                                                for(int j3=0;j3<tuplese->size()+tuplesUe->size();j3++){
                                                    const Tuple* auxTuple3=NULL;
                                                    bool addedToReason=false;
                                                    if(j3 < tuplese->size()){
                                                        auxTuple3 = tuplese->at(j3);
                                                        const auto & it = tupleToVar.find(*auxTuple3);
                                                        if(it!=tupleToVar.end()){
                                                            addedToReason=true;
                                                            reason.push_back(it->second * (auxTuple3->isNegated() ? -1:1));
                                                        }//closing if
                                                        if(tuplesUe != &EMPTY_TUPLES) {
                                                        }
                                                    }else{
                                                        auxTuple3 = tuplesUe->at(j3-tuplese->size());
                                                        aggrTupleU = auxTuple3;
                                                    }
                                                    const std::vector<const Tuple*>* tuplesf;
                                                    const std::vector<const Tuple*>* tuplesUf=&EMPTY_TUPLES;
                                                    tuplesf= &pf_0_1_.getValues({undefinedTuples->at(iUndef)->at(3),undefinedTuples->at(iUndef)->at(4)});
                                                    if(aggrTupleU==NULL)
                                                        tuplesUf= &uf_0_1_.getValues({undefinedTuples->at(iUndef)->at(3),undefinedTuples->at(iUndef)->at(4)});
                                                    for(int j5=0;j5<tuplesf->size()+tuplesUf->size();j5++){
                                                        const Tuple* auxTuple5=NULL;
                                                        bool addedToReason=false;
                                                        if(j5 < tuplesf->size()){
                                                            auxTuple5 = tuplesf->at(j5);
                                                            const auto & it = tupleToVar.find(*auxTuple5);
                                                            if(it!=tupleToVar.end()){
                                                                addedToReason=true;
                                                                reason.push_back(it->second * (auxTuple5->isNegated() ? -1:1));
                                                            }//closing if
                                                            if(tuplesUf != &EMPTY_TUPLES) {
                                                            }
                                                        }else{
                                                            auxTuple5 = tuplesUf->at(j5-tuplesf->size());
                                                            aggrTupleU = auxTuple5;
                                                        }
                                                        if(aggrTupleU == NULL){
                                                            std::cout<<"Tuple undefined not well formed"<<std::endl;
                                                            propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                                        }else{
                                                            const auto & it = tupleToVar.find(*aggrTupleU);
                                                            if(it != tupleToVar.end()) {
                                                                int sign = aggrTupleU->isNegated() ? -1 : 1;
                                                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                            }
                                                        }
                                                        if(addedToReason) reason.pop_back();
                                                    }
                                                    if(addedToReason) reason.pop_back();
                                                }
                                            }
                                        }
                                    }
                                }//close par
                            }//close par
                        }//close par
                    }//close par
                }//close par
            }//close par
        }//close if
        if(starter.getPredicateName() == &_c) { 
            const Tuple * tuple0 = &starter;
            if(facts[i] > 0){
                {
                    const Tuple * tupleU = NULL;
                    bool tupleUNegated = false;
                    int X = (*tuple0)[0];
                    int Y = (*tuple0)[1];
                    const std::vector<const Tuple* >* tuples;
                    tuples = &pd_0_.getValues({Y});
                    const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                    if(tupleU == NULL){
                        tuplesU = &ud_0_.getValues({Y});
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
                        int Z = (*tuple1)[1];
                        std::pair<AuxMap*,AuxMap*> joinTuples_0_2;
                        joinTuples_0_2.first = &p_e_X_A_Y_f_B_Y_;
                        joinTuples_0_2.second = &u_e_X_A_Y_f_B_Y_;
                        std::pair<AuxMap*,AuxMap*> joinTuples_0_2SharedVariables = *sharedVariables_0_ToAggregate_2[std::vector<int>({X,Y,Y})];
                        if(tupleU==NULL){
                            const std::vector<const Tuple*>* trueTuples = &p_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                            std::vector<int> reason;
                            for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                Tuple tuplee_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1),trueTuples->at(reasonIndex)->at(2)}),&_e);
                                const auto & ite_0 = tupleToVar.find(tuplee_0);
                                if(ite_0!=tupleToVar.end()){
                                    reason.push_back(ite_0->second * (tuplee_0.isNegated() ? -1:1));
                                }//closing if
                                Tuple tuplef_1(std::vector<int>({trueTuples->at(reasonIndex)->at(3),trueTuples->at(reasonIndex)->at(4)}),&_f);
                                const auto & itf_1 = tupleToVar.find(tuplef_1);
                                if(itf_1!=tupleToVar.end()){
                                    reason.push_back(itf_1->second * (tuplef_1.isNegated() ? -1:1));
                                }//closing if
                            }
                            if(tuple0!=tupleU){
                                const auto & it_reason0 = tupleToVar.find(Tuple(*tuple0));
                                if(it_reason0!=tupleToVar.end())
                                    reason.push_back(it_reason0->second * (tuple0->isNegated() ? -1:1));
                            }
                            if(tuple1!=tupleU){
                                const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                                if(it_reason1!=tupleToVar.end())
                                    reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                            }
                            if(joinTuples_0_2SharedVariables.first->size()>=3){
                                std::cout<<"conflict detected in propagator"<<std::endl;
                                propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                            }else{
                                if(joinTuples_0_2SharedVariables.first->size() == 3-1){
                                    const std::vector<const Tuple*>* undefinedTuples = &u_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                                    for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                        if(joinTuples_0_2SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1),undefinedTuples->at(iUndef)->at(3)}).size() == 0){
                                            const Tuple* aggrTupleU=NULL;
                                            const std::vector<const Tuple*>* tuplese;
                                            const std::vector<const Tuple*>* tuplesUe=&EMPTY_TUPLES;
                                            tuplese= &pe_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1),undefinedTuples->at(iUndef)->at(2)});
                                            if(aggrTupleU==NULL)
                                                tuplesUe= &ue_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1),undefinedTuples->at(iUndef)->at(2)});
                                            for(int j3=0;j3<tuplese->size()+tuplesUe->size();j3++){
                                                const Tuple* auxTuple3=NULL;
                                                bool addedToReason=false;
                                                if(j3 < tuplese->size()){
                                                    auxTuple3 = tuplese->at(j3);
                                                    const auto & it = tupleToVar.find(*auxTuple3);
                                                    if(it!=tupleToVar.end()){
                                                        addedToReason=true;
                                                        reason.push_back(it->second * (auxTuple3->isNegated() ? -1:1));
                                                    }//closing if
                                                    if(tuplesUe != &EMPTY_TUPLES) {
                                                    }
                                                }else{
                                                    auxTuple3 = tuplesUe->at(j3-tuplese->size());
                                                    aggrTupleU = auxTuple3;
                                                }
                                                const std::vector<const Tuple*>* tuplesf;
                                                const std::vector<const Tuple*>* tuplesUf=&EMPTY_TUPLES;
                                                tuplesf= &pf_0_1_.getValues({undefinedTuples->at(iUndef)->at(3),undefinedTuples->at(iUndef)->at(4)});
                                                if(aggrTupleU==NULL)
                                                    tuplesUf= &uf_0_1_.getValues({undefinedTuples->at(iUndef)->at(3),undefinedTuples->at(iUndef)->at(4)});
                                                for(int j5=0;j5<tuplesf->size()+tuplesUf->size();j5++){
                                                    const Tuple* auxTuple5=NULL;
                                                    bool addedToReason=false;
                                                    if(j5 < tuplesf->size()){
                                                        auxTuple5 = tuplesf->at(j5);
                                                        const auto & it = tupleToVar.find(*auxTuple5);
                                                        if(it!=tupleToVar.end()){
                                                            addedToReason=true;
                                                            reason.push_back(it->second * (auxTuple5->isNegated() ? -1:1));
                                                        }//closing if
                                                        if(tuplesUf != &EMPTY_TUPLES) {
                                                        }
                                                    }else{
                                                        auxTuple5 = tuplesUf->at(j5-tuplesf->size());
                                                        aggrTupleU = auxTuple5;
                                                    }
                                                    if(aggrTupleU == NULL){
                                                        std::cout<<"Tuple undefined not well formed"<<std::endl;
                                                        propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                                    }else{
                                                        const auto & it = tupleToVar.find(*aggrTupleU);
                                                        if(it != tupleToVar.end()) {
                                                            int sign = aggrTupleU->isNegated() ? -1 : 1;
                                                            auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                        }
                                                    }
                                                    if(addedToReason) reason.pop_back();
                                                }
                                                if(addedToReason) reason.pop_back();
                                            }
                                        }
                                    }
                                }
                            }
                        }else{
                        const std::vector<const Tuple*>* trueTuples = &p_e_X_A_Y_f_B_Y_0_2_4_.getValues({X,Y,Y});
                        std::vector<int> reason;
                        for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                            Tuple tuplee_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1),trueTuples->at(reasonIndex)->at(2)}),&_e);
                            const auto & ite_0 = tupleToVar.find(tuplee_0);
                            if(ite_0!=tupleToVar.end()){
                                reason.push_back(ite_0->second * (tuplee_0.isNegated() ? -1:1));
                            }//closing if
                            Tuple tuplef_1(std::vector<int>({trueTuples->at(reasonIndex)->at(3),trueTuples->at(reasonIndex)->at(4)}),&_f);
                            const auto & itf_1 = tupleToVar.find(tuplef_1);
                            if(itf_1!=tupleToVar.end()){
                                reason.push_back(itf_1->second * (tuplef_1.isNegated() ? -1:1));
                            }//closing if
                        }
                        if(tuple0!=tupleU){
                            const auto & it_reason0 = tupleToVar.find(Tuple(*tuple0));
                            if(it_reason0!=tupleToVar.end())
                                reason.push_back(it_reason0->second * (tuple0->isNegated() ? -1:1));
                        }
                        if(tuple1!=tupleU){
                            const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                            if(it_reason1!=tupleToVar.end())
                                reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                        }
                            if(joinTuples_0_2SharedVariables.first->size()>=3){
                                const auto & it = tupleToVar.find(*tupleU);
                                if(it != tupleToVar.end()) {
                                    int sign = tupleU->isNegated() ? -1 : 1;
                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                }
                            }
                        }
                    }//close par
                }//close loop nested join
            }//close loop nested join
        }//close predicate joins
    }
}
}
