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
const std::string _p_1 = "p_1";
PredicateWSet wp_1(2);
PredicateWSet up_1(2);
PredicateWSet fp_1(2);
const std::string _p_0 = "p_0";
PredicateWSet wp_0(1);
PredicateWSet up_0(1);
PredicateWSet fp_0(1);
const std::string _p_4 = "p_4";
PredicateWSet wp_4(2);
PredicateWSet up_4(2);
PredicateWSet fp_4(2);
const std::string _p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_ = "p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_";
PredicateWSet wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_(5);
PredicateWSet up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_(5);
std::map<std::vector<int>, std::pair< AuxMap*,AuxMap*>*> sharedVariables_0_ToAggregate_1;
std::map<std::vector<int>, std::pair< AuxMap*,AuxMap*>*> sharedVariables_1_ToAggregate_1;
Executor::~Executor() {
    for(auto sharedVar : sharedVariables_0_ToAggregate_1){
        delete sharedVar.second->first;
        delete sharedVar.second->second;
        delete sharedVar.second;
    }
    for(auto sharedVar : sharedVariables_1_ToAggregate_1){
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
AuxMap pp_4_0_1_({0,1});
AuxMap up_4_0_1_({0,1});
AuxMap fp_4_0_1_({0,1});
AuxMap pp_4_({});
AuxMap up_4_({});
AuxMap fp_4_({});
AuxMap pp_1_1_({1});
AuxMap up_1_1_({1});
AuxMap fp_1_1_({1});
AuxMap pp_0_({});
AuxMap up_0_({});
AuxMap fp_0_({});
AuxMap pp_1_0_1_({0,1});
AuxMap up_1_0_1_({0,1});
AuxMap fp_1_0_1_({0,1});
AuxMap pp_1_({});
AuxMap up_1_({});
AuxMap fp_1_({});
AuxMap pp_4_1_({1});
AuxMap up_4_1_({1});
AuxMap fp_4_1_({1});
AuxMap pp_0_0_({0});
AuxMap up_0_0_({0});
AuxMap fp_0_0_({0});
AuxMap p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_4_({4});
AuxMap u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_4_({4});
AuxMap p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_({});
AuxMap u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_({});
AuxMap p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_({1,3});
AuxMap u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_({1,3});
AuxMap p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_0_1_({0,1});
AuxMap u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_0_1_({0,1});
AuxMap p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_2_3_({2,3});
AuxMap u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_2_3_({2,3});
AuxMap pp_1_0_({0});
AuxMap up_1_0_({0});
AuxMap fp_1_0_({0});
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
    if(var<0 && ( tuple.getPredicateName() == &_p_0 || tuple.getPredicateName() == &_p_1 || tuple.getPredicateName() == &_p_4)){
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
    if(tuple.getPredicateName() == &_p_4){
        if(var > 0){
            int X_4 = tuple.at(0);
            int X_1 = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuples1 = pp_1_1_.getValues({X_1});
            const std::vector<const Tuple*>& tuplesU1 = up_1_1_.getValues({X_1});
            for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
                bool buildUndef=false;
                const Tuple* tuple1;
                if(i<tuples1.size())
                    tuple1=tuples1[i];
                else{
                    buildUndef=true;
                    tuple1=tuplesU1[i-tuples1.size()];
                }
                int X_0 = tuple1->at(0);
                const std::vector<const Tuple*>& tuples2 = pp_0_.getValues({});
                const std::vector<const Tuple*>& tuplesU2 = up_0_.getValues({});
                for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
                    const Tuple* tuple2;
                    if(i<tuples2.size())
                        tuple2=tuples2[i];
                    else{
                        buildUndef=true;
                        tuple2=tuplesU2[i-tuples2.size()];
                    }
                    int X_3 = tuple2->at(0);
                    Tuple t(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
                    if(!buildUndef && !buildFalse){
                        if(wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(t)==NULL){
                            const auto& insertResult = wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(t));
                            if (insertResult.second) {
                                for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
                                    auxMap -> insert2(*insertResult.first);
                                }
                                {
                                    Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                                    if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                                        auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                                        joinTuples->first->insert2(*insertResult.first);
                                    }
                                }
                                {
                                    Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                                    if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
                                        auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
                                        joinTuples->first->insert2(*insertResult.first);
                                    }
                                }
                                up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
                            }
                        }else{
                    }
                }else{
                if(!buildFalse){
                    if(up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(t)==NULL){
                            const auto& insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(t));
                            if (insertResult.second) {
                                for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
                                    auxMap -> insert2(*insertResult.first);
                                }
                                {
                                    Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                                    if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                                        auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                                        joinTuples->second->insert2(*insertResult.first);
                                    }
                                }
                                {
                                    Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                                    if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
                                        auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
                                        joinTuples->second->insert2(*insertResult.first);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }else{
        int X_4 = tuple.at(0);
        int X_1 = tuple.at(1);
        bool buildUndef=false;
        bool buildFalse=false;
        const std::vector<const Tuple*>& tuplesU1 = up_1_1_.getValues({X_1});
        const std::vector<const Tuple*>& tuples1 = pp_1_1_.getValues({X_1});
        const std::vector<const Tuple*>& tuplesF1 = fp_1_1_.getValues({X_1});
        for(int i=0;i<tuples1.size()+tuplesU1.size()+tuplesF1.size();i++){
            buildUndef=false;
            buildFalse=false;
            const Tuple* tuple1;
            if(i<tuples1.size())
                tuple1=tuples1[i];
            else if (i < tuplesU1.size()+tuples1.size()){
                buildUndef=true;
                tuple1=tuplesU1[i-tuples1.size()];
            }else{
                tuple1=tuplesF1[i-tuples1.size()-tuplesU1.size()];
                buildFalse=true;
            }
            int X_0 = tuple1->at(0);
            const std::vector<const Tuple*>& tuplesU2 = up_0_.getValues({});
            const std::vector<const Tuple*>& tuples2 = pp_0_.getValues({});
            const std::vector<const Tuple*>& tuplesF2 = fp_0_.getValues({});
            for(int i=0;i<tuples2.size()+tuplesU2.size()+tuplesF2.size();i++){
                const Tuple* tuple2;
                if(i<tuples2.size())
                    tuple2=tuples2[i];
                else if (i < tuplesU2.size()+tuples2.size()){
                    buildUndef=true;
                    tuple2=tuplesU2[i-tuples2.size()];
                }else{
                    tuple2=tuplesF2[i-tuples2.size()-tuplesU2.size()];
                    buildFalse=true;
                }
                int X_3 = tuple2->at(0);
                Tuple t(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
                up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
                wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
            }
        }
    }
}
if(tuple.getPredicateName() == &_p_1){
    if(var > 0){
        int X_0 = tuple.at(0);
        int X_1 = tuple.at(1);
        bool buildUndef=false;
        bool buildFalse=false;
        const std::vector<const Tuple*>& tuples0 = pp_4_1_.getValues({X_1});
        const std::vector<const Tuple*>& tuplesU0 = up_4_1_.getValues({X_1});
        for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
            bool buildUndef=false;
            const Tuple* tuple0;
            if(i<tuples0.size())
                tuple0=tuples0[i];
            else{
                buildUndef=true;
                tuple0=tuplesU0[i-tuples0.size()];
            }
            int X_4 = tuple0->at(0);
            const std::vector<const Tuple*>& tuples2 = pp_0_.getValues({});
            const std::vector<const Tuple*>& tuplesU2 = up_0_.getValues({});
            for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
                const Tuple* tuple2;
                if(i<tuples2.size())
                    tuple2=tuples2[i];
                else{
                    buildUndef=true;
                    tuple2=tuplesU2[i-tuples2.size()];
                }
                int X_3 = tuple2->at(0);
                Tuple t(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
                if(!buildUndef && !buildFalse){
                    if(wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(t)==NULL){
                        const auto& insertResult = wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                                if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                                    joinTuples->first->insert2(*insertResult.first);
                                }
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                                if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
                                    joinTuples->first->insert2(*insertResult.first);
                                }
                            }
                            up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
                        }
                    }else{
                }
            }else{
            if(!buildFalse){
                if(up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(t)==NULL){
                        const auto& insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                                if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                                    joinTuples->second->insert2(*insertResult.first);
                                }
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                                if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
                                    joinTuples->second->insert2(*insertResult.first);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}else{
    int X_0 = tuple.at(0);
    int X_1 = tuple.at(1);
    bool buildUndef=false;
    bool buildFalse=false;
    const std::vector<const Tuple*>& tuplesU0 = up_4_1_.getValues({X_1});
    const std::vector<const Tuple*>& tuples0 = pp_4_1_.getValues({X_1});
    const std::vector<const Tuple*>& tuplesF0 = fp_4_1_.getValues({X_1});
    for(int i=0;i<tuples0.size()+tuplesU0.size()+tuplesF0.size();i++){
        buildUndef=false;
        buildFalse=false;
        const Tuple* tuple0;
        if(i<tuples0.size())
            tuple0=tuples0[i];
        else if (i < tuplesU0.size()+tuples0.size()){
            buildUndef=true;
            tuple0=tuplesU0[i-tuples0.size()];
        }else{
            tuple0=tuplesF0[i-tuples0.size()-tuplesU0.size()];
            buildFalse=true;
        }
        int X_4 = tuple0->at(0);
        const std::vector<const Tuple*>& tuplesU2 = up_0_.getValues({});
        const std::vector<const Tuple*>& tuples2 = pp_0_.getValues({});
        const std::vector<const Tuple*>& tuplesF2 = fp_0_.getValues({});
        for(int i=0;i<tuples2.size()+tuplesU2.size()+tuplesF2.size();i++){
            const Tuple* tuple2;
            if(i<tuples2.size())
                tuple2=tuples2[i];
            else if (i < tuplesU2.size()+tuples2.size()){
                buildUndef=true;
                tuple2=tuplesU2[i-tuples2.size()];
            }else{
                tuple2=tuplesF2[i-tuples2.size()-tuplesU2.size()];
                buildFalse=true;
            }
            int X_3 = tuple2->at(0);
            Tuple t(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
            up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
            wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
        }
    }
}
}
if(tuple.getPredicateName() == &_p_0){
if(var > 0){
    int X_3 = tuple.at(0);
    bool buildUndef=false;
    bool buildFalse=false;
    const std::vector<const Tuple*>& tuples0 = pp_4_.getValues({});
    const std::vector<const Tuple*>& tuplesU0 = up_4_.getValues({});
    for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
        bool buildUndef=false;
        const Tuple* tuple0;
        if(i<tuples0.size())
            tuple0=tuples0[i];
        else{
            buildUndef=true;
            tuple0=tuplesU0[i-tuples0.size()];
        }
        int X_4 = tuple0->at(0);
        int X_1 = tuple0->at(1);
        const std::vector<const Tuple*>& tuples1 = pp_1_1_.getValues({X_1});
        const std::vector<const Tuple*>& tuplesU1 = up_1_1_.getValues({X_1});
        for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
            const Tuple* tuple1;
            if(i<tuples1.size())
                tuple1=tuples1[i];
            else{
                buildUndef=true;
                tuple1=tuplesU1[i-tuples1.size()];
            }
            int X_0 = tuple1->at(0);
            Tuple t(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
            if(!buildUndef && !buildFalse){
                if(wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(t)==NULL){
                    const auto& insertResult = wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(t));
                    if (insertResult.second) {
                        for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
                            auxMap -> insert2(*insertResult.first);
                        }
                        {
                            Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                            if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                                auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                                joinTuples->first->insert2(*insertResult.first);
                            }
                        }
                        {
                            Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                            if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
                                auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
                                joinTuples->first->insert2(*insertResult.first);
                            }
                        }
                        up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
                    }
                }else{
            }
        }else{
        if(!buildFalse){
            if(up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(t)==NULL){
                    const auto& insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(t));
                    if (insertResult.second) {
                        for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
                            auxMap -> insert2(*insertResult.first);
                        }
                        {
                            Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                            if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                                auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                                joinTuples->second->insert2(*insertResult.first);
                            }
                        }
                        {
                            Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                            if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
                                auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
                                joinTuples->second->insert2(*insertResult.first);
                            }
                        }
                    }
                }
            }
        }
    }
}
}else{
int X_3 = tuple.at(0);
bool buildUndef=false;
bool buildFalse=false;
const std::vector<const Tuple*>& tuplesU0 = up_4_.getValues({});
const std::vector<const Tuple*>& tuples0 = pp_4_.getValues({});
const std::vector<const Tuple*>& tuplesF0 = fp_4_.getValues({});
for(int i=0;i<tuples0.size()+tuplesU0.size()+tuplesF0.size();i++){
    buildUndef=false;
    buildFalse=false;
    const Tuple* tuple0;
    if(i<tuples0.size())
        tuple0=tuples0[i];
    else if (i < tuplesU0.size()+tuples0.size()){
        buildUndef=true;
        tuple0=tuplesU0[i-tuples0.size()];
    }else{
        tuple0=tuplesF0[i-tuples0.size()-tuplesU0.size()];
        buildFalse=true;
    }
    int X_4 = tuple0->at(0);
    int X_1 = tuple0->at(1);
    const std::vector<const Tuple*>& tuplesU1 = up_1_1_.getValues({X_1});
    const std::vector<const Tuple*>& tuples1 = pp_1_1_.getValues({X_1});
    const std::vector<const Tuple*>& tuplesF1 = fp_1_1_.getValues({X_1});
    for(int i=0;i<tuples1.size()+tuplesU1.size()+tuplesF1.size();i++){
        const Tuple* tuple1;
        if(i<tuples1.size())
            tuple1=tuples1[i];
        else if (i < tuplesU1.size()+tuples1.size()){
            buildUndef=true;
            tuple1=tuplesU1[i-tuples1.size()];
        }else{
            tuple1=tuplesF1[i-tuples1.size()-tuplesU1.size()];
            buildFalse=true;
        }
        int X_0 = tuple1->at(0);
        Tuple t(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
        up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
        wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
    }
}
}
}
if(tuple.getPredicateName() == &_p_1){
int X_1 = tuple.at(1);
if(var>0){
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(!sharedVariables_0_ToAggregate_1.count(sharedTuple)){
    sharedVariables_0_ToAggregate_1[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({4})),new AuxMap(std::vector<unsigned>({4})));
    const std::vector<const Tuple*>* trueJoinTuples = &p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
    const std::vector<const Tuple*>* undefJoinTuples = &u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
    for(int i=0;i<trueJoinTuples->size();i++){
        const auto findResult = wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(Tuple(*trueJoinTuples->at(i)));
        if(findResult!=NULL){
            sharedVariables_0_ToAggregate_1[sharedTuple]->first->insert2(*findResult);
        }
    }
    for(int i=0;i<undefJoinTuples->size();i++){
        const auto findResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(Tuple(*undefJoinTuples->at(i)));
        if(findResult!=NULL){
            sharedVariables_0_ToAggregate_1[sharedTuple]->second->insert2(*findResult);
        }
    }
}
}else{
}
}
if(tuple.getPredicateName() == &_p_4){
if(var > 0){
int X_4 = tuple.at(0);
int X_1 = tuple.at(1);
bool buildUndef=false;
bool buildFalse=false;
const std::vector<const Tuple*>& tuples1 = pp_1_1_.getValues({X_1});
const std::vector<const Tuple*>& tuplesU1 = up_1_1_.getValues({X_1});
for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
    bool buildUndef=false;
    const Tuple* tuple1;
    if(i<tuples1.size())
        tuple1=tuples1[i];
    else{
        buildUndef=true;
        tuple1=tuplesU1[i-tuples1.size()];
    }
    int X_0 = tuple1->at(0);
    const std::vector<const Tuple*>& tuples2 = pp_0_.getValues({});
    const std::vector<const Tuple*>& tuplesU2 = up_0_.getValues({});
    for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
        const Tuple* tuple2;
        if(i<tuples2.size())
            tuple2=tuples2[i];
        else{
            buildUndef=true;
            tuple2=tuplesU2[i-tuples2.size()];
        }
        int X_3 = tuple2->at(0);
        Tuple t(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
        if(!buildUndef){
            if(wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(t)==NULL){
                const auto& insertResult = wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(t));
                if (insertResult.second) {
                    for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
                        auxMap -> insert2(*insertResult.first);
                    }
                    {
                        Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                        if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                            joinTuples->first->insert2(*insertResult.first);
                        }
                    }
                    {
                        Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                        if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
                            joinTuples->first->insert2(*insertResult.first);
                        }
                    }
                    up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
                }
            }else{
        }
    }else{
    if(up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(t)==NULL){
            const auto& insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(t));
            if (insertResult.second) {
                for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
                    auxMap -> insert2(*insertResult.first);
                }
                {
                    Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                    if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                        auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                        joinTuples->second->insert2(*insertResult.first);
                    }
                }
                {
                    Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                    if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
                        auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
                        joinTuples->second->insert2(*insertResult.first);
                    }
                }
            }
        }
    }
}
}
}else{
int X_4 = tuple.at(0);
int X_1 = tuple.at(1);
bool buildUndef=false;
bool buildFalse=false;
const std::vector<const Tuple*>& tuplesU1 = up_1_1_.getValues({X_1});
const std::vector<const Tuple*>& tuples1 = pp_1_1_.getValues({X_1});
for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
bool buildUndef=false;
const Tuple* tuple1;
if(i<tuples1.size())
    tuple1=tuples1[i];
else{
    buildUndef=true;
    tuple1=tuplesU1[i-tuples1.size()];
}
int X_0 = tuple1->at(0);
const std::vector<const Tuple*>& tuplesU2 = up_0_.getValues({});
const std::vector<const Tuple*>& tuples2 = pp_0_.getValues({});
for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
    const Tuple* tuple2;
    if(i<tuples2.size())
        tuple2=tuples2[i];
    else{
        buildUndef=true;
        tuple2=tuplesU2[i-tuples2.size()];
    }
    int X_3 = tuple2->at(0);
    Tuple t(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
    up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
    wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
}
}
}
}
if(tuple.getPredicateName() == &_p_1){
if(var > 0){
int X_0 = tuple.at(0);
int X_1 = tuple.at(1);
bool buildUndef=false;
bool buildFalse=false;
const std::vector<const Tuple*>& tuples0 = pp_4_1_.getValues({X_1});
const std::vector<const Tuple*>& tuplesU0 = up_4_1_.getValues({X_1});
for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
bool buildUndef=false;
const Tuple* tuple0;
if(i<tuples0.size())
    tuple0=tuples0[i];
else{
    buildUndef=true;
    tuple0=tuplesU0[i-tuples0.size()];
}
int X_4 = tuple0->at(0);
const std::vector<const Tuple*>& tuples2 = pp_0_.getValues({});
const std::vector<const Tuple*>& tuplesU2 = up_0_.getValues({});
for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
    const Tuple* tuple2;
    if(i<tuples2.size())
        tuple2=tuples2[i];
    else{
        buildUndef=true;
        tuple2=tuplesU2[i-tuples2.size()];
    }
    int X_3 = tuple2->at(0);
    Tuple t(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
    if(!buildUndef){
        if(wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(t)==NULL){
            const auto& insertResult = wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(t));
            if (insertResult.second) {
                for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
                    auxMap -> insert2(*insertResult.first);
                }
                {
                    Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                    if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                        auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                        joinTuples->first->insert2(*insertResult.first);
                    }
                }
                {
                    Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                    if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
                        auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
                        joinTuples->first->insert2(*insertResult.first);
                    }
                }
                up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
            }
        }else{
    }
}else{
if(up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(t)==NULL){
        const auto& insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(t));
        if (insertResult.second) {
            for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
                auxMap -> insert2(*insertResult.first);
            }
            {
                Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                    joinTuples->second->insert2(*insertResult.first);
                }
            }
            {
                Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
                    auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
                    joinTuples->second->insert2(*insertResult.first);
                }
            }
        }
    }
}
}
}
}else{
int X_0 = tuple.at(0);
int X_1 = tuple.at(1);
bool buildUndef=false;
bool buildFalse=false;
const std::vector<const Tuple*>& tuplesU0 = up_4_1_.getValues({X_1});
const std::vector<const Tuple*>& tuples0 = pp_4_1_.getValues({X_1});
for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
bool buildUndef=false;
const Tuple* tuple0;
if(i<tuples0.size())
tuple0=tuples0[i];
else{
buildUndef=true;
tuple0=tuplesU0[i-tuples0.size()];
}
int X_4 = tuple0->at(0);
const std::vector<const Tuple*>& tuplesU2 = up_0_.getValues({});
const std::vector<const Tuple*>& tuples2 = pp_0_.getValues({});
for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
const Tuple* tuple2;
if(i<tuples2.size())
    tuple2=tuples2[i];
else{
    buildUndef=true;
    tuple2=tuplesU2[i-tuples2.size()];
}
int X_3 = tuple2->at(0);
Tuple t(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
}
}
}
}
if(tuple.getPredicateName() == &_p_0){
if(var > 0){
int X_3 = tuple.at(0);
bool buildUndef=false;
bool buildFalse=false;
const std::vector<const Tuple*>& tuples0 = pp_4_.getValues({});
const std::vector<const Tuple*>& tuplesU0 = up_4_.getValues({});
for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
bool buildUndef=false;
const Tuple* tuple0;
if(i<tuples0.size())
tuple0=tuples0[i];
else{
buildUndef=true;
tuple0=tuplesU0[i-tuples0.size()];
}
int X_4 = tuple0->at(0);
int X_1 = tuple0->at(1);
const std::vector<const Tuple*>& tuples1 = pp_1_1_.getValues({X_1});
const std::vector<const Tuple*>& tuplesU1 = up_1_1_.getValues({X_1});
for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
const Tuple* tuple1;
if(i<tuples1.size())
    tuple1=tuples1[i];
else{
    buildUndef=true;
    tuple1=tuplesU1[i-tuples1.size()];
}
int X_0 = tuple1->at(0);
Tuple t(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
if(!buildUndef){
    if(wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(t)==NULL){
        const auto& insertResult = wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(t));
        if (insertResult.second) {
            for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
                auxMap -> insert2(*insertResult.first);
            }
            {
                Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                    joinTuples->first->insert2(*insertResult.first);
                }
            }
            {
                Tuple sharedTuple(std::vector<int>({X_1,X_1}));
                if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
                    auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
                    joinTuples->first->insert2(*insertResult.first);
                }
            }
            up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
        }
    }else{
}
}else{
if(up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(t)==NULL){
    const auto& insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(t));
    if (insertResult.second) {
        for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
            auxMap -> insert2(*insertResult.first);
        }
        {
            Tuple sharedTuple(std::vector<int>({X_1,X_1}));
            if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                joinTuples->second->insert2(*insertResult.first);
            }
        }
        {
            Tuple sharedTuple(std::vector<int>({X_1,X_1}));
            if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
                auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
                joinTuples->second->insert2(*insertResult.first);
            }
        }
    }
}
}
}
}
}else{
int X_3 = tuple.at(0);
bool buildUndef=false;
bool buildFalse=false;
const std::vector<const Tuple*>& tuplesU0 = up_4_.getValues({});
const std::vector<const Tuple*>& tuples0 = pp_4_.getValues({});
for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
bool buildUndef=false;
const Tuple* tuple0;
if(i<tuples0.size())
tuple0=tuples0[i];
else{
buildUndef=true;
tuple0=tuplesU0[i-tuples0.size()];
}
int X_4 = tuple0->at(0);
int X_1 = tuple0->at(1);
const std::vector<const Tuple*>& tuplesU1 = up_1_1_.getValues({X_1});
const std::vector<const Tuple*>& tuples1 = pp_1_1_.getValues({X_1});
for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
const Tuple* tuple1;
if(i<tuples1.size())
tuple1=tuples1[i];
else{
buildUndef=true;
tuple1=tuplesU1[i-tuples1.size()];
}
int X_0 = tuple1->at(0);
Tuple t(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(t);
}
}
}
}
if(tuple.getPredicateName() == &_p_1){
int X_1 = tuple.at(1);
if(var>0){
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(!sharedVariables_1_ToAggregate_1.count(sharedTuple)){
sharedVariables_1_ToAggregate_1[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({4})),new AuxMap(std::vector<unsigned>({4})));
const std::vector<const Tuple*>* trueJoinTuples = &p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
const std::vector<const Tuple*>* undefJoinTuples = &u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
for(int i=0;i<trueJoinTuples->size();i++){
const auto findResult = wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(Tuple(*trueJoinTuples->at(i)));
if(findResult!=NULL){
sharedVariables_1_ToAggregate_1[sharedTuple]->first->insert2(*findResult);
}
}
for(int i=0;i<undefJoinTuples->size();i++){
const auto findResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(Tuple(*undefJoinTuples->at(i)));
if(findResult!=NULL){
sharedVariables_1_ToAggregate_1[sharedTuple]->second->insert2(*findResult);
}
}
}
}else{
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
if(var<0 && ( tuple.getPredicateName() == &_p_0 || tuple.getPredicateName() == &_p_1 || tuple.getPredicateName() == &_p_4)){
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
if(tuple.getPredicateName() == &_p_4){
if(var > 0){
const std::vector<const Tuple* >& trueJoinTuples = p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_0_1_.getValues({tuple[0],tuple[1]});
for(const Tuple* t :trueJoinTuples){
const auto insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(*t));
if(insertResult.second){
for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]) {
auxMap -> insert2(*insertResult.first);
}
int X_1 = t->at(1);
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
}
wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(*t);
}
}
int X_4 = tuple.at(0);
int X_1 = tuple.at(1);
bool buildUndef=false;
bool buildFalse=false;
const std::vector<const Tuple*>& tuplesU1 = pp_1_1_.getValues({X_1});
const std::vector<const Tuple*>& tuples1 = up_1_1_.getValues({X_1});
for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
const Tuple* tuple1;
if(i<tuples1.size())
tuple1=tuples1[i];
else{
tuple1=tuplesU1[i-tuples1.size()];
}
int X_0 = tuple1->at(0);
const std::vector<const Tuple*>& tuplesU2 = pp_0_.getValues({});
const std::vector<const Tuple*>& tuples2 = up_0_.getValues({});
for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
const Tuple* tuple2;
if(i<tuples2.size())
tuple2=tuples2[i];
else{
tuple2=tuplesU2[i-tuples2.size()];
}
int X_3 = tuple2->at(0);
Tuple tupleUndef(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
const auto& insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(tupleUndef));
if (insertResult.second) {
for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
auxMap -> insert2(*insertResult.first);
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
}
}
}
}
if(tuple.getPredicateName() == &_p_1){
if(var > 0){
const std::vector<const Tuple* >& trueJoinTuples = p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_2_3_.getValues({tuple[0],tuple[1]});
for(const Tuple* t :trueJoinTuples){
const auto insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(*t));
if(insertResult.second){
for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]) {
auxMap -> insert2(*insertResult.first);
}
int X_1 = t->at(1);
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
}
wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(*t);
}
}
int X_0 = tuple.at(0);
int X_1 = tuple.at(1);
bool buildUndef=false;
bool buildFalse=false;
const std::vector<const Tuple*>& tuplesU0 = pp_4_1_.getValues({X_1});
const std::vector<const Tuple*>& tuples0 = up_4_1_.getValues({X_1});
for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
const Tuple* tuple0;
if(i<tuples0.size())
tuple0=tuples0[i];
else{
tuple0=tuplesU0[i-tuples0.size()];
}
int X_4 = tuple0->at(0);
const std::vector<const Tuple*>& tuplesU2 = pp_0_.getValues({});
const std::vector<const Tuple*>& tuples2 = up_0_.getValues({});
for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
const Tuple* tuple2;
if(i<tuples2.size())
tuple2=tuples2[i];
else{
tuple2=tuplesU2[i-tuples2.size()];
}
int X_3 = tuple2->at(0);
Tuple tupleUndef(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
const auto& insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(tupleUndef));
if (insertResult.second) {
for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
auxMap -> insert2(*insertResult.first);
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
}
}
}
}
if(tuple.getPredicateName() == &_p_0){
if(var > 0){
const std::vector<const Tuple* >& trueJoinTuples = p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_4_.getValues({tuple[0]});
for(const Tuple* t :trueJoinTuples){
const auto insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(*t));
if(insertResult.second){
for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]) {
auxMap -> insert2(*insertResult.first);
}
int X_1 = t->at(1);
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
}
wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(*t);
}
}
int X_3 = tuple.at(0);
bool buildUndef=false;
bool buildFalse=false;
const std::vector<const Tuple*>& tuplesU0 = pp_4_.getValues({});
const std::vector<const Tuple*>& tuples0 = up_4_.getValues({});
for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
const Tuple* tuple0;
if(i<tuples0.size())
tuple0=tuples0[i];
else{
tuple0=tuplesU0[i-tuples0.size()];
}
int X_4 = tuple0->at(0);
int X_1 = tuple0->at(1);
const std::vector<const Tuple*>& tuplesU1 = pp_1_1_.getValues({X_1});
const std::vector<const Tuple*>& tuples1 = up_1_1_.getValues({X_1});
for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
const Tuple* tuple1;
if(i<tuples1.size())
tuple1=tuples1[i];
else{
tuple1=tuplesU1[i-tuples1.size()];
}
int X_0 = tuple1->at(0);
Tuple tupleUndef(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
const auto& insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(tupleUndef));
if (insertResult.second) {
for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
auxMap -> insert2(*insertResult.first);
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
}
}
}
}
if(tuple.getPredicateName() == &_p_1){
int X_1 = tuple.at(1);
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(!sharedVariables_0_ToAggregate_1.count(sharedTuple)){
sharedVariables_0_ToAggregate_1[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({4})),new AuxMap(std::vector<unsigned>({4})));
const std::vector<const Tuple*>* undefJoinTuples = &u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
const std::vector<const Tuple*>* trueJoinTuples = &p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
for(int i=0;i<undefJoinTuples->size();i++){
const auto findResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(Tuple(*undefJoinTuples->at(i)));
if(findResult!=NULL){
sharedVariables_0_ToAggregate_1[sharedTuple]->second->insert2(*findResult);
}
}
for(int i=0;i<trueJoinTuples->size();i++){
const auto findResult = wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(Tuple(*trueJoinTuples->at(i)));
if(findResult!=NULL){
sharedVariables_0_ToAggregate_1[sharedTuple]->first->insert2(*findResult);
}
}
}
}
if(tuple.getPredicateName() == &_p_4){
if(var > 0){
const std::vector<const Tuple* >& trueJoinTuples = p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_0_1_.getValues({tuple[0],tuple[1]});
for(const Tuple* t :trueJoinTuples){
const auto insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(*t));
if(insertResult.second){
for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]) {
auxMap -> insert2(*insertResult.first);
}
int X_1 = t->at(1);
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
}
wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(*t);
}
}
int X_4 = tuple.at(0);
int X_1 = tuple.at(1);
bool buildUndef=false;
bool buildFalse=false;
const std::vector<const Tuple*>& tuplesU1 = pp_1_1_.getValues({X_1});
const std::vector<const Tuple*>& tuples1 = up_1_1_.getValues({X_1});
for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
const Tuple* tuple1;
if(i<tuples1.size())
tuple1=tuples1[i];
else{
tuple1=tuplesU1[i-tuples1.size()];
}
int X_0 = tuple1->at(0);
const std::vector<const Tuple*>& tuplesU2 = pp_0_.getValues({});
const std::vector<const Tuple*>& tuples2 = up_0_.getValues({});
for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
const Tuple* tuple2;
if(i<tuples2.size())
tuple2=tuples2[i];
else{
tuple2=tuplesU2[i-tuples2.size()];
}
int X_3 = tuple2->at(0);
Tuple tupleUndef(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
const auto& insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(tupleUndef));
if (insertResult.second) {
for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
auxMap -> insert2(*insertResult.first);
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
}
}
}
}
if(tuple.getPredicateName() == &_p_1){
if(var > 0){
const std::vector<const Tuple* >& trueJoinTuples = p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_2_3_.getValues({tuple[0],tuple[1]});
for(const Tuple* t :trueJoinTuples){
const auto insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(*t));
if(insertResult.second){
for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]) {
auxMap -> insert2(*insertResult.first);
}
int X_1 = t->at(1);
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
}
wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(*t);
}
}
int X_0 = tuple.at(0);
int X_1 = tuple.at(1);
bool buildUndef=false;
bool buildFalse=false;
const std::vector<const Tuple*>& tuplesU0 = pp_4_1_.getValues({X_1});
const std::vector<const Tuple*>& tuples0 = up_4_1_.getValues({X_1});
for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
const Tuple* tuple0;
if(i<tuples0.size())
tuple0=tuples0[i];
else{
tuple0=tuplesU0[i-tuples0.size()];
}
int X_4 = tuple0->at(0);
const std::vector<const Tuple*>& tuplesU2 = pp_0_.getValues({});
const std::vector<const Tuple*>& tuples2 = up_0_.getValues({});
for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
const Tuple* tuple2;
if(i<tuples2.size())
tuple2=tuples2[i];
else{
tuple2=tuplesU2[i-tuples2.size()];
}
int X_3 = tuple2->at(0);
Tuple tupleUndef(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
const auto& insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(tupleUndef));
if (insertResult.second) {
for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
auxMap -> insert2(*insertResult.first);
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
}
}
}
}
if(tuple.getPredicateName() == &_p_0){
if(var > 0){
const std::vector<const Tuple* >& trueJoinTuples = p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_4_.getValues({tuple[0]});
for(const Tuple* t :trueJoinTuples){
const auto insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(*t));
if(insertResult.second){
for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]) {
auxMap -> insert2(*insertResult.first);
}
int X_1 = t->at(1);
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
}
wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.erase(*t);
}
}
int X_3 = tuple.at(0);
bool buildUndef=false;
bool buildFalse=false;
const std::vector<const Tuple*>& tuplesU0 = pp_4_.getValues({});
const std::vector<const Tuple*>& tuples0 = up_4_.getValues({});
for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
const Tuple* tuple0;
if(i<tuples0.size())
tuple0=tuples0[i];
else{
tuple0=tuplesU0[i-tuples0.size()];
}
int X_4 = tuple0->at(0);
int X_1 = tuple0->at(1);
const std::vector<const Tuple*>& tuplesU1 = pp_1_1_.getValues({X_1});
const std::vector<const Tuple*>& tuples1 = up_1_1_.getValues({X_1});
for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
const Tuple* tuple1;
if(i<tuples1.size())
tuple1=tuples1[i];
else{
tuple1=tuplesU1[i-tuples1.size()];
}
int X_0 = tuple1->at(0);
Tuple tupleUndef(std::vector<int>({X_4,X_1,X_0,X_1,X_3}),&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
const auto& insertResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.insert(Tuple(tupleUndef));
if (insertResult.second) {
for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_]){
auxMap -> insert2(*insertResult.first);
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
{
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(sharedVariables_1_ToAggregate_1.count(sharedTuple)!=0){
    auto joinTuples = sharedVariables_1_ToAggregate_1[sharedTuple];
    joinTuples->second->insert2(*insertResult.first);
}
}
}
}
}
}
if(tuple.getPredicateName() == &_p_1){
int X_1 = tuple.at(1);
Tuple sharedTuple(std::vector<int>({X_1,X_1}));
if(!sharedVariables_1_ToAggregate_1.count(sharedTuple)){
sharedVariables_1_ToAggregate_1[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({4})),new AuxMap(std::vector<unsigned>({4})));
const std::vector<const Tuple*>* undefJoinTuples = &u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
const std::vector<const Tuple*>* trueJoinTuples = &p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
for(int i=0;i<undefJoinTuples->size();i++){
const auto findResult = up_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(Tuple(*undefJoinTuples->at(i)));
if(findResult!=NULL){
sharedVariables_1_ToAggregate_1[sharedTuple]->second->insert2(*findResult);
}
}
for(int i=0;i<trueJoinTuples->size();i++){
const auto findResult = wp_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_.find(Tuple(*trueJoinTuples->at(i)));
if(findResult!=NULL){
sharedVariables_1_ToAggregate_1[sharedTuple]->first->insert2(*findResult);
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
predicateWSetMap[&_p_1]=&wp_1;
predicateUSetMap[&_p_1]=&up_1;
stringToUniqueStringPointer["p_1"] = &_p_1;
aggregatePredicatesAndVars [&_p_4]= std::vector<int>({});
predicateWSetMap[&_p_4]=&wp_4;
predicateFSetMap[&_p_4]=&fp_4;
predicateUSetMap[&_p_4]=&up_4;
stringToUniqueStringPointer["p_4"] = &_p_4;
aggregatePredicatesAndVars [&_p_1]= std::vector<int>({});
predicateWSetMap[&_p_1]=&wp_1;
predicateFSetMap[&_p_1]=&fp_1;
predicateUSetMap[&_p_1]=&up_1;
stringToUniqueStringPointer["p_1"] = &_p_1;
aggregatePredicatesAndVars [&_p_0]= std::vector<int>({0});
predicateWSetMap[&_p_0]=&wp_0;
predicateFSetMap[&_p_0]=&fp_0;
predicateUSetMap[&_p_0]=&up_0;
stringToUniqueStringPointer["p_0"] = &_p_0;
aggregatePredicatesAndVars [&_p_4]= std::vector<int>({});
predicateWSetMap[&_p_4]=&wp_4;
predicateFSetMap[&_p_4]=&fp_4;
predicateUSetMap[&_p_4]=&up_4;
stringToUniqueStringPointer["p_4"] = &_p_4;
aggregatePredicatesAndVars [&_p_1]= std::vector<int>({});
predicateWSetMap[&_p_1]=&wp_1;
predicateFSetMap[&_p_1]=&fp_1;
predicateUSetMap[&_p_1]=&up_1;
stringToUniqueStringPointer["p_1"] = &_p_1;
aggregatePredicatesAndVars [&_p_0]= std::vector<int>({0});
predicateWSetMap[&_p_0]=&wp_0;
predicateFSetMap[&_p_0]=&fp_0;
predicateUSetMap[&_p_0]=&up_0;
stringToUniqueStringPointer["p_0"] = &_p_0;
predicateToAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_].push_back(&p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
predicateToAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_].push_back(&p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_0_1_);
predicateToAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_].push_back(&p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_);
predicateToAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_].push_back(&p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_2_3_);
predicateToAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_].push_back(&p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_4_);
predicateToAuxiliaryMaps[&_p_0].push_back(&pp_0_);
predicateToAuxiliaryMaps[&_p_0].push_back(&pp_0_0_);
predicateToAuxiliaryMaps[&_p_4].push_back(&pp_4_);
predicateToAuxiliaryMaps[&_p_4].push_back(&pp_4_0_1_);
predicateToAuxiliaryMaps[&_p_4].push_back(&pp_4_1_);
predicateToAuxiliaryMaps[&_p_1].push_back(&pp_1_);
predicateToAuxiliaryMaps[&_p_1].push_back(&pp_1_0_);
predicateToAuxiliaryMaps[&_p_1].push_back(&pp_1_0_1_);
predicateToAuxiliaryMaps[&_p_1].push_back(&pp_1_1_);
predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_].push_back(&u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_);
predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_].push_back(&u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_0_1_);
predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_].push_back(&u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_);
predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_].push_back(&u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_2_3_);
predicateToUndefAuxiliaryMaps[&_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_].push_back(&u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_4_);
predicateToUndefAuxiliaryMaps[&_p_0].push_back(&up_0_);
predicateToUndefAuxiliaryMaps[&_p_0].push_back(&up_0_0_);
predicateToUndefAuxiliaryMaps[&_p_4].push_back(&up_4_);
predicateToUndefAuxiliaryMaps[&_p_4].push_back(&up_4_0_1_);
predicateToUndefAuxiliaryMaps[&_p_4].push_back(&up_4_1_);
predicateToUndefAuxiliaryMaps[&_p_1].push_back(&up_1_);
predicateToUndefAuxiliaryMaps[&_p_1].push_back(&up_1_0_);
predicateToUndefAuxiliaryMaps[&_p_1].push_back(&up_1_0_1_);
predicateToUndefAuxiliaryMaps[&_p_1].push_back(&up_1_1_);
predicateToFalseAuxiliaryMaps[&_p_0].push_back(&fp_0_);
predicateToFalseAuxiliaryMaps[&_p_0].push_back(&fp_0_0_);
predicateToFalseAuxiliaryMaps[&_p_4].push_back(&fp_4_);
predicateToFalseAuxiliaryMaps[&_p_4].push_back(&fp_4_0_1_);
predicateToFalseAuxiliaryMaps[&_p_4].push_back(&fp_4_1_);
predicateToFalseAuxiliaryMaps[&_p_1].push_back(&fp_1_);
predicateToFalseAuxiliaryMaps[&_p_1].push_back(&fp_1_0_);
predicateToFalseAuxiliaryMaps[&_p_1].push_back(&fp_1_0_1_);
predicateToFalseAuxiliaryMaps[&_p_1].push_back(&fp_1_1_);
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
tuples = &pp_1_0_.getValues({4});
const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
if(tupleU == NULL){
tuplesU = &up_1_0_.getValues({4});
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
int X_1 = (*tuple0)[1];
if( (*tuple0)[0] == 4){
std::pair<AuxMap*,AuxMap*> joinTuples_0_1;
joinTuples_0_1.first = &p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_;
joinTuples_0_1.second = &u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_;
std::pair<AuxMap*,AuxMap*> joinTuples_0_1SharedVariables = *sharedVariables_0_ToAggregate_1[std::vector<int>({X_1,X_1})];
int actualSize1 = joinTuples_0_1SharedVariables.first->size();
std::set<std::vector<int>> alreadyCounted1;
for(const Tuple * joinTupleUndef : u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1})){
if(joinTuples_0_1SharedVariables.first->getValues({joinTupleUndef->at(4)}).size()==0){
    if(joinTuples_0_1SharedVariables.second->getValues({joinTupleUndef->at(4)}).size()>0){
        std::vector<int> key({joinTupleUndef->at(4)});
        alreadyCounted1.insert(key);
    }
}
}
actualSize1+=alreadyCounted1.size();
if(tupleU==NULL){
if(!(actualSize1>=0)){
    std::cout<<"conflict detected in propagator"<<std::endl;
    propagatedLiteralsAndReasons.insert({-1, std::vector<int>()});
}else{
    if(actualSize1 == 0){
        const std::vector<const Tuple*>* undefinedTuples = &u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
        for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
            if(joinTuples_0_1SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(4)}).size() == 0 && joinTuples_0_1SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(4)}).size() == 1){
                {
                    const Tuple* aggrTupleU = up_4.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                    if(aggrTupleU != NULL){
                        const auto & it = tupleToVar.find(*aggrTupleU);
                        if(it != tupleToVar.end()) {
                            int sign = aggrTupleU->isNegated() ? 1:-1;
                            auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;
                        }
                    }
                }
                {
                    const Tuple* aggrTupleU = up_1.find({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                    if(aggrTupleU != NULL){
                        const auto & it = tupleToVar.find(*aggrTupleU);
                        if(it != tupleToVar.end()) {
                            int sign = aggrTupleU->isNegated() ? 1:-1;
                            auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;
                        }
                    }
                }
                {
                    const Tuple* aggrTupleU = up_0.find({undefinedTuples->at(iUndef)->at(4)});
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
if(!(actualSize1>=0)){
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
{
const Tuple * tupleU = NULL;
bool tupleUNegated = false;
const std::vector<const Tuple* >* tuples;
tuples = &pp_1_0_.getValues({4});
const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
if(tupleU == NULL){
tuplesU = &up_1_0_.getValues({4});
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
int X_1 = (*tuple0)[1];
if( (*tuple0)[0] == 4){
std::pair<AuxMap*,AuxMap*> joinTuples_1_1;
joinTuples_1_1.first = &p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_;
joinTuples_1_1.second = &u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_;
std::pair<AuxMap*,AuxMap*> joinTuples_1_1SharedVariables = *sharedVariables_1_ToAggregate_1[std::vector<int>({X_1,X_1})];
if(tupleU==NULL){
if(joinTuples_1_1SharedVariables.first->size()>=0+1){
    std::cout<<"conflict detected in propagator"<<std::endl;
    propagatedLiteralsAndReasons.insert({-1, std::vector<int>()});
}else{
    if(joinTuples_1_1SharedVariables.first->size() == 0+1-1){
        const std::vector<const Tuple*>* undefinedTuples = &u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
        for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
            if(joinTuples_1_1SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(4)}).size() == 0){
                const Tuple* aggrTupleU=NULL;
                const std::vector<const Tuple*>* tuplesp_4;
                const std::vector<const Tuple*>* tuplesUp_4=&EMPTY_TUPLES;
                tuplesp_4= &pp_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                if(aggrTupleU==NULL)
                    tuplesUp_4= &up_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                for(int j2=0;j2<tuplesp_4->size()+tuplesUp_4->size();j2++){
                    const Tuple* auxTuple2=NULL;
                    bool addedToReason=false;
                    if(j2 < tuplesp_4->size()){
                        auxTuple2 = tuplesp_4->at(j2);
                        if(tuplesUp_4 != &EMPTY_TUPLES) {
                        }
                    }else{
                        auxTuple2 = tuplesUp_4->at(j2-tuplesp_4->size());
                        aggrTupleU = auxTuple2;
                    }
                    const std::vector<const Tuple*>* tuplesp_1;
                    const std::vector<const Tuple*>* tuplesUp_1=&EMPTY_TUPLES;
                    tuplesp_1= &pp_1_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                    if(aggrTupleU==NULL)
                        tuplesUp_1= &up_1_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                    for(int j4=0;j4<tuplesp_1->size()+tuplesUp_1->size();j4++){
                        const Tuple* auxTuple4=NULL;
                        bool addedToReason=false;
                        if(j4 < tuplesp_1->size()){
                            auxTuple4 = tuplesp_1->at(j4);
                            if(tuplesUp_1 != &EMPTY_TUPLES) {
                            }
                        }else{
                            auxTuple4 = tuplesUp_1->at(j4-tuplesp_1->size());
                            aggrTupleU = auxTuple4;
                        }
                        const std::vector<const Tuple*>* tuplesp_0;
                        const std::vector<const Tuple*>* tuplesUp_0=&EMPTY_TUPLES;
                        tuplesp_0= &pp_0_0_.getValues({undefinedTuples->at(iUndef)->at(4)});
                        if(aggrTupleU==NULL)
                            tuplesUp_0= &up_0_0_.getValues({undefinedTuples->at(iUndef)->at(4)});
                        for(int j5=0;j5<tuplesp_0->size()+tuplesUp_0->size();j5++){
                            const Tuple* auxTuple5=NULL;
                            bool addedToReason=false;
                            if(j5 < tuplesp_0->size()){
                                auxTuple5 = tuplesp_0->at(j5);
                                if(tuplesUp_0 != &EMPTY_TUPLES) {
                                }
                            }else{
                                auxTuple5 = tuplesUp_0->at(j5-tuplesp_0->size());
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
}
}else{
if(joinTuples_1_1SharedVariables.first->size()>=0+1){
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
if(facts[i] > 0){
bool tupleUNegated = false;
const Tuple * tupleU = NULL;
if(starter.getPredicateName()== &_p_4 || starter.getPredicateName()== &_p_1 || starter.getPredicateName()== &_p_0){
for(const auto sharedVarTuple : sharedVariables_1_ToAggregate_1){
int X_1 = sharedVarTuple.first[0];
std::pair<AuxMap*,AuxMap*> joinTuples_1_1;
joinTuples_1_1.first=sharedVarTuple.second->first;
joinTuples_1_1.second=sharedVarTuple.second->second;
if(joinTuples_1_1.first->size()>=0+1){
const Tuple * tuple1 = (wp_1.find({4, X_1}));
if(!tuple1 && !tupleU ){
    tuple1 = tupleU = (up_1.find({4, X_1}));
    tupleUNegated = false;
}
if(tuple1){
    if( (*tuple1)[0] == 4){
        const std::vector<const Tuple*>* trueTuples = &p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
        std::vector<int> reason;
        for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
            Tuple tuplep_4_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_4);
            const auto & itp_4_0 = tupleToVar.find(tuplep_4_0);
            if(itp_4_0!=tupleToVar.end()){
                reason.push_back(itp_4_0->second * (tuplep_4_0.isNegated() ? -1:1));
            }//closing if
            Tuple tuplep_1_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_1);
            const auto & itp_1_1 = tupleToVar.find(tuplep_1_1);
            if(itp_1_1!=tupleToVar.end()){
                reason.push_back(itp_1_1->second * (tuplep_1_1.isNegated() ? -1:1));
            }//closing if
            Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4)}),&_p_0);
            const auto & itp_0_2 = tupleToVar.find(tuplep_0_2);
            if(itp_0_2!=tupleToVar.end()){
                reason.push_back(itp_0_2->second * (tuplep_0_2.isNegated() ? -1:1));
            }//closing if
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
    }//close par
}//close par
}//close par
else{
if(joinTuples_1_1.first->size()==0+1-1){
    const Tuple * tuple1 = (wp_1.find({4, X_1}));
    if(!tuple1 && !tupleU ){
        tuple1 = tupleU = (up_1.find({4, X_1}));
        tupleUNegated = false;
    }
    if(tuple1){
        if( (*tuple1)[0] == 4){
            if(tupleU ==  NULL) {
                std::pair<AuxMap*,AuxMap*> joinTuples_1_1;
                joinTuples_1_1.first = &p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_;
                joinTuples_1_1.second = &u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_;
                std::pair<AuxMap*,AuxMap*> joinTuples_1_1SharedVariables = *sharedVariables_1_ToAggregate_1[std::vector<int>({X_1,X_1})];
                const std::vector<const Tuple*>* trueTuples = &p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
                std::vector<int> reason;
                for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                    Tuple tuplep_4_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_4);
                    const auto & itp_4_0 = tupleToVar.find(tuplep_4_0);
                    if(itp_4_0!=tupleToVar.end()){
                        reason.push_back(itp_4_0->second * (tuplep_4_0.isNegated() ? -1:1));
                    }//closing if
                    Tuple tuplep_1_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_1);
                    const auto & itp_1_1 = tupleToVar.find(tuplep_1_1);
                    if(itp_1_1!=tupleToVar.end()){
                        reason.push_back(itp_1_1->second * (tuplep_1_1.isNegated() ? -1:1));
                    }//closing if
                    Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4)}),&_p_0);
                    const auto & itp_0_2 = tupleToVar.find(tuplep_0_2);
                    if(itp_0_2!=tupleToVar.end()){
                        reason.push_back(itp_0_2->second * (tuplep_0_2.isNegated() ? -1:1));
                    }//closing if
                }
                const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                if(it_reason1!=tupleToVar.end())
                    reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                const std::vector<const Tuple*>* undefinedTuples = &u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
                for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                    if(joinTuples_1_1SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(4)}).size() == 0){
                        const Tuple* aggrTupleU=NULL;
                        const std::vector<const Tuple*>* tuplesp_4;
                        const std::vector<const Tuple*>* tuplesUp_4=&EMPTY_TUPLES;
                        tuplesp_4= &pp_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                        if(aggrTupleU==NULL)
                            tuplesUp_4= &up_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                        for(int j2=0;j2<tuplesp_4->size()+tuplesUp_4->size();j2++){
                            const Tuple* auxTuple2=NULL;
                            bool addedToReason=false;
                            if(j2 < tuplesp_4->size()){
                                auxTuple2 = tuplesp_4->at(j2);
                                const auto & it = tupleToVar.find(*auxTuple2);
                                if(it!=tupleToVar.end()){
                                    addedToReason=true;
                                    reason.push_back(it->second * (auxTuple2->isNegated() ? -1:1));
                                }//closing if
                                if(tuplesUp_4 != &EMPTY_TUPLES) {
                                }
                            }else{
                                auxTuple2 = tuplesUp_4->at(j2-tuplesp_4->size());
                                aggrTupleU = auxTuple2;
                            }
                            const std::vector<const Tuple*>* tuplesp_1;
                            const std::vector<const Tuple*>* tuplesUp_1=&EMPTY_TUPLES;
                            tuplesp_1= &pp_1_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                            if(aggrTupleU==NULL)
                                tuplesUp_1= &up_1_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                            for(int j4=0;j4<tuplesp_1->size()+tuplesUp_1->size();j4++){
                                const Tuple* auxTuple4=NULL;
                                bool addedToReason=false;
                                if(j4 < tuplesp_1->size()){
                                    auxTuple4 = tuplesp_1->at(j4);
                                    const auto & it = tupleToVar.find(*auxTuple4);
                                    if(it!=tupleToVar.end()){
                                        addedToReason=true;
                                        reason.push_back(it->second * (auxTuple4->isNegated() ? -1:1));
                                    }//closing if
                                    if(tuplesUp_1 != &EMPTY_TUPLES) {
                                    }
                                }else{
                                    auxTuple4 = tuplesUp_1->at(j4-tuplesp_1->size());
                                    aggrTupleU = auxTuple4;
                                }
                                const std::vector<const Tuple*>* tuplesp_0;
                                const std::vector<const Tuple*>* tuplesUp_0=&EMPTY_TUPLES;
                                tuplesp_0= &pp_0_0_.getValues({undefinedTuples->at(iUndef)->at(4)});
                                if(aggrTupleU==NULL)
                                    tuplesUp_0= &up_0_0_.getValues({undefinedTuples->at(iUndef)->at(4)});
                                for(int j5=0;j5<tuplesp_0->size()+tuplesUp_0->size();j5++){
                                    const Tuple* auxTuple5=NULL;
                                    bool addedToReason=false;
                                    if(j5 < tuplesp_0->size()){
                                        auxTuple5 = tuplesp_0->at(j5);
                                        const auto & it = tupleToVar.find(*auxTuple5);
                                        if(it!=tupleToVar.end()){
                                            addedToReason=true;
                                            reason.push_back(it->second * (auxTuple5->isNegated() ? -1:1));
                                        }//closing if
                                        if(tuplesUp_0 != &EMPTY_TUPLES) {
                                        }
                                    }else{
                                        auxTuple5 = tuplesUp_0->at(j5-tuplesp_0->size());
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
if(starter.getPredicateName() == &_p_1) { 
const Tuple * tuple0 = &starter;
if(facts[i] > 0){
{
const Tuple * tupleU = NULL;
bool tupleUNegated = false;
int X_1 = (*tuple0)[1];
if( (*tuple0)[0] == 4){
std::pair<AuxMap*,AuxMap*> joinTuples_0_1;
joinTuples_0_1.first = &p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_;
joinTuples_0_1.second = &u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_;
std::pair<AuxMap*,AuxMap*> joinTuples_0_1SharedVariables = *sharedVariables_0_ToAggregate_1[std::vector<int>({X_1,X_1})];
int actualSize1 = joinTuples_0_1SharedVariables.first->size();
std::set<std::vector<int>> alreadyCounted1;
for(const Tuple * joinTupleUndef : u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1})){
    if(joinTuples_0_1SharedVariables.first->getValues({joinTupleUndef->at(4)}).size()==0){
        if(joinTuples_0_1SharedVariables.second->getValues({joinTupleUndef->at(4)}).size()>0){
            std::vector<int> key({joinTupleUndef->at(4)});
            alreadyCounted1.insert(key);
        }
    }
}
actualSize1+=alreadyCounted1.size();
if(tupleU==NULL){
    std::vector<int> reason;
    const std::vector<const Tuple*>* falseTuples0 = &fp_4_.getValues({});
    for(int i=0;i<falseTuples0->size();i++){
        bool joiningTupleFound=false;
        int X_4 = falseTuples0->at(i)->at(0);
        int X_1 = falseTuples0->at(i)->at(1);
        const std::vector<const Tuple*>* trueTuples1 = &pp_1_1_.getValues({X_1});
        const std::vector<const Tuple*>* undefTuples1 = &up_1_1_.getValues({X_1});
        const std::vector<const Tuple*>* falseTuples1 = &fp_1_1_.getValues({X_1});
        for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
            const Tuple * tuple1=NULL;
            if(i_1<trueTuples1->size())
                tuple1=trueTuples1->at(i_1);
            else if(i_1<trueTuples1->size()+undefTuples1->size())
                tuple1=undefTuples1->at(i_1-trueTuples1->size());
            else
                tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
            int X_0 = tuple1->at(0);
            const std::vector<const Tuple*>* trueTuples2 = &pp_0_.getValues({});
            const std::vector<const Tuple*>* undefTuples2 = &up_0_.getValues({});
            const std::vector<const Tuple*>* falseTuples2 = &fp_0_.getValues({});
            for(int i_2=0;!joiningTupleFound && i_2 < trueTuples2->size()+undefTuples2->size()+falseTuples2->size();i_2++){
                const Tuple * tuple2=NULL;
                if(i_2<trueTuples2->size())
                    tuple2=trueTuples2->at(i_2);
                else if(i_2<trueTuples2->size()+undefTuples2->size())
                    tuple2=undefTuples2->at(i_2-trueTuples2->size());
                else
                    tuple2=falseTuples2->at(i_2-trueTuples2->size()-undefTuples2->size());
                joiningTupleFound=true;
            }
        }
        if(joiningTupleFound){
            const auto & it = tupleToVar.find(*falseTuples0->at(i));
            if(it!=tupleToVar.end()){
                reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
            }//closing if
        }
    }
    const std::vector<const Tuple*>* falseTuples1 = &fp_1_.getValues({});
    for(int i=0;i<falseTuples1->size();i++){
        bool joiningTupleFound=false;
        int X_0 = falseTuples1->at(i)->at(0);
        int X_1 = falseTuples1->at(i)->at(1);
        const std::vector<const Tuple*>* trueTuples0 = &pp_4_1_.getValues({X_1});
        const std::vector<const Tuple*>* undefTuples0 = &up_4_1_.getValues({X_1});
        const std::vector<const Tuple*>* falseTuples0 = &fp_4_1_.getValues({X_1});
        for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
            const Tuple * tuple0=NULL;
            if(i_0<trueTuples0->size())
                tuple0=trueTuples0->at(i_0);
            else if(i_0<trueTuples0->size()+undefTuples0->size())
                tuple0=undefTuples0->at(i_0-trueTuples0->size());
            else
                tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
            int X_4 = tuple0->at(0);
            const std::vector<const Tuple*>* trueTuples2 = &pp_0_.getValues({});
            const std::vector<const Tuple*>* undefTuples2 = &up_0_.getValues({});
            const std::vector<const Tuple*>* falseTuples2 = &fp_0_.getValues({});
            for(int i_2=0;!joiningTupleFound && i_2 < trueTuples2->size()+undefTuples2->size()+falseTuples2->size();i_2++){
                const Tuple * tuple2=NULL;
                if(i_2<trueTuples2->size())
                    tuple2=trueTuples2->at(i_2);
                else if(i_2<trueTuples2->size()+undefTuples2->size())
                    tuple2=undefTuples2->at(i_2-trueTuples2->size());
                else
                    tuple2=falseTuples2->at(i_2-trueTuples2->size()-undefTuples2->size());
                joiningTupleFound=true;
            }
        }
        if(joiningTupleFound){
            const auto & it = tupleToVar.find(*falseTuples1->at(i));
            if(it!=tupleToVar.end()){
                reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
            }//closing if
        }
    }
    const std::vector<const Tuple*>* falseTuples2 = &fp_0_.getValues({});
    for(int i=0;i<falseTuples2->size();i++){
        bool joiningTupleFound=false;
        int X_3 = falseTuples2->at(i)->at(0);
        const std::vector<const Tuple*>* trueTuples0 = &pp_4_.getValues({});
        const std::vector<const Tuple*>* undefTuples0 = &up_4_.getValues({});
        const std::vector<const Tuple*>* falseTuples0 = &fp_4_.getValues({});
        for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
            const Tuple * tuple0=NULL;
            if(i_0<trueTuples0->size())
                tuple0=trueTuples0->at(i_0);
            else if(i_0<trueTuples0->size()+undefTuples0->size())
                tuple0=undefTuples0->at(i_0-trueTuples0->size());
            else
                tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
            int X_4 = tuple0->at(0);
            int X_1 = tuple0->at(1);
            const std::vector<const Tuple*>* trueTuples1 = &pp_1_1_.getValues({X_1});
            const std::vector<const Tuple*>* undefTuples1 = &up_1_1_.getValues({X_1});
            const std::vector<const Tuple*>* falseTuples1 = &fp_1_1_.getValues({X_1});
            for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                const Tuple * tuple1=NULL;
                if(i_1<trueTuples1->size())
                    tuple1=trueTuples1->at(i_1);
                else if(i_1<trueTuples1->size()+undefTuples1->size())
                    tuple1=undefTuples1->at(i_1-trueTuples1->size());
                else
                    tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                joiningTupleFound=true;
            }
        }
        if(joiningTupleFound){
            const auto & it = tupleToVar.find(*falseTuples2->at(i));
            if(it!=tupleToVar.end()){
                reason.push_back(it->second * (falseTuples2->at(i)->isNegated() ? 1:-1));
            }//closing if
        }
    }
    if(tuple0!=tupleU){
        const auto & it_reason0 = tupleToVar.find(Tuple(*tuple0));
        if(it_reason0!=tupleToVar.end())
            reason.push_back(it_reason0->second * (tuple0->isNegated() ? -1:1));
    }
    if(!(actualSize1>=0)){
        std::cout<<"conflict detected in propagator"<<std::endl;
        propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
    }else{
        if(actualSize1 == 0){
            const std::vector<const Tuple*>* undefinedTuples = &u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
            for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                if(joinTuples_0_1SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(4)}).size() == 0 && joinTuples_0_1SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(4)}).size() == 1){
                    {
                        const Tuple* aggrTupleU = up_4.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                        if(aggrTupleU != NULL){
                            const auto & it = tupleToVar.find(*aggrTupleU);
                            if(it != tupleToVar.end()) {
                                int sign = aggrTupleU->isNegated() ? 1:-1;
                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                            }
                        }
                    }
                    {
                        const Tuple* aggrTupleU = up_1.find({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                        if(aggrTupleU != NULL){
                            const auto & it = tupleToVar.find(*aggrTupleU);
                            if(it != tupleToVar.end()) {
                                int sign = aggrTupleU->isNegated() ? 1:-1;
                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                            }
                        }
                    }
                    {
                        const Tuple* aggrTupleU = up_0.find({undefinedTuples->at(iUndef)->at(4)});
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
const std::vector<const Tuple*>* falseTuples0 = &fp_4_.getValues({});
for(int i=0;i<falseTuples0->size();i++){
    bool joiningTupleFound=false;
    int X_4 = falseTuples0->at(i)->at(0);
    int X_1 = falseTuples0->at(i)->at(1);
    const std::vector<const Tuple*>* trueTuples1 = &pp_1_1_.getValues({X_1});
    const std::vector<const Tuple*>* undefTuples1 = &up_1_1_.getValues({X_1});
    const std::vector<const Tuple*>* falseTuples1 = &fp_1_1_.getValues({X_1});
    for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
        const Tuple * tuple1=NULL;
        if(i_1<trueTuples1->size())
            tuple1=trueTuples1->at(i_1);
        else if(i_1<trueTuples1->size()+undefTuples1->size())
            tuple1=undefTuples1->at(i_1-trueTuples1->size());
        else
            tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
        int X_0 = tuple1->at(0);
        const std::vector<const Tuple*>* trueTuples2 = &pp_0_.getValues({});
        const std::vector<const Tuple*>* undefTuples2 = &up_0_.getValues({});
        const std::vector<const Tuple*>* falseTuples2 = &fp_0_.getValues({});
        for(int i_2=0;!joiningTupleFound && i_2 < trueTuples2->size()+undefTuples2->size()+falseTuples2->size();i_2++){
            const Tuple * tuple2=NULL;
            if(i_2<trueTuples2->size())
                tuple2=trueTuples2->at(i_2);
            else if(i_2<trueTuples2->size()+undefTuples2->size())
                tuple2=undefTuples2->at(i_2-trueTuples2->size());
            else
                tuple2=falseTuples2->at(i_2-trueTuples2->size()-undefTuples2->size());
            joiningTupleFound=true;
        }
    }
    if(joiningTupleFound){
        const auto & it = tupleToVar.find(*falseTuples0->at(i));
        if(it!=tupleToVar.end()){
            reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
        }//closing if
    }
}
const std::vector<const Tuple*>* falseTuples1 = &fp_1_.getValues({});
for(int i=0;i<falseTuples1->size();i++){
    bool joiningTupleFound=false;
    int X_0 = falseTuples1->at(i)->at(0);
    int X_1 = falseTuples1->at(i)->at(1);
    const std::vector<const Tuple*>* trueTuples0 = &pp_4_1_.getValues({X_1});
    const std::vector<const Tuple*>* undefTuples0 = &up_4_1_.getValues({X_1});
    const std::vector<const Tuple*>* falseTuples0 = &fp_4_1_.getValues({X_1});
    for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
        const Tuple * tuple0=NULL;
        if(i_0<trueTuples0->size())
            tuple0=trueTuples0->at(i_0);
        else if(i_0<trueTuples0->size()+undefTuples0->size())
            tuple0=undefTuples0->at(i_0-trueTuples0->size());
        else
            tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
        int X_4 = tuple0->at(0);
        const std::vector<const Tuple*>* trueTuples2 = &pp_0_.getValues({});
        const std::vector<const Tuple*>* undefTuples2 = &up_0_.getValues({});
        const std::vector<const Tuple*>* falseTuples2 = &fp_0_.getValues({});
        for(int i_2=0;!joiningTupleFound && i_2 < trueTuples2->size()+undefTuples2->size()+falseTuples2->size();i_2++){
            const Tuple * tuple2=NULL;
            if(i_2<trueTuples2->size())
                tuple2=trueTuples2->at(i_2);
            else if(i_2<trueTuples2->size()+undefTuples2->size())
                tuple2=undefTuples2->at(i_2-trueTuples2->size());
            else
                tuple2=falseTuples2->at(i_2-trueTuples2->size()-undefTuples2->size());
            joiningTupleFound=true;
        }
    }
    if(joiningTupleFound){
        const auto & it = tupleToVar.find(*falseTuples1->at(i));
        if(it!=tupleToVar.end()){
            reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
        }//closing if
    }
}
const std::vector<const Tuple*>* falseTuples2 = &fp_0_.getValues({});
for(int i=0;i<falseTuples2->size();i++){
    bool joiningTupleFound=false;
    int X_3 = falseTuples2->at(i)->at(0);
    const std::vector<const Tuple*>* trueTuples0 = &pp_4_.getValues({});
    const std::vector<const Tuple*>* undefTuples0 = &up_4_.getValues({});
    const std::vector<const Tuple*>* falseTuples0 = &fp_4_.getValues({});
    for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
        const Tuple * tuple0=NULL;
        if(i_0<trueTuples0->size())
            tuple0=trueTuples0->at(i_0);
        else if(i_0<trueTuples0->size()+undefTuples0->size())
            tuple0=undefTuples0->at(i_0-trueTuples0->size());
        else
            tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
        int X_4 = tuple0->at(0);
        int X_1 = tuple0->at(1);
        const std::vector<const Tuple*>* trueTuples1 = &pp_1_1_.getValues({X_1});
        const std::vector<const Tuple*>* undefTuples1 = &up_1_1_.getValues({X_1});
        const std::vector<const Tuple*>* falseTuples1 = &fp_1_1_.getValues({X_1});
        for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
            const Tuple * tuple1=NULL;
            if(i_1<trueTuples1->size())
                tuple1=trueTuples1->at(i_1);
            else if(i_1<trueTuples1->size()+undefTuples1->size())
                tuple1=undefTuples1->at(i_1-trueTuples1->size());
            else
                tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
            joiningTupleFound=true;
        }
    }
    if(joiningTupleFound){
        const auto & it = tupleToVar.find(*falseTuples2->at(i));
        if(it!=tupleToVar.end()){
            reason.push_back(it->second * (falseTuples2->at(i)->isNegated() ? 1:-1));
        }//closing if
    }
}
if(tuple0!=tupleU){
    const auto & it_reason0 = tupleToVar.find(Tuple(*tuple0));
    if(it_reason0!=tupleToVar.end())
        reason.push_back(it_reason0->second * (tuple0->isNegated() ? -1:1));
}
    if(!(actualSize1>=0)){
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
if(facts[i] > 0){
{
const Tuple * tupleU = NULL;
bool tupleUNegated = false;
int X_1 = (*tuple0)[1];
if( (*tuple0)[0] == 4){
std::pair<AuxMap*,AuxMap*> joinTuples_1_1;
joinTuples_1_1.first = &p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_;
joinTuples_1_1.second = &u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_;
std::pair<AuxMap*,AuxMap*> joinTuples_1_1SharedVariables = *sharedVariables_1_ToAggregate_1[std::vector<int>({X_1,X_1})];
if(tupleU==NULL){
    const std::vector<const Tuple*>* trueTuples = &p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
    std::vector<int> reason;
    for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
        Tuple tuplep_4_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_4);
        const auto & itp_4_0 = tupleToVar.find(tuplep_4_0);
        if(itp_4_0!=tupleToVar.end()){
            reason.push_back(itp_4_0->second * (tuplep_4_0.isNegated() ? -1:1));
        }//closing if
        Tuple tuplep_1_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_1);
        const auto & itp_1_1 = tupleToVar.find(tuplep_1_1);
        if(itp_1_1!=tupleToVar.end()){
            reason.push_back(itp_1_1->second * (tuplep_1_1.isNegated() ? -1:1));
        }//closing if
        Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4)}),&_p_0);
        const auto & itp_0_2 = tupleToVar.find(tuplep_0_2);
        if(itp_0_2!=tupleToVar.end()){
            reason.push_back(itp_0_2->second * (tuplep_0_2.isNegated() ? -1:1));
        }//closing if
    }
    if(tuple0!=tupleU){
        const auto & it_reason0 = tupleToVar.find(Tuple(*tuple0));
        if(it_reason0!=tupleToVar.end())
            reason.push_back(it_reason0->second * (tuple0->isNegated() ? -1:1));
    }
    if(joinTuples_1_1SharedVariables.first->size()>=0+1){
        std::cout<<"conflict detected in propagator"<<std::endl;
        propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
    }else{
        if(joinTuples_1_1SharedVariables.first->size() == 0+1-1){
            const std::vector<const Tuple*>* undefinedTuples = &u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
            for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                if(joinTuples_1_1SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(4)}).size() == 0){
                    const Tuple* aggrTupleU=NULL;
                    const std::vector<const Tuple*>* tuplesp_4;
                    const std::vector<const Tuple*>* tuplesUp_4=&EMPTY_TUPLES;
                    tuplesp_4= &pp_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                    if(aggrTupleU==NULL)
                        tuplesUp_4= &up_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                    for(int j2=0;j2<tuplesp_4->size()+tuplesUp_4->size();j2++){
                        const Tuple* auxTuple2=NULL;
                        bool addedToReason=false;
                        if(j2 < tuplesp_4->size()){
                            auxTuple2 = tuplesp_4->at(j2);
                            const auto & it = tupleToVar.find(*auxTuple2);
                            if(it!=tupleToVar.end()){
                                addedToReason=true;
                                reason.push_back(it->second * (auxTuple2->isNegated() ? -1:1));
                            }//closing if
                            if(tuplesUp_4 != &EMPTY_TUPLES) {
                            }
                        }else{
                            auxTuple2 = tuplesUp_4->at(j2-tuplesp_4->size());
                            aggrTupleU = auxTuple2;
                        }
                        const std::vector<const Tuple*>* tuplesp_1;
                        const std::vector<const Tuple*>* tuplesUp_1=&EMPTY_TUPLES;
                        tuplesp_1= &pp_1_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                        if(aggrTupleU==NULL)
                            tuplesUp_1= &up_1_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                        for(int j4=0;j4<tuplesp_1->size()+tuplesUp_1->size();j4++){
                            const Tuple* auxTuple4=NULL;
                            bool addedToReason=false;
                            if(j4 < tuplesp_1->size()){
                                auxTuple4 = tuplesp_1->at(j4);
                                const auto & it = tupleToVar.find(*auxTuple4);
                                if(it!=tupleToVar.end()){
                                    addedToReason=true;
                                    reason.push_back(it->second * (auxTuple4->isNegated() ? -1:1));
                                }//closing if
                                if(tuplesUp_1 != &EMPTY_TUPLES) {
                                }
                            }else{
                                auxTuple4 = tuplesUp_1->at(j4-tuplesp_1->size());
                                aggrTupleU = auxTuple4;
                            }
                            const std::vector<const Tuple*>* tuplesp_0;
                            const std::vector<const Tuple*>* tuplesUp_0=&EMPTY_TUPLES;
                            tuplesp_0= &pp_0_0_.getValues({undefinedTuples->at(iUndef)->at(4)});
                            if(aggrTupleU==NULL)
                                tuplesUp_0= &up_0_0_.getValues({undefinedTuples->at(iUndef)->at(4)});
                            for(int j5=0;j5<tuplesp_0->size()+tuplesUp_0->size();j5++){
                                const Tuple* auxTuple5=NULL;
                                bool addedToReason=false;
                                if(j5 < tuplesp_0->size()){
                                    auxTuple5 = tuplesp_0->at(j5);
                                    const auto & it = tupleToVar.find(*auxTuple5);
                                    if(it!=tupleToVar.end()){
                                        addedToReason=true;
                                        reason.push_back(it->second * (auxTuple5->isNegated() ? -1:1));
                                    }//closing if
                                    if(tuplesUp_0 != &EMPTY_TUPLES) {
                                    }
                                }else{
                                    auxTuple5 = tuplesUp_0->at(j5-tuplesp_0->size());
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
                        if(addedToReason) reason.pop_back();
                    }
                }
            }
        }
    }
}else{
const std::vector<const Tuple*>* trueTuples = &p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
std::vector<int> reason;
for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
    Tuple tuplep_4_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_4);
    const auto & itp_4_0 = tupleToVar.find(tuplep_4_0);
    if(itp_4_0!=tupleToVar.end()){
        reason.push_back(itp_4_0->second * (tuplep_4_0.isNegated() ? -1:1));
    }//closing if
    Tuple tuplep_1_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_1);
    const auto & itp_1_1 = tupleToVar.find(tuplep_1_1);
    if(itp_1_1!=tupleToVar.end()){
        reason.push_back(itp_1_1->second * (tuplep_1_1.isNegated() ? -1:1));
    }//closing if
    Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4)}),&_p_0);
    const auto & itp_0_2 = tupleToVar.find(tuplep_0_2);
    if(itp_0_2!=tupleToVar.end()){
        reason.push_back(itp_0_2->second * (tuplep_0_2.isNegated() ? -1:1));
    }//closing if
}
if(tuple0!=tupleU){
    const auto & it_reason0 = tupleToVar.find(Tuple(*tuple0));
    if(it_reason0!=tupleToVar.end())
        reason.push_back(it_reason0->second * (tuple0->isNegated() ? -1:1));
}
    if(joinTuples_1_1SharedVariables.first->size()>=0+1){
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
if(facts[i] < 0){
bool tupleUNegated = false;
const Tuple * tupleU = NULL;
if(starter.getPredicateName()== &_p_4 || starter.getPredicateName()== &_p_1 || starter.getPredicateName()== &_p_0){
for(const auto sharedVarTuple : sharedVariables_0_ToAggregate_1){
int X_1 = sharedVarTuple.first[0];
std::pair<AuxMap*,AuxMap*> joinTuples_0_1;
joinTuples_0_1.first=sharedVarTuple.second->first;
joinTuples_0_1.second=sharedVarTuple.second->second;
int actualSize = joinTuples_0_1.first->size();
std::set<std::vector<int>> alreadyCounted;
for(const Tuple * joinTupleUndef : u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1})){
if(joinTuples_0_1.first->getValues({joinTupleUndef->at(4)}).size()==0){
    if(joinTuples_0_1.second->getValues({joinTupleUndef->at(4)}).size()>0){
        std::vector<int> key({joinTupleUndef->at(4)});
        alreadyCounted.insert(key);
    }
}
}
actualSize+=alreadyCounted.size();
if(!(actualSize>=0)){
const Tuple * tuple1 = (wp_1.find({4, X_1}));
if(!tuple1 && !tupleU ){
    tuple1 = tupleU = (up_1.find({4, X_1}));
    tupleUNegated = false;
}
if(tuple1){
    if( (*tuple1)[0] == 4){
        std::vector<int> reason;
        const std::vector<const Tuple*>* falseTuples0 = &fp_4_.getValues({});
        for(int i=0;i<falseTuples0->size();i++){
            bool joiningTupleFound=false;
            int X_4 = falseTuples0->at(i)->at(0);
            int X_1 = falseTuples0->at(i)->at(1);
            const std::vector<const Tuple*>* trueTuples1 = &pp_1_1_.getValues({X_1});
            const std::vector<const Tuple*>* undefTuples1 = &up_1_1_.getValues({X_1});
            const std::vector<const Tuple*>* falseTuples1 = &fp_1_1_.getValues({X_1});
            for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                const Tuple * tuple1=NULL;
                if(i_1<trueTuples1->size())
                    tuple1=trueTuples1->at(i_1);
                else if(i_1<trueTuples1->size()+undefTuples1->size())
                    tuple1=undefTuples1->at(i_1-trueTuples1->size());
                else
                    tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                int X_0 = tuple1->at(0);
                const std::vector<const Tuple*>* trueTuples2 = &pp_0_.getValues({});
                const std::vector<const Tuple*>* undefTuples2 = &up_0_.getValues({});
                const std::vector<const Tuple*>* falseTuples2 = &fp_0_.getValues({});
                for(int i_2=0;!joiningTupleFound && i_2 < trueTuples2->size()+undefTuples2->size()+falseTuples2->size();i_2++){
                    const Tuple * tuple2=NULL;
                    if(i_2<trueTuples2->size())
                        tuple2=trueTuples2->at(i_2);
                    else if(i_2<trueTuples2->size()+undefTuples2->size())
                        tuple2=undefTuples2->at(i_2-trueTuples2->size());
                    else
                        tuple2=falseTuples2->at(i_2-trueTuples2->size()-undefTuples2->size());
                    joiningTupleFound=true;
                }
            }
            if(joiningTupleFound){
                const auto & it = tupleToVar.find(*falseTuples0->at(i));
                if(it!=tupleToVar.end()){
                    reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                }//closing if
            }
        }
        const std::vector<const Tuple*>* falseTuples1 = &fp_1_.getValues({});
        for(int i=0;i<falseTuples1->size();i++){
            bool joiningTupleFound=false;
            int X_0 = falseTuples1->at(i)->at(0);
            int X_1 = falseTuples1->at(i)->at(1);
            const std::vector<const Tuple*>* trueTuples0 = &pp_4_1_.getValues({X_1});
            const std::vector<const Tuple*>* undefTuples0 = &up_4_1_.getValues({X_1});
            const std::vector<const Tuple*>* falseTuples0 = &fp_4_1_.getValues({X_1});
            for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                const Tuple * tuple0=NULL;
                if(i_0<trueTuples0->size())
                    tuple0=trueTuples0->at(i_0);
                else if(i_0<trueTuples0->size()+undefTuples0->size())
                    tuple0=undefTuples0->at(i_0-trueTuples0->size());
                else
                    tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                int X_4 = tuple0->at(0);
                const std::vector<const Tuple*>* trueTuples2 = &pp_0_.getValues({});
                const std::vector<const Tuple*>* undefTuples2 = &up_0_.getValues({});
                const std::vector<const Tuple*>* falseTuples2 = &fp_0_.getValues({});
                for(int i_2=0;!joiningTupleFound && i_2 < trueTuples2->size()+undefTuples2->size()+falseTuples2->size();i_2++){
                    const Tuple * tuple2=NULL;
                    if(i_2<trueTuples2->size())
                        tuple2=trueTuples2->at(i_2);
                    else if(i_2<trueTuples2->size()+undefTuples2->size())
                        tuple2=undefTuples2->at(i_2-trueTuples2->size());
                    else
                        tuple2=falseTuples2->at(i_2-trueTuples2->size()-undefTuples2->size());
                    joiningTupleFound=true;
                }
            }
            if(joiningTupleFound){
                const auto & it = tupleToVar.find(*falseTuples1->at(i));
                if(it!=tupleToVar.end()){
                    reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
                }//closing if
            }
        }
        const std::vector<const Tuple*>* falseTuples2 = &fp_0_.getValues({});
        for(int i=0;i<falseTuples2->size();i++){
            bool joiningTupleFound=false;
            int X_3 = falseTuples2->at(i)->at(0);
            const std::vector<const Tuple*>* trueTuples0 = &pp_4_.getValues({});
            const std::vector<const Tuple*>* undefTuples0 = &up_4_.getValues({});
            const std::vector<const Tuple*>* falseTuples0 = &fp_4_.getValues({});
            for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                const Tuple * tuple0=NULL;
                if(i_0<trueTuples0->size())
                    tuple0=trueTuples0->at(i_0);
                else if(i_0<trueTuples0->size()+undefTuples0->size())
                    tuple0=undefTuples0->at(i_0-trueTuples0->size());
                else
                    tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                int X_4 = tuple0->at(0);
                int X_1 = tuple0->at(1);
                const std::vector<const Tuple*>* trueTuples1 = &pp_1_1_.getValues({X_1});
                const std::vector<const Tuple*>* undefTuples1 = &up_1_1_.getValues({X_1});
                const std::vector<const Tuple*>* falseTuples1 = &fp_1_1_.getValues({X_1});
                for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                    const Tuple * tuple1=NULL;
                    if(i_1<trueTuples1->size())
                        tuple1=trueTuples1->at(i_1);
                    else if(i_1<trueTuples1->size()+undefTuples1->size())
                        tuple1=undefTuples1->at(i_1-trueTuples1->size());
                    else
                        tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                    joiningTupleFound=true;
                }
            }
            if(joiningTupleFound){
                const auto & it = tupleToVar.find(*falseTuples2->at(i));
                if(it!=tupleToVar.end()){
                    reason.push_back(it->second * (falseTuples2->at(i)->isNegated() ? 1:-1));
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
    }//close par
}//close par
}//close par
else{
if(actualSize==0){
    const Tuple * tuple1 = (wp_1.find({4, X_1}));
    if(!tuple1 && !tupleU ){
        tuple1 = tupleU = (up_1.find({4, X_1}));
        tupleUNegated = false;
    }
    if(tuple1){
        if( (*tuple1)[0] == 4){
            if(tupleU ==  NULL) {
                std::pair<AuxMap*,AuxMap*> joinTuples_0_1;
                joinTuples_0_1.first = &p_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_;
                joinTuples_0_1.second = &u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_;
                std::pair<AuxMap*,AuxMap*> joinTuples_0_1SharedVariables = *sharedVariables_0_ToAggregate_1[std::vector<int>({X_1,X_1})];
                std::vector<int> reason;
                const std::vector<const Tuple*>* falseTuples0 = &fp_4_.getValues({});
                for(int i=0;i<falseTuples0->size();i++){
                    bool joiningTupleFound=false;
                    int X_4 = falseTuples0->at(i)->at(0);
                    int X_1 = falseTuples0->at(i)->at(1);
                    const std::vector<const Tuple*>* trueTuples1 = &pp_1_1_.getValues({X_1});
                    const std::vector<const Tuple*>* undefTuples1 = &up_1_1_.getValues({X_1});
                    const std::vector<const Tuple*>* falseTuples1 = &fp_1_1_.getValues({X_1});
                    for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                        const Tuple * tuple1=NULL;
                        if(i_1<trueTuples1->size())
                            tuple1=trueTuples1->at(i_1);
                        else if(i_1<trueTuples1->size()+undefTuples1->size())
                            tuple1=undefTuples1->at(i_1-trueTuples1->size());
                        else
                            tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                        int X_0 = tuple1->at(0);
                        const std::vector<const Tuple*>* trueTuples2 = &pp_0_.getValues({});
                        const std::vector<const Tuple*>* undefTuples2 = &up_0_.getValues({});
                        const std::vector<const Tuple*>* falseTuples2 = &fp_0_.getValues({});
                        for(int i_2=0;!joiningTupleFound && i_2 < trueTuples2->size()+undefTuples2->size()+falseTuples2->size();i_2++){
                            const Tuple * tuple2=NULL;
                            if(i_2<trueTuples2->size())
                                tuple2=trueTuples2->at(i_2);
                            else if(i_2<trueTuples2->size()+undefTuples2->size())
                                tuple2=undefTuples2->at(i_2-trueTuples2->size());
                            else
                                tuple2=falseTuples2->at(i_2-trueTuples2->size()-undefTuples2->size());
                            joiningTupleFound=true;
                        }
                    }
                    if(joiningTupleFound){
                        const auto & it = tupleToVar.find(*falseTuples0->at(i));
                        if(it!=tupleToVar.end()){
                            reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                        }//closing if
                    }
                }
                const std::vector<const Tuple*>* falseTuples1 = &fp_1_.getValues({});
                for(int i=0;i<falseTuples1->size();i++){
                    bool joiningTupleFound=false;
                    int X_0 = falseTuples1->at(i)->at(0);
                    int X_1 = falseTuples1->at(i)->at(1);
                    const std::vector<const Tuple*>* trueTuples0 = &pp_4_1_.getValues({X_1});
                    const std::vector<const Tuple*>* undefTuples0 = &up_4_1_.getValues({X_1});
                    const std::vector<const Tuple*>* falseTuples0 = &fp_4_1_.getValues({X_1});
                    for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                        const Tuple * tuple0=NULL;
                        if(i_0<trueTuples0->size())
                            tuple0=trueTuples0->at(i_0);
                        else if(i_0<trueTuples0->size()+undefTuples0->size())
                            tuple0=undefTuples0->at(i_0-trueTuples0->size());
                        else
                            tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                        int X_4 = tuple0->at(0);
                        const std::vector<const Tuple*>* trueTuples2 = &pp_0_.getValues({});
                        const std::vector<const Tuple*>* undefTuples2 = &up_0_.getValues({});
                        const std::vector<const Tuple*>* falseTuples2 = &fp_0_.getValues({});
                        for(int i_2=0;!joiningTupleFound && i_2 < trueTuples2->size()+undefTuples2->size()+falseTuples2->size();i_2++){
                            const Tuple * tuple2=NULL;
                            if(i_2<trueTuples2->size())
                                tuple2=trueTuples2->at(i_2);
                            else if(i_2<trueTuples2->size()+undefTuples2->size())
                                tuple2=undefTuples2->at(i_2-trueTuples2->size());
                            else
                                tuple2=falseTuples2->at(i_2-trueTuples2->size()-undefTuples2->size());
                            joiningTupleFound=true;
                        }
                    }
                    if(joiningTupleFound){
                        const auto & it = tupleToVar.find(*falseTuples1->at(i));
                        if(it!=tupleToVar.end()){
                            reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
                        }//closing if
                    }
                }
                const std::vector<const Tuple*>* falseTuples2 = &fp_0_.getValues({});
                for(int i=0;i<falseTuples2->size();i++){
                    bool joiningTupleFound=false;
                    int X_3 = falseTuples2->at(i)->at(0);
                    const std::vector<const Tuple*>* trueTuples0 = &pp_4_.getValues({});
                    const std::vector<const Tuple*>* undefTuples0 = &up_4_.getValues({});
                    const std::vector<const Tuple*>* falseTuples0 = &fp_4_.getValues({});
                    for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                        const Tuple * tuple0=NULL;
                        if(i_0<trueTuples0->size())
                            tuple0=trueTuples0->at(i_0);
                        else if(i_0<trueTuples0->size()+undefTuples0->size())
                            tuple0=undefTuples0->at(i_0-trueTuples0->size());
                        else
                            tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                        int X_4 = tuple0->at(0);
                        int X_1 = tuple0->at(1);
                        const std::vector<const Tuple*>* trueTuples1 = &pp_1_1_.getValues({X_1});
                        const std::vector<const Tuple*>* undefTuples1 = &up_1_1_.getValues({X_1});
                        const std::vector<const Tuple*>* falseTuples1 = &fp_1_1_.getValues({X_1});
                        for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                            const Tuple * tuple1=NULL;
                            if(i_1<trueTuples1->size())
                                tuple1=trueTuples1->at(i_1);
                            else if(i_1<trueTuples1->size()+undefTuples1->size())
                                tuple1=undefTuples1->at(i_1-trueTuples1->size());
                            else
                                tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                            joiningTupleFound=true;
                        }
                    }
                    if(joiningTupleFound){
                        const auto & it = tupleToVar.find(*falseTuples2->at(i));
                        if(it!=tupleToVar.end()){
                            reason.push_back(it->second * (falseTuples2->at(i)->isNegated() ? 1:-1));
                        }//closing if
                    }
                }
                const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                if(it_reason1!=tupleToVar.end())
                    reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                const std::vector<const Tuple*>* undefinedTuples = &u_p_4_X_4_X_1_p_1_X_0_X_1_p_0_X_3_1_3_.getValues({X_1,X_1});
                for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                    if(joinTuples_0_1SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(4)}).size() == 0 && joinTuples_0_1SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(4)}).size() == 1){
                        {
                            const Tuple* aggrTupleU = up_4.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                            if(aggrTupleU != NULL){
                                const auto & it = tupleToVar.find(*aggrTupleU);
                                if(it != tupleToVar.end()) {
                                    int sign = aggrTupleU->isNegated() ? 1:-1;
                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                }
                            }
                        }
                        {
                            const Tuple* aggrTupleU = up_1.find({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                            if(aggrTupleU != NULL){
                                const auto & it = tupleToVar.find(*aggrTupleU);
                                if(it != tupleToVar.end()) {
                                    int sign = aggrTupleU->isNegated() ? 1:-1;
                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                }
                            }
                        }
                        {
                            const Tuple* aggrTupleU = up_0.find({undefinedTuples->at(iUndef)->at(4)});
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
}//close par
}//close par
}//close par
}//close if
}
}
}
