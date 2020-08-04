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
const std::string _p_2 = "p_2";
PredicateWSet wp_2(1);
PredicateWSet up_2(1);
PredicateWSet fp_2(1);
const std::string _p_3 = "p_3";
PredicateWSet wp_3(2);
PredicateWSet up_3(2);
PredicateWSet fp_3(2);
const std::string _p_0 = "p_0";
PredicateWSet wp_0(3);
PredicateWSet up_0(3);
PredicateWSet fp_0(3);
const std::string _p_4 = "p_4";
PredicateWSet wp_4(2);
PredicateWSet up_4(2);
PredicateWSet fp_4(2);
const std::string _p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_ = "p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_";
PredicateWSet wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_(7);
PredicateWSet up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_(7);
std::map<std::vector<int>, std::pair< AuxMap*,AuxMap*>*> sharedVariables_0_ToAggregate_2;
std::map<std::vector<int>, std::pair< AuxMap*,AuxMap*>*> sharedVariables_0_ToAggregate_3;
Executor::~Executor() {
    for(auto sharedVar : sharedVariables_0_ToAggregate_2){
        delete sharedVar.second->first;
        delete sharedVar.second->second;
        delete sharedVar.second;
    }
    for(auto sharedVar : sharedVariables_0_ToAggregate_3){
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
AuxMap pp_3_0_1_({0,1});
AuxMap up_3_0_1_({0,1});
AuxMap fp_3_0_1_({0,1});
AuxMap pp_3_({});
AuxMap up_3_({});
AuxMap fp_3_({});
AuxMap pp_4_0_({0});
AuxMap up_4_0_({0});
AuxMap fp_4_0_({0});
AuxMap pp_0_0_2_({0,2});
AuxMap up_0_0_2_({0,2});
AuxMap fp_0_0_2_({0,2});
AuxMap pp_4_0_1_({0,1});
AuxMap up_4_0_1_({0,1});
AuxMap fp_4_0_1_({0,1});
AuxMap pp_4_({});
AuxMap up_4_({});
AuxMap fp_4_({});
AuxMap pp_3_1_({1});
AuxMap up_3_1_({1});
AuxMap fp_3_1_({1});
AuxMap pp_0_0_1_2_({0,1,2});
AuxMap up_0_0_1_2_({0,1,2});
AuxMap fp_0_0_1_2_({0,1,2});
AuxMap pp_0_({});
AuxMap up_0_({});
AuxMap fp_0_({});
AuxMap p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_({0});
AuxMap u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_({0});
AuxMap p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_({});
AuxMap u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_({});
AuxMap p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_1_({0,1});
AuxMap u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_1_({0,1});
AuxMap p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_2_3_({2,3});
AuxMap u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_2_3_({2,3});
AuxMap p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_4_5_6_({4,5,6});
AuxMap u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_4_5_6_({4,5,6});
AuxMap pp_3_0_({0});
AuxMap up_3_0_({0});
AuxMap fp_3_0_({0});
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
    if(var<0 && ( tuple.getPredicateName() == &_p_0 || tuple.getPredicateName() == &_p_3 || tuple.getPredicateName() == &_p_4)){
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
    if(tuple.getPredicateName() == &_p_3){
        if(var > 0){
            int X_3 = tuple.at(0);
            int X_4 = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuples1 = pp_4_0_.getValues({X_4});
            const std::vector<const Tuple*>& tuplesU1 = up_4_0_.getValues({X_4});
            for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
                bool buildUndef=false;
                const Tuple* tuple1;
                if(i<tuples1.size())
                    tuple1=tuples1[i];
                else{
                    buildUndef=true;
                    tuple1=tuplesU1[i-tuples1.size()];
                }
                int X_0 = tuple1->at(1);
                const std::vector<const Tuple*>& tuples2 = pp_0_0_2_.getValues({X_0, X_0});
                const std::vector<const Tuple*>& tuplesU2 = up_0_0_2_.getValues({X_0, X_0});
                for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
                    const Tuple* tuple2;
                    if(i<tuples2.size())
                        tuple2=tuples2[i];
                    else{
                        buildUndef=true;
                        tuple2=tuplesU2[i-tuples2.size()];
                    }
                    if(tuple2->at(0) == tuple2->at(2)){
                        int X_1 = tuple2->at(1);
                        Tuple t(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                        if(!buildUndef){
                            if(wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.find(t)==NULL){
                                const auto& insertResult = wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(t));
                                if (insertResult.second) {
                                    for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                                        auxMap -> insert2(*insertResult.first);
                                    }
                                    up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                                }
                            }
                        }else{
                        if(up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.find(t)==NULL){
                                const auto& insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(t));
                                if (insertResult.second) {
                                    for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                                        auxMap -> insert2(*insertResult.first);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }else{
            int X_3 = tuple.at(0);
            int X_4 = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuplesU1 = up_4_0_.getValues({X_4});
            const std::vector<const Tuple*>& tuples1 = pp_4_0_.getValues({X_4});
            for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
                bool buildUndef=false;
                const Tuple* tuple1;
                if(i<tuples1.size())
                    tuple1=tuples1[i];
                else{
                    buildUndef=true;
                    tuple1=tuplesU1[i-tuples1.size()];
                }
                int X_0 = tuple1->at(1);
                const std::vector<const Tuple*>& tuplesU2 = up_0_0_2_.getValues({X_0, X_0});
                const std::vector<const Tuple*>& tuples2 = pp_0_0_2_.getValues({X_0, X_0});
                for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
                    const Tuple* tuple2;
                    if(i<tuples2.size())
                        tuple2=tuples2[i];
                    else{
                        buildUndef=true;
                        tuple2=tuplesU2[i-tuples2.size()];
                    }
                    if(tuple2->at(0) == tuple2->at(2)){
                        int X_1 = tuple2->at(1);
                        Tuple t(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                        up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                        wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_p_4){
        if(var > 0){
            int X_4 = tuple.at(0);
            int X_0 = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuples0 = pp_3_1_.getValues({X_4});
            const std::vector<const Tuple*>& tuplesU0 = up_3_1_.getValues({X_4});
            for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
                bool buildUndef=false;
                const Tuple* tuple0;
                if(i<tuples0.size())
                    tuple0=tuples0[i];
                else{
                    buildUndef=true;
                    tuple0=tuplesU0[i-tuples0.size()];
                }
                int X_3 = tuple0->at(0);
                const std::vector<const Tuple*>& tuples2 = pp_0_0_2_.getValues({X_0, X_0});
                const std::vector<const Tuple*>& tuplesU2 = up_0_0_2_.getValues({X_0, X_0});
                for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
                    const Tuple* tuple2;
                    if(i<tuples2.size())
                        tuple2=tuples2[i];
                    else{
                        buildUndef=true;
                        tuple2=tuplesU2[i-tuples2.size()];
                    }
                    if(tuple2->at(0) == tuple2->at(2)){
                        int X_1 = tuple2->at(1);
                        Tuple t(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                        if(!buildUndef){
                            if(wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.find(t)==NULL){
                                const auto& insertResult = wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(t));
                                if (insertResult.second) {
                                    for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                                        auxMap -> insert2(*insertResult.first);
                                    }
                                    up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                                }
                            }
                        }else{
                        if(up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.find(t)==NULL){
                                const auto& insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(t));
                                if (insertResult.second) {
                                    for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                                        auxMap -> insert2(*insertResult.first);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }else{
            int X_4 = tuple.at(0);
            int X_0 = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuplesU0 = up_3_1_.getValues({X_4});
            const std::vector<const Tuple*>& tuples0 = pp_3_1_.getValues({X_4});
            for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
                bool buildUndef=false;
                const Tuple* tuple0;
                if(i<tuples0.size())
                    tuple0=tuples0[i];
                else{
                    buildUndef=true;
                    tuple0=tuplesU0[i-tuples0.size()];
                }
                int X_3 = tuple0->at(0);
                const std::vector<const Tuple*>& tuplesU2 = up_0_0_2_.getValues({X_0, X_0});
                const std::vector<const Tuple*>& tuples2 = pp_0_0_2_.getValues({X_0, X_0});
                for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
                    const Tuple* tuple2;
                    if(i<tuples2.size())
                        tuple2=tuples2[i];
                    else{
                        buildUndef=true;
                        tuple2=tuplesU2[i-tuples2.size()];
                    }
                    if(tuple2->at(0) == tuple2->at(2)){
                        int X_1 = tuple2->at(1);
                        Tuple t(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                        up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                        wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_p_0){
        if(var > 0){
            int X_0 = tuple.at(0);
            int X_1 = tuple.at(1);
            if(tuple.at(0) == tuple.at(2)){
                bool buildUndef=false;
                bool buildFalse=false;
                const std::vector<const Tuple*>& tuples0 = pp_3_.getValues({});
                const std::vector<const Tuple*>& tuplesU0 = up_3_.getValues({});
                for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
                    bool buildUndef=false;
                    const Tuple* tuple0;
                    if(i<tuples0.size())
                        tuple0=tuples0[i];
                    else{
                        buildUndef=true;
                        tuple0=tuplesU0[i-tuples0.size()];
                    }
                    int X_3 = tuple0->at(0);
                    int X_4 = tuple0->at(1);
                    const std::vector<const Tuple*>& tuples1 = pp_4_0_1_.getValues({X_4, X_0});
                    const std::vector<const Tuple*>& tuplesU1 = up_4_0_1_.getValues({X_4, X_0});
                    for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
                        const Tuple* tuple1;
                        if(i<tuples1.size())
                            tuple1=tuples1[i];
                        else{
                            buildUndef=true;
                            tuple1=tuplesU1[i-tuples1.size()];
                        }
                        Tuple t(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                        if(!buildUndef){
                            if(wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.find(t)==NULL){
                                const auto& insertResult = wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(t));
                                if (insertResult.second) {
                                    for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                                        auxMap -> insert2(*insertResult.first);
                                    }
                                    up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                                }
                            }
                        }else{
                        if(up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.find(t)==NULL){
                                const auto& insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(t));
                                if (insertResult.second) {
                                    for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                                        auxMap -> insert2(*insertResult.first);
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
            if(tuple.at(0) == tuple.at(2)){
                bool buildUndef=false;
                bool buildFalse=false;
                const std::vector<const Tuple*>& tuplesU0 = up_3_.getValues({});
                const std::vector<const Tuple*>& tuples0 = pp_3_.getValues({});
                for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
                    bool buildUndef=false;
                    const Tuple* tuple0;
                    if(i<tuples0.size())
                        tuple0=tuples0[i];
                    else{
                        buildUndef=true;
                        tuple0=tuplesU0[i-tuples0.size()];
                    }
                    int X_3 = tuple0->at(0);
                    int X_4 = tuple0->at(1);
                    const std::vector<const Tuple*>& tuplesU1 = up_4_0_1_.getValues({X_4, X_0});
                    const std::vector<const Tuple*>& tuples1 = pp_4_0_1_.getValues({X_4, X_0});
                    for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
                        const Tuple* tuple1;
                        if(i<tuples1.size())
                            tuple1=tuples1[i];
                        else{
                            buildUndef=true;
                            tuple1=tuplesU1[i-tuples1.size()];
                        }
                        Tuple t(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                        up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                        wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_p_3){
        if(var > 0){
            int X_3 = tuple.at(0);
            int X_4 = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuples1 = pp_4_0_.getValues({X_4});
            const std::vector<const Tuple*>& tuplesU1 = up_4_0_.getValues({X_4});
            for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
                bool buildUndef=false;
                const Tuple* tuple1;
                if(i<tuples1.size())
                    tuple1=tuples1[i];
                else{
                    buildUndef=true;
                    tuple1=tuplesU1[i-tuples1.size()];
                }
                int X_0 = tuple1->at(1);
                const std::vector<const Tuple*>& tuples2 = pp_0_0_2_.getValues({X_0, X_0});
                const std::vector<const Tuple*>& tuplesU2 = up_0_0_2_.getValues({X_0, X_0});
                for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
                    const Tuple* tuple2;
                    if(i<tuples2.size())
                        tuple2=tuples2[i];
                    else{
                        buildUndef=true;
                        tuple2=tuplesU2[i-tuples2.size()];
                    }
                    if(tuple2->at(0) == tuple2->at(2)){
                        int X_1 = tuple2->at(1);
                        Tuple t(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                        if(!buildUndef && !buildFalse){
                            if(wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.find(t)==NULL){
                                const auto& insertResult = wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(t));
                                if (insertResult.second) {
                                    for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                                        auxMap -> insert2(*insertResult.first);
                                    }
                                    up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                                }
                            }
                        }else{
                        if(!buildFalse){
                            if(up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.find(t)==NULL){
                                    const auto& insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(t));
                                    if (insertResult.second) {
                                        for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                                            auxMap -> insert2(*insertResult.first);
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
            int X_4 = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuplesU1 = up_4_0_.getValues({X_4});
            const std::vector<const Tuple*>& tuples1 = pp_4_0_.getValues({X_4});
            const std::vector<const Tuple*>& tuplesF1 = fp_4_0_.getValues({X_4});
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
                int X_0 = tuple1->at(1);
                const std::vector<const Tuple*>& tuplesU2 = up_0_0_2_.getValues({X_0, X_0});
                const std::vector<const Tuple*>& tuples2 = pp_0_0_2_.getValues({X_0, X_0});
                const std::vector<const Tuple*>& tuplesF2 = fp_0_0_2_.getValues({X_0, X_0});
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
                    if(tuple2->at(0) == tuple2->at(2)){
                        int X_1 = tuple2->at(1);
                        Tuple t(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                        up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                        wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_p_4){
        if(var > 0){
            int X_4 = tuple.at(0);
            int X_0 = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuples0 = pp_3_1_.getValues({X_4});
            const std::vector<const Tuple*>& tuplesU0 = up_3_1_.getValues({X_4});
            for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
                bool buildUndef=false;
                const Tuple* tuple0;
                if(i<tuples0.size())
                    tuple0=tuples0[i];
                else{
                    buildUndef=true;
                    tuple0=tuplesU0[i-tuples0.size()];
                }
                int X_3 = tuple0->at(0);
                const std::vector<const Tuple*>& tuples2 = pp_0_0_2_.getValues({X_0, X_0});
                const std::vector<const Tuple*>& tuplesU2 = up_0_0_2_.getValues({X_0, X_0});
                for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
                    const Tuple* tuple2;
                    if(i<tuples2.size())
                        tuple2=tuples2[i];
                    else{
                        buildUndef=true;
                        tuple2=tuplesU2[i-tuples2.size()];
                    }
                    if(tuple2->at(0) == tuple2->at(2)){
                        int X_1 = tuple2->at(1);
                        Tuple t(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                        if(!buildUndef && !buildFalse){
                            if(wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.find(t)==NULL){
                                const auto& insertResult = wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(t));
                                if (insertResult.second) {
                                    for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                                        auxMap -> insert2(*insertResult.first);
                                    }
                                    up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                                }
                            }
                        }else{
                        if(!buildFalse){
                            if(up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.find(t)==NULL){
                                    const auto& insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(t));
                                    if (insertResult.second) {
                                        for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                                            auxMap -> insert2(*insertResult.first);
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
            int X_0 = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuplesU0 = up_3_1_.getValues({X_4});
            const std::vector<const Tuple*>& tuples0 = pp_3_1_.getValues({X_4});
            const std::vector<const Tuple*>& tuplesF0 = fp_3_1_.getValues({X_4});
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
                int X_3 = tuple0->at(0);
                const std::vector<const Tuple*>& tuplesU2 = up_0_0_2_.getValues({X_0, X_0});
                const std::vector<const Tuple*>& tuples2 = pp_0_0_2_.getValues({X_0, X_0});
                const std::vector<const Tuple*>& tuplesF2 = fp_0_0_2_.getValues({X_0, X_0});
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
                    if(tuple2->at(0) == tuple2->at(2)){
                        int X_1 = tuple2->at(1);
                        Tuple t(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                        up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                        wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_p_0){
        if(var > 0){
            int X_0 = tuple.at(0);
            int X_1 = tuple.at(1);
            if(tuple.at(0) == tuple.at(2)){
                bool buildUndef=false;
                bool buildFalse=false;
                const std::vector<const Tuple*>& tuples0 = pp_3_.getValues({});
                const std::vector<const Tuple*>& tuplesU0 = up_3_.getValues({});
                for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
                    bool buildUndef=false;
                    const Tuple* tuple0;
                    if(i<tuples0.size())
                        tuple0=tuples0[i];
                    else{
                        buildUndef=true;
                        tuple0=tuplesU0[i-tuples0.size()];
                    }
                    int X_3 = tuple0->at(0);
                    int X_4 = tuple0->at(1);
                    const std::vector<const Tuple*>& tuples1 = pp_4_0_1_.getValues({X_4, X_0});
                    const std::vector<const Tuple*>& tuplesU1 = up_4_0_1_.getValues({X_4, X_0});
                    for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
                        const Tuple* tuple1;
                        if(i<tuples1.size())
                            tuple1=tuples1[i];
                        else{
                            buildUndef=true;
                            tuple1=tuplesU1[i-tuples1.size()];
                        }
                        Tuple t(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                        if(!buildUndef && !buildFalse){
                            if(wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.find(t)==NULL){
                                const auto& insertResult = wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(t));
                                if (insertResult.second) {
                                    for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                                        auxMap -> insert2(*insertResult.first);
                                    }
                                    up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                                }
                            }
                        }else{
                        if(!buildFalse){
                            if(up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.find(t)==NULL){
                                    const auto& insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(t));
                                    if (insertResult.second) {
                                        for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                                            auxMap -> insert2(*insertResult.first);
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
            if(tuple.at(0) == tuple.at(2)){
                bool buildUndef=false;
                bool buildFalse=false;
                const std::vector<const Tuple*>& tuplesU0 = up_3_.getValues({});
                const std::vector<const Tuple*>& tuples0 = pp_3_.getValues({});
                const std::vector<const Tuple*>& tuplesF0 = fp_3_.getValues({});
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
                    int X_3 = tuple0->at(0);
                    int X_4 = tuple0->at(1);
                    const std::vector<const Tuple*>& tuplesU1 = up_4_0_1_.getValues({X_4, X_0});
                    const std::vector<const Tuple*>& tuples1 = pp_4_0_1_.getValues({X_4, X_0});
                    const std::vector<const Tuple*>& tuplesF1 = fp_4_0_1_.getValues({X_4, X_0});
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
                        Tuple t(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                        up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                        wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(t);
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_p_2){
        const std::vector<const Tuple *>& tuples1 = pp_3_0_.getValues({4});
        const std::vector<const Tuple *>& tuplesU1 = up_3_0_.getValues({4});
        const Tuple * tuple1;
        for(int i=0; i< tuples1.size() + tuplesU1.size();i++){
            if(i<tuples1.size())
                tuple1= tuples1[i];
            else tuple1 = tuplesU1[i-tuples1.size()];
            int X_2 = tuple1->at(1);
        }
    }
    if(tuple.getPredicateName() == &_p_3){
        int X_2 = tuple.at(1);
        std::vector<const Tuple *> tuples0;
        const Tuple* t0= wp_2.find({3});
        if(t0!=NULL) tuples0.push_back(t0);
        std::vector<const Tuple *> tuplesU0;
        const Tuple* tU0= up_2.find({3});
        if(tU0!=NULL) tuplesU0.push_back(tU0);
        const Tuple * tuple0;
        for(int i=0; i< tuples0.size() + tuplesU0.size();i++){
            if(i<tuples0.size())
                tuple0= tuples0[i];
            else tuple0 = tuplesU0[i-tuples0.size()];
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
    if(var<0 && ( tuple.getPredicateName() == &_p_0 || tuple.getPredicateName() == &_p_3 || tuple.getPredicateName() == &_p_4)){
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
    if(tuple.getPredicateName() == &_p_3){
        if(var > 0){
            const std::vector<const Tuple* >& trueJoinTuples = p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_1_.getValues({tuple[0],tuple[1]});
            for(const Tuple* t :trueJoinTuples){
                const auto insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(*t));
                if(insertResult.second){
                    for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                }
                wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(*t);
            }
        }
        int X_3 = tuple.at(0);
        int X_4 = tuple.at(1);
        bool buildUndef=false;
        bool buildFalse=false;
        const std::vector<const Tuple*>& tuplesU1 = pp_4_0_.getValues({X_4});
        const std::vector<const Tuple*>& tuples1 = up_4_0_.getValues({X_4});
        for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
            const Tuple* tuple1;
            if(i<tuples1.size())
                tuple1=tuples1[i];
            else{
                tuple1=tuplesU1[i-tuples1.size()];
            }
            int X_0 = tuple1->at(1);
            const std::vector<const Tuple*>& tuplesU2 = pp_0_0_2_.getValues({X_0, X_0});
            const std::vector<const Tuple*>& tuples2 = up_0_0_2_.getValues({X_0, X_0});
            for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
                const Tuple* tuple2;
                if(i<tuples2.size())
                    tuple2=tuples2[i];
                else{
                    tuple2=tuplesU2[i-tuples2.size()];
                }
                if(tuple2->at(0) == tuple2->at(2)){
                    int X_1 = tuple2->at(1);
                    Tuple tupleUndef(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                    const auto& insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(tupleUndef));
                    if (insertResult.second) {
                        for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                            auxMap -> insert2(*insertResult.first);
                        }
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_p_4){
        if(var > 0){
            const std::vector<const Tuple* >& trueJoinTuples = p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_2_3_.getValues({tuple[0],tuple[1]});
            for(const Tuple* t :trueJoinTuples){
                const auto insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(*t));
                if(insertResult.second){
                    for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                }
                wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(*t);
            }
        }
        int X_4 = tuple.at(0);
        int X_0 = tuple.at(1);
        bool buildUndef=false;
        bool buildFalse=false;
        const std::vector<const Tuple*>& tuplesU0 = pp_3_1_.getValues({X_4});
        const std::vector<const Tuple*>& tuples0 = up_3_1_.getValues({X_4});
        for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
            const Tuple* tuple0;
            if(i<tuples0.size())
                tuple0=tuples0[i];
            else{
                tuple0=tuplesU0[i-tuples0.size()];
            }
            int X_3 = tuple0->at(0);
            const std::vector<const Tuple*>& tuplesU2 = pp_0_0_2_.getValues({X_0, X_0});
            const std::vector<const Tuple*>& tuples2 = up_0_0_2_.getValues({X_0, X_0});
            for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
                const Tuple* tuple2;
                if(i<tuples2.size())
                    tuple2=tuples2[i];
                else{
                    tuple2=tuplesU2[i-tuples2.size()];
                }
                if(tuple2->at(0) == tuple2->at(2)){
                    int X_1 = tuple2->at(1);
                    Tuple tupleUndef(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                    const auto& insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(tupleUndef));
                    if (insertResult.second) {
                        for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                            auxMap -> insert2(*insertResult.first);
                        }
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_p_0){
        if(var > 0){
            const std::vector<const Tuple* >& trueJoinTuples = p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_4_5_6_.getValues({tuple[0],tuple[1],tuple[2]});
            for(const Tuple* t :trueJoinTuples){
                const auto insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(*t));
                if(insertResult.second){
                    for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                }
                wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(*t);
            }
        }
        int X_0 = tuple.at(0);
        int X_1 = tuple.at(1);
        if(tuple.at(0) == tuple.at(2)){
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuplesU0 = pp_3_.getValues({});
            const std::vector<const Tuple*>& tuples0 = up_3_.getValues({});
            for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
                const Tuple* tuple0;
                if(i<tuples0.size())
                    tuple0=tuples0[i];
                else{
                    tuple0=tuplesU0[i-tuples0.size()];
                }
                int X_3 = tuple0->at(0);
                int X_4 = tuple0->at(1);
                const std::vector<const Tuple*>& tuplesU1 = pp_4_0_1_.getValues({X_4, X_0});
                const std::vector<const Tuple*>& tuples1 = up_4_0_1_.getValues({X_4, X_0});
                for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
                    const Tuple* tuple1;
                    if(i<tuples1.size())
                        tuple1=tuples1[i];
                    else{
                        tuple1=tuplesU1[i-tuples1.size()];
                    }
                    Tuple tupleUndef(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                    const auto& insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(tupleUndef));
                    if (insertResult.second) {
                        for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                            auxMap -> insert2(*insertResult.first);
                        }
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_p_3){
        if(var > 0){
            const std::vector<const Tuple* >& trueJoinTuples = p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_1_.getValues({tuple[0],tuple[1]});
            for(const Tuple* t :trueJoinTuples){
                const auto insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(*t));
                if(insertResult.second){
                    for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                }
                wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(*t);
            }
        }
        int X_3 = tuple.at(0);
        int X_4 = tuple.at(1);
        bool buildUndef=false;
        bool buildFalse=false;
        const std::vector<const Tuple*>& tuplesU1 = pp_4_0_.getValues({X_4});
        const std::vector<const Tuple*>& tuples1 = up_4_0_.getValues({X_4});
        for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
            const Tuple* tuple1;
            if(i<tuples1.size())
                tuple1=tuples1[i];
            else{
                tuple1=tuplesU1[i-tuples1.size()];
            }
            int X_0 = tuple1->at(1);
            const std::vector<const Tuple*>& tuplesU2 = pp_0_0_2_.getValues({X_0, X_0});
            const std::vector<const Tuple*>& tuples2 = up_0_0_2_.getValues({X_0, X_0});
            for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
                const Tuple* tuple2;
                if(i<tuples2.size())
                    tuple2=tuples2[i];
                else{
                    tuple2=tuplesU2[i-tuples2.size()];
                }
                if(tuple2->at(0) == tuple2->at(2)){
                    int X_1 = tuple2->at(1);
                    Tuple tupleUndef(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                    const auto& insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(tupleUndef));
                    if (insertResult.second) {
                        for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                            auxMap -> insert2(*insertResult.first);
                        }
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_p_4){
        if(var > 0){
            const std::vector<const Tuple* >& trueJoinTuples = p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_2_3_.getValues({tuple[0],tuple[1]});
            for(const Tuple* t :trueJoinTuples){
                const auto insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(*t));
                if(insertResult.second){
                    for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                }
                wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(*t);
            }
        }
        int X_4 = tuple.at(0);
        int X_0 = tuple.at(1);
        bool buildUndef=false;
        bool buildFalse=false;
        const std::vector<const Tuple*>& tuplesU0 = pp_3_1_.getValues({X_4});
        const std::vector<const Tuple*>& tuples0 = up_3_1_.getValues({X_4});
        for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
            const Tuple* tuple0;
            if(i<tuples0.size())
                tuple0=tuples0[i];
            else{
                tuple0=tuplesU0[i-tuples0.size()];
            }
            int X_3 = tuple0->at(0);
            const std::vector<const Tuple*>& tuplesU2 = pp_0_0_2_.getValues({X_0, X_0});
            const std::vector<const Tuple*>& tuples2 = up_0_0_2_.getValues({X_0, X_0});
            for(int i=0;i<tuples2.size()+tuplesU2.size();i++){
                const Tuple* tuple2;
                if(i<tuples2.size())
                    tuple2=tuples2[i];
                else{
                    tuple2=tuplesU2[i-tuples2.size()];
                }
                if(tuple2->at(0) == tuple2->at(2)){
                    int X_1 = tuple2->at(1);
                    Tuple tupleUndef(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                    const auto& insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(tupleUndef));
                    if (insertResult.second) {
                        for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                            auxMap -> insert2(*insertResult.first);
                        }
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_p_0){
        if(var > 0){
            const std::vector<const Tuple* >& trueJoinTuples = p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_4_5_6_.getValues({tuple[0],tuple[1],tuple[2]});
            for(const Tuple* t :trueJoinTuples){
                const auto insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(*t));
                if(insertResult.second){
                    for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                }
                wp_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.erase(*t);
            }
        }
        int X_0 = tuple.at(0);
        int X_1 = tuple.at(1);
        if(tuple.at(0) == tuple.at(2)){
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuplesU0 = pp_3_.getValues({});
            const std::vector<const Tuple*>& tuples0 = up_3_.getValues({});
            for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
                const Tuple* tuple0;
                if(i<tuples0.size())
                    tuple0=tuples0[i];
                else{
                    tuple0=tuplesU0[i-tuples0.size()];
                }
                int X_3 = tuple0->at(0);
                int X_4 = tuple0->at(1);
                const std::vector<const Tuple*>& tuplesU1 = pp_4_0_1_.getValues({X_4, X_0});
                const std::vector<const Tuple*>& tuples1 = up_4_0_1_.getValues({X_4, X_0});
                for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
                    const Tuple* tuple1;
                    if(i<tuples1.size())
                        tuple1=tuples1[i];
                    else{
                        tuple1=tuplesU1[i-tuples1.size()];
                    }
                    Tuple tupleUndef(std::vector<int>({X_3,X_4,X_4,X_0,X_0,X_1,X_0}),&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
                    const auto& insertResult = up_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.insert(Tuple(tupleUndef));
                    if (insertResult.second) {
                        for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_]){
                            auxMap -> insert2(*insertResult.first);
                        }
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_p_2){
        const std::vector<const Tuple *>& undefTuples1 = up_3_0_.getValues({4});
        const std::vector<const Tuple*>& trueTuples1 = pp_3_0_.getValues({4});
        for(int i=0;i<undefTuples1.size()+trueTuples1.size();i++){
            const Tuple * tuple1;
            if(i<undefTuples1.size())
                tuple1 = undefTuples1[i];
            else tuple1 = trueTuples1[i-undefTuples1.size()];
            int X_2 = tuple1->at(1);
        }
    }
    if(tuple.getPredicateName() == &_p_3){
        int X_2 = tuple.at(1);
        const Tuple* undefT = up_2.find({3});
        const Tuple* trueT = wp_2.find({3});
        std::vector<const Tuple *> undefTuples0;
        if(undefT != NULL) undefTuples0.push_back(undefT);
        std::vector<const Tuple *> trueTuples0;
        if(trueT != NULL) trueTuples0.push_back(undefT);
        for(int i=0;i<undefTuples0.size()+trueTuples0.size();i++){
            const Tuple * tuple0;
            if(i<undefTuples0.size())
                tuple0 = undefTuples0[i];
            else tuple0 = trueTuples0[i-undefTuples0.size()];
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
    predicateWSetMap[&_p_2]=&wp_2;
    predicateUSetMap[&_p_2]=&up_2;
    stringToUniqueStringPointer["p_2"] = &_p_2;
    predicateWSetMap[&_p_3]=&wp_3;
    predicateUSetMap[&_p_3]=&up_3;
    stringToUniqueStringPointer["p_3"] = &_p_3;
    aggregatePredicatesAndVars [&_p_3]= std::vector<int>({0});
    predicateWSetMap[&_p_3]=&wp_3;
    predicateFSetMap[&_p_3]=&fp_3;
    predicateUSetMap[&_p_3]=&up_3;
    stringToUniqueStringPointer["p_3"] = &_p_3;
    aggregatePredicatesAndVars [&_p_4]= std::vector<int>({});
    predicateWSetMap[&_p_4]=&wp_4;
    predicateFSetMap[&_p_4]=&fp_4;
    predicateUSetMap[&_p_4]=&up_4;
    stringToUniqueStringPointer["p_4"] = &_p_4;
    aggregatePredicatesAndVars [&_p_0]= std::vector<int>({});
    predicateWSetMap[&_p_0]=&wp_0;
    predicateFSetMap[&_p_0]=&fp_0;
    predicateUSetMap[&_p_0]=&up_0;
    stringToUniqueStringPointer["p_0"] = &_p_0;
    aggregatePredicatesAndVars [&_p_3]= std::vector<int>({0});
    predicateWSetMap[&_p_3]=&wp_3;
    predicateFSetMap[&_p_3]=&fp_3;
    predicateUSetMap[&_p_3]=&up_3;
    stringToUniqueStringPointer["p_3"] = &_p_3;
    aggregatePredicatesAndVars [&_p_4]= std::vector<int>({});
    predicateWSetMap[&_p_4]=&wp_4;
    predicateFSetMap[&_p_4]=&fp_4;
    predicateUSetMap[&_p_4]=&up_4;
    stringToUniqueStringPointer["p_4"] = &_p_4;
    aggregatePredicatesAndVars [&_p_0]= std::vector<int>({});
    predicateWSetMap[&_p_0]=&wp_0;
    predicateFSetMap[&_p_0]=&fp_0;
    predicateUSetMap[&_p_0]=&up_0;
    stringToUniqueStringPointer["p_0"] = &_p_0;
    predicateToAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_].push_back(&p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
    predicateToAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_].push_back(&p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_);
    predicateToAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_].push_back(&p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_1_);
    predicateToAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_].push_back(&p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_2_3_);
    predicateToAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_].push_back(&p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_4_5_6_);
    predicateToAuxiliaryMaps[&_p_0].push_back(&pp_0_);
    predicateToAuxiliaryMaps[&_p_0].push_back(&pp_0_0_1_2_);
    predicateToAuxiliaryMaps[&_p_0].push_back(&pp_0_0_2_);
    predicateToAuxiliaryMaps[&_p_3].push_back(&pp_3_);
    predicateToAuxiliaryMaps[&_p_3].push_back(&pp_3_0_);
    predicateToAuxiliaryMaps[&_p_3].push_back(&pp_3_0_1_);
    predicateToAuxiliaryMaps[&_p_3].push_back(&pp_3_1_);
    predicateToAuxiliaryMaps[&_p_4].push_back(&pp_4_);
    predicateToAuxiliaryMaps[&_p_4].push_back(&pp_4_0_);
    predicateToAuxiliaryMaps[&_p_4].push_back(&pp_4_0_1_);
    predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_].push_back(&u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_);
    predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_].push_back(&u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_);
    predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_].push_back(&u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_1_);
    predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_].push_back(&u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_2_3_);
    predicateToUndefAuxiliaryMaps[&_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_].push_back(&u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_4_5_6_);
    predicateToUndefAuxiliaryMaps[&_p_0].push_back(&up_0_);
    predicateToUndefAuxiliaryMaps[&_p_0].push_back(&up_0_0_1_2_);
    predicateToUndefAuxiliaryMaps[&_p_0].push_back(&up_0_0_2_);
    predicateToUndefAuxiliaryMaps[&_p_3].push_back(&up_3_);
    predicateToUndefAuxiliaryMaps[&_p_3].push_back(&up_3_0_);
    predicateToUndefAuxiliaryMaps[&_p_3].push_back(&up_3_0_1_);
    predicateToUndefAuxiliaryMaps[&_p_3].push_back(&up_3_1_);
    predicateToUndefAuxiliaryMaps[&_p_4].push_back(&up_4_);
    predicateToUndefAuxiliaryMaps[&_p_4].push_back(&up_4_0_);
    predicateToUndefAuxiliaryMaps[&_p_4].push_back(&up_4_0_1_);
    predicateToFalseAuxiliaryMaps[&_p_0].push_back(&fp_0_);
    predicateToFalseAuxiliaryMaps[&_p_0].push_back(&fp_0_0_1_2_);
    predicateToFalseAuxiliaryMaps[&_p_0].push_back(&fp_0_0_2_);
    predicateToFalseAuxiliaryMaps[&_p_3].push_back(&fp_3_);
    predicateToFalseAuxiliaryMaps[&_p_3].push_back(&fp_3_0_);
    predicateToFalseAuxiliaryMaps[&_p_3].push_back(&fp_3_0_1_);
    predicateToFalseAuxiliaryMaps[&_p_3].push_back(&fp_3_1_);
    predicateToFalseAuxiliaryMaps[&_p_4].push_back(&fp_4_);
    predicateToFalseAuxiliaryMaps[&_p_4].push_back(&fp_4_0_);
    predicateToFalseAuxiliaryMaps[&_p_4].push_back(&fp_4_0_1_);
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
            const Tuple * tuple0 = (wp_2.find({3}));
            if(!tuple0 && !tupleU ){
                tuple0 = tupleU = (up_2.find({3}));
                tupleUNegated = false;
            }
            if(tuple0){
                if( (*tuple0)[0] == 3){
                    const std::vector<const Tuple* >* tuples;
                    tuples = &pp_3_0_.getValues({4});
                    const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                    if(tupleU == NULL){
                        tuplesU = &up_3_0_.getValues({4});
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
                        int X_2 = (*tuple1)[1];
                        if( (*tuple1)[0] == 4){
                            std::pair<AuxMap*,AuxMap*> joinTuples_0_2;
                            joinTuples_0_2.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                            joinTuples_0_2.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                            std::pair<AuxMap*,AuxMap*> joinTuples_0_2SharedVariables;
                            joinTuples_0_2SharedVariables.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                            joinTuples_0_2SharedVariables.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                            int actualSize2 = joinTuples_0_2SharedVariables.first->size();
                            std::set<std::vector<int>> alreadyCounted2;
                            for(const Tuple * joinTupleUndef : u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({})){
                                if(joinTuples_0_2SharedVariables.first->getValues({joinTupleUndef->at(0)}).size()==0){
                                    if(joinTuples_0_2SharedVariables.second->getValues({joinTupleUndef->at(0)}).size()>0){
                                        std::vector<int> key({joinTupleUndef->at(0)});
                                        alreadyCounted2.insert(key);
                                    }
                                }
                            }
                            actualSize2+=alreadyCounted2.size();
                            if(tupleU==NULL){
                                if(joinTuples_0_2SharedVariables.first->size()>=1){
                                    if(!(actualSize2>=1+1)){
                                        std::cout<<"conflict detected in propagator"<<std::endl;
                                        propagatedLiteralsAndReasons.insert({-1, std::vector<int>()});
                                    }else{
                                        if(actualSize2 == 1+1){
                                            const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_2.second->getValues({});
                                            for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                if(joinTuples_0_2SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0 && joinTuples_0_2SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 1){
                                                    {
                                                        const Tuple* aggrTupleU = up_3.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                        if(aggrTupleU != NULL){
                                                            const auto & it = tupleToVar.find(*aggrTupleU);
                                                            if(it != tupleToVar.end()) {
                                                                int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                atomsTable[it->second].print();
                                                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;
                                                            }
                                                        }
                                                    }
                                                    {
                                                        const Tuple* aggrTupleU = up_4.find({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                        if(aggrTupleU != NULL){
                                                            const auto & it = tupleToVar.find(*aggrTupleU);
                                                            if(it != tupleToVar.end()) {
                                                                int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                atomsTable[it->second].print();
                                                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;
                                                            }
                                                        }
                                                    }
                                                    {
                                                        const Tuple* aggrTupleU = up_0.find({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                        if(aggrTupleU != NULL){
                                                            const auto & it = tupleToVar.find(*aggrTupleU);
                                                            if(it != tupleToVar.end()) {
                                                                int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                atomsTable[it->second].print();
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
                                if(!(actualSize2>=1+1) && (joinTuples_0_2SharedVariables.first->size()>=1)){
                                    const auto & it = tupleToVar.find(*tupleU);
                                    if(it != tupleToVar.end()) {
                                        int sign = tupleU->isNegated() ? -1 : 1;
                                        auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;
                                    }
                                }
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_0_3;
                            joinTuples_0_3.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                            joinTuples_0_3.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                            std::pair<AuxMap*,AuxMap*> joinTuples_0_3SharedVariables;
                            joinTuples_0_3SharedVariables.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                            joinTuples_0_3SharedVariables.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                            int actualSize3 = joinTuples_0_3SharedVariables.first->size();
                            std::set<std::vector<int>> alreadyCounted3;
                            for(const Tuple * joinTupleUndef : u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({})){
                                if(joinTuples_0_3SharedVariables.first->getValues({joinTupleUndef->at(0)}).size()==0){
                                    if(joinTuples_0_3SharedVariables.second->getValues({joinTupleUndef->at(0)}).size()>0){
                                        std::vector<int> key({joinTupleUndef->at(0)});
                                        alreadyCounted3.insert(key);
                                    }
                                }
                            }
                            actualSize3+=alreadyCounted3.size();
                            if(tupleU==NULL){
                                if(!(actualSize3>=1+1)){
                                    if(joinTuples_0_3SharedVariables.first->size()>=1){
                                        std::cout<<"conflict detected in propagator"<<std::endl;
                                        propagatedLiteralsAndReasons.insert({-1, std::vector<int>()});
                                    }else{
                                        if(joinTuples_0_3SharedVariables.first->size() == 1-1){
                                            const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_3.second->getValues({});
                                            for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                if(joinTuples_0_3SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0){
                                                    const Tuple* aggrTupleU=NULL;
                                                    const std::vector<const Tuple*>* tuplesp_3;
                                                    const std::vector<const Tuple*>* tuplesUp_3=&EMPTY_TUPLES;
                                                    tuplesp_3= &pp_3_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                    if(aggrTupleU==NULL)
                                                        tuplesUp_3= &up_3_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                    for(int j2=0;j2<tuplesp_3->size()+tuplesUp_3->size();j2++){
                                                        const Tuple* auxTuple2=NULL;
                                                        bool addedToReason=false;
                                                        if(j2 < tuplesp_3->size()){
                                                            auxTuple2 = tuplesp_3->at(j2);
                                                            if(tuplesUp_3 != &EMPTY_TUPLES) {
                                                            }
                                                        }else{
                                                            auxTuple2 = tuplesUp_3->at(j2-tuplesp_3->size());
                                                            aggrTupleU = auxTuple2;
                                                        }
                                                        const std::vector<const Tuple*>* tuplesp_4;
                                                        const std::vector<const Tuple*>* tuplesUp_4=&EMPTY_TUPLES;
                                                        tuplesp_4= &pp_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                        if(aggrTupleU==NULL)
                                                            tuplesUp_4= &up_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                        for(int j4=0;j4<tuplesp_4->size()+tuplesUp_4->size();j4++){
                                                            const Tuple* auxTuple4=NULL;
                                                            bool addedToReason=false;
                                                            if(j4 < tuplesp_4->size()){
                                                                auxTuple4 = tuplesp_4->at(j4);
                                                                if(tuplesUp_4 != &EMPTY_TUPLES) {
                                                                }
                                                            }else{
                                                                auxTuple4 = tuplesUp_4->at(j4-tuplesp_4->size());
                                                                aggrTupleU = auxTuple4;
                                                            }
                                                            const std::vector<const Tuple*>* tuplesp_0;
                                                            const std::vector<const Tuple*>* tuplesUp_0=&EMPTY_TUPLES;
                                                            tuplesp_0= &pp_0_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                            if(aggrTupleU==NULL)
                                                                tuplesUp_0= &up_0_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                            for(int j7=0;j7<tuplesp_0->size()+tuplesUp_0->size();j7++){
                                                                const Tuple* auxTuple7=NULL;
                                                                bool addedToReason=false;
                                                                if(j7 < tuplesp_0->size()){
                                                                    auxTuple7 = tuplesp_0->at(j7);
                                                                    if(tuplesUp_0 != &EMPTY_TUPLES) {
                                                                    }
                                                                }else{
                                                                    auxTuple7 = tuplesUp_0->at(j7-tuplesp_0->size());
                                                                    aggrTupleU = auxTuple7;
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
                                }
                            }else{
                                if(joinTuples_0_3SharedVariables.first->size()>=1 && (!(actualSize3>=1+1))){
                                    const auto & it = tupleToVar.find(*tupleU);
                                    if(it != tupleToVar.end()) {
                                        int sign = tupleU->isNegated() ? -1 : 1;
                                        auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;
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
        if(starter.getPredicateName() == &_p_3) { 
            const Tuple * tuple0 = &starter;
            if(facts[i] > 0){
                {
                    const Tuple * tupleU = NULL;
                    bool tupleUNegated = false;
                    int X_2 = (*tuple0)[1];
                    if( (*tuple0)[0] == 4){
                        const Tuple * tuple1 = (wp_2.find({3}));
                        if(!tuple1 && !tupleU ){
                            tuple1 = tupleU = (up_2.find({3}));
                            tupleUNegated = false;
                        }
                        if(tuple1){
                            if( (*tuple1)[0] == 3){
                                std::pair<AuxMap*,AuxMap*> joinTuples_0_2;
                                joinTuples_0_2.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                joinTuples_0_2.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                std::pair<AuxMap*,AuxMap*> joinTuples_0_2SharedVariables;
                                joinTuples_0_2SharedVariables.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                joinTuples_0_2SharedVariables.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                int actualSize2 = joinTuples_0_2SharedVariables.first->size();
                                std::set<std::vector<int>> alreadyCounted2;
                                for(const Tuple * joinTupleUndef : u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({})){
                                    if(joinTuples_0_2SharedVariables.first->getValues({joinTupleUndef->at(0)}).size()==0){
                                        if(joinTuples_0_2SharedVariables.second->getValues({joinTupleUndef->at(0)}).size()>0){
                                            std::vector<int> key({joinTupleUndef->at(0)});
                                            alreadyCounted2.insert(key);
                                        }
                                    }
                                }
                                actualSize2+=alreadyCounted2.size();
                                if(tupleU==NULL){
                                    const std::vector<const Tuple*>* trueTuples = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({});
                                    std::vector<int> reason;
                                    for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                        Tuple tuplep_3_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_3);
                                        const auto & itp_3_0 = tupleToVar.find(tuplep_3_0);
                                        if(itp_3_0!=tupleToVar.end()){
                                            reason.push_back(itp_3_0->second * (tuplep_3_0.isNegated() ? -1:1));
                                        }//closing if
                                        Tuple tuplep_4_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_4);
                                        const auto & itp_4_1 = tupleToVar.find(tuplep_4_1);
                                        if(itp_4_1!=tupleToVar.end()){
                                            reason.push_back(itp_4_1->second * (tuplep_4_1.isNegated() ? -1:1));
                                        }//closing if
                                        Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4),trueTuples->at(reasonIndex)->at(5),trueTuples->at(reasonIndex)->at(6)}),&_p_0);
                                        const auto & itp_0_2 = tupleToVar.find(tuplep_0_2);
                                        if(itp_0_2!=tupleToVar.end()){
                                            reason.push_back(itp_0_2->second * (tuplep_0_2.isNegated() ? -1:1));
                                        }//closing if
                                    }
                                    std::cout<<"StarterForReason1"<<std::endl;
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
                                    if(joinTuples_0_2SharedVariables.first->size()>=1){
                                        const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                        for(int i=0;i<falseTuples0->size();i++){
                                            bool joiningTupleFound=false;
                                            int X_3 = falseTuples0->at(i)->at(0);
                                            int X_4 = falseTuples0->at(i)->at(1);
                                            const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_.getValues({X_4});
                                            const std::vector<const Tuple*>* undefTuples1 = &up_4_0_.getValues({X_4});
                                            const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_.getValues({X_4});
                                            for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                                                const Tuple * tuple1=NULL;
                                                if(i_1<trueTuples1->size())
                                                    tuple1=trueTuples1->at(i_1);
                                                else if(i_1<trueTuples1->size()+undefTuples1->size())
                                                    tuple1=undefTuples1->at(i_1-trueTuples1->size());
                                                else
                                                    tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                                                int X_0 = tuple1->at(1);
                                                const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                                const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                                const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                        const std::vector<const Tuple*>* falseTuples1 = &fp_4_.getValues({});
                                        for(int i=0;i<falseTuples1->size();i++){
                                            bool joiningTupleFound=false;
                                            int X_4 = falseTuples1->at(i)->at(0);
                                            int X_0 = falseTuples1->at(i)->at(1);
                                            const std::vector<const Tuple*>* trueTuples0 = &pp_3_1_.getValues({X_4});
                                            const std::vector<const Tuple*>* undefTuples0 = &up_3_1_.getValues({X_4});
                                            const std::vector<const Tuple*>* falseTuples0 = &fp_3_1_.getValues({X_4});
                                            for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                                const Tuple * tuple0=NULL;
                                                if(i_0<trueTuples0->size())
                                                    tuple0=trueTuples0->at(i_0);
                                                else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                    tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                                else
                                                    tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                                int X_3 = tuple0->at(0);
                                                const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                                const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                                const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                            int X_0 = falseTuples2->at(i)->at(0);
                                            int X_1 = falseTuples2->at(i)->at(1);
                                            const std::vector<const Tuple*>* trueTuples0 = &pp_3_.getValues({});
                                            const std::vector<const Tuple*>* undefTuples0 = &up_3_.getValues({});
                                            const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                            for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                                const Tuple * tuple0=NULL;
                                                if(i_0<trueTuples0->size())
                                                    tuple0=trueTuples0->at(i_0);
                                                else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                    tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                                else
                                                    tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                                int X_3 = tuple0->at(0);
                                                int X_4 = tuple0->at(1);
                                                const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_1_.getValues({X_4, X_0});
                                                const std::vector<const Tuple*>* undefTuples1 = &up_4_0_1_.getValues({X_4, X_0});
                                                const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_1_.getValues({X_4, X_0});
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
                                        if(!(actualSize2>=1+1)){
                                            std::cout<<"conflict detected in propagator"<<std::endl;
                                            propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                        }else{
                                            if(actualSize2 == 1+1){
                                                const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_2.second->getValues({});
                                                for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                    if(joinTuples_0_2SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0 && joinTuples_0_2SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 1){
                                                        {
                                                            const Tuple* aggrTupleU = up_3.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                            if(aggrTupleU != NULL){
                                                                const auto & it = tupleToVar.find(*aggrTupleU);
                                                                if(it != tupleToVar.end()) {
                                                                    int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                    atomsTable[it->second].print();
                                                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                                }
                                                            }
                                                        }
                                                        {
                                                            const Tuple* aggrTupleU = up_4.find({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                            if(aggrTupleU != NULL){
                                                                const auto & it = tupleToVar.find(*aggrTupleU);
                                                                if(it != tupleToVar.end()) {
                                                                    int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                    atomsTable[it->second].print();
                                                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                                }
                                                            }
                                                        }
                                                        {
                                                            const Tuple* aggrTupleU = up_0.find({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                            if(aggrTupleU != NULL){
                                                                const auto & it = tupleToVar.find(*aggrTupleU);
                                                                if(it != tupleToVar.end()) {
                                                                    int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                    atomsTable[it->second].print();
                                                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }else{
                                std::vector<int> reason;
                                const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                for(int i=0;i<falseTuples0->size();i++){
                                    bool joiningTupleFound=false;
                                    int X_3 = falseTuples0->at(i)->at(0);
                                    int X_4 = falseTuples0->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_.getValues({X_4});
                                    const std::vector<const Tuple*>* undefTuples1 = &up_4_0_.getValues({X_4});
                                    const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_.getValues({X_4});
                                    for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                                        const Tuple * tuple1=NULL;
                                        if(i_1<trueTuples1->size())
                                            tuple1=trueTuples1->at(i_1);
                                        else if(i_1<trueTuples1->size()+undefTuples1->size())
                                            tuple1=undefTuples1->at(i_1-trueTuples1->size());
                                        else
                                            tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                                        int X_0 = tuple1->at(1);
                                        const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                        const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                        const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                const std::vector<const Tuple*>* falseTuples1 = &fp_4_.getValues({});
                                for(int i=0;i<falseTuples1->size();i++){
                                    bool joiningTupleFound=false;
                                    int X_4 = falseTuples1->at(i)->at(0);
                                    int X_0 = falseTuples1->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples0 = &pp_3_1_.getValues({X_4});
                                    const std::vector<const Tuple*>* undefTuples0 = &up_3_1_.getValues({X_4});
                                    const std::vector<const Tuple*>* falseTuples0 = &fp_3_1_.getValues({X_4});
                                    for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                        const Tuple * tuple0=NULL;
                                        if(i_0<trueTuples0->size())
                                            tuple0=trueTuples0->at(i_0);
                                        else if(i_0<trueTuples0->size()+undefTuples0->size())
                                            tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                        else
                                            tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                        int X_3 = tuple0->at(0);
                                        const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                        const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                        const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                    int X_0 = falseTuples2->at(i)->at(0);
                                    int X_1 = falseTuples2->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples0 = &pp_3_.getValues({});
                                    const std::vector<const Tuple*>* undefTuples0 = &up_3_.getValues({});
                                    const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                    for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                        const Tuple * tuple0=NULL;
                                        if(i_0<trueTuples0->size())
                                            tuple0=trueTuples0->at(i_0);
                                        else if(i_0<trueTuples0->size()+undefTuples0->size())
                                            tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                        else
                                            tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                        int X_3 = tuple0->at(0);
                                        int X_4 = tuple0->at(1);
                                        const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_1_.getValues({X_4, X_0});
                                        const std::vector<const Tuple*>* undefTuples1 = &up_4_0_1_.getValues({X_4, X_0});
                                        const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_1_.getValues({X_4, X_0});
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
                                if(tuple1!=tupleU){
                                    const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                                    if(it_reason1!=tupleToVar.end())
                                        reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                                }
                                const std::vector<const Tuple*>* trueTuples = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({});
                                for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                    Tuple tuplep_3_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_3);
                                    const auto & itp_3_0 = tupleToVar.find(tuplep_3_0);
                                    if(itp_3_0!=tupleToVar.end()){
                                        reason.push_back(itp_3_0->second * (tuplep_3_0.isNegated() ? -1:1));
                                    }//closing if
                                    Tuple tuplep_4_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_4);
                                    const auto & itp_4_1 = tupleToVar.find(tuplep_4_1);
                                    if(itp_4_1!=tupleToVar.end()){
                                        reason.push_back(itp_4_1->second * (tuplep_4_1.isNegated() ? -1:1));
                                    }//closing if
                                    Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4),trueTuples->at(reasonIndex)->at(5),trueTuples->at(reasonIndex)->at(6)}),&_p_0);
                                    const auto & itp_0_2 = tupleToVar.find(tuplep_0_2);
                                    if(itp_0_2!=tupleToVar.end()){
                                        reason.push_back(itp_0_2->second * (tuplep_0_2.isNegated() ? -1:1));
                                    }//closing if
                                }
                                    if(!(actualSize2>=1+1) && (joinTuples_0_2SharedVariables.first->size()>=1)){
                                        const auto & it = tupleToVar.find(*tupleU);
                                        if(it != tupleToVar.end()) {
                                            int sign = tupleU->isNegated() ? -1 : 1;
                                            auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                        }
                                    }
                                }
                                std::pair<AuxMap*,AuxMap*> joinTuples_0_3;
                                joinTuples_0_3.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                joinTuples_0_3.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                std::pair<AuxMap*,AuxMap*> joinTuples_0_3SharedVariables;
                                joinTuples_0_3SharedVariables.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                joinTuples_0_3SharedVariables.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                int actualSize3 = joinTuples_0_3SharedVariables.first->size();
                                std::set<std::vector<int>> alreadyCounted3;
                                for(const Tuple * joinTupleUndef : u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({})){
                                    if(joinTuples_0_3SharedVariables.first->getValues({joinTupleUndef->at(0)}).size()==0){
                                        if(joinTuples_0_3SharedVariables.second->getValues({joinTupleUndef->at(0)}).size()>0){
                                            std::vector<int> key({joinTupleUndef->at(0)});
                                            alreadyCounted3.insert(key);
                                        }
                                    }
                                }
                                actualSize3+=alreadyCounted3.size();
                                if(tupleU==NULL){
                                    std::vector<int> reason;
                                    const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                    for(int i=0;i<falseTuples0->size();i++){
                                        bool joiningTupleFound=false;
                                        int X_3 = falseTuples0->at(i)->at(0);
                                        int X_4 = falseTuples0->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_.getValues({X_4});
                                        const std::vector<const Tuple*>* undefTuples1 = &up_4_0_.getValues({X_4});
                                        const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_.getValues({X_4});
                                        for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                                            const Tuple * tuple1=NULL;
                                            if(i_1<trueTuples1->size())
                                                tuple1=trueTuples1->at(i_1);
                                            else if(i_1<trueTuples1->size()+undefTuples1->size())
                                                tuple1=undefTuples1->at(i_1-trueTuples1->size());
                                            else
                                                tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                                            int X_0 = tuple1->at(1);
                                            const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                            const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                            const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                    const std::vector<const Tuple*>* falseTuples1 = &fp_4_.getValues({});
                                    for(int i=0;i<falseTuples1->size();i++){
                                        bool joiningTupleFound=false;
                                        int X_4 = falseTuples1->at(i)->at(0);
                                        int X_0 = falseTuples1->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples0 = &pp_3_1_.getValues({X_4});
                                        const std::vector<const Tuple*>* undefTuples0 = &up_3_1_.getValues({X_4});
                                        const std::vector<const Tuple*>* falseTuples0 = &fp_3_1_.getValues({X_4});
                                        for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                            const Tuple * tuple0=NULL;
                                            if(i_0<trueTuples0->size())
                                                tuple0=trueTuples0->at(i_0);
                                            else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                            else
                                                tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                            int X_3 = tuple0->at(0);
                                            const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                            const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                            const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                        int X_0 = falseTuples2->at(i)->at(0);
                                        int X_1 = falseTuples2->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples0 = &pp_3_.getValues({});
                                        const std::vector<const Tuple*>* undefTuples0 = &up_3_.getValues({});
                                        const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                        for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                            const Tuple * tuple0=NULL;
                                            if(i_0<trueTuples0->size())
                                                tuple0=trueTuples0->at(i_0);
                                            else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                            else
                                                tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                            int X_3 = tuple0->at(0);
                                            int X_4 = tuple0->at(1);
                                            const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_1_.getValues({X_4, X_0});
                                            const std::vector<const Tuple*>* undefTuples1 = &up_4_0_1_.getValues({X_4, X_0});
                                            const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_1_.getValues({X_4, X_0});
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
                                    std::cout<<"StarterForReason1"<<std::endl;
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
                                    if(!(actualSize3>=1+1)){
                                        const std::vector<const Tuple*>* trueTuples = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({});
                                        for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                            Tuple tuplep_3_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_3);
                                            const auto & itp_3_0 = tupleToVar.find(tuplep_3_0);
                                            if(itp_3_0!=tupleToVar.end()){
                                                reason.push_back(itp_3_0->second * (tuplep_3_0.isNegated() ? -1:1));
                                            }//closing if
                                            Tuple tuplep_4_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_4);
                                            const auto & itp_4_1 = tupleToVar.find(tuplep_4_1);
                                            if(itp_4_1!=tupleToVar.end()){
                                                reason.push_back(itp_4_1->second * (tuplep_4_1.isNegated() ? -1:1));
                                            }//closing if
                                            Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4),trueTuples->at(reasonIndex)->at(5),trueTuples->at(reasonIndex)->at(6)}),&_p_0);
                                            const auto & itp_0_2 = tupleToVar.find(tuplep_0_2);
                                            if(itp_0_2!=tupleToVar.end()){
                                                reason.push_back(itp_0_2->second * (tuplep_0_2.isNegated() ? -1:1));
                                            }//closing if
                                        }
                                        if(joinTuples_0_3SharedVariables.first->size()>=1){
                                            std::cout<<"conflict detected in propagator"<<std::endl;
                                            propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                        }else{
                                            if(joinTuples_0_3SharedVariables.first->size() == 1-1){
                                                const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_3.second->getValues({});
                                                for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                    if(joinTuples_0_3SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0){
                                                        const Tuple* aggrTupleU=NULL;
                                                        const std::vector<const Tuple*>* tuplesp_3;
                                                        const std::vector<const Tuple*>* tuplesUp_3=&EMPTY_TUPLES;
                                                        tuplesp_3= &pp_3_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                        if(aggrTupleU==NULL)
                                                            tuplesUp_3= &up_3_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                        for(int j2=0;j2<tuplesp_3->size()+tuplesUp_3->size();j2++){
                                                            const Tuple* auxTuple2=NULL;
                                                            bool addedToReason=false;
                                                            if(j2 < tuplesp_3->size()){
                                                                auxTuple2 = tuplesp_3->at(j2);
                                                                const auto & it = tupleToVar.find(*auxTuple2);
                                                                if(it!=tupleToVar.end()){
                                                                    addedToReason=true;
                                                                    reason.push_back(it->second * (auxTuple2->isNegated() ? -1:1));
                                                                }//closing if
                                                                if(tuplesUp_3 != &EMPTY_TUPLES) {
                                                                }
                                                            }else{
                                                                auxTuple2 = tuplesUp_3->at(j2-tuplesp_3->size());
                                                                aggrTupleU = auxTuple2;
                                                            }
                                                            const std::vector<const Tuple*>* tuplesp_4;
                                                            const std::vector<const Tuple*>* tuplesUp_4=&EMPTY_TUPLES;
                                                            tuplesp_4= &pp_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                            if(aggrTupleU==NULL)
                                                                tuplesUp_4= &up_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                            for(int j4=0;j4<tuplesp_4->size()+tuplesUp_4->size();j4++){
                                                                const Tuple* auxTuple4=NULL;
                                                                bool addedToReason=false;
                                                                if(j4 < tuplesp_4->size()){
                                                                    auxTuple4 = tuplesp_4->at(j4);
                                                                    const auto & it = tupleToVar.find(*auxTuple4);
                                                                    if(it!=tupleToVar.end()){
                                                                        addedToReason=true;
                                                                        reason.push_back(it->second * (auxTuple4->isNegated() ? -1:1));
                                                                    }//closing if
                                                                    if(tuplesUp_4 != &EMPTY_TUPLES) {
                                                                    }
                                                                }else{
                                                                    auxTuple4 = tuplesUp_4->at(j4-tuplesp_4->size());
                                                                    aggrTupleU = auxTuple4;
                                                                }
                                                                const std::vector<const Tuple*>* tuplesp_0;
                                                                const std::vector<const Tuple*>* tuplesUp_0=&EMPTY_TUPLES;
                                                                tuplesp_0= &pp_0_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                                if(aggrTupleU==NULL)
                                                                    tuplesUp_0= &up_0_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                                for(int j7=0;j7<tuplesp_0->size()+tuplesUp_0->size();j7++){
                                                                    const Tuple* auxTuple7=NULL;
                                                                    bool addedToReason=false;
                                                                    if(j7 < tuplesp_0->size()){
                                                                        auxTuple7 = tuplesp_0->at(j7);
                                                                        const auto & it = tupleToVar.find(*auxTuple7);
                                                                        if(it!=tupleToVar.end()){
                                                                            addedToReason=true;
                                                                            reason.push_back(it->second * (auxTuple7->isNegated() ? -1:1));
                                                                        }//closing if
                                                                        if(tuplesUp_0 != &EMPTY_TUPLES) {
                                                                        }
                                                                    }else{
                                                                        auxTuple7 = tuplesUp_0->at(j7-tuplesp_0->size());
                                                                        aggrTupleU = auxTuple7;
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
                                    }
                                }else{
                                const std::vector<const Tuple*>* trueTuples = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({});
                                std::vector<int> reason;
                                for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                    Tuple tuplep_3_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_3);
                                    const auto & itp_3_0 = tupleToVar.find(tuplep_3_0);
                                    if(itp_3_0!=tupleToVar.end()){
                                        reason.push_back(itp_3_0->second * (tuplep_3_0.isNegated() ? -1:1));
                                    }//closing if
                                    Tuple tuplep_4_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_4);
                                    const auto & itp_4_1 = tupleToVar.find(tuplep_4_1);
                                    if(itp_4_1!=tupleToVar.end()){
                                        reason.push_back(itp_4_1->second * (tuplep_4_1.isNegated() ? -1:1));
                                    }//closing if
                                    Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4),trueTuples->at(reasonIndex)->at(5),trueTuples->at(reasonIndex)->at(6)}),&_p_0);
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
                                if(tuple1!=tupleU){
                                    const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                                    if(it_reason1!=tupleToVar.end())
                                        reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                                }
                                const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                for(int i=0;i<falseTuples0->size();i++){
                                    bool joiningTupleFound=false;
                                    int X_3 = falseTuples0->at(i)->at(0);
                                    int X_4 = falseTuples0->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_.getValues({X_4});
                                    const std::vector<const Tuple*>* undefTuples1 = &up_4_0_.getValues({X_4});
                                    const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_.getValues({X_4});
                                    for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                                        const Tuple * tuple1=NULL;
                                        if(i_1<trueTuples1->size())
                                            tuple1=trueTuples1->at(i_1);
                                        else if(i_1<trueTuples1->size()+undefTuples1->size())
                                            tuple1=undefTuples1->at(i_1-trueTuples1->size());
                                        else
                                            tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                                        int X_0 = tuple1->at(1);
                                        const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                        const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                        const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                const std::vector<const Tuple*>* falseTuples1 = &fp_4_.getValues({});
                                for(int i=0;i<falseTuples1->size();i++){
                                    bool joiningTupleFound=false;
                                    int X_4 = falseTuples1->at(i)->at(0);
                                    int X_0 = falseTuples1->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples0 = &pp_3_1_.getValues({X_4});
                                    const std::vector<const Tuple*>* undefTuples0 = &up_3_1_.getValues({X_4});
                                    const std::vector<const Tuple*>* falseTuples0 = &fp_3_1_.getValues({X_4});
                                    for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                        const Tuple * tuple0=NULL;
                                        if(i_0<trueTuples0->size())
                                            tuple0=trueTuples0->at(i_0);
                                        else if(i_0<trueTuples0->size()+undefTuples0->size())
                                            tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                        else
                                            tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                        int X_3 = tuple0->at(0);
                                        const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                        const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                        const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                    int X_0 = falseTuples2->at(i)->at(0);
                                    int X_1 = falseTuples2->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples0 = &pp_3_.getValues({});
                                    const std::vector<const Tuple*>* undefTuples0 = &up_3_.getValues({});
                                    const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                    for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                        const Tuple * tuple0=NULL;
                                        if(i_0<trueTuples0->size())
                                            tuple0=trueTuples0->at(i_0);
                                        else if(i_0<trueTuples0->size()+undefTuples0->size())
                                            tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                        else
                                            tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                        int X_3 = tuple0->at(0);
                                        int X_4 = tuple0->at(1);
                                        const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_1_.getValues({X_4, X_0});
                                        const std::vector<const Tuple*>* undefTuples1 = &up_4_0_1_.getValues({X_4, X_0});
                                        const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_1_.getValues({X_4, X_0});
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
                                    if(joinTuples_0_3SharedVariables.first->size()>=1 && (!(actualSize3>=1+1))){
                                        const auto & it = tupleToVar.find(*tupleU);
                                        if(it != tupleToVar.end()) {
                                            int sign = tupleU->isNegated() ? -1 : 1;
                                            auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
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
            if(facts[i] > 0){
                {
                    const Tuple * tupleU = NULL;
                    bool tupleUNegated = false;
                    if( (*tuple0)[0] == 3){
                        const std::vector<const Tuple* >* tuples;
                        tuples = &pp_3_0_.getValues({4});
                        const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                        if(tupleU == NULL){
                            tuplesU = &up_3_0_.getValues({4});
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
                            int X_2 = (*tuple1)[1];
                            if( (*tuple1)[0] == 4){
                                std::pair<AuxMap*,AuxMap*> joinTuples_0_2;
                                joinTuples_0_2.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                joinTuples_0_2.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                std::pair<AuxMap*,AuxMap*> joinTuples_0_2SharedVariables;
                                joinTuples_0_2SharedVariables.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                joinTuples_0_2SharedVariables.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                int actualSize2 = joinTuples_0_2SharedVariables.first->size();
                                std::set<std::vector<int>> alreadyCounted2;
                                for(const Tuple * joinTupleUndef : u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({})){
                                    if(joinTuples_0_2SharedVariables.first->getValues({joinTupleUndef->at(0)}).size()==0){
                                        if(joinTuples_0_2SharedVariables.second->getValues({joinTupleUndef->at(0)}).size()>0){
                                            std::vector<int> key({joinTupleUndef->at(0)});
                                            alreadyCounted2.insert(key);
                                        }
                                    }
                                }
                                actualSize2+=alreadyCounted2.size();
                                if(tupleU==NULL){
                                    const std::vector<const Tuple*>* trueTuples = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({});
                                    std::vector<int> reason;
                                    for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                        Tuple tuplep_3_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_3);
                                        const auto & itp_3_0 = tupleToVar.find(tuplep_3_0);
                                        if(itp_3_0!=tupleToVar.end()){
                                            reason.push_back(itp_3_0->second * (tuplep_3_0.isNegated() ? -1:1));
                                        }//closing if
                                        Tuple tuplep_4_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_4);
                                        const auto & itp_4_1 = tupleToVar.find(tuplep_4_1);
                                        if(itp_4_1!=tupleToVar.end()){
                                            reason.push_back(itp_4_1->second * (tuplep_4_1.isNegated() ? -1:1));
                                        }//closing if
                                        Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4),trueTuples->at(reasonIndex)->at(5),trueTuples->at(reasonIndex)->at(6)}),&_p_0);
                                        const auto & itp_0_2 = tupleToVar.find(tuplep_0_2);
                                        if(itp_0_2!=tupleToVar.end()){
                                            reason.push_back(itp_0_2->second * (tuplep_0_2.isNegated() ? -1:1));
                                        }//closing if
                                    }
                                    std::cout<<"StarterForReason1"<<std::endl;
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
                                    if(joinTuples_0_2SharedVariables.first->size()>=1){
                                        const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                        for(int i=0;i<falseTuples0->size();i++){
                                            bool joiningTupleFound=false;
                                            int X_3 = falseTuples0->at(i)->at(0);
                                            int X_4 = falseTuples0->at(i)->at(1);
                                            const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_.getValues({X_4});
                                            const std::vector<const Tuple*>* undefTuples1 = &up_4_0_.getValues({X_4});
                                            const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_.getValues({X_4});
                                            for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                                                const Tuple * tuple1=NULL;
                                                if(i_1<trueTuples1->size())
                                                    tuple1=trueTuples1->at(i_1);
                                                else if(i_1<trueTuples1->size()+undefTuples1->size())
                                                    tuple1=undefTuples1->at(i_1-trueTuples1->size());
                                                else
                                                    tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                                                int X_0 = tuple1->at(1);
                                                const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                                const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                                const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                        const std::vector<const Tuple*>* falseTuples1 = &fp_4_.getValues({});
                                        for(int i=0;i<falseTuples1->size();i++){
                                            bool joiningTupleFound=false;
                                            int X_4 = falseTuples1->at(i)->at(0);
                                            int X_0 = falseTuples1->at(i)->at(1);
                                            const std::vector<const Tuple*>* trueTuples0 = &pp_3_1_.getValues({X_4});
                                            const std::vector<const Tuple*>* undefTuples0 = &up_3_1_.getValues({X_4});
                                            const std::vector<const Tuple*>* falseTuples0 = &fp_3_1_.getValues({X_4});
                                            for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                                const Tuple * tuple0=NULL;
                                                if(i_0<trueTuples0->size())
                                                    tuple0=trueTuples0->at(i_0);
                                                else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                    tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                                else
                                                    tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                                int X_3 = tuple0->at(0);
                                                const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                                const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                                const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                            int X_0 = falseTuples2->at(i)->at(0);
                                            int X_1 = falseTuples2->at(i)->at(1);
                                            const std::vector<const Tuple*>* trueTuples0 = &pp_3_.getValues({});
                                            const std::vector<const Tuple*>* undefTuples0 = &up_3_.getValues({});
                                            const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                            for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                                const Tuple * tuple0=NULL;
                                                if(i_0<trueTuples0->size())
                                                    tuple0=trueTuples0->at(i_0);
                                                else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                    tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                                else
                                                    tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                                int X_3 = tuple0->at(0);
                                                int X_4 = tuple0->at(1);
                                                const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_1_.getValues({X_4, X_0});
                                                const std::vector<const Tuple*>* undefTuples1 = &up_4_0_1_.getValues({X_4, X_0});
                                                const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_1_.getValues({X_4, X_0});
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
                                        if(!(actualSize2>=1+1)){
                                            std::cout<<"conflict detected in propagator"<<std::endl;
                                            propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                        }else{
                                            if(actualSize2 == 1+1){
                                                const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_2.second->getValues({});
                                                for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                    if(joinTuples_0_2SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0 && joinTuples_0_2SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 1){
                                                        {
                                                            const Tuple* aggrTupleU = up_3.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                            if(aggrTupleU != NULL){
                                                                const auto & it = tupleToVar.find(*aggrTupleU);
                                                                if(it != tupleToVar.end()) {
                                                                    int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                    atomsTable[it->second].print();
                                                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                                }
                                                            }
                                                        }
                                                        {
                                                            const Tuple* aggrTupleU = up_4.find({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                            if(aggrTupleU != NULL){
                                                                const auto & it = tupleToVar.find(*aggrTupleU);
                                                                if(it != tupleToVar.end()) {
                                                                    int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                    atomsTable[it->second].print();
                                                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                                }
                                                            }
                                                        }
                                                        {
                                                            const Tuple* aggrTupleU = up_0.find({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                            if(aggrTupleU != NULL){
                                                                const auto & it = tupleToVar.find(*aggrTupleU);
                                                                if(it != tupleToVar.end()) {
                                                                    int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                    atomsTable[it->second].print();
                                                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }else{
                                std::vector<int> reason;
                                const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                for(int i=0;i<falseTuples0->size();i++){
                                    bool joiningTupleFound=false;
                                    int X_3 = falseTuples0->at(i)->at(0);
                                    int X_4 = falseTuples0->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_.getValues({X_4});
                                    const std::vector<const Tuple*>* undefTuples1 = &up_4_0_.getValues({X_4});
                                    const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_.getValues({X_4});
                                    for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                                        const Tuple * tuple1=NULL;
                                        if(i_1<trueTuples1->size())
                                            tuple1=trueTuples1->at(i_1);
                                        else if(i_1<trueTuples1->size()+undefTuples1->size())
                                            tuple1=undefTuples1->at(i_1-trueTuples1->size());
                                        else
                                            tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                                        int X_0 = tuple1->at(1);
                                        const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                        const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                        const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                const std::vector<const Tuple*>* falseTuples1 = &fp_4_.getValues({});
                                for(int i=0;i<falseTuples1->size();i++){
                                    bool joiningTupleFound=false;
                                    int X_4 = falseTuples1->at(i)->at(0);
                                    int X_0 = falseTuples1->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples0 = &pp_3_1_.getValues({X_4});
                                    const std::vector<const Tuple*>* undefTuples0 = &up_3_1_.getValues({X_4});
                                    const std::vector<const Tuple*>* falseTuples0 = &fp_3_1_.getValues({X_4});
                                    for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                        const Tuple * tuple0=NULL;
                                        if(i_0<trueTuples0->size())
                                            tuple0=trueTuples0->at(i_0);
                                        else if(i_0<trueTuples0->size()+undefTuples0->size())
                                            tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                        else
                                            tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                        int X_3 = tuple0->at(0);
                                        const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                        const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                        const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                    int X_0 = falseTuples2->at(i)->at(0);
                                    int X_1 = falseTuples2->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples0 = &pp_3_.getValues({});
                                    const std::vector<const Tuple*>* undefTuples0 = &up_3_.getValues({});
                                    const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                    for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                        const Tuple * tuple0=NULL;
                                        if(i_0<trueTuples0->size())
                                            tuple0=trueTuples0->at(i_0);
                                        else if(i_0<trueTuples0->size()+undefTuples0->size())
                                            tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                        else
                                            tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                        int X_3 = tuple0->at(0);
                                        int X_4 = tuple0->at(1);
                                        const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_1_.getValues({X_4, X_0});
                                        const std::vector<const Tuple*>* undefTuples1 = &up_4_0_1_.getValues({X_4, X_0});
                                        const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_1_.getValues({X_4, X_0});
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
                                if(tuple1!=tupleU){
                                    const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                                    if(it_reason1!=tupleToVar.end())
                                        reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                                }
                                const std::vector<const Tuple*>* trueTuples = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({});
                                for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                    Tuple tuplep_3_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_3);
                                    const auto & itp_3_0 = tupleToVar.find(tuplep_3_0);
                                    if(itp_3_0!=tupleToVar.end()){
                                        reason.push_back(itp_3_0->second * (tuplep_3_0.isNegated() ? -1:1));
                                    }//closing if
                                    Tuple tuplep_4_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_4);
                                    const auto & itp_4_1 = tupleToVar.find(tuplep_4_1);
                                    if(itp_4_1!=tupleToVar.end()){
                                        reason.push_back(itp_4_1->second * (tuplep_4_1.isNegated() ? -1:1));
                                    }//closing if
                                    Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4),trueTuples->at(reasonIndex)->at(5),trueTuples->at(reasonIndex)->at(6)}),&_p_0);
                                    const auto & itp_0_2 = tupleToVar.find(tuplep_0_2);
                                    if(itp_0_2!=tupleToVar.end()){
                                        reason.push_back(itp_0_2->second * (tuplep_0_2.isNegated() ? -1:1));
                                    }//closing if
                                }
                                    if(!(actualSize2>=1+1) && (joinTuples_0_2SharedVariables.first->size()>=1)){
                                        const auto & it = tupleToVar.find(*tupleU);
                                        if(it != tupleToVar.end()) {
                                            int sign = tupleU->isNegated() ? -1 : 1;
                                            auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                        }
                                    }
                                }
                                std::pair<AuxMap*,AuxMap*> joinTuples_0_3;
                                joinTuples_0_3.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                joinTuples_0_3.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                std::pair<AuxMap*,AuxMap*> joinTuples_0_3SharedVariables;
                                joinTuples_0_3SharedVariables.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                joinTuples_0_3SharedVariables.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                int actualSize3 = joinTuples_0_3SharedVariables.first->size();
                                std::set<std::vector<int>> alreadyCounted3;
                                for(const Tuple * joinTupleUndef : u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({})){
                                    if(joinTuples_0_3SharedVariables.first->getValues({joinTupleUndef->at(0)}).size()==0){
                                        if(joinTuples_0_3SharedVariables.second->getValues({joinTupleUndef->at(0)}).size()>0){
                                            std::vector<int> key({joinTupleUndef->at(0)});
                                            alreadyCounted3.insert(key);
                                        }
                                    }
                                }
                                actualSize3+=alreadyCounted3.size();
                                if(tupleU==NULL){
                                    std::vector<int> reason;
                                    const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                    for(int i=0;i<falseTuples0->size();i++){
                                        bool joiningTupleFound=false;
                                        int X_3 = falseTuples0->at(i)->at(0);
                                        int X_4 = falseTuples0->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_.getValues({X_4});
                                        const std::vector<const Tuple*>* undefTuples1 = &up_4_0_.getValues({X_4});
                                        const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_.getValues({X_4});
                                        for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                                            const Tuple * tuple1=NULL;
                                            if(i_1<trueTuples1->size())
                                                tuple1=trueTuples1->at(i_1);
                                            else if(i_1<trueTuples1->size()+undefTuples1->size())
                                                tuple1=undefTuples1->at(i_1-trueTuples1->size());
                                            else
                                                tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                                            int X_0 = tuple1->at(1);
                                            const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                            const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                            const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                    const std::vector<const Tuple*>* falseTuples1 = &fp_4_.getValues({});
                                    for(int i=0;i<falseTuples1->size();i++){
                                        bool joiningTupleFound=false;
                                        int X_4 = falseTuples1->at(i)->at(0);
                                        int X_0 = falseTuples1->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples0 = &pp_3_1_.getValues({X_4});
                                        const std::vector<const Tuple*>* undefTuples0 = &up_3_1_.getValues({X_4});
                                        const std::vector<const Tuple*>* falseTuples0 = &fp_3_1_.getValues({X_4});
                                        for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                            const Tuple * tuple0=NULL;
                                            if(i_0<trueTuples0->size())
                                                tuple0=trueTuples0->at(i_0);
                                            else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                            else
                                                tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                            int X_3 = tuple0->at(0);
                                            const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                            const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                            const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                        int X_0 = falseTuples2->at(i)->at(0);
                                        int X_1 = falseTuples2->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples0 = &pp_3_.getValues({});
                                        const std::vector<const Tuple*>* undefTuples0 = &up_3_.getValues({});
                                        const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                        for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                            const Tuple * tuple0=NULL;
                                            if(i_0<trueTuples0->size())
                                                tuple0=trueTuples0->at(i_0);
                                            else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                            else
                                                tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                            int X_3 = tuple0->at(0);
                                            int X_4 = tuple0->at(1);
                                            const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_1_.getValues({X_4, X_0});
                                            const std::vector<const Tuple*>* undefTuples1 = &up_4_0_1_.getValues({X_4, X_0});
                                            const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_1_.getValues({X_4, X_0});
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
                                    std::cout<<"StarterForReason1"<<std::endl;
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
                                    if(!(actualSize3>=1+1)){
                                        const std::vector<const Tuple*>* trueTuples = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({});
                                        for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                            Tuple tuplep_3_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_3);
                                            const auto & itp_3_0 = tupleToVar.find(tuplep_3_0);
                                            if(itp_3_0!=tupleToVar.end()){
                                                reason.push_back(itp_3_0->second * (tuplep_3_0.isNegated() ? -1:1));
                                            }//closing if
                                            Tuple tuplep_4_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_4);
                                            const auto & itp_4_1 = tupleToVar.find(tuplep_4_1);
                                            if(itp_4_1!=tupleToVar.end()){
                                                reason.push_back(itp_4_1->second * (tuplep_4_1.isNegated() ? -1:1));
                                            }//closing if
                                            Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4),trueTuples->at(reasonIndex)->at(5),trueTuples->at(reasonIndex)->at(6)}),&_p_0);
                                            const auto & itp_0_2 = tupleToVar.find(tuplep_0_2);
                                            if(itp_0_2!=tupleToVar.end()){
                                                reason.push_back(itp_0_2->second * (tuplep_0_2.isNegated() ? -1:1));
                                            }//closing if
                                        }
                                        if(joinTuples_0_3SharedVariables.first->size()>=1){
                                            std::cout<<"conflict detected in propagator"<<std::endl;
                                            propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                        }else{
                                            if(joinTuples_0_3SharedVariables.first->size() == 1-1){
                                                const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_3.second->getValues({});
                                                for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                    if(joinTuples_0_3SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0){
                                                        const Tuple* aggrTupleU=NULL;
                                                        const std::vector<const Tuple*>* tuplesp_3;
                                                        const std::vector<const Tuple*>* tuplesUp_3=&EMPTY_TUPLES;
                                                        tuplesp_3= &pp_3_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                        if(aggrTupleU==NULL)
                                                            tuplesUp_3= &up_3_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                        for(int j2=0;j2<tuplesp_3->size()+tuplesUp_3->size();j2++){
                                                            const Tuple* auxTuple2=NULL;
                                                            bool addedToReason=false;
                                                            if(j2 < tuplesp_3->size()){
                                                                auxTuple2 = tuplesp_3->at(j2);
                                                                const auto & it = tupleToVar.find(*auxTuple2);
                                                                if(it!=tupleToVar.end()){
                                                                    addedToReason=true;
                                                                    reason.push_back(it->second * (auxTuple2->isNegated() ? -1:1));
                                                                }//closing if
                                                                if(tuplesUp_3 != &EMPTY_TUPLES) {
                                                                }
                                                            }else{
                                                                auxTuple2 = tuplesUp_3->at(j2-tuplesp_3->size());
                                                                aggrTupleU = auxTuple2;
                                                            }
                                                            const std::vector<const Tuple*>* tuplesp_4;
                                                            const std::vector<const Tuple*>* tuplesUp_4=&EMPTY_TUPLES;
                                                            tuplesp_4= &pp_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                            if(aggrTupleU==NULL)
                                                                tuplesUp_4= &up_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                            for(int j4=0;j4<tuplesp_4->size()+tuplesUp_4->size();j4++){
                                                                const Tuple* auxTuple4=NULL;
                                                                bool addedToReason=false;
                                                                if(j4 < tuplesp_4->size()){
                                                                    auxTuple4 = tuplesp_4->at(j4);
                                                                    const auto & it = tupleToVar.find(*auxTuple4);
                                                                    if(it!=tupleToVar.end()){
                                                                        addedToReason=true;
                                                                        reason.push_back(it->second * (auxTuple4->isNegated() ? -1:1));
                                                                    }//closing if
                                                                    if(tuplesUp_4 != &EMPTY_TUPLES) {
                                                                    }
                                                                }else{
                                                                    auxTuple4 = tuplesUp_4->at(j4-tuplesp_4->size());
                                                                    aggrTupleU = auxTuple4;
                                                                }
                                                                const std::vector<const Tuple*>* tuplesp_0;
                                                                const std::vector<const Tuple*>* tuplesUp_0=&EMPTY_TUPLES;
                                                                tuplesp_0= &pp_0_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                                if(aggrTupleU==NULL)
                                                                    tuplesUp_0= &up_0_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                                for(int j7=0;j7<tuplesp_0->size()+tuplesUp_0->size();j7++){
                                                                    const Tuple* auxTuple7=NULL;
                                                                    bool addedToReason=false;
                                                                    if(j7 < tuplesp_0->size()){
                                                                        auxTuple7 = tuplesp_0->at(j7);
                                                                        const auto & it = tupleToVar.find(*auxTuple7);
                                                                        if(it!=tupleToVar.end()){
                                                                            addedToReason=true;
                                                                            reason.push_back(it->second * (auxTuple7->isNegated() ? -1:1));
                                                                        }//closing if
                                                                        if(tuplesUp_0 != &EMPTY_TUPLES) {
                                                                        }
                                                                    }else{
                                                                        auxTuple7 = tuplesUp_0->at(j7-tuplesp_0->size());
                                                                        aggrTupleU = auxTuple7;
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
                                    }
                                }else{
                                const std::vector<const Tuple*>* trueTuples = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({});
                                std::vector<int> reason;
                                for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                    Tuple tuplep_3_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_3);
                                    const auto & itp_3_0 = tupleToVar.find(tuplep_3_0);
                                    if(itp_3_0!=tupleToVar.end()){
                                        reason.push_back(itp_3_0->second * (tuplep_3_0.isNegated() ? -1:1));
                                    }//closing if
                                    Tuple tuplep_4_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_4);
                                    const auto & itp_4_1 = tupleToVar.find(tuplep_4_1);
                                    if(itp_4_1!=tupleToVar.end()){
                                        reason.push_back(itp_4_1->second * (tuplep_4_1.isNegated() ? -1:1));
                                    }//closing if
                                    Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4),trueTuples->at(reasonIndex)->at(5),trueTuples->at(reasonIndex)->at(6)}),&_p_0);
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
                                if(tuple1!=tupleU){
                                    const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                                    if(it_reason1!=tupleToVar.end())
                                        reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                                }
                                const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                for(int i=0;i<falseTuples0->size();i++){
                                    bool joiningTupleFound=false;
                                    int X_3 = falseTuples0->at(i)->at(0);
                                    int X_4 = falseTuples0->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_.getValues({X_4});
                                    const std::vector<const Tuple*>* undefTuples1 = &up_4_0_.getValues({X_4});
                                    const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_.getValues({X_4});
                                    for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                                        const Tuple * tuple1=NULL;
                                        if(i_1<trueTuples1->size())
                                            tuple1=trueTuples1->at(i_1);
                                        else if(i_1<trueTuples1->size()+undefTuples1->size())
                                            tuple1=undefTuples1->at(i_1-trueTuples1->size());
                                        else
                                            tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                                        int X_0 = tuple1->at(1);
                                        const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                        const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                        const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                const std::vector<const Tuple*>* falseTuples1 = &fp_4_.getValues({});
                                for(int i=0;i<falseTuples1->size();i++){
                                    bool joiningTupleFound=false;
                                    int X_4 = falseTuples1->at(i)->at(0);
                                    int X_0 = falseTuples1->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples0 = &pp_3_1_.getValues({X_4});
                                    const std::vector<const Tuple*>* undefTuples0 = &up_3_1_.getValues({X_4});
                                    const std::vector<const Tuple*>* falseTuples0 = &fp_3_1_.getValues({X_4});
                                    for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                        const Tuple * tuple0=NULL;
                                        if(i_0<trueTuples0->size())
                                            tuple0=trueTuples0->at(i_0);
                                        else if(i_0<trueTuples0->size()+undefTuples0->size())
                                            tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                        else
                                            tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                        int X_3 = tuple0->at(0);
                                        const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                        const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                        const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                    int X_0 = falseTuples2->at(i)->at(0);
                                    int X_1 = falseTuples2->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples0 = &pp_3_.getValues({});
                                    const std::vector<const Tuple*>* undefTuples0 = &up_3_.getValues({});
                                    const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                    for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                        const Tuple * tuple0=NULL;
                                        if(i_0<trueTuples0->size())
                                            tuple0=trueTuples0->at(i_0);
                                        else if(i_0<trueTuples0->size()+undefTuples0->size())
                                            tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                        else
                                            tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                        int X_3 = tuple0->at(0);
                                        int X_4 = tuple0->at(1);
                                        const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_1_.getValues({X_4, X_0});
                                        const std::vector<const Tuple*>* undefTuples1 = &up_4_0_1_.getValues({X_4, X_0});
                                        const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_1_.getValues({X_4, X_0});
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
                                    if(joinTuples_0_3SharedVariables.first->size()>=1 && (!(actualSize3>=1+1))){
                                        const auto & it = tupleToVar.find(*tupleU);
                                        if(it != tupleToVar.end()) {
                                            int sign = tupleU->isNegated() ? -1 : 1;
                                            auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                        }
                                    }
                                }
                            }//close par
                        }//close par
                    }//close par
                }//close loop nested join
            }//close loop nested join
        }//close predicate joins
        if(facts[i] < 0){
            bool tupleUNegated = false;
            const Tuple * tupleU = NULL;
            if(starter.getPredicateName()== &_p_3 || starter.getPredicateName()== &_p_4 || starter.getPredicateName()== &_p_0){
                std::pair<AuxMap*,AuxMap*> joinTuples_0_3;
                joinTuples_0_3.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                joinTuples_0_3.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                int actualSize = joinTuples_0_3.first->size();
                std::set<std::vector<int>> alreadyCounted;
                for(const Tuple * joinTupleUndef : u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({})){
                    if(joinTuples_0_3.first->getValues({joinTupleUndef->at(0)}).size()==0){
                        if(joinTuples_0_3.second->getValues({joinTupleUndef->at(0)}).size()>0){
                            std::vector<int> key({joinTupleUndef->at(0)});
                            alreadyCounted.insert(key);
                        }
                    }
                }
                actualSize+=alreadyCounted.size();
                if(!(actualSize>=1+1)){
                    const Tuple * tuple1 = (wp_2.find({3}));
                    if(!tuple1 && !tupleU ){
                        tuple1 = tupleU = (up_2.find({3}));
                        tupleUNegated = false;
                    }
                    if(tuple1){
                        if( (*tuple1)[0] == 3){
                            const std::vector<const Tuple* >* tuples;
                            tuples = &pp_3_0_.getValues({4});
                            const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                            if(tupleU == NULL){
                                tuplesU = &up_3_0_.getValues({4});
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
                                int X_2 = (*tuple2)[1];
                                if( (*tuple2)[0] == 4){
                                    std::pair<AuxMap*,AuxMap*> joinTuples_0_2;
                                    joinTuples_0_2.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                    joinTuples_0_2.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                    std::pair<AuxMap*,AuxMap*> joinTuples_0_2SharedVariables;
                                    joinTuples_0_2SharedVariables.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                    joinTuples_0_2SharedVariables.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                    if(tupleU==NULL){
                                        const std::vector<const Tuple*>* trueTuples = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({});
                                        std::vector<int> reason;
                                        for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                            Tuple tuplep_3_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_3);
                                            const auto & itp_3_0 = tupleToVar.find(tuplep_3_0);
                                            if(itp_3_0!=tupleToVar.end()){
                                                reason.push_back(itp_3_0->second * (tuplep_3_0.isNegated() ? -1:1));
                                            }//closing if
                                            Tuple tuplep_4_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_4);
                                            const auto & itp_4_1 = tupleToVar.find(tuplep_4_1);
                                            if(itp_4_1!=tupleToVar.end()){
                                                reason.push_back(itp_4_1->second * (tuplep_4_1.isNegated() ? -1:1));
                                            }//closing if
                                            Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4),trueTuples->at(reasonIndex)->at(5),trueTuples->at(reasonIndex)->at(6)}),&_p_0);
                                            const auto & itp_0_2 = tupleToVar.find(tuplep_0_2);
                                            if(itp_0_2!=tupleToVar.end()){
                                                reason.push_back(itp_0_2->second * (tuplep_0_2.isNegated() ? -1:1));
                                            }//closing if
                                        }
                                        std::cout<<"StarterForReason0"<<std::endl;
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
                                        if(joinTuples_0_2SharedVariables.first->size()>=1){
                                            std::cout<<"conflict detected in propagator"<<std::endl;
                                            propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                        }else{
                                            if(joinTuples_0_2SharedVariables.first->size() == 1-1){
                                                const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_2.second->getValues({});
                                                for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                    if(joinTuples_0_2SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0){
                                                        const Tuple* aggrTupleU=NULL;
                                                        const std::vector<const Tuple*>* tuplesp_3;
                                                        const std::vector<const Tuple*>* tuplesUp_3=&EMPTY_TUPLES;
                                                        tuplesp_3= &pp_3_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                        if(aggrTupleU==NULL)
                                                            tuplesUp_3= &up_3_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                        for(int j2=0;j2<tuplesp_3->size()+tuplesUp_3->size();j2++){
                                                            const Tuple* auxTuple2=NULL;
                                                            bool addedToReason=false;
                                                            if(j2 < tuplesp_3->size()){
                                                                auxTuple2 = tuplesp_3->at(j2);
                                                                const auto & it = tupleToVar.find(*auxTuple2);
                                                                if(it!=tupleToVar.end()){
                                                                    addedToReason=true;
                                                                    reason.push_back(it->second * (auxTuple2->isNegated() ? -1:1));
                                                                }//closing if
                                                                if(tuplesUp_3 != &EMPTY_TUPLES) {
                                                                }
                                                            }else{
                                                                auxTuple2 = tuplesUp_3->at(j2-tuplesp_3->size());
                                                                aggrTupleU = auxTuple2;
                                                            }
                                                            const std::vector<const Tuple*>* tuplesp_4;
                                                            const std::vector<const Tuple*>* tuplesUp_4=&EMPTY_TUPLES;
                                                            tuplesp_4= &pp_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                            if(aggrTupleU==NULL)
                                                                tuplesUp_4= &up_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                            for(int j4=0;j4<tuplesp_4->size()+tuplesUp_4->size();j4++){
                                                                const Tuple* auxTuple4=NULL;
                                                                bool addedToReason=false;
                                                                if(j4 < tuplesp_4->size()){
                                                                    auxTuple4 = tuplesp_4->at(j4);
                                                                    const auto & it = tupleToVar.find(*auxTuple4);
                                                                    if(it!=tupleToVar.end()){
                                                                        addedToReason=true;
                                                                        reason.push_back(it->second * (auxTuple4->isNegated() ? -1:1));
                                                                    }//closing if
                                                                    if(tuplesUp_4 != &EMPTY_TUPLES) {
                                                                    }
                                                                }else{
                                                                    auxTuple4 = tuplesUp_4->at(j4-tuplesp_4->size());
                                                                    aggrTupleU = auxTuple4;
                                                                }
                                                                const std::vector<const Tuple*>* tuplesp_0;
                                                                const std::vector<const Tuple*>* tuplesUp_0=&EMPTY_TUPLES;
                                                                tuplesp_0= &pp_0_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                                if(aggrTupleU==NULL)
                                                                    tuplesUp_0= &up_0_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                                for(int j7=0;j7<tuplesp_0->size()+tuplesUp_0->size();j7++){
                                                                    const Tuple* auxTuple7=NULL;
                                                                    bool addedToReason=false;
                                                                    if(j7 < tuplesp_0->size()){
                                                                        auxTuple7 = tuplesp_0->at(j7);
                                                                        const auto & it = tupleToVar.find(*auxTuple7);
                                                                        if(it!=tupleToVar.end()){
                                                                            addedToReason=true;
                                                                            reason.push_back(it->second * (auxTuple7->isNegated() ? -1:1));
                                                                        }//closing if
                                                                        if(tuplesUp_0 != &EMPTY_TUPLES) {
                                                                        }
                                                                    }else{
                                                                        auxTuple7 = tuplesUp_0->at(j7-tuplesp_0->size());
                                                                        aggrTupleU = auxTuple7;
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
                                    const std::vector<const Tuple*>* trueTuples = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({});
                                    std::vector<int> reason;
                                    for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                        Tuple tuplep_3_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_3);
                                        const auto & itp_3_0 = tupleToVar.find(tuplep_3_0);
                                        if(itp_3_0!=tupleToVar.end()){
                                            reason.push_back(itp_3_0->second * (tuplep_3_0.isNegated() ? -1:1));
                                        }//closing if
                                        Tuple tuplep_4_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_4);
                                        const auto & itp_4_1 = tupleToVar.find(tuplep_4_1);
                                        if(itp_4_1!=tupleToVar.end()){
                                            reason.push_back(itp_4_1->second * (tuplep_4_1.isNegated() ? -1:1));
                                        }//closing if
                                        Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4),trueTuples->at(reasonIndex)->at(5),trueTuples->at(reasonIndex)->at(6)}),&_p_0);
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
                                    if(tuple2!=tupleU){
                                        const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                        if(it_reason2!=tupleToVar.end())
                                            reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                    }
                                        if(joinTuples_0_2SharedVariables.first->size()>=1){
                                            const auto & it = tupleToVar.find(*tupleU);
                                            if(it != tupleToVar.end()) {
                                                int sign = tupleU->isNegated() ? -1 : 1;
                                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                            }
                                        }
                                    }
                                    std::vector<int> reason;
                                    const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                    for(int i=0;i<falseTuples0->size();i++){
                                        bool joiningTupleFound=false;
                                        int X_3 = falseTuples0->at(i)->at(0);
                                        int X_4 = falseTuples0->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_.getValues({X_4});
                                        const std::vector<const Tuple*>* undefTuples1 = &up_4_0_.getValues({X_4});
                                        const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_.getValues({X_4});
                                        for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                                            const Tuple * tuple1=NULL;
                                            if(i_1<trueTuples1->size())
                                                tuple1=trueTuples1->at(i_1);
                                            else if(i_1<trueTuples1->size()+undefTuples1->size())
                                                tuple1=undefTuples1->at(i_1-trueTuples1->size());
                                            else
                                                tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                                            int X_0 = tuple1->at(1);
                                            const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                            const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                            const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                    const std::vector<const Tuple*>* falseTuples1 = &fp_4_.getValues({});
                                    for(int i=0;i<falseTuples1->size();i++){
                                        bool joiningTupleFound=false;
                                        int X_4 = falseTuples1->at(i)->at(0);
                                        int X_0 = falseTuples1->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples0 = &pp_3_1_.getValues({X_4});
                                        const std::vector<const Tuple*>* undefTuples0 = &up_3_1_.getValues({X_4});
                                        const std::vector<const Tuple*>* falseTuples0 = &fp_3_1_.getValues({X_4});
                                        for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                            const Tuple * tuple0=NULL;
                                            if(i_0<trueTuples0->size())
                                                tuple0=trueTuples0->at(i_0);
                                            else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                            else
                                                tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                            int X_3 = tuple0->at(0);
                                            const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                            const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                            const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                        int X_0 = falseTuples2->at(i)->at(0);
                                        int X_1 = falseTuples2->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples0 = &pp_3_.getValues({});
                                        const std::vector<const Tuple*>* undefTuples0 = &up_3_.getValues({});
                                        const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                        for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                            const Tuple * tuple0=NULL;
                                            if(i_0<trueTuples0->size())
                                                tuple0=trueTuples0->at(i_0);
                                            else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                            else
                                                tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                            int X_3 = tuple0->at(0);
                                            int X_4 = tuple0->at(1);
                                            const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_1_.getValues({X_4, X_0});
                                            const std::vector<const Tuple*>* undefTuples1 = &up_4_0_1_.getValues({X_4, X_0});
                                            const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_1_.getValues({X_4, X_0});
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
                                    if(tuple2!=tupleU){
                                        const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                        if(it_reason2!=tupleToVar.end())
                                            reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                    }
                                    if(!tupleU) {
                                        std::cout<<"conflict detected in propagator"<<std::endl;
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
                    }//close par
                }//close par
                else{
                    if(actualSize==1+1){
                        const Tuple * tuple1 = (wp_2.find({3}));
                        if(!tuple1 && !tupleU ){
                            tuple1 = tupleU = (up_2.find({3}));
                            tupleUNegated = false;
                        }
                        if(tuple1){
                            if( (*tuple1)[0] == 3){
                                const std::vector<const Tuple* >* tuples;
                                tuples = &pp_3_0_.getValues({4});
                                const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                                if(tupleU == NULL){
                                    tuplesU = &up_3_0_.getValues({4});
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
                                    int X_2 = (*tuple2)[1];
                                    if( (*tuple2)[0] == 4){
                                        std::pair<AuxMap*,AuxMap*> joinTuples_0_2;
                                        joinTuples_0_2.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                        joinTuples_0_2.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                        std::pair<AuxMap*,AuxMap*> joinTuples_0_2SharedVariables;
                                        joinTuples_0_2SharedVariables.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                        joinTuples_0_2SharedVariables.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                        if(tupleU==NULL){
                                            const std::vector<const Tuple*>* trueTuples = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({});
                                            std::vector<int> reason;
                                            for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                                Tuple tuplep_3_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_3);
                                                const auto & itp_3_0 = tupleToVar.find(tuplep_3_0);
                                                if(itp_3_0!=tupleToVar.end()){
                                                    reason.push_back(itp_3_0->second * (tuplep_3_0.isNegated() ? -1:1));
                                                }//closing if
                                                Tuple tuplep_4_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_4);
                                                const auto & itp_4_1 = tupleToVar.find(tuplep_4_1);
                                                if(itp_4_1!=tupleToVar.end()){
                                                    reason.push_back(itp_4_1->second * (tuplep_4_1.isNegated() ? -1:1));
                                                }//closing if
                                                Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4),trueTuples->at(reasonIndex)->at(5),trueTuples->at(reasonIndex)->at(6)}),&_p_0);
                                                const auto & itp_0_2 = tupleToVar.find(tuplep_0_2);
                                                if(itp_0_2!=tupleToVar.end()){
                                                    reason.push_back(itp_0_2->second * (tuplep_0_2.isNegated() ? -1:1));
                                                }//closing if
                                            }
                                            std::cout<<"StarterForReason0"<<std::endl;
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
                                            if(joinTuples_0_2SharedVariables.first->size()>=1){
                                                std::cout<<"conflict detected in propagator"<<std::endl;
                                                propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                            }else{
                                                if(joinTuples_0_2SharedVariables.first->size() == 1-1){
                                                    const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_2.second->getValues({});
                                                    for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                        if(joinTuples_0_2SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0){
                                                            const Tuple* aggrTupleU=NULL;
                                                            const std::vector<const Tuple*>* tuplesp_3;
                                                            const std::vector<const Tuple*>* tuplesUp_3=&EMPTY_TUPLES;
                                                            tuplesp_3= &pp_3_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                            if(aggrTupleU==NULL)
                                                                tuplesUp_3= &up_3_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                            for(int j2=0;j2<tuplesp_3->size()+tuplesUp_3->size();j2++){
                                                                const Tuple* auxTuple2=NULL;
                                                                bool addedToReason=false;
                                                                if(j2 < tuplesp_3->size()){
                                                                    auxTuple2 = tuplesp_3->at(j2);
                                                                    const auto & it = tupleToVar.find(*auxTuple2);
                                                                    if(it!=tupleToVar.end()){
                                                                        addedToReason=true;
                                                                        reason.push_back(it->second * (auxTuple2->isNegated() ? -1:1));
                                                                    }//closing if
                                                                    if(tuplesUp_3 != &EMPTY_TUPLES) {
                                                                    }
                                                                }else{
                                                                    auxTuple2 = tuplesUp_3->at(j2-tuplesp_3->size());
                                                                    aggrTupleU = auxTuple2;
                                                                }
                                                                const std::vector<const Tuple*>* tuplesp_4;
                                                                const std::vector<const Tuple*>* tuplesUp_4=&EMPTY_TUPLES;
                                                                tuplesp_4= &pp_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                                if(aggrTupleU==NULL)
                                                                    tuplesUp_4= &up_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                                for(int j4=0;j4<tuplesp_4->size()+tuplesUp_4->size();j4++){
                                                                    const Tuple* auxTuple4=NULL;
                                                                    bool addedToReason=false;
                                                                    if(j4 < tuplesp_4->size()){
                                                                        auxTuple4 = tuplesp_4->at(j4);
                                                                        const auto & it = tupleToVar.find(*auxTuple4);
                                                                        if(it!=tupleToVar.end()){
                                                                            addedToReason=true;
                                                                            reason.push_back(it->second * (auxTuple4->isNegated() ? -1:1));
                                                                        }//closing if
                                                                        if(tuplesUp_4 != &EMPTY_TUPLES) {
                                                                        }
                                                                    }else{
                                                                        auxTuple4 = tuplesUp_4->at(j4-tuplesp_4->size());
                                                                        aggrTupleU = auxTuple4;
                                                                    }
                                                                    const std::vector<const Tuple*>* tuplesp_0;
                                                                    const std::vector<const Tuple*>* tuplesUp_0=&EMPTY_TUPLES;
                                                                    tuplesp_0= &pp_0_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                                    if(aggrTupleU==NULL)
                                                                        tuplesUp_0= &up_0_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                                    for(int j7=0;j7<tuplesp_0->size()+tuplesUp_0->size();j7++){
                                                                        const Tuple* auxTuple7=NULL;
                                                                        bool addedToReason=false;
                                                                        if(j7 < tuplesp_0->size()){
                                                                            auxTuple7 = tuplesp_0->at(j7);
                                                                            const auto & it = tupleToVar.find(*auxTuple7);
                                                                            if(it!=tupleToVar.end()){
                                                                                addedToReason=true;
                                                                                reason.push_back(it->second * (auxTuple7->isNegated() ? -1:1));
                                                                            }//closing if
                                                                            if(tuplesUp_0 != &EMPTY_TUPLES) {
                                                                            }
                                                                        }else{
                                                                            auxTuple7 = tuplesUp_0->at(j7-tuplesp_0->size());
                                                                            aggrTupleU = auxTuple7;
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
                                        const std::vector<const Tuple*>* trueTuples = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({});
                                        std::vector<int> reason;
                                        for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                            Tuple tuplep_3_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_3);
                                            const auto & itp_3_0 = tupleToVar.find(tuplep_3_0);
                                            if(itp_3_0!=tupleToVar.end()){
                                                reason.push_back(itp_3_0->second * (tuplep_3_0.isNegated() ? -1:1));
                                            }//closing if
                                            Tuple tuplep_4_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_4);
                                            const auto & itp_4_1 = tupleToVar.find(tuplep_4_1);
                                            if(itp_4_1!=tupleToVar.end()){
                                                reason.push_back(itp_4_1->second * (tuplep_4_1.isNegated() ? -1:1));
                                            }//closing if
                                            Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4),trueTuples->at(reasonIndex)->at(5),trueTuples->at(reasonIndex)->at(6)}),&_p_0);
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
                                        if(tuple2!=tupleU){
                                            const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                            if(it_reason2!=tupleToVar.end())
                                                reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                        }
                                            if(joinTuples_0_2SharedVariables.first->size()>=1){
                                                const auto & it = tupleToVar.find(*tupleU);
                                                if(it != tupleToVar.end()) {
                                                    int sign = tupleU->isNegated() ? -1 : 1;
                                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                }
                                            }
                                        }
                                        if(tupleU ==  NULL) {
                                            std::pair<AuxMap*,AuxMap*> joinTuples_0_3;
                                            joinTuples_0_3.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                            joinTuples_0_3.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                            std::pair<AuxMap*,AuxMap*> joinTuples_0_3SharedVariables;
                                            joinTuples_0_3SharedVariables.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                            joinTuples_0_3SharedVariables.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                            std::vector<int> reason;
                                            const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                            for(int i=0;i<falseTuples0->size();i++){
                                                bool joiningTupleFound=false;
                                                int X_3 = falseTuples0->at(i)->at(0);
                                                int X_4 = falseTuples0->at(i)->at(1);
                                                const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_.getValues({X_4});
                                                const std::vector<const Tuple*>* undefTuples1 = &up_4_0_.getValues({X_4});
                                                const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_.getValues({X_4});
                                                for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                                                    const Tuple * tuple1=NULL;
                                                    if(i_1<trueTuples1->size())
                                                        tuple1=trueTuples1->at(i_1);
                                                    else if(i_1<trueTuples1->size()+undefTuples1->size())
                                                        tuple1=undefTuples1->at(i_1-trueTuples1->size());
                                                    else
                                                        tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                                                    int X_0 = tuple1->at(1);
                                                    const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                                    const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                                    const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                            const std::vector<const Tuple*>* falseTuples1 = &fp_4_.getValues({});
                                            for(int i=0;i<falseTuples1->size();i++){
                                                bool joiningTupleFound=false;
                                                int X_4 = falseTuples1->at(i)->at(0);
                                                int X_0 = falseTuples1->at(i)->at(1);
                                                const std::vector<const Tuple*>* trueTuples0 = &pp_3_1_.getValues({X_4});
                                                const std::vector<const Tuple*>* undefTuples0 = &up_3_1_.getValues({X_4});
                                                const std::vector<const Tuple*>* falseTuples0 = &fp_3_1_.getValues({X_4});
                                                for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                                    const Tuple * tuple0=NULL;
                                                    if(i_0<trueTuples0->size())
                                                        tuple0=trueTuples0->at(i_0);
                                                    else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                        tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                                    else
                                                        tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                                    int X_3 = tuple0->at(0);
                                                    const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                                    const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                                    const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                                int X_0 = falseTuples2->at(i)->at(0);
                                                int X_1 = falseTuples2->at(i)->at(1);
                                                const std::vector<const Tuple*>* trueTuples0 = &pp_3_.getValues({});
                                                const std::vector<const Tuple*>* undefTuples0 = &up_3_.getValues({});
                                                const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                                for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                                    const Tuple * tuple0=NULL;
                                                    if(i_0<trueTuples0->size())
                                                        tuple0=trueTuples0->at(i_0);
                                                    else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                        tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                                    else
                                                        tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                                    int X_3 = tuple0->at(0);
                                                    int X_4 = tuple0->at(1);
                                                    const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_1_.getValues({X_4, X_0});
                                                    const std::vector<const Tuple*>* undefTuples1 = &up_4_0_1_.getValues({X_4, X_0});
                                                    const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_1_.getValues({X_4, X_0});
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
                                            const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                            if(it_reason2!=tupleToVar.end())
                                                reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                            const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_3.second->getValues({});
                                            for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                if(joinTuples_0_3SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0 && joinTuples_0_3SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 1){
                                                    {
                                                        const Tuple* aggrTupleU = up_3.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                        if(aggrTupleU != NULL){
                                                            const auto & it = tupleToVar.find(*aggrTupleU);
                                                            if(it != tupleToVar.end()) {
                                                                int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                atomsTable[it->second].print();
                                                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                            }
                                                        }
                                                    }
                                                    {
                                                        const Tuple* aggrTupleU = up_4.find({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                        if(aggrTupleU != NULL){
                                                            const auto & it = tupleToVar.find(*aggrTupleU);
                                                            if(it != tupleToVar.end()) {
                                                                int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                atomsTable[it->second].print();
                                                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                            }
                                                        }
                                                    }
                                                    {
                                                        const Tuple* aggrTupleU = up_0.find({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                        if(aggrTupleU != NULL){
                                                            const auto & it = tupleToVar.find(*aggrTupleU);
                                                            if(it != tupleToVar.end()) {
                                                                int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                atomsTable[it->second].print();
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
            }//close par
        }//close if
        if(facts[i] > 0){
            bool tupleUNegated = false;
            const Tuple * tupleU = NULL;
            if(starter.getPredicateName()== &_p_3 || starter.getPredicateName()== &_p_4 || starter.getPredicateName()== &_p_0){
                std::pair<AuxMap*,AuxMap*> joinTuples_0_2;
                joinTuples_0_2.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                joinTuples_0_2.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                if(joinTuples_0_2.first->size()>=1){
                    const Tuple * tuple1 = (wp_2.find({3}));
                    if(!tuple1 && !tupleU ){
                        tuple1 = tupleU = (up_2.find({3}));
                        tupleUNegated = false;
                    }
                    if(tuple1){
                        if( (*tuple1)[0] == 3){
                            const std::vector<const Tuple* >* tuples;
                            tuples = &pp_3_0_.getValues({4});
                            const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                            if(tupleU == NULL){
                                tuplesU = &up_3_0_.getValues({4});
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
                                int X_2 = (*tuple2)[1];
                                if( (*tuple2)[0] == 4){
                                    std::pair<AuxMap*,AuxMap*> joinTuples_0_3;
                                    joinTuples_0_3.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                    joinTuples_0_3.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                    std::pair<AuxMap*,AuxMap*> joinTuples_0_3SharedVariables;
                                    joinTuples_0_3SharedVariables.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                    joinTuples_0_3SharedVariables.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                    int actualSize3 = joinTuples_0_3SharedVariables.first->size();
                                    std::set<std::vector<int>> alreadyCounted3;
                                    for(const Tuple * joinTupleUndef : u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({})){
                                        if(joinTuples_0_3SharedVariables.first->getValues({joinTupleUndef->at(0)}).size()==0){
                                            if(joinTuples_0_3SharedVariables.second->getValues({joinTupleUndef->at(0)}).size()>0){
                                                std::vector<int> key({joinTupleUndef->at(0)});
                                                alreadyCounted3.insert(key);
                                            }
                                        }
                                    }
                                    actualSize3+=alreadyCounted3.size();
                                    if(tupleU==NULL){
                                        std::vector<int> reason;
                                        const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                        for(int i=0;i<falseTuples0->size();i++){
                                            bool joiningTupleFound=false;
                                            int X_3 = falseTuples0->at(i)->at(0);
                                            int X_4 = falseTuples0->at(i)->at(1);
                                            const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_.getValues({X_4});
                                            const std::vector<const Tuple*>* undefTuples1 = &up_4_0_.getValues({X_4});
                                            const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_.getValues({X_4});
                                            for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                                                const Tuple * tuple1=NULL;
                                                if(i_1<trueTuples1->size())
                                                    tuple1=trueTuples1->at(i_1);
                                                else if(i_1<trueTuples1->size()+undefTuples1->size())
                                                    tuple1=undefTuples1->at(i_1-trueTuples1->size());
                                                else
                                                    tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                                                int X_0 = tuple1->at(1);
                                                const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                                const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                                const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                        const std::vector<const Tuple*>* falseTuples1 = &fp_4_.getValues({});
                                        for(int i=0;i<falseTuples1->size();i++){
                                            bool joiningTupleFound=false;
                                            int X_4 = falseTuples1->at(i)->at(0);
                                            int X_0 = falseTuples1->at(i)->at(1);
                                            const std::vector<const Tuple*>* trueTuples0 = &pp_3_1_.getValues({X_4});
                                            const std::vector<const Tuple*>* undefTuples0 = &up_3_1_.getValues({X_4});
                                            const std::vector<const Tuple*>* falseTuples0 = &fp_3_1_.getValues({X_4});
                                            for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                                const Tuple * tuple0=NULL;
                                                if(i_0<trueTuples0->size())
                                                    tuple0=trueTuples0->at(i_0);
                                                else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                    tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                                else
                                                    tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                                int X_3 = tuple0->at(0);
                                                const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                                const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                                const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                            int X_0 = falseTuples2->at(i)->at(0);
                                            int X_1 = falseTuples2->at(i)->at(1);
                                            const std::vector<const Tuple*>* trueTuples0 = &pp_3_.getValues({});
                                            const std::vector<const Tuple*>* undefTuples0 = &up_3_.getValues({});
                                            const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                            for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                                const Tuple * tuple0=NULL;
                                                if(i_0<trueTuples0->size())
                                                    tuple0=trueTuples0->at(i_0);
                                                else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                    tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                                else
                                                    tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                                int X_3 = tuple0->at(0);
                                                int X_4 = tuple0->at(1);
                                                const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_1_.getValues({X_4, X_0});
                                                const std::vector<const Tuple*>* undefTuples1 = &up_4_0_1_.getValues({X_4, X_0});
                                                const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_1_.getValues({X_4, X_0});
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
                                        std::cout<<"StarterForReason0"<<std::endl;
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
                                        if(!(actualSize3>=1+1)){
                                            std::cout<<"conflict detected in propagator"<<std::endl;
                                            propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                        }else{
                                            if(actualSize3 == 1+1){
                                                const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_3.second->getValues({});
                                                for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                    if(joinTuples_0_3SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0 && joinTuples_0_3SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 1){
                                                        {
                                                            const Tuple* aggrTupleU = up_3.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                            if(aggrTupleU != NULL){
                                                                const auto & it = tupleToVar.find(*aggrTupleU);
                                                                if(it != tupleToVar.end()) {
                                                                    int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                    atomsTable[it->second].print();
                                                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                                }
                                                            }
                                                        }
                                                        {
                                                            const Tuple* aggrTupleU = up_4.find({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                            if(aggrTupleU != NULL){
                                                                const auto & it = tupleToVar.find(*aggrTupleU);
                                                                if(it != tupleToVar.end()) {
                                                                    int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                    atomsTable[it->second].print();
                                                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                                }
                                                            }
                                                        }
                                                        {
                                                            const Tuple* aggrTupleU = up_0.find({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                            if(aggrTupleU != NULL){
                                                                const auto & it = tupleToVar.find(*aggrTupleU);
                                                                if(it != tupleToVar.end()) {
                                                                    int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                    atomsTable[it->second].print();
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
                                    const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                    for(int i=0;i<falseTuples0->size();i++){
                                        bool joiningTupleFound=false;
                                        int X_3 = falseTuples0->at(i)->at(0);
                                        int X_4 = falseTuples0->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_.getValues({X_4});
                                        const std::vector<const Tuple*>* undefTuples1 = &up_4_0_.getValues({X_4});
                                        const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_.getValues({X_4});
                                        for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                                            const Tuple * tuple1=NULL;
                                            if(i_1<trueTuples1->size())
                                                tuple1=trueTuples1->at(i_1);
                                            else if(i_1<trueTuples1->size()+undefTuples1->size())
                                                tuple1=undefTuples1->at(i_1-trueTuples1->size());
                                            else
                                                tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                                            int X_0 = tuple1->at(1);
                                            const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                            const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                            const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                    const std::vector<const Tuple*>* falseTuples1 = &fp_4_.getValues({});
                                    for(int i=0;i<falseTuples1->size();i++){
                                        bool joiningTupleFound=false;
                                        int X_4 = falseTuples1->at(i)->at(0);
                                        int X_0 = falseTuples1->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples0 = &pp_3_1_.getValues({X_4});
                                        const std::vector<const Tuple*>* undefTuples0 = &up_3_1_.getValues({X_4});
                                        const std::vector<const Tuple*>* falseTuples0 = &fp_3_1_.getValues({X_4});
                                        for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                            const Tuple * tuple0=NULL;
                                            if(i_0<trueTuples0->size())
                                                tuple0=trueTuples0->at(i_0);
                                            else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                            else
                                                tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                            int X_3 = tuple0->at(0);
                                            const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                            const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                            const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                        int X_0 = falseTuples2->at(i)->at(0);
                                        int X_1 = falseTuples2->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples0 = &pp_3_.getValues({});
                                        const std::vector<const Tuple*>* undefTuples0 = &up_3_.getValues({});
                                        const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                        for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                            const Tuple * tuple0=NULL;
                                            if(i_0<trueTuples0->size())
                                                tuple0=trueTuples0->at(i_0);
                                            else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                            else
                                                tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                            int X_3 = tuple0->at(0);
                                            int X_4 = tuple0->at(1);
                                            const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_1_.getValues({X_4, X_0});
                                            const std::vector<const Tuple*>* undefTuples1 = &up_4_0_1_.getValues({X_4, X_0});
                                            const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_1_.getValues({X_4, X_0});
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
                                    if(tuple2!=tupleU){
                                        const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                        if(it_reason2!=tupleToVar.end())
                                            reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                    }
                                        if(!(actualSize3>=1+1)){
                                            const auto & it = tupleToVar.find(*tupleU);
                                            if(it != tupleToVar.end()) {
                                                int sign = tupleU->isNegated() ? -1 : 1;
                                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                            }
                                        }
                                    }
                                    const std::vector<const Tuple*>* trueTuples = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({});
                                    std::vector<int> reason;
                                    for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                        Tuple tuplep_3_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_3);
                                        const auto & itp_3_0 = tupleToVar.find(tuplep_3_0);
                                        if(itp_3_0!=tupleToVar.end()){
                                            reason.push_back(itp_3_0->second * (tuplep_3_0.isNegated() ? -1:1));
                                        }//closing if
                                        Tuple tuplep_4_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_4);
                                        const auto & itp_4_1 = tupleToVar.find(tuplep_4_1);
                                        if(itp_4_1!=tupleToVar.end()){
                                            reason.push_back(itp_4_1->second * (tuplep_4_1.isNegated() ? -1:1));
                                        }//closing if
                                        Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4),trueTuples->at(reasonIndex)->at(5),trueTuples->at(reasonIndex)->at(6)}),&_p_0);
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
                                    if(tuple2!=tupleU){
                                        const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                        if(it_reason2!=tupleToVar.end())
                                            reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                    }
                                    if(!tupleU) {
                                        std::cout<<"conflict detected in propagator"<<std::endl;
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
                    }//close par
                }//close par
                else{
                    if(joinTuples_0_2.first->size()==1-1){
                        const Tuple * tuple1 = (wp_2.find({3}));
                        if(!tuple1 && !tupleU ){
                            tuple1 = tupleU = (up_2.find({3}));
                            tupleUNegated = false;
                        }
                        if(tuple1){
                            if( (*tuple1)[0] == 3){
                                const std::vector<const Tuple* >* tuples;
                                tuples = &pp_3_0_.getValues({4});
                                const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                                if(tupleU == NULL){
                                    tuplesU = &up_3_0_.getValues({4});
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
                                    int X_2 = (*tuple2)[1];
                                    if( (*tuple2)[0] == 4){
                                        std::pair<AuxMap*,AuxMap*> joinTuples_0_3;
                                        joinTuples_0_3.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                        joinTuples_0_3.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                        std::pair<AuxMap*,AuxMap*> joinTuples_0_3SharedVariables;
                                        joinTuples_0_3SharedVariables.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                        joinTuples_0_3SharedVariables.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                        int actualSize3 = joinTuples_0_3SharedVariables.first->size();
                                        std::set<std::vector<int>> alreadyCounted3;
                                        for(const Tuple * joinTupleUndef : u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({})){
                                            if(joinTuples_0_3SharedVariables.first->getValues({joinTupleUndef->at(0)}).size()==0){
                                                if(joinTuples_0_3SharedVariables.second->getValues({joinTupleUndef->at(0)}).size()>0){
                                                    std::vector<int> key({joinTupleUndef->at(0)});
                                                    alreadyCounted3.insert(key);
                                                }
                                            }
                                        }
                                        actualSize3+=alreadyCounted3.size();
                                        if(tupleU==NULL){
                                            std::vector<int> reason;
                                            const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                            for(int i=0;i<falseTuples0->size();i++){
                                                bool joiningTupleFound=false;
                                                int X_3 = falseTuples0->at(i)->at(0);
                                                int X_4 = falseTuples0->at(i)->at(1);
                                                const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_.getValues({X_4});
                                                const std::vector<const Tuple*>* undefTuples1 = &up_4_0_.getValues({X_4});
                                                const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_.getValues({X_4});
                                                for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                                                    const Tuple * tuple1=NULL;
                                                    if(i_1<trueTuples1->size())
                                                        tuple1=trueTuples1->at(i_1);
                                                    else if(i_1<trueTuples1->size()+undefTuples1->size())
                                                        tuple1=undefTuples1->at(i_1-trueTuples1->size());
                                                    else
                                                        tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                                                    int X_0 = tuple1->at(1);
                                                    const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                                    const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                                    const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                            const std::vector<const Tuple*>* falseTuples1 = &fp_4_.getValues({});
                                            for(int i=0;i<falseTuples1->size();i++){
                                                bool joiningTupleFound=false;
                                                int X_4 = falseTuples1->at(i)->at(0);
                                                int X_0 = falseTuples1->at(i)->at(1);
                                                const std::vector<const Tuple*>* trueTuples0 = &pp_3_1_.getValues({X_4});
                                                const std::vector<const Tuple*>* undefTuples0 = &up_3_1_.getValues({X_4});
                                                const std::vector<const Tuple*>* falseTuples0 = &fp_3_1_.getValues({X_4});
                                                for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                                    const Tuple * tuple0=NULL;
                                                    if(i_0<trueTuples0->size())
                                                        tuple0=trueTuples0->at(i_0);
                                                    else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                        tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                                    else
                                                        tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                                    int X_3 = tuple0->at(0);
                                                    const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                                    const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                                    const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                                int X_0 = falseTuples2->at(i)->at(0);
                                                int X_1 = falseTuples2->at(i)->at(1);
                                                const std::vector<const Tuple*>* trueTuples0 = &pp_3_.getValues({});
                                                const std::vector<const Tuple*>* undefTuples0 = &up_3_.getValues({});
                                                const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                                for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                                    const Tuple * tuple0=NULL;
                                                    if(i_0<trueTuples0->size())
                                                        tuple0=trueTuples0->at(i_0);
                                                    else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                        tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                                    else
                                                        tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                                    int X_3 = tuple0->at(0);
                                                    int X_4 = tuple0->at(1);
                                                    const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_1_.getValues({X_4, X_0});
                                                    const std::vector<const Tuple*>* undefTuples1 = &up_4_0_1_.getValues({X_4, X_0});
                                                    const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_1_.getValues({X_4, X_0});
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
                                            std::cout<<"StarterForReason0"<<std::endl;
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
                                            if(!(actualSize3>=1+1)){
                                                std::cout<<"conflict detected in propagator"<<std::endl;
                                                propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                            }else{
                                                if(actualSize3 == 1+1){
                                                    const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_3.second->getValues({});
                                                    for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                        if(joinTuples_0_3SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0 && joinTuples_0_3SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 1){
                                                            {
                                                                const Tuple* aggrTupleU = up_3.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                                if(aggrTupleU != NULL){
                                                                    const auto & it = tupleToVar.find(*aggrTupleU);
                                                                    if(it != tupleToVar.end()) {
                                                                        int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                        atomsTable[it->second].print();
                                                                        auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                                    }
                                                                }
                                                            }
                                                            {
                                                                const Tuple* aggrTupleU = up_4.find({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                                if(aggrTupleU != NULL){
                                                                    const auto & it = tupleToVar.find(*aggrTupleU);
                                                                    if(it != tupleToVar.end()) {
                                                                        int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                        atomsTable[it->second].print();
                                                                        auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                                    }
                                                                }
                                                            }
                                                            {
                                                                const Tuple* aggrTupleU = up_0.find({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                                if(aggrTupleU != NULL){
                                                                    const auto & it = tupleToVar.find(*aggrTupleU);
                                                                    if(it != tupleToVar.end()) {
                                                                        int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                        atomsTable[it->second].print();
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
                                        const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                        for(int i=0;i<falseTuples0->size();i++){
                                            bool joiningTupleFound=false;
                                            int X_3 = falseTuples0->at(i)->at(0);
                                            int X_4 = falseTuples0->at(i)->at(1);
                                            const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_.getValues({X_4});
                                            const std::vector<const Tuple*>* undefTuples1 = &up_4_0_.getValues({X_4});
                                            const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_.getValues({X_4});
                                            for(int i_1=0;!joiningTupleFound && i_1 < trueTuples1->size()+undefTuples1->size()+falseTuples1->size();i_1++){
                                                const Tuple * tuple1=NULL;
                                                if(i_1<trueTuples1->size())
                                                    tuple1=trueTuples1->at(i_1);
                                                else if(i_1<trueTuples1->size()+undefTuples1->size())
                                                    tuple1=undefTuples1->at(i_1-trueTuples1->size());
                                                else
                                                    tuple1=falseTuples1->at(i_1-trueTuples1->size()-undefTuples1->size());
                                                int X_0 = tuple1->at(1);
                                                const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                                const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                                const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                        const std::vector<const Tuple*>* falseTuples1 = &fp_4_.getValues({});
                                        for(int i=0;i<falseTuples1->size();i++){
                                            bool joiningTupleFound=false;
                                            int X_4 = falseTuples1->at(i)->at(0);
                                            int X_0 = falseTuples1->at(i)->at(1);
                                            const std::vector<const Tuple*>* trueTuples0 = &pp_3_1_.getValues({X_4});
                                            const std::vector<const Tuple*>* undefTuples0 = &up_3_1_.getValues({X_4});
                                            const std::vector<const Tuple*>* falseTuples0 = &fp_3_1_.getValues({X_4});
                                            for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                                const Tuple * tuple0=NULL;
                                                if(i_0<trueTuples0->size())
                                                    tuple0=trueTuples0->at(i_0);
                                                else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                    tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                                else
                                                    tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                                int X_3 = tuple0->at(0);
                                                const std::vector<const Tuple*>* trueTuples2 = &pp_0_0_2_.getValues({X_0, X_0});
                                                const std::vector<const Tuple*>* undefTuples2 = &up_0_0_2_.getValues({X_0, X_0});
                                                const std::vector<const Tuple*>* falseTuples2 = &fp_0_0_2_.getValues({X_0, X_0});
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
                                            int X_0 = falseTuples2->at(i)->at(0);
                                            int X_1 = falseTuples2->at(i)->at(1);
                                            const std::vector<const Tuple*>* trueTuples0 = &pp_3_.getValues({});
                                            const std::vector<const Tuple*>* undefTuples0 = &up_3_.getValues({});
                                            const std::vector<const Tuple*>* falseTuples0 = &fp_3_.getValues({});
                                            for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                                const Tuple * tuple0=NULL;
                                                if(i_0<trueTuples0->size())
                                                    tuple0=trueTuples0->at(i_0);
                                                else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                    tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                                else
                                                    tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                                int X_3 = tuple0->at(0);
                                                int X_4 = tuple0->at(1);
                                                const std::vector<const Tuple*>* trueTuples1 = &pp_4_0_1_.getValues({X_4, X_0});
                                                const std::vector<const Tuple*>* undefTuples1 = &up_4_0_1_.getValues({X_4, X_0});
                                                const std::vector<const Tuple*>* falseTuples1 = &fp_4_0_1_.getValues({X_4, X_0});
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
                                        if(tuple2!=tupleU){
                                            const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                            if(it_reason2!=tupleToVar.end())
                                                reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                        }
                                            if(!(actualSize3>=1+1)){
                                                const auto & it = tupleToVar.find(*tupleU);
                                                if(it != tupleToVar.end()) {
                                                    int sign = tupleU->isNegated() ? -1 : 1;
                                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                }
                                            }
                                        }
                                        if(tupleU ==  NULL) {
                                            std::pair<AuxMap*,AuxMap*> joinTuples_0_2;
                                            joinTuples_0_2.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                            joinTuples_0_2.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_;
                                            std::pair<AuxMap*,AuxMap*> joinTuples_0_2SharedVariables;
                                            joinTuples_0_2SharedVariables.first = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                            joinTuples_0_2SharedVariables.second = &u_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_0_;
                                            const std::vector<const Tuple*>* trueTuples = &p_p_3_X_3_X_4_p_4_X_4_X_0_p_0_X_0_X_1_X_0_.getValues({});
                                            std::vector<int> reason;
                                            for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                                Tuple tuplep_3_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_p_3);
                                                const auto & itp_3_0 = tupleToVar.find(tuplep_3_0);
                                                if(itp_3_0!=tupleToVar.end()){
                                                    reason.push_back(itp_3_0->second * (tuplep_3_0.isNegated() ? -1:1));
                                                }//closing if
                                                Tuple tuplep_4_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_p_4);
                                                const auto & itp_4_1 = tupleToVar.find(tuplep_4_1);
                                                if(itp_4_1!=tupleToVar.end()){
                                                    reason.push_back(itp_4_1->second * (tuplep_4_1.isNegated() ? -1:1));
                                                }//closing if
                                                Tuple tuplep_0_2(std::vector<int>({trueTuples->at(reasonIndex)->at(4),trueTuples->at(reasonIndex)->at(5),trueTuples->at(reasonIndex)->at(6)}),&_p_0);
                                                const auto & itp_0_2 = tupleToVar.find(tuplep_0_2);
                                                if(itp_0_2!=tupleToVar.end()){
                                                    reason.push_back(itp_0_2->second * (tuplep_0_2.isNegated() ? -1:1));
                                                }//closing if
                                            }
                                            const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                                            if(it_reason1!=tupleToVar.end())
                                                reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                                            const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                            if(it_reason2!=tupleToVar.end())
                                                reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                            const std::vector<const Tuple*>* undefinedTuples = &joinTuples_0_2.second->getValues({});
                                            for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                if(joinTuples_0_2SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(0)}).size() == 0){
                                                    const Tuple* aggrTupleU=NULL;
                                                    const std::vector<const Tuple*>* tuplesp_3;
                                                    const std::vector<const Tuple*>* tuplesUp_3=&EMPTY_TUPLES;
                                                    tuplesp_3= &pp_3_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                    if(aggrTupleU==NULL)
                                                        tuplesUp_3= &up_3_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                    for(int j2=0;j2<tuplesp_3->size()+tuplesUp_3->size();j2++){
                                                        const Tuple* auxTuple2=NULL;
                                                        bool addedToReason=false;
                                                        if(j2 < tuplesp_3->size()){
                                                            auxTuple2 = tuplesp_3->at(j2);
                                                            const auto & it = tupleToVar.find(*auxTuple2);
                                                            if(it!=tupleToVar.end()){
                                                                addedToReason=true;
                                                                reason.push_back(it->second * (auxTuple2->isNegated() ? -1:1));
                                                            }//closing if
                                                            if(tuplesUp_3 != &EMPTY_TUPLES) {
                                                            }
                                                        }else{
                                                            auxTuple2 = tuplesUp_3->at(j2-tuplesp_3->size());
                                                            aggrTupleU = auxTuple2;
                                                        }
                                                        const std::vector<const Tuple*>* tuplesp_4;
                                                        const std::vector<const Tuple*>* tuplesUp_4=&EMPTY_TUPLES;
                                                        tuplesp_4= &pp_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                        if(aggrTupleU==NULL)
                                                            tuplesUp_4= &up_4_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                        for(int j4=0;j4<tuplesp_4->size()+tuplesUp_4->size();j4++){
                                                            const Tuple* auxTuple4=NULL;
                                                            bool addedToReason=false;
                                                            if(j4 < tuplesp_4->size()){
                                                                auxTuple4 = tuplesp_4->at(j4);
                                                                const auto & it = tupleToVar.find(*auxTuple4);
                                                                if(it!=tupleToVar.end()){
                                                                    addedToReason=true;
                                                                    reason.push_back(it->second * (auxTuple4->isNegated() ? -1:1));
                                                                }//closing if
                                                                if(tuplesUp_4 != &EMPTY_TUPLES) {
                                                                }
                                                            }else{
                                                                auxTuple4 = tuplesUp_4->at(j4-tuplesp_4->size());
                                                                aggrTupleU = auxTuple4;
                                                            }
                                                            const std::vector<const Tuple*>* tuplesp_0;
                                                            const std::vector<const Tuple*>* tuplesUp_0=&EMPTY_TUPLES;
                                                            tuplesp_0= &pp_0_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                            if(aggrTupleU==NULL)
                                                                tuplesUp_0= &up_0_0_1_2_.getValues({undefinedTuples->at(iUndef)->at(4),undefinedTuples->at(iUndef)->at(5),undefinedTuples->at(iUndef)->at(6)});
                                                            for(int j7=0;j7<tuplesp_0->size()+tuplesUp_0->size();j7++){
                                                                const Tuple* auxTuple7=NULL;
                                                                bool addedToReason=false;
                                                                if(j7 < tuplesp_0->size()){
                                                                    auxTuple7 = tuplesp_0->at(j7);
                                                                    const auto & it = tupleToVar.find(*auxTuple7);
                                                                    if(it!=tupleToVar.end()){
                                                                        addedToReason=true;
                                                                        reason.push_back(it->second * (auxTuple7->isNegated() ? -1:1));
                                                                    }//closing if
                                                                    if(tuplesUp_0 != &EMPTY_TUPLES) {
                                                                    }
                                                                }else{
                                                                    auxTuple7 = tuplesUp_0->at(j7-tuplesp_0->size());
                                                                    aggrTupleU = auxTuple7;
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
            }//close par
        }//close if
    }
}
}
