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
const std::string _clique = "clique";
PredicateWSet wclique(1);
PredicateWSet uclique(1);
PredicateWSet fclique(1);
const std::string _cliquesize = "cliquesize";
PredicateWSet wcliquesize(2);
PredicateWSet ucliquesize(2);
PredicateWSet fcliquesize(2);
const std::string _in = "in";
PredicateWSet win(2);
PredicateWSet uin(2);
PredicateWSet fin(2);
const std::string _rest = "rest";
PredicateWSet wrest(3);
PredicateWSet urest(3);
PredicateWSet frest(3);
const std::string _aux = "aux";
PredicateWSet waux(2);
PredicateWSet uaux(2);
PredicateWSet faux(2);
const std::string _member = "member";
PredicateWSet wmember(2);
PredicateWSet umember(2);
PredicateWSet fmember(2);
const std::string _pair = "pair";
PredicateWSet wpair(2);
PredicateWSet upair(2);
PredicateWSet fpair(2);
const std::string _in_C_X_pair_C_X_ = "in_C_X_pair_C_X_";
PredicateWSet win_C_X_pair_C_X_(4);
PredicateWSet uin_C_X_pair_C_X_(4);
std::map<std::vector<int>, std::pair< AuxMap*,AuxMap*>*> sharedVariables_0_ToAggregate_1;
const std::string _aux_R_Y_member_Y_R_ = "aux_R_Y_member_Y_R_";
PredicateWSet waux_R_Y_member_Y_R_(4);
PredicateWSet uaux_R_Y_member_Y_R_(4);
std::map<std::vector<int>, std::pair< AuxMap*,AuxMap*>*> sharedVariables_1_ToAggregate_3;
std::map<std::vector<int>, std::pair< AuxMap*,AuxMap*>*> sharedVariables_1_ToAggregate_4;
Executor::~Executor() {
    for(auto sharedVar : sharedVariables_0_ToAggregate_1){
        delete sharedVar.second->first;
        delete sharedVar.second->second;
        delete sharedVar.second;
    }
    for(auto sharedVar : sharedVariables_1_ToAggregate_3){
        delete sharedVar.second->first;
        delete sharedVar.second->second;
        delete sharedVar.second;
    }
    for(auto sharedVar : sharedVariables_1_ToAggregate_4){
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
AuxMap pin_0_1_({0,1});
AuxMap uin_0_1_({0,1});
AuxMap fin_0_1_({0,1});
AuxMap pin_({});
AuxMap uin_({});
AuxMap fin_({});
AuxMap ppair_0_1_({0,1});
AuxMap upair_0_1_({0,1});
AuxMap fpair_0_1_({0,1});
AuxMap ppair_({});
AuxMap upair_({});
AuxMap fpair_({});
AuxMap p_in_C_X_pair_C_X_1_({1});
AuxMap u_in_C_X_pair_C_X_1_({1});
AuxMap p_in_C_X_pair_C_X_({});
AuxMap u_in_C_X_pair_C_X_({});
AuxMap p_in_C_X_pair_C_X_0_2_({0,2});
AuxMap u_in_C_X_pair_C_X_0_2_({0,2});
AuxMap p_in_C_X_pair_C_X_0_1_({0,1});
AuxMap u_in_C_X_pair_C_X_0_1_({0,1});
AuxMap p_in_C_X_pair_C_X_2_3_({2,3});
AuxMap u_in_C_X_pair_C_X_2_3_({2,3});
AuxMap pclique_0_({0});
AuxMap uclique_0_({0});
AuxMap pclique_({});
AuxMap uclique_({});
AuxMap paux_0_1_({0,1});
AuxMap uaux_0_1_({0,1});
AuxMap faux_0_1_({0,1});
AuxMap paux_({});
AuxMap uaux_({});
AuxMap faux_({});
AuxMap pmember_0_1_({0,1});
AuxMap umember_0_1_({0,1});
AuxMap fmember_0_1_({0,1});
AuxMap pmember_({});
AuxMap umember_({});
AuxMap fmember_({});
AuxMap p_aux_R_Y_member_Y_R_1_({1});
AuxMap u_aux_R_Y_member_Y_R_1_({1});
AuxMap p_aux_R_Y_member_Y_R_({});
AuxMap u_aux_R_Y_member_Y_R_({});
AuxMap p_aux_R_Y_member_Y_R_0_3_({0,3});
AuxMap u_aux_R_Y_member_Y_R_0_3_({0,3});
AuxMap p_aux_R_Y_member_Y_R_0_1_({0,1});
AuxMap u_aux_R_Y_member_Y_R_0_1_({0,1});
AuxMap p_aux_R_Y_member_Y_R_2_3_({2,3});
AuxMap u_aux_R_Y_member_Y_R_2_3_({2,3});
AuxMap prest_0_1_2_({0,1,2});
AuxMap urest_0_1_2_({0,1,2});
AuxMap pcliquesize_0_1_({0,1});
AuxMap ucliquesize_0_1_({0,1});
AuxMap prest_2_({2});
AuxMap urest_2_({2});
AuxMap prest_0_1_({0,1});
AuxMap urest_0_1_({0,1});
//printing aux maps needed for reasons of negative literals;
//printing functions prototypes for reasons of negative literals;
void explain_not_cliquesize_0_1_(const std::vector<int> &, std::unordered_set<std::string> &, std::vector<const Tuple *> &);
void explainPositiveLiteral(const Tuple *, std::unordered_set<std::string> &, std::vector<const Tuple*> &);
//printing functions for reasons of negative literals;
void explain_not_cliquesize_0_1_(const std::vector<int> & lit, std::unordered_set<std::string> & open_set, std::vector<const Tuple *> & output){
    std::string canonicalRep = _cliquesize;
    canonicalRep += std::to_string(lit[0]);
    canonicalRep += ",";
    canonicalRep += std::to_string(lit[1]);
    if(open_set.find(canonicalRep)!=open_set.end()){
        return;
    }
    open_set.insert(canonicalRep);
    open_set.erase(canonicalRep);
}
void createFunctionsMap() {
    explainNegativeFunctionsMap[&_cliquesize] = explain_not_cliquesize_0_1_;
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
    if(var<0 && ( tuple.getPredicateName() == &_aux || tuple.getPredicateName() == &_in || tuple.getPredicateName() == &_member || tuple.getPredicateName() == &_pair)){
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
    if(tuple.getPredicateName() == &_in){
        if(var > 0){
            int C = tuple.at(0);
            int X = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuples1 = ppair_0_1_.getValues({C, X});
            const std::vector<const Tuple*>& tuplesU1 = upair_0_1_.getValues({C, X});
            for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
                bool buildUndef=false;
                const Tuple* tuple1;
                if(i<tuples1.size())
                    tuple1=tuples1[i];
                else{
                    buildUndef=true;
                    tuple1=tuplesU1[i-tuples1.size()];
                }
                Tuple t(std::vector<int>({C,X,C,X}),&_in_C_X_pair_C_X_);
                if(!buildUndef){
                    if(win_C_X_pair_C_X_.find(t)==NULL){
                        const auto& insertResult = win_C_X_pair_C_X_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_in_C_X_pair_C_X_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({C,C}));
                                if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                                    joinTuples->first->insert2(*insertResult.first);
                                }
                            }
                            uin_C_X_pair_C_X_.erase(t);
                        }
                    }else{
                    }
                }else{
                    if(uin_C_X_pair_C_X_.find(t)==NULL){
                        const auto& insertResult = uin_C_X_pair_C_X_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_in_C_X_pair_C_X_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({C,C}));
                                if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                                    joinTuples->second->insert2(*insertResult.first);
                                }
                            }
                        }
                    }
                }
            }
        }else{
            int C = tuple.at(0);
            int X = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuplesU1 = upair_0_1_.getValues({C, X});
            const std::vector<const Tuple*>& tuples1 = ppair_0_1_.getValues({C, X});
            for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
                bool buildUndef=false;
                const Tuple* tuple1;
                if(i<tuples1.size())
                    tuple1=tuples1[i];
                else{
                    buildUndef=true;
                    tuple1=tuplesU1[i-tuples1.size()];
                }
                Tuple t(std::vector<int>({C,X,C,X}),&_in_C_X_pair_C_X_);
                uin_C_X_pair_C_X_.erase(t);
                win_C_X_pair_C_X_.erase(t);
            }
        }
    }
    if(tuple.getPredicateName() == &_pair){
        if(var > 0){
            int C = tuple.at(0);
            int X = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuples0 = pin_0_1_.getValues({C, X});
            const std::vector<const Tuple*>& tuplesU0 = uin_0_1_.getValues({C, X});
            for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
                bool buildUndef=false;
                const Tuple* tuple0;
                if(i<tuples0.size())
                    tuple0=tuples0[i];
                else{
                    buildUndef=true;
                    tuple0=tuplesU0[i-tuples0.size()];
                }
                Tuple t(std::vector<int>({C,X,C,X}),&_in_C_X_pair_C_X_);
                if(!buildUndef){
                    if(win_C_X_pair_C_X_.find(t)==NULL){
                        const auto& insertResult = win_C_X_pair_C_X_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_in_C_X_pair_C_X_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({C,C}));
                                if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                                    joinTuples->first->insert2(*insertResult.first);
                                }
                            }
                            uin_C_X_pair_C_X_.erase(t);
                        }
                    }else{
                    }
                }else{
                    if(uin_C_X_pair_C_X_.find(t)==NULL){
                        const auto& insertResult = uin_C_X_pair_C_X_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_in_C_X_pair_C_X_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({C,C}));
                                if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                                    joinTuples->second->insert2(*insertResult.first);
                                }
                            }
                        }
                    }
                }
            }
        }else{
            int C = tuple.at(0);
            int X = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuplesU0 = uin_0_1_.getValues({C, X});
            const std::vector<const Tuple*>& tuples0 = pin_0_1_.getValues({C, X});
            for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
                bool buildUndef=false;
                const Tuple* tuple0;
                if(i<tuples0.size())
                    tuple0=tuples0[i];
                else{
                    buildUndef=true;
                    tuple0=tuplesU0[i-tuples0.size()];
                }
                Tuple t(std::vector<int>({C,X,C,X}),&_in_C_X_pair_C_X_);
                uin_C_X_pair_C_X_.erase(t);
                win_C_X_pair_C_X_.erase(t);
            }
        }
    }
    if(tuple.getPredicateName() == &_clique){
        int C = tuple.at(0);
        if(var>0){
            Tuple sharedTuple(std::vector<int>({C,C}));
            if(!sharedVariables_0_ToAggregate_1.count(sharedTuple)){
                sharedVariables_0_ToAggregate_1[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                const std::vector<const Tuple*>* trueJoinTuples = &p_in_C_X_pair_C_X_0_2_.getValues({C,C});
                const std::vector<const Tuple*>* undefJoinTuples = &u_in_C_X_pair_C_X_0_2_.getValues({C,C});
                for(int i=0;i<trueJoinTuples->size();i++){
                    const auto findResult = win_C_X_pair_C_X_.find(Tuple(*trueJoinTuples->at(i)));
                    if(findResult!=NULL){
                        sharedVariables_0_ToAggregate_1[sharedTuple]->first->insert2(*findResult);
                    }
                }
                for(int i=0;i<undefJoinTuples->size();i++){
                    const auto findResult = uin_C_X_pair_C_X_.find(Tuple(*undefJoinTuples->at(i)));
                    if(findResult!=NULL){
                        sharedVariables_0_ToAggregate_1[sharedTuple]->second->insert2(*findResult);
                    }
                }
            }
        }else{
        }
    }
    if(tuple.getPredicateName() == &_aux){
        if(var > 0){
            int R = tuple.at(0);
            int Y = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuples1 = pmember_0_1_.getValues({Y, R});
            const std::vector<const Tuple*>& tuplesU1 = umember_0_1_.getValues({Y, R});
            for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
                bool buildUndef=false;
                const Tuple* tuple1;
                if(i<tuples1.size())
                    tuple1=tuples1[i];
                else{
                    buildUndef=true;
                    tuple1=tuplesU1[i-tuples1.size()];
                }
                Tuple t(std::vector<int>({R,Y,Y,R}),&_aux_R_Y_member_Y_R_);
                if(!buildUndef){
                    if(waux_R_Y_member_Y_R_.find(t)==NULL){
                        const auto& insertResult = waux_R_Y_member_Y_R_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_aux_R_Y_member_Y_R_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(sharedVariables_1_ToAggregate_3.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_1_ToAggregate_3[sharedTuple];
                                    joinTuples->first->insert2(*insertResult.first);
                                }
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(sharedVariables_1_ToAggregate_4.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_1_ToAggregate_4[sharedTuple];
                                    joinTuples->first->insert2(*insertResult.first);
                                }
                            }
                            uaux_R_Y_member_Y_R_.erase(t);
                        }
                    }else{
                    }
                }else{
                    if(uaux_R_Y_member_Y_R_.find(t)==NULL){
                        const auto& insertResult = uaux_R_Y_member_Y_R_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(sharedVariables_1_ToAggregate_3.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_1_ToAggregate_3[sharedTuple];
                                    joinTuples->second->insert2(*insertResult.first);
                                }
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(sharedVariables_1_ToAggregate_4.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_1_ToAggregate_4[sharedTuple];
                                    joinTuples->second->insert2(*insertResult.first);
                                }
                            }
                        }
                    }
                }
            }
        }else{
            int R = tuple.at(0);
            int Y = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuplesU1 = umember_0_1_.getValues({Y, R});
            const std::vector<const Tuple*>& tuples1 = pmember_0_1_.getValues({Y, R});
            for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
                bool buildUndef=false;
                const Tuple* tuple1;
                if(i<tuples1.size())
                    tuple1=tuples1[i];
                else{
                    buildUndef=true;
                    tuple1=tuplesU1[i-tuples1.size()];
                }
                Tuple t(std::vector<int>({R,Y,Y,R}),&_aux_R_Y_member_Y_R_);
                uaux_R_Y_member_Y_R_.erase(t);
                waux_R_Y_member_Y_R_.erase(t);
            }
        }
    }
    if(tuple.getPredicateName() == &_member){
        if(var > 0){
            int Y = tuple.at(0);
            int R = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuples0 = paux_0_1_.getValues({R, Y});
            const std::vector<const Tuple*>& tuplesU0 = uaux_0_1_.getValues({R, Y});
            for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
                bool buildUndef=false;
                const Tuple* tuple0;
                if(i<tuples0.size())
                    tuple0=tuples0[i];
                else{
                    buildUndef=true;
                    tuple0=tuplesU0[i-tuples0.size()];
                }
                Tuple t(std::vector<int>({R,Y,Y,R}),&_aux_R_Y_member_Y_R_);
                if(!buildUndef){
                    if(waux_R_Y_member_Y_R_.find(t)==NULL){
                        const auto& insertResult = waux_R_Y_member_Y_R_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_aux_R_Y_member_Y_R_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(sharedVariables_1_ToAggregate_3.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_1_ToAggregate_3[sharedTuple];
                                    joinTuples->first->insert2(*insertResult.first);
                                }
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(sharedVariables_1_ToAggregate_4.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_1_ToAggregate_4[sharedTuple];
                                    joinTuples->first->insert2(*insertResult.first);
                                }
                            }
                            uaux_R_Y_member_Y_R_.erase(t);
                        }
                    }else{
                    }
                }else{
                    if(uaux_R_Y_member_Y_R_.find(t)==NULL){
                        const auto& insertResult = uaux_R_Y_member_Y_R_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(sharedVariables_1_ToAggregate_3.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_1_ToAggregate_3[sharedTuple];
                                    joinTuples->second->insert2(*insertResult.first);
                                }
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(sharedVariables_1_ToAggregate_4.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_1_ToAggregate_4[sharedTuple];
                                    joinTuples->second->insert2(*insertResult.first);
                                }
                            }
                        }
                    }
                }
            }
        }else{
            int Y = tuple.at(0);
            int R = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuplesU0 = uaux_0_1_.getValues({R, Y});
            const std::vector<const Tuple*>& tuples0 = paux_0_1_.getValues({R, Y});
            for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
                bool buildUndef=false;
                const Tuple* tuple0;
                if(i<tuples0.size())
                    tuple0=tuples0[i];
                else{
                    buildUndef=true;
                    tuple0=tuplesU0[i-tuples0.size()];
                }
                Tuple t(std::vector<int>({R,Y,Y,R}),&_aux_R_Y_member_Y_R_);
                uaux_R_Y_member_Y_R_.erase(t);
                waux_R_Y_member_Y_R_.erase(t);
            }
        }
    }
    if(tuple.getPredicateName() == &_aux){
        if(var > 0){
            int R = tuple.at(0);
            int Y = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuples1 = pmember_0_1_.getValues({Y, R});
            const std::vector<const Tuple*>& tuplesU1 = umember_0_1_.getValues({Y, R});
            for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
                bool buildUndef=false;
                const Tuple* tuple1;
                if(i<tuples1.size())
                    tuple1=tuples1[i];
                else{
                    buildUndef=true;
                    tuple1=tuplesU1[i-tuples1.size()];
                }
                Tuple t(std::vector<int>({R,Y,Y,R}),&_aux_R_Y_member_Y_R_);
                if(!buildUndef && !buildFalse){
                    if(waux_R_Y_member_Y_R_.find(t)==NULL){
                        const auto& insertResult = waux_R_Y_member_Y_R_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_aux_R_Y_member_Y_R_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(sharedVariables_1_ToAggregate_3.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_1_ToAggregate_3[sharedTuple];
                                    joinTuples->first->insert2(*insertResult.first);
                                }
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(sharedVariables_1_ToAggregate_4.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_1_ToAggregate_4[sharedTuple];
                                    joinTuples->first->insert2(*insertResult.first);
                                }
                            }
                            uaux_R_Y_member_Y_R_.erase(t);
                        }
                    }else{
                    }
                }else{
                    if(!buildFalse){
                        if(uaux_R_Y_member_Y_R_.find(t)==NULL){
                            const auto& insertResult = uaux_R_Y_member_Y_R_.insert(Tuple(t));
                            if (insertResult.second) {
                                for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_]){
                                    auxMap -> insert2(*insertResult.first);
                                }
                                {
                                    Tuple sharedTuple(std::vector<int>({R,R}));
                                    if(sharedVariables_1_ToAggregate_3.count(sharedTuple)!=0){
                                        auto joinTuples = sharedVariables_1_ToAggregate_3[sharedTuple];
                                        joinTuples->second->insert2(*insertResult.first);
                                    }
                                }
                                {
                                    Tuple sharedTuple(std::vector<int>({R,R}));
                                    if(sharedVariables_1_ToAggregate_4.count(sharedTuple)!=0){
                                        auto joinTuples = sharedVariables_1_ToAggregate_4[sharedTuple];
                                        joinTuples->second->insert2(*insertResult.first);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }else{
            int R = tuple.at(0);
            int Y = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuplesU1 = umember_0_1_.getValues({Y, R});
            const std::vector<const Tuple*>& tuples1 = pmember_0_1_.getValues({Y, R});
            const std::vector<const Tuple*>& tuplesF1 = fmember_0_1_.getValues({Y, R});
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
                Tuple t(std::vector<int>({R,Y,Y,R}),&_aux_R_Y_member_Y_R_);
                uaux_R_Y_member_Y_R_.erase(t);
                waux_R_Y_member_Y_R_.erase(t);
            }
        }
    }
    if(tuple.getPredicateName() == &_member){
        if(var > 0){
            int Y = tuple.at(0);
            int R = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuples0 = paux_0_1_.getValues({R, Y});
            const std::vector<const Tuple*>& tuplesU0 = uaux_0_1_.getValues({R, Y});
            for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
                bool buildUndef=false;
                const Tuple* tuple0;
                if(i<tuples0.size())
                    tuple0=tuples0[i];
                else{
                    buildUndef=true;
                    tuple0=tuplesU0[i-tuples0.size()];
                }
                Tuple t(std::vector<int>({R,Y,Y,R}),&_aux_R_Y_member_Y_R_);
                if(!buildUndef && !buildFalse){
                    if(waux_R_Y_member_Y_R_.find(t)==NULL){
                        const auto& insertResult = waux_R_Y_member_Y_R_.insert(Tuple(t));
                        if (insertResult.second) {
                            for(AuxMap* auxMap : predicateToAuxiliaryMaps[&_aux_R_Y_member_Y_R_]){
                                auxMap -> insert2(*insertResult.first);
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(sharedVariables_1_ToAggregate_3.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_1_ToAggregate_3[sharedTuple];
                                    joinTuples->first->insert2(*insertResult.first);
                                }
                            }
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(sharedVariables_1_ToAggregate_4.count(sharedTuple)!=0){
                                    auto joinTuples = sharedVariables_1_ToAggregate_4[sharedTuple];
                                    joinTuples->first->insert2(*insertResult.first);
                                }
                            }
                            uaux_R_Y_member_Y_R_.erase(t);
                        }
                    }else{
                    }
                }else{
                    if(!buildFalse){
                        if(uaux_R_Y_member_Y_R_.find(t)==NULL){
                            const auto& insertResult = uaux_R_Y_member_Y_R_.insert(Tuple(t));
                            if (insertResult.second) {
                                for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_]){
                                    auxMap -> insert2(*insertResult.first);
                                }
                                {
                                    Tuple sharedTuple(std::vector<int>({R,R}));
                                    if(sharedVariables_1_ToAggregate_3.count(sharedTuple)!=0){
                                        auto joinTuples = sharedVariables_1_ToAggregate_3[sharedTuple];
                                        joinTuples->second->insert2(*insertResult.first);
                                    }
                                }
                                {
                                    Tuple sharedTuple(std::vector<int>({R,R}));
                                    if(sharedVariables_1_ToAggregate_4.count(sharedTuple)!=0){
                                        auto joinTuples = sharedVariables_1_ToAggregate_4[sharedTuple];
                                        joinTuples->second->insert2(*insertResult.first);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }else{
            int Y = tuple.at(0);
            int R = tuple.at(1);
            bool buildUndef=false;
            bool buildFalse=false;
            const std::vector<const Tuple*>& tuplesU0 = uaux_0_1_.getValues({R, Y});
            const std::vector<const Tuple*>& tuples0 = paux_0_1_.getValues({R, Y});
            const std::vector<const Tuple*>& tuplesF0 = faux_0_1_.getValues({R, Y});
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
                Tuple t(std::vector<int>({R,Y,Y,R}),&_aux_R_Y_member_Y_R_);
                uaux_R_Y_member_Y_R_.erase(t);
                waux_R_Y_member_Y_R_.erase(t);
            }
        }
    }
    if(tuple.getPredicateName() == &_in){
        int C = tuple.at(0);
        int X = tuple.at(1);
        const std::vector<const Tuple *>& tuples1 = prest_0_1_.getValues({C, X});
        const std::vector<const Tuple *>& tuplesU1 = urest_0_1_.getValues({C, X});
        const Tuple * tuple1;
        for(int i=0; i< tuples1.size() + tuplesU1.size();i++){
            if(i<tuples1.size())
                tuple1= tuples1[i];
            else tuple1 = tuplesU1[i-tuples1.size()];
            int R = tuple1->at(2);
            std::vector<const Tuple *> tuples2;
            const Tuple* t2= wcliquesize.find({R, 1});
            if(t2!=NULL) tuples2.push_back(t2);
            std::vector<const Tuple *> tuplesU2;
            const Tuple* tU2= ucliquesize.find({R, 1});
            if(tU2!=NULL) tuplesU2.push_back(tU2);
            const Tuple * tuple2;
            for(int i=0; i< tuples2.size() + tuplesU2.size();i++){
                if(i<tuples2.size())
                    tuple2= tuples2[i];
                else tuple2 = tuplesU2[i-tuples2.size()];
                if(var>0){
                    Tuple sharedTuple(std::vector<int>({R,R}));
                    if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                        sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        for(int i=0;i<trueJoinTuples->size();i++){
                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                            }
                        }
                        for(int i=0;i<undefJoinTuples->size();i++){
                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                            }
                        }
                    }
                }else{
                }
                if(var>0){
                    Tuple sharedTuple(std::vector<int>({R,R}));
                    if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                        sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        for(int i=0;i<trueJoinTuples->size();i++){
                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                            }
                        }
                        for(int i=0;i<undefJoinTuples->size();i++){
                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                            }
                        }
                    }
                }else{
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_rest){
        int C = tuple.at(0);
        int X = tuple.at(1);
        int R = tuple.at(2);
        std::vector<const Tuple *> tuples0;
        const Tuple* t0= win.find({C, X});
        if(t0!=NULL) tuples0.push_back(t0);
        std::vector<const Tuple *> tuplesU0;
        const Tuple* tU0= uin.find({C, X});
        if(tU0!=NULL) tuplesU0.push_back(tU0);
        const Tuple * tuple0;
        for(int i=0; i< tuples0.size() + tuplesU0.size();i++){
            if(i<tuples0.size())
                tuple0= tuples0[i];
            else tuple0 = tuplesU0[i-tuples0.size()];
            std::vector<const Tuple *> tuples2;
            const Tuple* t2= wcliquesize.find({R, 1});
            if(t2!=NULL) tuples2.push_back(t2);
            std::vector<const Tuple *> tuplesU2;
            const Tuple* tU2= ucliquesize.find({R, 1});
            if(tU2!=NULL) tuplesU2.push_back(tU2);
            const Tuple * tuple2;
            for(int i=0; i< tuples2.size() + tuplesU2.size();i++){
                if(i<tuples2.size())
                    tuple2= tuples2[i];
                else tuple2 = tuplesU2[i-tuples2.size()];
                if(var>0){
                    Tuple sharedTuple(std::vector<int>({R,R}));
                    if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                        sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        for(int i=0;i<trueJoinTuples->size();i++){
                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                            }
                        }
                        for(int i=0;i<undefJoinTuples->size();i++){
                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                            }
                        }
                    }
                }else{
                }
                if(var>0){
                    Tuple sharedTuple(std::vector<int>({R,R}));
                    if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                        sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        for(int i=0;i<trueJoinTuples->size();i++){
                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                            }
                        }
                        for(int i=0;i<undefJoinTuples->size();i++){
                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                            }
                        }
                    }
                }else{
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_cliquesize){
        int R = tuple.at(0);
        const std::vector<const Tuple *>& tuples0 = pin_.getValues({});
        const std::vector<const Tuple *>& tuplesU0 = uin_.getValues({});
        const Tuple * tuple0;
        for(int i=0; i< tuples0.size() + tuplesU0.size();i++){
            if(i<tuples0.size())
                tuple0= tuples0[i];
            else tuple0 = tuplesU0[i-tuples0.size()];
            int C = tuple0->at(0);
            int X = tuple0->at(1);
            std::vector<const Tuple *> tuples1;
            const Tuple* t1= wrest.find({C, X, R});
            if(t1!=NULL) tuples1.push_back(t1);
            std::vector<const Tuple *> tuplesU1;
            const Tuple* tU1= urest.find({C, X, R});
            if(tU1!=NULL) tuplesU1.push_back(tU1);
            const Tuple * tuple1;
            for(int i=0; i< tuples1.size() + tuplesU1.size();i++){
                if(i<tuples1.size())
                    tuple1= tuples1[i];
                else tuple1 = tuplesU1[i-tuples1.size()];
                if(var>0){
                    Tuple sharedTuple(std::vector<int>({R,R}));
                    if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                        sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        for(int i=0;i<trueJoinTuples->size();i++){
                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                            }
                        }
                        for(int i=0;i<undefJoinTuples->size();i++){
                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                            }
                        }
                    }
                }else{
                }
                if(var>0){
                    Tuple sharedTuple(std::vector<int>({R,R}));
                    if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                        sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        for(int i=0;i<trueJoinTuples->size();i++){
                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                            }
                        }
                        for(int i=0;i<undefJoinTuples->size();i++){
                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                            }
                        }
                    }
                }else{
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
    if(var<0 && ( tuple.getPredicateName() == &_aux || tuple.getPredicateName() == &_in || tuple.getPredicateName() == &_member || tuple.getPredicateName() == &_pair)){
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
    if(tuple.getPredicateName() == &_in){
        if(var > 0){
            const std::vector<const Tuple* >& trueJoinTuples = p_in_C_X_pair_C_X_0_1_.getValues({tuple[0],tuple[1]});
            for(const Tuple* t :trueJoinTuples){
                const auto insertResult = uin_C_X_pair_C_X_.insert(Tuple(*t));
                if(insertResult.second){
                    for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_in_C_X_pair_C_X_]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                    int C = t->at(0);
                    {
                        Tuple sharedTuple(std::vector<int>({C,C}));
                        if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                }
                win_C_X_pair_C_X_.erase(*t);
            }
        }
        int C = tuple.at(0);
        int X = tuple.at(1);
        bool buildUndef=false;
        bool buildFalse=false;
        const std::vector<const Tuple*>& tuplesU1 = ppair_0_1_.getValues({C, X});
        const std::vector<const Tuple*>& tuples1 = upair_0_1_.getValues({C, X});
        for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
            const Tuple* tuple1;
            if(i<tuples1.size())
                tuple1=tuples1[i];
            else{
                tuple1=tuplesU1[i-tuples1.size()];
            }
            Tuple tupleUndef(std::vector<int>({C,X,C,X}),&_in_C_X_pair_C_X_);
            if(uin_C_X_pair_C_X_.find(Tuple(tupleUndef)) == NULL){
                const auto& insertResult = uin_C_X_pair_C_X_.insert(Tuple(tupleUndef));
                if (insertResult.second) {
                    for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_in_C_X_pair_C_X_]){
                        auxMap -> insert2(*insertResult.first);
                    }
                    {
                        Tuple sharedTuple(std::vector<int>({C,C}));
                        if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_pair){
        if(var > 0){
            const std::vector<const Tuple* >& trueJoinTuples = p_in_C_X_pair_C_X_2_3_.getValues({tuple[0],tuple[1]});
            for(const Tuple* t :trueJoinTuples){
                const auto insertResult = uin_C_X_pair_C_X_.insert(Tuple(*t));
                if(insertResult.second){
                    for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_in_C_X_pair_C_X_]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                    int C = t->at(0);
                    {
                        Tuple sharedTuple(std::vector<int>({C,C}));
                        if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                }
                win_C_X_pair_C_X_.erase(*t);
            }
        }
        int C = tuple.at(0);
        int X = tuple.at(1);
        bool buildUndef=false;
        bool buildFalse=false;
        const std::vector<const Tuple*>& tuplesU0 = pin_0_1_.getValues({C, X});
        const std::vector<const Tuple*>& tuples0 = uin_0_1_.getValues({C, X});
        for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
            const Tuple* tuple0;
            if(i<tuples0.size())
                tuple0=tuples0[i];
            else{
                tuple0=tuplesU0[i-tuples0.size()];
            }
            Tuple tupleUndef(std::vector<int>({C,X,C,X}),&_in_C_X_pair_C_X_);
            if(uin_C_X_pair_C_X_.find(Tuple(tupleUndef)) == NULL){
                const auto& insertResult = uin_C_X_pair_C_X_.insert(Tuple(tupleUndef));
                if (insertResult.second) {
                    for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_in_C_X_pair_C_X_]){
                        auxMap -> insert2(*insertResult.first);
                    }
                    {
                        Tuple sharedTuple(std::vector<int>({C,C}));
                        if(sharedVariables_0_ToAggregate_1.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_0_ToAggregate_1[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_clique){
        int C = tuple.at(0);
        {
            Tuple sharedTuple(std::vector<int>({C,C}));
            if(!sharedVariables_0_ToAggregate_1.count(sharedTuple)){
                sharedVariables_0_ToAggregate_1[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                const std::vector<const Tuple*>* undefJoinTuples = &u_in_C_X_pair_C_X_0_2_.getValues({C,C});
                const std::vector<const Tuple*>* trueJoinTuples = &p_in_C_X_pair_C_X_0_2_.getValues({C,C});
                for(int i=0;i<undefJoinTuples->size();i++){
                    const auto findResult = uin_C_X_pair_C_X_.find(Tuple(*undefJoinTuples->at(i)));
                    if(findResult!=NULL){
                        sharedVariables_0_ToAggregate_1[sharedTuple]->second->insert2(*findResult);
                    }
                }
                for(int i=0;i<trueJoinTuples->size();i++){
                    const auto findResult = win_C_X_pair_C_X_.find(Tuple(*trueJoinTuples->at(i)));
                    if(findResult!=NULL){
                        sharedVariables_0_ToAggregate_1[sharedTuple]->first->insert2(*findResult);
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_aux){
        if(var > 0){
            const std::vector<const Tuple* >& trueJoinTuples = p_aux_R_Y_member_Y_R_0_1_.getValues({tuple[0],tuple[1]});
            for(const Tuple* t :trueJoinTuples){
                const auto insertResult = uaux_R_Y_member_Y_R_.insert(Tuple(*t));
                if(insertResult.second){
                    for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                    int R = t->at(0);
                    {
                        Tuple sharedTuple(std::vector<int>({R,R}));
                        if(sharedVariables_1_ToAggregate_3.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_3[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                    {
                        Tuple sharedTuple(std::vector<int>({R,R}));
                        if(sharedVariables_1_ToAggregate_4.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_4[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                }
                waux_R_Y_member_Y_R_.erase(*t);
            }
        }
        int R = tuple.at(0);
        int Y = tuple.at(1);
        bool buildUndef=false;
        bool buildFalse=false;
        const std::vector<const Tuple*>& tuplesU1 = pmember_0_1_.getValues({Y, R});
        const std::vector<const Tuple*>& tuples1 = umember_0_1_.getValues({Y, R});
        for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
            const Tuple* tuple1;
            if(i<tuples1.size())
                tuple1=tuples1[i];
            else{
                tuple1=tuplesU1[i-tuples1.size()];
            }
            Tuple tupleUndef(std::vector<int>({R,Y,Y,R}),&_aux_R_Y_member_Y_R_);
            if(uaux_R_Y_member_Y_R_.find(Tuple(tupleUndef)) == NULL){
                const auto& insertResult = uaux_R_Y_member_Y_R_.insert(Tuple(tupleUndef));
                if (insertResult.second) {
                    for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_]){
                        auxMap -> insert2(*insertResult.first);
                    }
                    {
                        Tuple sharedTuple(std::vector<int>({R,R}));
                        if(sharedVariables_1_ToAggregate_3.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_3[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                    {
                        Tuple sharedTuple(std::vector<int>({R,R}));
                        if(sharedVariables_1_ToAggregate_4.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_4[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_member){
        if(var > 0){
            const std::vector<const Tuple* >& trueJoinTuples = p_aux_R_Y_member_Y_R_2_3_.getValues({tuple[0],tuple[1]});
            for(const Tuple* t :trueJoinTuples){
                const auto insertResult = uaux_R_Y_member_Y_R_.insert(Tuple(*t));
                if(insertResult.second){
                    for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                    int R = t->at(0);
                    {
                        Tuple sharedTuple(std::vector<int>({R,R}));
                        if(sharedVariables_1_ToAggregate_3.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_3[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                    {
                        Tuple sharedTuple(std::vector<int>({R,R}));
                        if(sharedVariables_1_ToAggregate_4.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_4[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                }
                waux_R_Y_member_Y_R_.erase(*t);
            }
        }
        int Y = tuple.at(0);
        int R = tuple.at(1);
        bool buildUndef=false;
        bool buildFalse=false;
        const std::vector<const Tuple*>& tuplesU0 = paux_0_1_.getValues({R, Y});
        const std::vector<const Tuple*>& tuples0 = uaux_0_1_.getValues({R, Y});
        for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
            const Tuple* tuple0;
            if(i<tuples0.size())
                tuple0=tuples0[i];
            else{
                tuple0=tuplesU0[i-tuples0.size()];
            }
            Tuple tupleUndef(std::vector<int>({R,Y,Y,R}),&_aux_R_Y_member_Y_R_);
            if(uaux_R_Y_member_Y_R_.find(Tuple(tupleUndef)) == NULL){
                const auto& insertResult = uaux_R_Y_member_Y_R_.insert(Tuple(tupleUndef));
                if (insertResult.second) {
                    for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_]){
                        auxMap -> insert2(*insertResult.first);
                    }
                    {
                        Tuple sharedTuple(std::vector<int>({R,R}));
                        if(sharedVariables_1_ToAggregate_3.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_3[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                    {
                        Tuple sharedTuple(std::vector<int>({R,R}));
                        if(sharedVariables_1_ToAggregate_4.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_4[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_aux){
        if(var > 0){
            const std::vector<const Tuple* >& trueJoinTuples = p_aux_R_Y_member_Y_R_0_1_.getValues({tuple[0],tuple[1]});
            for(const Tuple* t :trueJoinTuples){
                const auto insertResult = uaux_R_Y_member_Y_R_.insert(Tuple(*t));
                if(insertResult.second){
                    for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                    int R = t->at(0);
                    {
                        Tuple sharedTuple(std::vector<int>({R,R}));
                        if(sharedVariables_1_ToAggregate_3.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_3[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                    {
                        Tuple sharedTuple(std::vector<int>({R,R}));
                        if(sharedVariables_1_ToAggregate_4.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_4[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                }
                waux_R_Y_member_Y_R_.erase(*t);
            }
        }
        int R = tuple.at(0);
        int Y = tuple.at(1);
        bool buildUndef=false;
        bool buildFalse=false;
        const std::vector<const Tuple*>& tuplesU1 = pmember_0_1_.getValues({Y, R});
        const std::vector<const Tuple*>& tuples1 = umember_0_1_.getValues({Y, R});
        for(int i=0;i<tuples1.size()+tuplesU1.size();i++){
            const Tuple* tuple1;
            if(i<tuples1.size())
                tuple1=tuples1[i];
            else{
                tuple1=tuplesU1[i-tuples1.size()];
            }
            Tuple tupleUndef(std::vector<int>({R,Y,Y,R}),&_aux_R_Y_member_Y_R_);
            if(uaux_R_Y_member_Y_R_.find(Tuple(tupleUndef)) == NULL){
                const auto& insertResult = uaux_R_Y_member_Y_R_.insert(Tuple(tupleUndef));
                if (insertResult.second) {
                    for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_]){
                        auxMap -> insert2(*insertResult.first);
                    }
                    {
                        Tuple sharedTuple(std::vector<int>({R,R}));
                        if(sharedVariables_1_ToAggregate_3.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_3[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                    {
                        Tuple sharedTuple(std::vector<int>({R,R}));
                        if(sharedVariables_1_ToAggregate_4.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_4[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_member){
        if(var > 0){
            const std::vector<const Tuple* >& trueJoinTuples = p_aux_R_Y_member_Y_R_2_3_.getValues({tuple[0],tuple[1]});
            for(const Tuple* t :trueJoinTuples){
                const auto insertResult = uaux_R_Y_member_Y_R_.insert(Tuple(*t));
                if(insertResult.second){
                    for (AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_]) {
                        auxMap -> insert2(*insertResult.first);
                    }
                    int R = t->at(0);
                    {
                        Tuple sharedTuple(std::vector<int>({R,R}));
                        if(sharedVariables_1_ToAggregate_3.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_3[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                    {
                        Tuple sharedTuple(std::vector<int>({R,R}));
                        if(sharedVariables_1_ToAggregate_4.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_4[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                }
                waux_R_Y_member_Y_R_.erase(*t);
            }
        }
        int Y = tuple.at(0);
        int R = tuple.at(1);
        bool buildUndef=false;
        bool buildFalse=false;
        const std::vector<const Tuple*>& tuplesU0 = paux_0_1_.getValues({R, Y});
        const std::vector<const Tuple*>& tuples0 = uaux_0_1_.getValues({R, Y});
        for(int i=0;i<tuples0.size()+tuplesU0.size();i++){
            const Tuple* tuple0;
            if(i<tuples0.size())
                tuple0=tuples0[i];
            else{
                tuple0=tuplesU0[i-tuples0.size()];
            }
            Tuple tupleUndef(std::vector<int>({R,Y,Y,R}),&_aux_R_Y_member_Y_R_);
            if(uaux_R_Y_member_Y_R_.find(Tuple(tupleUndef)) == NULL){
                const auto& insertResult = uaux_R_Y_member_Y_R_.insert(Tuple(tupleUndef));
                if (insertResult.second) {
                    for(AuxMap* auxMap : predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_]){
                        auxMap -> insert2(*insertResult.first);
                    }
                    {
                        Tuple sharedTuple(std::vector<int>({R,R}));
                        if(sharedVariables_1_ToAggregate_3.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_3[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                    {
                        Tuple sharedTuple(std::vector<int>({R,R}));
                        if(sharedVariables_1_ToAggregate_4.count(sharedTuple)!=0){
                            auto joinTuples = sharedVariables_1_ToAggregate_4[sharedTuple];
                            joinTuples->second->insert2(*insertResult.first);
                        }
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_in){
        int C = tuple.at(0);
        int X = tuple.at(1);
        const std::vector<const Tuple *>& undefTuples1 = urest_0_1_.getValues({C, X});
        const std::vector<const Tuple*>& trueTuples1 = prest_0_1_.getValues({C, X});
        for(int i=0;i<undefTuples1.size()+trueTuples1.size();i++){
            const Tuple * tuple1;
            if(i<undefTuples1.size())
                tuple1 = undefTuples1[i];
            else tuple1 = trueTuples1[i-undefTuples1.size()];
            int R = tuple1->at(2);
            const Tuple* undefT = ucliquesize.find({R, 1});
            const Tuple* trueT = wcliquesize.find({R, 1});
            std::vector<const Tuple *> undefTuples2;
            if(undefT != NULL) undefTuples2.push_back(undefT);
            std::vector<const Tuple *> trueTuples2;
            if(trueT != NULL) trueTuples2.push_back(undefT);
            for(int i=0;i<undefTuples2.size()+trueTuples2.size();i++){
                const Tuple * tuple2;
                if(i<undefTuples2.size())
                    tuple2 = undefTuples2[i];
                else tuple2 = trueTuples2[i-undefTuples2.size()];
                {
                    Tuple sharedTuple(std::vector<int>({R,R}));
                    if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                        sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        for(int i=0;i<undefJoinTuples->size();i++){
                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                            }
                        }
                        for(int i=0;i<trueJoinTuples->size();i++){
                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                            }
                        }
                    }
                }
                {
                    Tuple sharedTuple(std::vector<int>({R,R}));
                    if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                        sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        for(int i=0;i<undefJoinTuples->size();i++){
                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                            }
                        }
                        for(int i=0;i<trueJoinTuples->size();i++){
                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                            }
                        }
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_rest){
        int C = tuple.at(0);
        int X = tuple.at(1);
        int R = tuple.at(2);
        const Tuple* undefT = uin.find({C, X});
        const Tuple* trueT = win.find({C, X});
        std::vector<const Tuple *> undefTuples0;
        if(undefT != NULL) undefTuples0.push_back(undefT);
        std::vector<const Tuple *> trueTuples0;
        if(trueT != NULL) trueTuples0.push_back(undefT);
        for(int i=0;i<undefTuples0.size()+trueTuples0.size();i++){
            const Tuple * tuple0;
            if(i<undefTuples0.size())
                tuple0 = undefTuples0[i];
            else tuple0 = trueTuples0[i-undefTuples0.size()];
            const Tuple* undefT = ucliquesize.find({R, 1});
            const Tuple* trueT = wcliquesize.find({R, 1});
            std::vector<const Tuple *> undefTuples2;
            if(undefT != NULL) undefTuples2.push_back(undefT);
            std::vector<const Tuple *> trueTuples2;
            if(trueT != NULL) trueTuples2.push_back(undefT);
            for(int i=0;i<undefTuples2.size()+trueTuples2.size();i++){
                const Tuple * tuple2;
                if(i<undefTuples2.size())
                    tuple2 = undefTuples2[i];
                else tuple2 = trueTuples2[i-undefTuples2.size()];
                {
                    Tuple sharedTuple(std::vector<int>({R,R}));
                    if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                        sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        for(int i=0;i<undefJoinTuples->size();i++){
                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                            }
                        }
                        for(int i=0;i<trueJoinTuples->size();i++){
                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                            }
                        }
                    }
                }
                {
                    Tuple sharedTuple(std::vector<int>({R,R}));
                    if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                        sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        for(int i=0;i<undefJoinTuples->size();i++){
                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                            }
                        }
                        for(int i=0;i<trueJoinTuples->size();i++){
                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                            }
                        }
                    }
                }
            }
        }
    }
    if(tuple.getPredicateName() == &_cliquesize){
        int R = tuple.at(0);
        const std::vector<const Tuple *>& undefTuples0 = uin_.getValues({});
        const std::vector<const Tuple*>& trueTuples0 = pin_.getValues({});
        for(int i=0;i<undefTuples0.size()+trueTuples0.size();i++){
            const Tuple * tuple0;
            if(i<undefTuples0.size())
                tuple0 = undefTuples0[i];
            else tuple0 = trueTuples0[i-undefTuples0.size()];
            int C = tuple0->at(0);
            int X = tuple0->at(1);
            const Tuple* undefT = urest.find({C, X, R});
            const Tuple* trueT = wrest.find({C, X, R});
            std::vector<const Tuple *> undefTuples1;
            if(undefT != NULL) undefTuples1.push_back(undefT);
            std::vector<const Tuple *> trueTuples1;
            if(trueT != NULL) trueTuples1.push_back(undefT);
            for(int i=0;i<undefTuples1.size()+trueTuples1.size();i++){
                const Tuple * tuple1;
                if(i<undefTuples1.size())
                    tuple1 = undefTuples1[i];
                else tuple1 = trueTuples1[i-undefTuples1.size()];
                {
                    Tuple sharedTuple(std::vector<int>({R,R}));
                    if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                        sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        for(int i=0;i<undefJoinTuples->size();i++){
                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                            }
                        }
                        for(int i=0;i<trueJoinTuples->size();i++){
                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                            }
                        }
                    }
                }
                {
                    Tuple sharedTuple(std::vector<int>({R,R}));
                    if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                        sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        for(int i=0;i<undefJoinTuples->size();i++){
                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                            }
                        }
                        for(int i=0;i<trueJoinTuples->size();i++){
                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                            }
                        }
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
    predicateWSetMap[&_clique]=&wclique;
    predicateUSetMap[&_clique]=&uclique;
    stringToUniqueStringPointer["clique"] = &_clique;
    predicateWSetMap[&_cliquesize]=&wcliquesize;
    predicateUSetMap[&_cliquesize]=&ucliquesize;
    stringToUniqueStringPointer["cliquesize"] = &_cliquesize;
    predicateWSetMap[&_in]=&win;
    predicateUSetMap[&_in]=&uin;
    stringToUniqueStringPointer["in"] = &_in;
    predicateWSetMap[&_rest]=&wrest;
    predicateUSetMap[&_rest]=&urest;
    stringToUniqueStringPointer["rest"] = &_rest;
    aggregatePredicatesAndVars [&_in]= std::vector<int>({1});
    predicateWSetMap[&_in]=&win;
    predicateFSetMap[&_in]=&fin;
    predicateUSetMap[&_in]=&uin;
    stringToUniqueStringPointer["in"] = &_in;
    aggregatePredicatesAndVars [&_pair]= std::vector<int>({1});
    predicateWSetMap[&_pair]=&wpair;
    predicateFSetMap[&_pair]=&fpair;
    predicateUSetMap[&_pair]=&upair;
    stringToUniqueStringPointer["pair"] = &_pair;
    aggregatePredicatesAndVars [&_aux]= std::vector<int>({1});
    predicateWSetMap[&_aux]=&waux;
    predicateFSetMap[&_aux]=&faux;
    predicateUSetMap[&_aux]=&uaux;
    stringToUniqueStringPointer["aux"] = &_aux;
    aggregatePredicatesAndVars [&_member]= std::vector<int>({0});
    predicateWSetMap[&_member]=&wmember;
    predicateFSetMap[&_member]=&fmember;
    predicateUSetMap[&_member]=&umember;
    stringToUniqueStringPointer["member"] = &_member;
    aggregatePredicatesAndVars [&_aux]= std::vector<int>({1});
    predicateWSetMap[&_aux]=&waux;
    predicateFSetMap[&_aux]=&faux;
    predicateUSetMap[&_aux]=&uaux;
    stringToUniqueStringPointer["aux"] = &_aux;
    aggregatePredicatesAndVars [&_member]= std::vector<int>({0});
    predicateWSetMap[&_member]=&wmember;
    predicateFSetMap[&_member]=&fmember;
    predicateUSetMap[&_member]=&umember;
    stringToUniqueStringPointer["member"] = &_member;
    predicateToAuxiliaryMaps[&_cliquesize].push_back(&pcliquesize_0_1_);
    predicateToAuxiliaryMaps[&_rest].push_back(&prest_0_1_);
    predicateToAuxiliaryMaps[&_rest].push_back(&prest_0_1_2_);
    predicateToAuxiliaryMaps[&_rest].push_back(&prest_2_);
    predicateToAuxiliaryMaps[&_aux_R_Y_member_Y_R_].push_back(&p_aux_R_Y_member_Y_R_);
    predicateToAuxiliaryMaps[&_aux_R_Y_member_Y_R_].push_back(&p_aux_R_Y_member_Y_R_0_1_);
    predicateToAuxiliaryMaps[&_aux_R_Y_member_Y_R_].push_back(&p_aux_R_Y_member_Y_R_0_3_);
    predicateToAuxiliaryMaps[&_aux_R_Y_member_Y_R_].push_back(&p_aux_R_Y_member_Y_R_1_);
    predicateToAuxiliaryMaps[&_aux_R_Y_member_Y_R_].push_back(&p_aux_R_Y_member_Y_R_2_3_);
    predicateToAuxiliaryMaps[&_pair].push_back(&ppair_);
    predicateToAuxiliaryMaps[&_pair].push_back(&ppair_0_1_);
    predicateToAuxiliaryMaps[&_in].push_back(&pin_);
    predicateToAuxiliaryMaps[&_in].push_back(&pin_0_1_);
    predicateToAuxiliaryMaps[&_aux].push_back(&paux_);
    predicateToAuxiliaryMaps[&_aux].push_back(&paux_0_1_);
    predicateToAuxiliaryMaps[&_clique].push_back(&pclique_);
    predicateToAuxiliaryMaps[&_clique].push_back(&pclique_0_);
    predicateToAuxiliaryMaps[&_in_C_X_pair_C_X_].push_back(&p_in_C_X_pair_C_X_);
    predicateToAuxiliaryMaps[&_in_C_X_pair_C_X_].push_back(&p_in_C_X_pair_C_X_0_1_);
    predicateToAuxiliaryMaps[&_in_C_X_pair_C_X_].push_back(&p_in_C_X_pair_C_X_0_2_);
    predicateToAuxiliaryMaps[&_in_C_X_pair_C_X_].push_back(&p_in_C_X_pair_C_X_1_);
    predicateToAuxiliaryMaps[&_in_C_X_pair_C_X_].push_back(&p_in_C_X_pair_C_X_2_3_);
    predicateToAuxiliaryMaps[&_member].push_back(&pmember_);
    predicateToAuxiliaryMaps[&_member].push_back(&pmember_0_1_);
    predicateToUndefAuxiliaryMaps[&_cliquesize].push_back(&ucliquesize_0_1_);
    predicateToUndefAuxiliaryMaps[&_rest].push_back(&urest_0_1_);
    predicateToUndefAuxiliaryMaps[&_rest].push_back(&urest_0_1_2_);
    predicateToUndefAuxiliaryMaps[&_rest].push_back(&urest_2_);
    predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_].push_back(&u_aux_R_Y_member_Y_R_);
    predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_].push_back(&u_aux_R_Y_member_Y_R_0_1_);
    predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_].push_back(&u_aux_R_Y_member_Y_R_0_3_);
    predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_].push_back(&u_aux_R_Y_member_Y_R_1_);
    predicateToUndefAuxiliaryMaps[&_aux_R_Y_member_Y_R_].push_back(&u_aux_R_Y_member_Y_R_2_3_);
    predicateToUndefAuxiliaryMaps[&_pair].push_back(&upair_);
    predicateToUndefAuxiliaryMaps[&_pair].push_back(&upair_0_1_);
    predicateToUndefAuxiliaryMaps[&_in].push_back(&uin_);
    predicateToUndefAuxiliaryMaps[&_in].push_back(&uin_0_1_);
    predicateToUndefAuxiliaryMaps[&_aux].push_back(&uaux_);
    predicateToUndefAuxiliaryMaps[&_aux].push_back(&uaux_0_1_);
    predicateToUndefAuxiliaryMaps[&_clique].push_back(&uclique_);
    predicateToUndefAuxiliaryMaps[&_clique].push_back(&uclique_0_);
    predicateToUndefAuxiliaryMaps[&_in_C_X_pair_C_X_].push_back(&u_in_C_X_pair_C_X_);
    predicateToUndefAuxiliaryMaps[&_in_C_X_pair_C_X_].push_back(&u_in_C_X_pair_C_X_0_1_);
    predicateToUndefAuxiliaryMaps[&_in_C_X_pair_C_X_].push_back(&u_in_C_X_pair_C_X_0_2_);
    predicateToUndefAuxiliaryMaps[&_in_C_X_pair_C_X_].push_back(&u_in_C_X_pair_C_X_1_);
    predicateToUndefAuxiliaryMaps[&_in_C_X_pair_C_X_].push_back(&u_in_C_X_pair_C_X_2_3_);
    predicateToUndefAuxiliaryMaps[&_member].push_back(&umember_);
    predicateToUndefAuxiliaryMaps[&_member].push_back(&umember_0_1_);
    predicateToFalseAuxiliaryMaps[&_member].push_back(&fmember_);
    predicateToFalseAuxiliaryMaps[&_member].push_back(&fmember_0_1_);
    predicateToFalseAuxiliaryMaps[&_aux].push_back(&faux_);
    predicateToFalseAuxiliaryMaps[&_aux].push_back(&faux_0_1_);
    predicateToFalseAuxiliaryMaps[&_in].push_back(&fin_);
    predicateToFalseAuxiliaryMaps[&_in].push_back(&fin_0_1_);
    predicateToFalseAuxiliaryMaps[&_pair].push_back(&fpair_);
    predicateToFalseAuxiliaryMaps[&_pair].push_back(&fpair_0_1_);
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
            tuples = &pclique_.getValues({});
            const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
            if(tupleU == NULL){
                tuplesU = &uclique_.getValues({});
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
                int C = (*tuple0)[0];
                std::pair<AuxMap*,AuxMap*> joinTuples_0_1;
                joinTuples_0_1.first = &p_in_C_X_pair_C_X_;
                joinTuples_0_1.second = &u_in_C_X_pair_C_X_;
                {
                    Tuple sharedTuple(std::vector<int>({C,C}));
                    if(!sharedVariables_0_ToAggregate_1.count(sharedTuple)){
                        sharedVariables_0_ToAggregate_1[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                        const std::vector<const Tuple*>* trueJoinTuples = &p_in_C_X_pair_C_X_0_2_.getValues({C,C});
                        const std::vector<const Tuple*>* undefJoinTuples = &u_in_C_X_pair_C_X_0_2_.getValues({C,C});
                        for(int i=0;i<trueJoinTuples->size();i++){
                            const auto findResult = win_C_X_pair_C_X_.find(Tuple(*trueJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_0_ToAggregate_1[sharedTuple]->first->insert2(*findResult);
                            }
                        }
                        for(int i=0;i<undefJoinTuples->size();i++){
                            const auto findResult = uin_C_X_pair_C_X_.find(Tuple(*undefJoinTuples->at(i)));
                            if(findResult!=NULL){
                                sharedVariables_0_ToAggregate_1[sharedTuple]->second->insert2(*findResult);
                            }
                        }
                    }
                }
                std::pair<AuxMap*,AuxMap*> joinTuples_0_1SharedVariables = *sharedVariables_0_ToAggregate_1[std::vector<int>({C,C})];
                if(tupleU==NULL){
                    if(joinTuples_0_1SharedVariables.first->size()>=2+1){
                        std::cout<<"conflict detected in propagator0"<<std::endl;
                        propagatedLiteralsAndReasons.insert({-1, std::vector<int>()});
                    }else{
                        if(joinTuples_0_1SharedVariables.first->size() == 2+1-1){
                            const std::vector<const Tuple*>* undefinedTuples = &u_in_C_X_pair_C_X_0_2_.getValues({C,C});
                            for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                if(joinTuples_0_1SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 0){
                                    const Tuple* aggrTupleU=NULL;
                                    const std::vector<const Tuple*>* tuplesin;
                                    const std::vector<const Tuple*>* tuplesUin=&EMPTY_TUPLES;
                                    tuplesin= &pin_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                    if(aggrTupleU==NULL)
                                        tuplesUin= &uin_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                    for(int j2=0;j2<tuplesin->size()+tuplesUin->size();j2++){
                                        const Tuple* auxTuple2=NULL;
                                        bool addedToReason=false;
                                        if(j2 < tuplesin->size()){
                                            auxTuple2 = tuplesin->at(j2);
                                            if(tuplesUin != &EMPTY_TUPLES) {
                                            }
                                        }else{
                                            auxTuple2 = tuplesUin->at(j2-tuplesin->size());
                                            aggrTupleU = auxTuple2;
                                        }
                                        const std::vector<const Tuple*>* tuplespair;
                                        const std::vector<const Tuple*>* tuplesUpair=&EMPTY_TUPLES;
                                        tuplespair= &ppair_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                        if(aggrTupleU==NULL)
                                            tuplesUpair= &upair_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                        for(int j4=0;j4<tuplespair->size()+tuplesUpair->size();j4++){
                                            const Tuple* auxTuple4=NULL;
                                            bool addedToReason=false;
                                            if(j4 < tuplespair->size()){
                                                auxTuple4 = tuplespair->at(j4);
                                                if(tuplesUpair != &EMPTY_TUPLES) {
                                                }
                                            }else{
                                                auxTuple4 = tuplesUpair->at(j4-tuplespair->size());
                                                aggrTupleU = auxTuple4;
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
                    }
                }else{
                    if(joinTuples_0_1SharedVariables.first->size()>=2+1){
                        const auto & it = tupleToVar.find(*tupleU);
                        if(it != tupleToVar.end()) {
                            int sign = tupleU->isNegated() ? -1 : 1;
                            auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;
                        }
                    }
                }
            }//close par
        }//close local scope
        {
            const Tuple * tupleU = NULL;
            bool tupleUNegated = false;
            const std::vector<const Tuple* >* tuples;
            tuples = &pin_.getValues({});
            const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
            if(tupleU == NULL){
                tuplesU = &uin_.getValues({});
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
                int C = (*tuple0)[0];
                int X = (*tuple0)[1];
                const std::vector<const Tuple* >* tuples;
                tuples = &prest_0_1_.getValues({C, X});
                const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                if(tupleU == NULL){
                    tuplesU = &urest_0_1_.getValues({C, X});
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
                    int R = (*tuple1)[2];
                    const Tuple negativeTuple = Tuple({R, 1}, &_cliquesize, true);
                    const Tuple * tuple2 = &negativeTuple;
                    bool lTrue = (wcliquesize.find(negativeTuple)!=NULL);
                    const Tuple * undefTuple = ucliquesize.find(negativeTuple);
                    if((!lTrue && undefTuple == NULL) || (undefTuple && tupleU == NULL)){
                        if(undefTuple){
                            tuple2 = tupleU = undefTuple;
                            tupleU->print();
                            tupleUNegated = true;
                        }
                        std::pair<AuxMap*,AuxMap*> joinTuples_1_3;
                        joinTuples_1_3.first = &p_aux_R_Y_member_Y_R_;
                        joinTuples_1_3.second = &u_aux_R_Y_member_Y_R_;
                        {
                            Tuple sharedTuple(std::vector<int>({R,R}));
                            if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                                sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                for(int i=0;i<trueJoinTuples->size();i++){
                                    const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                    if(findResult!=NULL){
                                        sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                                    }
                                }
                                for(int i=0;i<undefJoinTuples->size();i++){
                                    const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                    if(findResult!=NULL){
                                        sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                                    }
                                }
                            }
                        }
                        std::pair<AuxMap*,AuxMap*> joinTuples_1_3SharedVariables = *sharedVariables_1_ToAggregate_3[std::vector<int>({R,R})];
                        if(tupleU==NULL){
                            if(joinTuples_1_3SharedVariables.first->size()>=0){
                                std::pair<AuxMap*,AuxMap*> joinTuples_1_4;
                                joinTuples_1_4.first = &p_aux_R_Y_member_Y_R_;
                                joinTuples_1_4.second = &u_aux_R_Y_member_Y_R_;
                                {
                                    Tuple sharedTuple(std::vector<int>({R,R}));
                                    if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                                        sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                        for(int i=0;i<trueJoinTuples->size();i++){
                                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                            if(findResult!=NULL){
                                                sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                                            }
                                        }
                                        for(int i=0;i<undefJoinTuples->size();i++){
                                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                            if(findResult!=NULL){
                                                sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                                            }
                                        }
                                    }
                                }
                                std::pair<AuxMap*,AuxMap*> joinTuples_1_4SharedVariables = *sharedVariables_1_ToAggregate_4[std::vector<int>({R,R})];
                                int actualSize4 = joinTuples_1_4SharedVariables.first->size();
                                std::set<std::vector<int>> alreadyCounted4;
                                for(const Tuple * joinTupleUndef : u_aux_R_Y_member_Y_R_0_3_.getValues({R,R})){
                                    if(joinTuples_1_4SharedVariables.first->getValues({joinTupleUndef->at(1)}).size()==0){
                                        if(joinTuples_1_4SharedVariables.second->getValues({joinTupleUndef->at(1)}).size()>0){
                                            std::vector<int> key({joinTupleUndef->at(1)});
                                            alreadyCounted4.insert(key);
                                        }
                                    }
                                }
                                actualSize4+=alreadyCounted4.size();
                                if(!(actualSize4>=0+1)){
                                    std::cout<<"conflict detected in propagatoron last aggregate starting from literal1"<<std::endl;
                                    propagatedLiteralsAndReasons.insert({-1, std::vector<int>()});
                                }else{
                                    if(actualSize4 == 0+1){
                                        const std::vector<const Tuple*>* undefinedTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                        for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                            if(joinTuples_1_4SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 0 && joinTuples_1_4SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 1){
                                                {
                                                    const Tuple* aggrTupleU = uaux.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                    if(aggrTupleU != NULL){
                                                        const auto & it = tupleToVar.find(*aggrTupleU);
                                                        if(it != tupleToVar.end()) {
                                                            int sign = aggrTupleU->isNegated() ? 1:-1;
                                                            auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;
                                                        }
                                                    }
                                                }
                                                {
                                                    const Tuple* aggrTupleU = umember.find({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
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
                            }//close ext aggregate if
                        }else{
                        std::pair<AuxMap*,AuxMap*> joinTuples_1_4;
                        joinTuples_1_4.first = &p_aux_R_Y_member_Y_R_;
                        joinTuples_1_4.second = &u_aux_R_Y_member_Y_R_;
                        {
                            Tuple sharedTuple(std::vector<int>({R,R}));
                            if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                                sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                for(int i=0;i<trueJoinTuples->size();i++){
                                    const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                    if(findResult!=NULL){
                                        sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                                    }
                                }
                                for(int i=0;i<undefJoinTuples->size();i++){
                                    const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                    if(findResult!=NULL){
                                        sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                                    }
                                }
                            }
                        }
                        std::pair<AuxMap*,AuxMap*> joinTuples_1_4SharedVariables = *sharedVariables_1_ToAggregate_4[std::vector<int>({R,R})];
                        int actualSize4 = joinTuples_1_4SharedVariables.first->size();
                        std::set<std::vector<int>> alreadyCounted4;
                        for(const Tuple * joinTupleUndef : u_aux_R_Y_member_Y_R_0_3_.getValues({R,R})){
                            if(joinTuples_1_4SharedVariables.first->getValues({joinTupleUndef->at(1)}).size()==0){
                                if(joinTuples_1_4SharedVariables.second->getValues({joinTupleUndef->at(1)}).size()>0){
                                    std::vector<int> key({joinTupleUndef->at(1)});
                                    alreadyCounted4.insert(key);
                                }
                            }
                        }
                        actualSize4+=alreadyCounted4.size();
                            if(!(actualSize4>=0+1) && (joinTuples_1_3SharedVariables.first->size()>=0)){
                                const auto & it = tupleToVar.find(*tupleU);
                                if(it != tupleToVar.end()) {
                                    int sign = tupleU->isNegated() ? -1 : 1;
                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>()}).first->second;
                                }
                            }
                        }
                        std::pair<AuxMap*,AuxMap*> joinTuples_1_4;
                        joinTuples_1_4.first = &p_aux_R_Y_member_Y_R_;
                        joinTuples_1_4.second = &u_aux_R_Y_member_Y_R_;
                        {
                            Tuple sharedTuple(std::vector<int>({R,R}));
                            if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                                sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                for(int i=0;i<trueJoinTuples->size();i++){
                                    const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                    if(findResult!=NULL){
                                        sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                                    }
                                }
                                for(int i=0;i<undefJoinTuples->size();i++){
                                    const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                    if(findResult!=NULL){
                                        sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                                    }
                                }
                            }
                        }
                        std::pair<AuxMap*,AuxMap*> joinTuples_1_4SharedVariables = *sharedVariables_1_ToAggregate_4[std::vector<int>({R,R})];
                        int actualSize4 = joinTuples_1_4SharedVariables.first->size();
                        std::set<std::vector<int>> alreadyCounted4;
                        for(const Tuple * joinTupleUndef : u_aux_R_Y_member_Y_R_0_3_.getValues({R,R})){
                            if(joinTuples_1_4SharedVariables.first->getValues({joinTupleUndef->at(1)}).size()==0){
                                if(joinTuples_1_4SharedVariables.second->getValues({joinTupleUndef->at(1)}).size()>0){
                                    std::vector<int> key({joinTupleUndef->at(1)});
                                    alreadyCounted4.insert(key);
                                }
                            }
                        }
                        actualSize4+=alreadyCounted4.size();
                        if(tupleU==NULL){
                            if(!(actualSize4>=0+1)){
                                std::pair<AuxMap*,AuxMap*> joinTuples_1_3;
                                joinTuples_1_3.first = &p_aux_R_Y_member_Y_R_;
                                joinTuples_1_3.second = &u_aux_R_Y_member_Y_R_;
                                {
                                    Tuple sharedTuple(std::vector<int>({R,R}));
                                    if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                                        sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                        for(int i=0;i<trueJoinTuples->size();i++){
                                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                            if(findResult!=NULL){
                                                sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                                            }
                                        }
                                        for(int i=0;i<undefJoinTuples->size();i++){
                                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                            if(findResult!=NULL){
                                                sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                                            }
                                        }
                                    }
                                }
                                std::pair<AuxMap*,AuxMap*> joinTuples_1_3SharedVariables = *sharedVariables_1_ToAggregate_3[std::vector<int>({R,R})];
                                if(joinTuples_1_3SharedVariables.first->size()>=0){
                                    std::cout<<"conflict detected in propagatoron last aggregate starting from literal1"<<std::endl;
                                    propagatedLiteralsAndReasons.insert({-1, std::vector<int>()});
                                }else{
                                    if(joinTuples_1_3SharedVariables.first->size() == 0-1){
                                        const std::vector<const Tuple*>* undefinedTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                        for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                            if(joinTuples_1_3SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 0){
                                                const Tuple* aggrTupleU=NULL;
                                                const std::vector<const Tuple*>* tuplesaux;
                                                const std::vector<const Tuple*>* tuplesUaux=&EMPTY_TUPLES;
                                                tuplesaux= &paux_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                if(aggrTupleU==NULL)
                                                    tuplesUaux= &uaux_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                for(int j2=0;j2<tuplesaux->size()+tuplesUaux->size();j2++){
                                                    const Tuple* auxTuple2=NULL;
                                                    bool addedToReason=false;
                                                    if(j2 < tuplesaux->size()){
                                                        auxTuple2 = tuplesaux->at(j2);
                                                        if(tuplesUaux != &EMPTY_TUPLES) {
                                                        }
                                                    }else{
                                                        auxTuple2 = tuplesUaux->at(j2-tuplesaux->size());
                                                        aggrTupleU = auxTuple2;
                                                    }
                                                    const std::vector<const Tuple*>* tuplesmember;
                                                    const std::vector<const Tuple*>* tuplesUmember=&EMPTY_TUPLES;
                                                    tuplesmember= &pmember_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                    if(aggrTupleU==NULL)
                                                        tuplesUmember= &umember_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                    for(int j4=0;j4<tuplesmember->size()+tuplesUmember->size();j4++){
                                                        const Tuple* auxTuple4=NULL;
                                                        bool addedToReason=false;
                                                        if(j4 < tuplesmember->size()){
                                                            auxTuple4 = tuplesmember->at(j4);
                                                            if(tuplesUmember != &EMPTY_TUPLES) {
                                                            }
                                                        }else{
                                                            auxTuple4 = tuplesUmember->at(j4-tuplesmember->size());
                                                            aggrTupleU = auxTuple4;
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
                                }
                            }//close ext aggregate if
                        }else{
                        std::pair<AuxMap*,AuxMap*> joinTuples_1_3;
                        joinTuples_1_3.first = &p_aux_R_Y_member_Y_R_;
                        joinTuples_1_3.second = &u_aux_R_Y_member_Y_R_;
                        {
                            Tuple sharedTuple(std::vector<int>({R,R}));
                            if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                                sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                for(int i=0;i<trueJoinTuples->size();i++){
                                    const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                    if(findResult!=NULL){
                                        sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                                    }
                                }
                                for(int i=0;i<undefJoinTuples->size();i++){
                                    const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                    if(findResult!=NULL){
                                        sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                                    }
                                }
                            }
                        }
                        std::pair<AuxMap*,AuxMap*> joinTuples_1_3SharedVariables = *sharedVariables_1_ToAggregate_3[std::vector<int>({R,R})];
                            if(joinTuples_1_3SharedVariables.first->size()>=0 && (!(actualSize4>=0+1))){
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
        }//close local scope
    }//close decision level == -1
    for(unsigned i=1;i<facts.size();i++) {
        unsigned factVar = facts[i] > 0 ? facts[i] : -facts[i];
        Tuple starter = atomsTable[factVar];
        starter.setNegated(facts[i]<0);
        if(starter.getPredicateName() == &_rest) { 
            const Tuple * tuple0 = &starter;
            if(facts[i] > 0){
                {
                    const Tuple * tupleU = NULL;
                    bool tupleUNegated = false;
                    int C = (*tuple0)[0];
                    int X = (*tuple0)[1];
                    int R = (*tuple0)[2];
                    const Tuple * tuple1 = (win.find({C, X}));
                    if(!tuple1 && !tupleU ){
                        tuple1 = tupleU = (uin.find({C, X}));
                        tupleUNegated = false;
                    }
                    if(tuple1){
                        const Tuple negativeTuple = Tuple({R, 1}, &_cliquesize, true);
                        const Tuple * tuple2 = &negativeTuple;
                        bool lTrue = (wcliquesize.find(negativeTuple)!=NULL);
                        const Tuple * undefTuple = ucliquesize.find(negativeTuple);
                        if((!lTrue && undefTuple == NULL) || (undefTuple && tupleU == NULL)){
                            if(undefTuple){
                                tuple2 = tupleU = undefTuple;
                                tupleU->print();
                                tupleUNegated = true;
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_1_3;
                            joinTuples_1_3.first = &p_aux_R_Y_member_Y_R_;
                            joinTuples_1_3.second = &u_aux_R_Y_member_Y_R_;
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                                    sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                    const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    for(int i=0;i<trueJoinTuples->size();i++){
                                        const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                        if(findResult!=NULL){
                                            sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                                        }
                                    }
                                    for(int i=0;i<undefJoinTuples->size();i++){
                                        const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                        if(findResult!=NULL){
                                            sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                                        }
                                    }
                                }
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_1_3SharedVariables = *sharedVariables_1_ToAggregate_3[std::vector<int>({R,R})];
                            if(tupleU==NULL){
                                const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                std::vector<int> reason;
                                for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                    Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                                    const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                                    if(itaux_0!=tupleToVar.end()){
                                        reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                                    }//closing if
                                    Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                                    const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                                    if(itmember_1!=tupleToVar.end()){
                                        reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
                                    }//closing if
                                }
                                if(joinTuples_1_3SharedVariables.first->size()>=0){
                                    std::pair<AuxMap*,AuxMap*> joinTuples_1_4;
                                    joinTuples_1_4.first = &p_aux_R_Y_member_Y_R_;
                                    joinTuples_1_4.second = &u_aux_R_Y_member_Y_R_;
                                    {
                                        Tuple sharedTuple(std::vector<int>({R,R}));
                                        if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                                            sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                            const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            for(int i=0;i<trueJoinTuples->size();i++){
                                                const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                                if(findResult!=NULL){
                                                    sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                                                }
                                            }
                                            for(int i=0;i<undefJoinTuples->size();i++){
                                                const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                                if(findResult!=NULL){
                                                    sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                                                }
                                            }
                                        }
                                    }
                                    std::pair<AuxMap*,AuxMap*> joinTuples_1_4SharedVariables = *sharedVariables_1_ToAggregate_4[std::vector<int>({R,R})];
                                    int actualSize4 = joinTuples_1_4SharedVariables.first->size();
                                    std::set<std::vector<int>> alreadyCounted4;
                                    for(const Tuple * joinTupleUndef : u_aux_R_Y_member_Y_R_0_3_.getValues({R,R})){
                                        if(joinTuples_1_4SharedVariables.first->getValues({joinTupleUndef->at(1)}).size()==0){
                                            if(joinTuples_1_4SharedVariables.second->getValues({joinTupleUndef->at(1)}).size()>0){
                                                std::vector<int> key({joinTupleUndef->at(1)});
                                                alreadyCounted4.insert(key);
                                            }
                                        }
                                    }
                                    actualSize4+=alreadyCounted4.size();
                                    const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                                    for(int i=0;i<falseTuples0->size();i++){
                                        bool joiningTupleFound=false;
                                        int R = falseTuples0->at(i)->at(0);
                                        int Y = falseTuples0->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                                        const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                                        const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                                        if(joiningTupleFound){
                                            const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                            if(it!=tupleToVar.end()){
                                                reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                            }//closing if
                                        }
                                    }
                                    const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                                    for(int i=0;i<falseTuples1->size();i++){
                                        bool joiningTupleFound=false;
                                        int Y = falseTuples1->at(i)->at(0);
                                        int R = falseTuples1->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                                        const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                                        const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                                        for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                            const Tuple * tuple0=NULL;
                                            if(i_0<trueTuples0->size())
                                                tuple0=trueTuples0->at(i_0);
                                            else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                            else
                                                tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                            joiningTupleFound=true;
                                        }
                                        if(joiningTupleFound){
                                            const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                            if(it!=tupleToVar.end()){
                                                reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
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
                                    if(tuple2!=tupleU){
                                        const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                        if(it_reason2!=tupleToVar.end())
                                            reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                    }
                                    const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                                    if(it_reason_starter!=tupleToVar.end())
                                        reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                                    if(!(actualSize4>=0+1)){
                                        std::cout<<"conflict detected in propagatoron last aggregate starting from literal1"<<std::endl;
                                        propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                    }else{
                                        if(actualSize4 == 0+1){
                                            const std::vector<const Tuple*>* undefinedTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                if(joinTuples_1_4SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 0 && joinTuples_1_4SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 1){
                                                    {
                                                        const Tuple* aggrTupleU = uaux.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                        if(aggrTupleU != NULL){
                                                            const auto & it = tupleToVar.find(*aggrTupleU);
                                                            if(it != tupleToVar.end()) {
                                                                int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                            }
                                                        }
                                                    }
                                                    {
                                                        const Tuple* aggrTupleU = umember.find({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
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
                                }//close ext aggregate if
                            }else{
                            std::vector<int> reason;
                            const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                            for(int i=0;i<falseTuples0->size();i++){
                                bool joiningTupleFound=false;
                                int R = falseTuples0->at(i)->at(0);
                                int Y = falseTuples0->at(i)->at(1);
                                const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                                const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                                const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                                if(joiningTupleFound){
                                    const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                    if(it!=tupleToVar.end()){
                                        reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                    }//closing if
                                }
                            }
                            const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                            for(int i=0;i<falseTuples1->size();i++){
                                bool joiningTupleFound=false;
                                int Y = falseTuples1->at(i)->at(0);
                                int R = falseTuples1->at(i)->at(1);
                                const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                                const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                                const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                                for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                    const Tuple * tuple0=NULL;
                                    if(i_0<trueTuples0->size())
                                        tuple0=trueTuples0->at(i_0);
                                    else if(i_0<trueTuples0->size()+undefTuples0->size())
                                        tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                    else
                                        tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                    joiningTupleFound=true;
                                }
                                if(joiningTupleFound){
                                    const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                    if(it!=tupleToVar.end()){
                                        reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
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
                            if(tuple2!=tupleU){
                                const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                if(it_reason2!=tupleToVar.end())
                                    reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                            }
                            const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                            for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                                const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                                if(itaux_0!=tupleToVar.end()){
                                    reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                                }//closing if
                                Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                                const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                                if(itmember_1!=tupleToVar.end()){
                                    reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
                                }//closing if
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_1_4;
                            joinTuples_1_4.first = &p_aux_R_Y_member_Y_R_;
                            joinTuples_1_4.second = &u_aux_R_Y_member_Y_R_;
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                                    sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                    const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    for(int i=0;i<trueJoinTuples->size();i++){
                                        const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                        if(findResult!=NULL){
                                            sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                                        }
                                    }
                                    for(int i=0;i<undefJoinTuples->size();i++){
                                        const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                        if(findResult!=NULL){
                                            sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                                        }
                                    }
                                }
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_1_4SharedVariables = *sharedVariables_1_ToAggregate_4[std::vector<int>({R,R})];
                            int actualSize4 = joinTuples_1_4SharedVariables.first->size();
                            std::set<std::vector<int>> alreadyCounted4;
                            for(const Tuple * joinTupleUndef : u_aux_R_Y_member_Y_R_0_3_.getValues({R,R})){
                                if(joinTuples_1_4SharedVariables.first->getValues({joinTupleUndef->at(1)}).size()==0){
                                    if(joinTuples_1_4SharedVariables.second->getValues({joinTupleUndef->at(1)}).size()>0){
                                        std::vector<int> key({joinTupleUndef->at(1)});
                                        alreadyCounted4.insert(key);
                                    }
                                }
                            }
                            actualSize4+=alreadyCounted4.size();
                                if(!(actualSize4>=0+1) && (joinTuples_1_3SharedVariables.first->size()>=0)){
                                    const auto & it = tupleToVar.find(*tupleU);
                                    if(it != tupleToVar.end()) {
                                        int sign = tupleU->isNegated() ? -1 : 1;
                                        auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                    }
                                }
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_1_4;
                            joinTuples_1_4.first = &p_aux_R_Y_member_Y_R_;
                            joinTuples_1_4.second = &u_aux_R_Y_member_Y_R_;
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                                    sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                    const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    for(int i=0;i<trueJoinTuples->size();i++){
                                        const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                        if(findResult!=NULL){
                                            sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                                        }
                                    }
                                    for(int i=0;i<undefJoinTuples->size();i++){
                                        const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                        if(findResult!=NULL){
                                            sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                                        }
                                    }
                                }
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_1_4SharedVariables = *sharedVariables_1_ToAggregate_4[std::vector<int>({R,R})];
                            int actualSize4 = joinTuples_1_4SharedVariables.first->size();
                            std::set<std::vector<int>> alreadyCounted4;
                            for(const Tuple * joinTupleUndef : u_aux_R_Y_member_Y_R_0_3_.getValues({R,R})){
                                if(joinTuples_1_4SharedVariables.first->getValues({joinTupleUndef->at(1)}).size()==0){
                                    if(joinTuples_1_4SharedVariables.second->getValues({joinTupleUndef->at(1)}).size()>0){
                                        std::vector<int> key({joinTupleUndef->at(1)});
                                        alreadyCounted4.insert(key);
                                    }
                                }
                            }
                            actualSize4+=alreadyCounted4.size();
                            if(tupleU==NULL){
                                std::vector<int> reason;
                                const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                                for(int i=0;i<falseTuples0->size();i++){
                                    bool joiningTupleFound=false;
                                    int R = falseTuples0->at(i)->at(0);
                                    int Y = falseTuples0->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                                    const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                                    const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                                    if(joiningTupleFound){
                                        const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                        if(it!=tupleToVar.end()){
                                            reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                        }//closing if
                                    }
                                }
                                const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                                for(int i=0;i<falseTuples1->size();i++){
                                    bool joiningTupleFound=false;
                                    int Y = falseTuples1->at(i)->at(0);
                                    int R = falseTuples1->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                                    const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                                    const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                                    for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                        const Tuple * tuple0=NULL;
                                        if(i_0<trueTuples0->size())
                                            tuple0=trueTuples0->at(i_0);
                                        else if(i_0<trueTuples0->size()+undefTuples0->size())
                                            tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                        else
                                            tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                        joiningTupleFound=true;
                                    }
                                    if(joiningTupleFound){
                                        const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                        if(it!=tupleToVar.end()){
                                            reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
                                        }//closing if
                                    }
                                }
                                if(!(actualSize4>=0+1)){
                                    std::pair<AuxMap*,AuxMap*> joinTuples_1_3;
                                    joinTuples_1_3.first = &p_aux_R_Y_member_Y_R_;
                                    joinTuples_1_3.second = &u_aux_R_Y_member_Y_R_;
                                    {
                                        Tuple sharedTuple(std::vector<int>({R,R}));
                                        if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                                            sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                            const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            for(int i=0;i<trueJoinTuples->size();i++){
                                                const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                                if(findResult!=NULL){
                                                    sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                                                }
                                            }
                                            for(int i=0;i<undefJoinTuples->size();i++){
                                                const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                                if(findResult!=NULL){
                                                    sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                                                }
                                            }
                                        }
                                    }
                                    std::pair<AuxMap*,AuxMap*> joinTuples_1_3SharedVariables = *sharedVariables_1_ToAggregate_3[std::vector<int>({R,R})];
                                    const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                        Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                                        const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                                        if(itaux_0!=tupleToVar.end()){
                                            reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                                        }//closing if
                                        Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                                        const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                                        if(itmember_1!=tupleToVar.end()){
                                            reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
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
                                    if(tuple2!=tupleU){
                                        const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                        if(it_reason2!=tupleToVar.end())
                                            reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                    }
                                    const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                                    if(it_reason_starter!=tupleToVar.end())
                                        reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                                    if(joinTuples_1_3SharedVariables.first->size()>=0){
                                        std::cout<<"conflict detected in propagatoron last aggregate starting from literal1"<<std::endl;
                                        propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                    }else{
                                        if(joinTuples_1_3SharedVariables.first->size() == 0-1){
                                            const std::vector<const Tuple*>* undefinedTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                if(joinTuples_1_3SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 0){
                                                    const Tuple* aggrTupleU=NULL;
                                                    const std::vector<const Tuple*>* tuplesaux;
                                                    const std::vector<const Tuple*>* tuplesUaux=&EMPTY_TUPLES;
                                                    tuplesaux= &paux_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                    if(aggrTupleU==NULL)
                                                        tuplesUaux= &uaux_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                    for(int j2=0;j2<tuplesaux->size()+tuplesUaux->size();j2++){
                                                        const Tuple* auxTuple2=NULL;
                                                        bool addedToReason=false;
                                                        if(j2 < tuplesaux->size()){
                                                            auxTuple2 = tuplesaux->at(j2);
                                                            const auto & it = tupleToVar.find(*auxTuple2);
                                                            if(it!=tupleToVar.end()){
                                                                addedToReason=true;
                                                                reason.push_back(it->second * (auxTuple2->isNegated() ? -1:1));
                                                            }//closing if
                                                            if(tuplesUaux != &EMPTY_TUPLES) {
                                                            }
                                                        }else{
                                                            auxTuple2 = tuplesUaux->at(j2-tuplesaux->size());
                                                            aggrTupleU = auxTuple2;
                                                        }
                                                        const std::vector<const Tuple*>* tuplesmember;
                                                        const std::vector<const Tuple*>* tuplesUmember=&EMPTY_TUPLES;
                                                        tuplesmember= &pmember_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                        if(aggrTupleU==NULL)
                                                            tuplesUmember= &umember_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                        for(int j4=0;j4<tuplesmember->size()+tuplesUmember->size();j4++){
                                                            const Tuple* auxTuple4=NULL;
                                                            bool addedToReason=false;
                                                            if(j4 < tuplesmember->size()){
                                                                auxTuple4 = tuplesmember->at(j4);
                                                                const auto & it = tupleToVar.find(*auxTuple4);
                                                                if(it!=tupleToVar.end()){
                                                                    addedToReason=true;
                                                                    reason.push_back(it->second * (auxTuple4->isNegated() ? -1:1));
                                                                }//closing if
                                                                if(tuplesUmember != &EMPTY_TUPLES) {
                                                                }
                                                            }else{
                                                                auxTuple4 = tuplesUmember->at(j4-tuplesmember->size());
                                                                aggrTupleU = auxTuple4;
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
                                                        if(addedToReason) reason.pop_back();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }//close ext aggregate if
                            }else{
                            const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                            std::vector<int> reason;
                            for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                                const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                                if(itaux_0!=tupleToVar.end()){
                                    reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                                }//closing if
                                Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                                const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                                if(itmember_1!=tupleToVar.end()){
                                    reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
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
                            if(tuple2!=tupleU){
                                const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                if(it_reason2!=tupleToVar.end())
                                    reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                            }
                            const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                            for(int i=0;i<falseTuples0->size();i++){
                                bool joiningTupleFound=false;
                                int R = falseTuples0->at(i)->at(0);
                                int Y = falseTuples0->at(i)->at(1);
                                const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                                const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                                const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                                if(joiningTupleFound){
                                    const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                    if(it!=tupleToVar.end()){
                                        reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                    }//closing if
                                }
                            }
                            const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                            for(int i=0;i<falseTuples1->size();i++){
                                bool joiningTupleFound=false;
                                int Y = falseTuples1->at(i)->at(0);
                                int R = falseTuples1->at(i)->at(1);
                                const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                                const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                                const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                                for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                    const Tuple * tuple0=NULL;
                                    if(i_0<trueTuples0->size())
                                        tuple0=trueTuples0->at(i_0);
                                    else if(i_0<trueTuples0->size()+undefTuples0->size())
                                        tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                    else
                                        tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                    joiningTupleFound=true;
                                }
                                if(joiningTupleFound){
                                    const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                    if(it!=tupleToVar.end()){
                                        reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
                                    }//closing if
                                }
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_1_3;
                            joinTuples_1_3.first = &p_aux_R_Y_member_Y_R_;
                            joinTuples_1_3.second = &u_aux_R_Y_member_Y_R_;
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                                    sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                    const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    for(int i=0;i<trueJoinTuples->size();i++){
                                        const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                        if(findResult!=NULL){
                                            sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                                        }
                                    }
                                    for(int i=0;i<undefJoinTuples->size();i++){
                                        const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                        if(findResult!=NULL){
                                            sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                                        }
                                    }
                                }
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_1_3SharedVariables = *sharedVariables_1_ToAggregate_3[std::vector<int>({R,R})];
                                if(joinTuples_1_3SharedVariables.first->size()>=0 && (!(actualSize4>=0+1))){
                                    const auto & it = tupleToVar.find(*tupleU);
                                    if(it != tupleToVar.end()) {
                                        int sign = tupleU->isNegated() ? -1 : 1;
                                        auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                    }
                                }
                            }
                        }//close par
                    }//close par
                }//close loop nested join
            }//close loop nested join
        }//close predicate joins
        if(starter.getPredicateName() == &_clique) { 
            const Tuple * tuple0 = &starter;
            if(facts[i] > 0){
                {
                    const Tuple * tupleU = NULL;
                    bool tupleUNegated = false;
                    int C = (*tuple0)[0];
                    std::pair<AuxMap*,AuxMap*> joinTuples_0_1;
                    joinTuples_0_1.first = &p_in_C_X_pair_C_X_;
                    joinTuples_0_1.second = &u_in_C_X_pair_C_X_;
                    {
                        Tuple sharedTuple(std::vector<int>({C,C}));
                        if(!sharedVariables_0_ToAggregate_1.count(sharedTuple)){
                            sharedVariables_0_ToAggregate_1[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                            const std::vector<const Tuple*>* trueJoinTuples = &p_in_C_X_pair_C_X_0_2_.getValues({C,C});
                            const std::vector<const Tuple*>* undefJoinTuples = &u_in_C_X_pair_C_X_0_2_.getValues({C,C});
                            for(int i=0;i<trueJoinTuples->size();i++){
                                const auto findResult = win_C_X_pair_C_X_.find(Tuple(*trueJoinTuples->at(i)));
                                if(findResult!=NULL){
                                    sharedVariables_0_ToAggregate_1[sharedTuple]->first->insert2(*findResult);
                                }
                            }
                            for(int i=0;i<undefJoinTuples->size();i++){
                                const auto findResult = uin_C_X_pair_C_X_.find(Tuple(*undefJoinTuples->at(i)));
                                if(findResult!=NULL){
                                    sharedVariables_0_ToAggregate_1[sharedTuple]->second->insert2(*findResult);
                                }
                            }
                        }
                    }
                    std::pair<AuxMap*,AuxMap*> joinTuples_0_1SharedVariables = *sharedVariables_0_ToAggregate_1[std::vector<int>({C,C})];
                    if(tupleU==NULL){
                        const std::vector<const Tuple*>* trueTuples = &p_in_C_X_pair_C_X_0_2_.getValues({C,C});
                        std::vector<int> reason;
                        for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                            Tuple tuplein_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_in);
                            const auto & itin_0 = tupleToVar.find(tuplein_0);
                            if(itin_0!=tupleToVar.end()){
                                reason.push_back(itin_0->second * (tuplein_0.isNegated() ? -1:1));
                            }//closing if
                            Tuple tuplepair_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_pair);
                            const auto & itpair_1 = tupleToVar.find(tuplepair_1);
                            if(itpair_1!=tupleToVar.end()){
                                reason.push_back(itpair_1->second * (tuplepair_1.isNegated() ? -1:1));
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
                        if(joinTuples_0_1SharedVariables.first->size()>=2+1){
                            std::cout<<"conflict detected in propagator0"<<std::endl;
                            propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                        }else{
                            if(joinTuples_0_1SharedVariables.first->size() == 2+1-1){
                                const std::vector<const Tuple*>* undefinedTuples = &u_in_C_X_pair_C_X_0_2_.getValues({C,C});
                                for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                    if(joinTuples_0_1SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 0){
                                        const Tuple* aggrTupleU=NULL;
                                        const std::vector<const Tuple*>* tuplesin;
                                        const std::vector<const Tuple*>* tuplesUin=&EMPTY_TUPLES;
                                        tuplesin= &pin_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                        if(aggrTupleU==NULL)
                                            tuplesUin= &uin_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                        for(int j2=0;j2<tuplesin->size()+tuplesUin->size();j2++){
                                            const Tuple* auxTuple2=NULL;
                                            bool addedToReason=false;
                                            if(j2 < tuplesin->size()){
                                                auxTuple2 = tuplesin->at(j2);
                                                const auto & it = tupleToVar.find(*auxTuple2);
                                                if(it!=tupleToVar.end()){
                                                    addedToReason=true;
                                                    reason.push_back(it->second * (auxTuple2->isNegated() ? -1:1));
                                                }//closing if
                                                if(tuplesUin != &EMPTY_TUPLES) {
                                                }
                                            }else{
                                                auxTuple2 = tuplesUin->at(j2-tuplesin->size());
                                                aggrTupleU = auxTuple2;
                                            }
                                            const std::vector<const Tuple*>* tuplespair;
                                            const std::vector<const Tuple*>* tuplesUpair=&EMPTY_TUPLES;
                                            tuplespair= &ppair_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                            if(aggrTupleU==NULL)
                                                tuplesUpair= &upair_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                            for(int j4=0;j4<tuplespair->size()+tuplesUpair->size();j4++){
                                                const Tuple* auxTuple4=NULL;
                                                bool addedToReason=false;
                                                if(j4 < tuplespair->size()){
                                                    auxTuple4 = tuplespair->at(j4);
                                                    const auto & it = tupleToVar.find(*auxTuple4);
                                                    if(it!=tupleToVar.end()){
                                                        addedToReason=true;
                                                        reason.push_back(it->second * (auxTuple4->isNegated() ? -1:1));
                                                    }//closing if
                                                    if(tuplesUpair != &EMPTY_TUPLES) {
                                                    }
                                                }else{
                                                    auxTuple4 = tuplesUpair->at(j4-tuplespair->size());
                                                    aggrTupleU = auxTuple4;
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
                                            if(addedToReason) reason.pop_back();
                                        }
                                    }
                                }
                            }
                        }
                    }else{
                    const std::vector<const Tuple*>* trueTuples = &p_in_C_X_pair_C_X_0_2_.getValues({C,C});
                    std::vector<int> reason;
                    for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                        Tuple tuplein_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_in);
                        const auto & itin_0 = tupleToVar.find(tuplein_0);
                        if(itin_0!=tupleToVar.end()){
                            reason.push_back(itin_0->second * (tuplein_0.isNegated() ? -1:1));
                        }//closing if
                        Tuple tuplepair_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_pair);
                        const auto & itpair_1 = tupleToVar.find(tuplepair_1);
                        if(itpair_1!=tupleToVar.end()){
                            reason.push_back(itpair_1->second * (tuplepair_1.isNegated() ? -1:1));
                        }//closing if
                    }
                    if(tuple0!=tupleU){
                        const auto & it_reason0 = tupleToVar.find(Tuple(*tuple0));
                        if(it_reason0!=tupleToVar.end())
                            reason.push_back(it_reason0->second * (tuple0->isNegated() ? -1:1));
                    }
                        if(joinTuples_0_1SharedVariables.first->size()>=2+1){
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
        if(starter.getPredicateName() == &_cliquesize) { 
            const Tuple * tuple0 = &starter;
            if(facts[i] < 0){
                {
                    const Tuple * tupleU = NULL;
                    bool tupleUNegated = false;
                    int R = (*tuple0)[0];
                    if( (*tuple0)[1] == 1){
                        const std::vector<const Tuple* >* tuples;
                        tuples = &pin_.getValues({});
                        const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                        if(tupleU == NULL){
                            tuplesU = &uin_.getValues({});
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
                            int C = (*tuple1)[0];
                            int X = (*tuple1)[1];
                            const Tuple * tuple2 = (wrest.find({C, X, R}));
                            if(!tuple2 && !tupleU ){
                                tuple2 = tupleU = (urest.find({C, X, R}));
                                tupleUNegated = false;
                            }
                            if(tuple2){
                                std::pair<AuxMap*,AuxMap*> joinTuples_1_3;
                                joinTuples_1_3.first = &p_aux_R_Y_member_Y_R_;
                                joinTuples_1_3.second = &u_aux_R_Y_member_Y_R_;
                                {
                                    Tuple sharedTuple(std::vector<int>({R,R}));
                                    if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                                        sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                        for(int i=0;i<trueJoinTuples->size();i++){
                                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                            if(findResult!=NULL){
                                                sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                                            }
                                        }
                                        for(int i=0;i<undefJoinTuples->size();i++){
                                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                            if(findResult!=NULL){
                                                sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                                            }
                                        }
                                    }
                                }
                                std::pair<AuxMap*,AuxMap*> joinTuples_1_3SharedVariables = *sharedVariables_1_ToAggregate_3[std::vector<int>({R,R})];
                                if(tupleU==NULL){
                                    const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    std::vector<int> reason;
                                    for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                        Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                                        const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                                        if(itaux_0!=tupleToVar.end()){
                                            reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                                        }//closing if
                                        Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                                        const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                                        if(itmember_1!=tupleToVar.end()){
                                            reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
                                        }//closing if
                                    }
                                    if(joinTuples_1_3SharedVariables.first->size()>=0){
                                        std::pair<AuxMap*,AuxMap*> joinTuples_1_4;
                                        joinTuples_1_4.first = &p_aux_R_Y_member_Y_R_;
                                        joinTuples_1_4.second = &u_aux_R_Y_member_Y_R_;
                                        {
                                            Tuple sharedTuple(std::vector<int>({R,R}));
                                            if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                                                sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                                const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                                const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                                for(int i=0;i<trueJoinTuples->size();i++){
                                                    const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                                    if(findResult!=NULL){
                                                        sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                                                    }
                                                }
                                                for(int i=0;i<undefJoinTuples->size();i++){
                                                    const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                                    if(findResult!=NULL){
                                                        sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                                                    }
                                                }
                                            }
                                        }
                                        std::pair<AuxMap*,AuxMap*> joinTuples_1_4SharedVariables = *sharedVariables_1_ToAggregate_4[std::vector<int>({R,R})];
                                        int actualSize4 = joinTuples_1_4SharedVariables.first->size();
                                        std::set<std::vector<int>> alreadyCounted4;
                                        for(const Tuple * joinTupleUndef : u_aux_R_Y_member_Y_R_0_3_.getValues({R,R})){
                                            if(joinTuples_1_4SharedVariables.first->getValues({joinTupleUndef->at(1)}).size()==0){
                                                if(joinTuples_1_4SharedVariables.second->getValues({joinTupleUndef->at(1)}).size()>0){
                                                    std::vector<int> key({joinTupleUndef->at(1)});
                                                    alreadyCounted4.insert(key);
                                                }
                                            }
                                        }
                                        actualSize4+=alreadyCounted4.size();
                                        const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                                        for(int i=0;i<falseTuples0->size();i++){
                                            bool joiningTupleFound=false;
                                            int R = falseTuples0->at(i)->at(0);
                                            int Y = falseTuples0->at(i)->at(1);
                                            const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                                            const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                                            const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                                            if(joiningTupleFound){
                                                const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                                if(it!=tupleToVar.end()){
                                                    reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                                }//closing if
                                            }
                                        }
                                        const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                                        for(int i=0;i<falseTuples1->size();i++){
                                            bool joiningTupleFound=false;
                                            int Y = falseTuples1->at(i)->at(0);
                                            int R = falseTuples1->at(i)->at(1);
                                            const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                                            const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                                            const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                                            for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                                const Tuple * tuple0=NULL;
                                                if(i_0<trueTuples0->size())
                                                    tuple0=trueTuples0->at(i_0);
                                                else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                    tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                                else
                                                    tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                                joiningTupleFound=true;
                                            }
                                            if(joiningTupleFound){
                                                const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                                if(it!=tupleToVar.end()){
                                                    reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
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
                                        if(tuple2!=tupleU){
                                            const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                            if(it_reason2!=tupleToVar.end())
                                                reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                        }
                                        const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                                        if(it_reason_starter!=tupleToVar.end())
                                            reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                                        if(!(actualSize4>=0+1)){
                                            std::cout<<"conflict detected in propagatoron last aggregate starting from literal1"<<std::endl;
                                            propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                        }else{
                                            if(actualSize4 == 0+1){
                                                const std::vector<const Tuple*>* undefinedTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                                for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                    if(joinTuples_1_4SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 0 && joinTuples_1_4SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 1){
                                                        {
                                                            const Tuple* aggrTupleU = uaux.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                            if(aggrTupleU != NULL){
                                                                const auto & it = tupleToVar.find(*aggrTupleU);
                                                                if(it != tupleToVar.end()) {
                                                                    int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                                }
                                                            }
                                                        }
                                                        {
                                                            const Tuple* aggrTupleU = umember.find({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
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
                                    }//close ext aggregate if
                                }else{
                                std::vector<int> reason;
                                const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                                for(int i=0;i<falseTuples0->size();i++){
                                    bool joiningTupleFound=false;
                                    int R = falseTuples0->at(i)->at(0);
                                    int Y = falseTuples0->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                                    const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                                    const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                                    if(joiningTupleFound){
                                        const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                        if(it!=tupleToVar.end()){
                                            reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                        }//closing if
                                    }
                                }
                                const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                                for(int i=0;i<falseTuples1->size();i++){
                                    bool joiningTupleFound=false;
                                    int Y = falseTuples1->at(i)->at(0);
                                    int R = falseTuples1->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                                    const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                                    const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                                    for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                        const Tuple * tuple0=NULL;
                                        if(i_0<trueTuples0->size())
                                            tuple0=trueTuples0->at(i_0);
                                        else if(i_0<trueTuples0->size()+undefTuples0->size())
                                            tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                        else
                                            tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                        joiningTupleFound=true;
                                    }
                                    if(joiningTupleFound){
                                        const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                        if(it!=tupleToVar.end()){
                                            reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
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
                                if(tuple2!=tupleU){
                                    const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                    if(it_reason2!=tupleToVar.end())
                                        reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                }
                                const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                    Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                                    const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                                    if(itaux_0!=tupleToVar.end()){
                                        reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                                    }//closing if
                                    Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                                    const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                                    if(itmember_1!=tupleToVar.end()){
                                        reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
                                    }//closing if
                                }
                                std::pair<AuxMap*,AuxMap*> joinTuples_1_4;
                                joinTuples_1_4.first = &p_aux_R_Y_member_Y_R_;
                                joinTuples_1_4.second = &u_aux_R_Y_member_Y_R_;
                                {
                                    Tuple sharedTuple(std::vector<int>({R,R}));
                                    if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                                        sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                        for(int i=0;i<trueJoinTuples->size();i++){
                                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                            if(findResult!=NULL){
                                                sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                                            }
                                        }
                                        for(int i=0;i<undefJoinTuples->size();i++){
                                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                            if(findResult!=NULL){
                                                sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                                            }
                                        }
                                    }
                                }
                                std::pair<AuxMap*,AuxMap*> joinTuples_1_4SharedVariables = *sharedVariables_1_ToAggregate_4[std::vector<int>({R,R})];
                                int actualSize4 = joinTuples_1_4SharedVariables.first->size();
                                std::set<std::vector<int>> alreadyCounted4;
                                for(const Tuple * joinTupleUndef : u_aux_R_Y_member_Y_R_0_3_.getValues({R,R})){
                                    if(joinTuples_1_4SharedVariables.first->getValues({joinTupleUndef->at(1)}).size()==0){
                                        if(joinTuples_1_4SharedVariables.second->getValues({joinTupleUndef->at(1)}).size()>0){
                                            std::vector<int> key({joinTupleUndef->at(1)});
                                            alreadyCounted4.insert(key);
                                        }
                                    }
                                }
                                actualSize4+=alreadyCounted4.size();
                                    if(!(actualSize4>=0+1) && (joinTuples_1_3SharedVariables.first->size()>=0)){
                                        const auto & it = tupleToVar.find(*tupleU);
                                        if(it != tupleToVar.end()) {
                                            int sign = tupleU->isNegated() ? -1 : 1;
                                            auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                        }
                                    }
                                }
                                std::pair<AuxMap*,AuxMap*> joinTuples_1_4;
                                joinTuples_1_4.first = &p_aux_R_Y_member_Y_R_;
                                joinTuples_1_4.second = &u_aux_R_Y_member_Y_R_;
                                {
                                    Tuple sharedTuple(std::vector<int>({R,R}));
                                    if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                                        sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                        for(int i=0;i<trueJoinTuples->size();i++){
                                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                            if(findResult!=NULL){
                                                sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                                            }
                                        }
                                        for(int i=0;i<undefJoinTuples->size();i++){
                                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                            if(findResult!=NULL){
                                                sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                                            }
                                        }
                                    }
                                }
                                std::pair<AuxMap*,AuxMap*> joinTuples_1_4SharedVariables = *sharedVariables_1_ToAggregate_4[std::vector<int>({R,R})];
                                int actualSize4 = joinTuples_1_4SharedVariables.first->size();
                                std::set<std::vector<int>> alreadyCounted4;
                                for(const Tuple * joinTupleUndef : u_aux_R_Y_member_Y_R_0_3_.getValues({R,R})){
                                    if(joinTuples_1_4SharedVariables.first->getValues({joinTupleUndef->at(1)}).size()==0){
                                        if(joinTuples_1_4SharedVariables.second->getValues({joinTupleUndef->at(1)}).size()>0){
                                            std::vector<int> key({joinTupleUndef->at(1)});
                                            alreadyCounted4.insert(key);
                                        }
                                    }
                                }
                                actualSize4+=alreadyCounted4.size();
                                if(tupleU==NULL){
                                    std::vector<int> reason;
                                    const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                                    for(int i=0;i<falseTuples0->size();i++){
                                        bool joiningTupleFound=false;
                                        int R = falseTuples0->at(i)->at(0);
                                        int Y = falseTuples0->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                                        const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                                        const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                                        if(joiningTupleFound){
                                            const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                            if(it!=tupleToVar.end()){
                                                reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                            }//closing if
                                        }
                                    }
                                    const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                                    for(int i=0;i<falseTuples1->size();i++){
                                        bool joiningTupleFound=false;
                                        int Y = falseTuples1->at(i)->at(0);
                                        int R = falseTuples1->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                                        const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                                        const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                                        for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                            const Tuple * tuple0=NULL;
                                            if(i_0<trueTuples0->size())
                                                tuple0=trueTuples0->at(i_0);
                                            else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                            else
                                                tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                            joiningTupleFound=true;
                                        }
                                        if(joiningTupleFound){
                                            const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                            if(it!=tupleToVar.end()){
                                                reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
                                            }//closing if
                                        }
                                    }
                                    if(!(actualSize4>=0+1)){
                                        std::pair<AuxMap*,AuxMap*> joinTuples_1_3;
                                        joinTuples_1_3.first = &p_aux_R_Y_member_Y_R_;
                                        joinTuples_1_3.second = &u_aux_R_Y_member_Y_R_;
                                        {
                                            Tuple sharedTuple(std::vector<int>({R,R}));
                                            if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                                                sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                                const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                                const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                                for(int i=0;i<trueJoinTuples->size();i++){
                                                    const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                                    if(findResult!=NULL){
                                                        sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                                                    }
                                                }
                                                for(int i=0;i<undefJoinTuples->size();i++){
                                                    const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                                    if(findResult!=NULL){
                                                        sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                                                    }
                                                }
                                            }
                                        }
                                        std::pair<AuxMap*,AuxMap*> joinTuples_1_3SharedVariables = *sharedVariables_1_ToAggregate_3[std::vector<int>({R,R})];
                                        const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                        for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                            Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                                            const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                                            if(itaux_0!=tupleToVar.end()){
                                                reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                                            }//closing if
                                            Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                                            const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                                            if(itmember_1!=tupleToVar.end()){
                                                reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
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
                                        if(tuple2!=tupleU){
                                            const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                            if(it_reason2!=tupleToVar.end())
                                                reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                        }
                                        const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                                        if(it_reason_starter!=tupleToVar.end())
                                            reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                                        if(joinTuples_1_3SharedVariables.first->size()>=0){
                                            std::cout<<"conflict detected in propagatoron last aggregate starting from literal1"<<std::endl;
                                            propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                        }else{
                                            if(joinTuples_1_3SharedVariables.first->size() == 0-1){
                                                const std::vector<const Tuple*>* undefinedTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                                for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                    if(joinTuples_1_3SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 0){
                                                        const Tuple* aggrTupleU=NULL;
                                                        const std::vector<const Tuple*>* tuplesaux;
                                                        const std::vector<const Tuple*>* tuplesUaux=&EMPTY_TUPLES;
                                                        tuplesaux= &paux_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                        if(aggrTupleU==NULL)
                                                            tuplesUaux= &uaux_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                        for(int j2=0;j2<tuplesaux->size()+tuplesUaux->size();j2++){
                                                            const Tuple* auxTuple2=NULL;
                                                            bool addedToReason=false;
                                                            if(j2 < tuplesaux->size()){
                                                                auxTuple2 = tuplesaux->at(j2);
                                                                const auto & it = tupleToVar.find(*auxTuple2);
                                                                if(it!=tupleToVar.end()){
                                                                    addedToReason=true;
                                                                    reason.push_back(it->second * (auxTuple2->isNegated() ? -1:1));
                                                                }//closing if
                                                                if(tuplesUaux != &EMPTY_TUPLES) {
                                                                }
                                                            }else{
                                                                auxTuple2 = tuplesUaux->at(j2-tuplesaux->size());
                                                                aggrTupleU = auxTuple2;
                                                            }
                                                            const std::vector<const Tuple*>* tuplesmember;
                                                            const std::vector<const Tuple*>* tuplesUmember=&EMPTY_TUPLES;
                                                            tuplesmember= &pmember_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                            if(aggrTupleU==NULL)
                                                                tuplesUmember= &umember_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                            for(int j4=0;j4<tuplesmember->size()+tuplesUmember->size();j4++){
                                                                const Tuple* auxTuple4=NULL;
                                                                bool addedToReason=false;
                                                                if(j4 < tuplesmember->size()){
                                                                    auxTuple4 = tuplesmember->at(j4);
                                                                    const auto & it = tupleToVar.find(*auxTuple4);
                                                                    if(it!=tupleToVar.end()){
                                                                        addedToReason=true;
                                                                        reason.push_back(it->second * (auxTuple4->isNegated() ? -1:1));
                                                                    }//closing if
                                                                    if(tuplesUmember != &EMPTY_TUPLES) {
                                                                    }
                                                                }else{
                                                                    auxTuple4 = tuplesUmember->at(j4-tuplesmember->size());
                                                                    aggrTupleU = auxTuple4;
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
                                                            if(addedToReason) reason.pop_back();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }//close ext aggregate if
                                }else{
                                const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                std::vector<int> reason;
                                for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                    Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                                    const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                                    if(itaux_0!=tupleToVar.end()){
                                        reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                                    }//closing if
                                    Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                                    const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                                    if(itmember_1!=tupleToVar.end()){
                                        reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
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
                                if(tuple2!=tupleU){
                                    const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                    if(it_reason2!=tupleToVar.end())
                                        reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                }
                                const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                                for(int i=0;i<falseTuples0->size();i++){
                                    bool joiningTupleFound=false;
                                    int R = falseTuples0->at(i)->at(0);
                                    int Y = falseTuples0->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                                    const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                                    const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                                    if(joiningTupleFound){
                                        const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                        if(it!=tupleToVar.end()){
                                            reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                        }//closing if
                                    }
                                }
                                const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                                for(int i=0;i<falseTuples1->size();i++){
                                    bool joiningTupleFound=false;
                                    int Y = falseTuples1->at(i)->at(0);
                                    int R = falseTuples1->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                                    const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                                    const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                                    for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                        const Tuple * tuple0=NULL;
                                        if(i_0<trueTuples0->size())
                                            tuple0=trueTuples0->at(i_0);
                                        else if(i_0<trueTuples0->size()+undefTuples0->size())
                                            tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                        else
                                            tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                        joiningTupleFound=true;
                                    }
                                    if(joiningTupleFound){
                                        const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                        if(it!=tupleToVar.end()){
                                            reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
                                        }//closing if
                                    }
                                }
                                std::pair<AuxMap*,AuxMap*> joinTuples_1_3;
                                joinTuples_1_3.first = &p_aux_R_Y_member_Y_R_;
                                joinTuples_1_3.second = &u_aux_R_Y_member_Y_R_;
                                {
                                    Tuple sharedTuple(std::vector<int>({R,R}));
                                    if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                                        sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                        const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                        const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                        for(int i=0;i<trueJoinTuples->size();i++){
                                            const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                            if(findResult!=NULL){
                                                sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                                            }
                                        }
                                        for(int i=0;i<undefJoinTuples->size();i++){
                                            const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                            if(findResult!=NULL){
                                                sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                                            }
                                        }
                                    }
                                }
                                std::pair<AuxMap*,AuxMap*> joinTuples_1_3SharedVariables = *sharedVariables_1_ToAggregate_3[std::vector<int>({R,R})];
                                    if(joinTuples_1_3SharedVariables.first->size()>=0 && (!(actualSize4>=0+1))){
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
        if(facts[i] > 0){
            bool tupleUNegated = false;
            const Tuple * tupleU = NULL;
            if(starter.getPredicateName()== &_in || starter.getPredicateName()== &_pair){
                for(const auto sharedVarTuple : sharedVariables_0_ToAggregate_1){
                    tupleU=NULL;
                    int C = sharedVarTuple.first[0];
                    std::pair<AuxMap*,AuxMap*> joinTuples_0_1;
                    joinTuples_0_1.first=sharedVarTuple.second->first;
                    joinTuples_0_1.second=sharedVarTuple.second->second;
                    if(joinTuples_0_1.first->size()>=2+1){
                        const Tuple * tuple1 = (wclique.find({C}));
                        if(!tuple1 && !tupleU ){
                            tuple1 = tupleU = (uclique.find({C}));
                            tupleUNegated = false;
                        }
                        if(tuple1){
                            const std::vector<const Tuple*>* trueTuples = &p_in_C_X_pair_C_X_0_2_.getValues({C,C});
                            std::vector<int> reason;
                            for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                Tuple tuplein_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_in);
                                const auto & itin_0 = tupleToVar.find(tuplein_0);
                                if(itin_0!=tupleToVar.end()){
                                    reason.push_back(itin_0->second * (tuplein_0.isNegated() ? -1:1));
                                }//closing if
                                Tuple tuplepair_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_pair);
                                const auto & itpair_1 = tupleToVar.find(tuplepair_1);
                                if(itpair_1!=tupleToVar.end()){
                                    reason.push_back(itpair_1->second * (tuplepair_1.isNegated() ? -1:1));
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
                        }//close par
                    }//close par
                    else{
                        if(joinTuples_0_1.first->size()==2+1-1){
                            const Tuple * tuple1 = (wclique.find({C}));
                            if(!tuple1 && !tupleU ){
                                tuple1 = tupleU = (uclique.find({C}));
                                tupleUNegated = false;
                            }
                            if(tuple1){
                                if(tupleU ==  NULL) {
                                    std::pair<AuxMap*,AuxMap*> joinTuples_0_1;
                                    joinTuples_0_1.first = &p_in_C_X_pair_C_X_;
                                    joinTuples_0_1.second = &u_in_C_X_pair_C_X_;
                                    std::pair<AuxMap*,AuxMap*> joinTuples_0_1SharedVariables = *sharedVariables_0_ToAggregate_1[std::vector<int>({C,C})];
                                    const std::vector<const Tuple*>* trueTuples = &p_in_C_X_pair_C_X_0_2_.getValues({C,C});
                                    std::vector<int> reason;
                                    for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                        Tuple tuplein_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_in);
                                        const auto & itin_0 = tupleToVar.find(tuplein_0);
                                        if(itin_0!=tupleToVar.end()){
                                            reason.push_back(itin_0->second * (tuplein_0.isNegated() ? -1:1));
                                        }//closing if
                                        Tuple tuplepair_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_pair);
                                        const auto & itpair_1 = tupleToVar.find(tuplepair_1);
                                        if(itpair_1!=tupleToVar.end()){
                                            reason.push_back(itpair_1->second * (tuplepair_1.isNegated() ? -1:1));
                                        }//closing if
                                    }
                                    const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                                    if(it_reason1!=tupleToVar.end())
                                        reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                                    const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                                    if(it_reason_starter!=tupleToVar.end())
                                        reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                                    const std::vector<const Tuple*>* undefinedTuples = &u_in_C_X_pair_C_X_0_2_.getValues({C,C});
                                    for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                        if(joinTuples_0_1SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 0){
                                            const Tuple* aggrTupleU=NULL;
                                            const std::vector<const Tuple*>* tuplesin;
                                            const std::vector<const Tuple*>* tuplesUin=&EMPTY_TUPLES;
                                            tuplesin= &pin_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                            if(aggrTupleU==NULL)
                                                tuplesUin= &uin_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                            for(int j2=0;j2<tuplesin->size()+tuplesUin->size();j2++){
                                                const Tuple* auxTuple2=NULL;
                                                bool addedToReason=false;
                                                if(j2 < tuplesin->size()){
                                                    auxTuple2 = tuplesin->at(j2);
                                                    const auto & it = tupleToVar.find(*auxTuple2);
                                                    if(it!=tupleToVar.end()){
                                                        addedToReason=true;
                                                        reason.push_back(it->second * (auxTuple2->isNegated() ? -1:1));
                                                    }//closing if
                                                    if(tuplesUin != &EMPTY_TUPLES) {
                                                    }
                                                }else{
                                                    auxTuple2 = tuplesUin->at(j2-tuplesin->size());
                                                    aggrTupleU = auxTuple2;
                                                }
                                                const std::vector<const Tuple*>* tuplespair;
                                                const std::vector<const Tuple*>* tuplesUpair=&EMPTY_TUPLES;
                                                tuplespair= &ppair_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                if(aggrTupleU==NULL)
                                                    tuplesUpair= &upair_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                for(int j4=0;j4<tuplespair->size()+tuplesUpair->size();j4++){
                                                    const Tuple* auxTuple4=NULL;
                                                    bool addedToReason=false;
                                                    if(j4 < tuplespair->size()){
                                                        auxTuple4 = tuplespair->at(j4);
                                                        const auto & it = tupleToVar.find(*auxTuple4);
                                                        if(it!=tupleToVar.end()){
                                                            addedToReason=true;
                                                            reason.push_back(it->second * (auxTuple4->isNegated() ? -1:1));
                                                        }//closing if
                                                        if(tuplesUpair != &EMPTY_TUPLES) {
                                                        }
                                                    }else{
                                                        auxTuple4 = tuplesUpair->at(j4-tuplespair->size());
                                                        aggrTupleU = auxTuple4;
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
        }//close if
        if(facts[i] > 0){
            bool tupleUNegated = false;
            const Tuple * tupleU = NULL;
            if(starter.getPredicateName()== &_aux || starter.getPredicateName()== &_member){
                for(const auto sharedVarTuple : sharedVariables_1_ToAggregate_3){
                    tupleU=NULL;
                    int R = sharedVarTuple.first[0];
                    std::pair<AuxMap*,AuxMap*> joinTuples_1_3;
                    joinTuples_1_3.first=sharedVarTuple.second->first;
                    joinTuples_1_3.second=sharedVarTuple.second->second;
                    if(joinTuples_1_3.first->size()>=0){
                        const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                        std::vector<int> reason;
                        for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                            Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                            const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                            if(itaux_0!=tupleToVar.end()){
                                reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                            }//closing if
                            Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                            const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                            if(itmember_1!=tupleToVar.end()){
                                reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
                            }//closing if
                        }
                        const std::vector<const Tuple* >* tuples;
                        tuples = &pin_.getValues({});
                        const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                        if(tupleU == NULL){
                            tuplesU = &uin_.getValues({});
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
                            int C = (*tuple1)[0];
                            int X = (*tuple1)[1];
                            const Tuple * tuple2 = (wrest.find({C, X, R}));
                            if(!tuple2 && !tupleU ){
                                tuple2 = tupleU = (urest.find({C, X, R}));
                                tupleUNegated = false;
                            }
                            if(tuple2){
                                const Tuple negativeTuple = Tuple({R, 1}, &_cliquesize, true);
                                const Tuple * tuple3 = &negativeTuple;
                                bool lTrue = (wcliquesize.find(negativeTuple)!=NULL);
                                const Tuple * undefTuple = ucliquesize.find(negativeTuple);
                                if((!lTrue && undefTuple == NULL) || (undefTuple && tupleU == NULL)){
                                    if(undefTuple){
                                        tuple3 = tupleU = undefTuple;
                                        tupleU->print();
                                        tupleUNegated = true;
                                    }
                                    std::pair<AuxMap*,AuxMap*> joinTuples_1_4;
                                    joinTuples_1_4.first = &p_aux_R_Y_member_Y_R_;
                                    joinTuples_1_4.second = &u_aux_R_Y_member_Y_R_;
                                    {
                                        Tuple sharedTuple(std::vector<int>({R,R}));
                                        if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                                            sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                            const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            for(int i=0;i<trueJoinTuples->size();i++){
                                                const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                                if(findResult!=NULL){
                                                    sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                                                }
                                            }
                                            for(int i=0;i<undefJoinTuples->size();i++){
                                                const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                                if(findResult!=NULL){
                                                    sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                                                }
                                            }
                                        }
                                    }
                                    std::pair<AuxMap*,AuxMap*> joinTuples_1_4SharedVariables = *sharedVariables_1_ToAggregate_4[std::vector<int>({R,R})];
                                    int actualSize4 = joinTuples_1_4SharedVariables.first->size();
                                    std::set<std::vector<int>> alreadyCounted4;
                                    for(const Tuple * joinTupleUndef : u_aux_R_Y_member_Y_R_0_3_.getValues({R,R})){
                                        if(joinTuples_1_4SharedVariables.first->getValues({joinTupleUndef->at(1)}).size()==0){
                                            if(joinTuples_1_4SharedVariables.second->getValues({joinTupleUndef->at(1)}).size()>0){
                                                std::vector<int> key({joinTupleUndef->at(1)});
                                                alreadyCounted4.insert(key);
                                            }
                                        }
                                    }
                                    actualSize4+=alreadyCounted4.size();
                                    if(tupleU==NULL){
                                        const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                                        for(int i=0;i<falseTuples0->size();i++){
                                            bool joiningTupleFound=false;
                                            int R = falseTuples0->at(i)->at(0);
                                            int Y = falseTuples0->at(i)->at(1);
                                            const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                                            const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                                            const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                                            if(joiningTupleFound){
                                                const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                                if(it!=tupleToVar.end()){
                                                    reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                                }//closing if
                                            }
                                        }
                                        const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                                        for(int i=0;i<falseTuples1->size();i++){
                                            bool joiningTupleFound=false;
                                            int Y = falseTuples1->at(i)->at(0);
                                            int R = falseTuples1->at(i)->at(1);
                                            const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                                            const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                                            const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                                            for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                                const Tuple * tuple0=NULL;
                                                if(i_0<trueTuples0->size())
                                                    tuple0=trueTuples0->at(i_0);
                                                else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                    tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                                else
                                                    tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                                joiningTupleFound=true;
                                            }
                                            if(joiningTupleFound){
                                                const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                                if(it!=tupleToVar.end()){
                                                    reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
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
                                        if(tuple3!=tupleU){
                                            const auto & it_reason3 = tupleToVar.find(Tuple(*tuple3));
                                            if(it_reason3!=tupleToVar.end())
                                                reason.push_back(it_reason3->second * (tuple3->isNegated() ? -1:1));
                                        }
                                        const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                                        if(it_reason_starter!=tupleToVar.end())
                                            reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                                        if(!(actualSize4>=0+1)){
                                            std::cout<<"conflict detected in propagator1"<<std::endl;
                                            propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                        }else{
                                            if(actualSize4 == 0+1){
                                                const std::vector<const Tuple*>* undefinedTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                                for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                    if(joinTuples_1_4SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 0 && joinTuples_1_4SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 1){
                                                        {
                                                            const Tuple* aggrTupleU = uaux.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                            if(aggrTupleU != NULL){
                                                                const auto & it = tupleToVar.find(*aggrTupleU);
                                                                if(it != tupleToVar.end()) {
                                                                    int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                    auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                                }
                                                            }
                                                        }
                                                        {
                                                            const Tuple* aggrTupleU = umember.find({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
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
                                    const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                                    for(int i=0;i<falseTuples0->size();i++){
                                        bool joiningTupleFound=false;
                                        int R = falseTuples0->at(i)->at(0);
                                        int Y = falseTuples0->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                                        const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                                        const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                                        if(joiningTupleFound){
                                            const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                            if(it!=tupleToVar.end()){
                                                reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                            }//closing if
                                        }
                                    }
                                    const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                                    for(int i=0;i<falseTuples1->size();i++){
                                        bool joiningTupleFound=false;
                                        int Y = falseTuples1->at(i)->at(0);
                                        int R = falseTuples1->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                                        const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                                        const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                                        for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                            const Tuple * tuple0=NULL;
                                            if(i_0<trueTuples0->size())
                                                tuple0=trueTuples0->at(i_0);
                                            else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                            else
                                                tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                            joiningTupleFound=true;
                                        }
                                        if(joiningTupleFound){
                                            const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                            if(it!=tupleToVar.end()){
                                                reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
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
                                    if(tuple3!=tupleU){
                                        const auto & it_reason3 = tupleToVar.find(Tuple(*tuple3));
                                        if(it_reason3!=tupleToVar.end())
                                            reason.push_back(it_reason3->second * (tuple3->isNegated() ? -1:1));
                                    }
                                    const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                                    if(it_reason_starter!=tupleToVar.end())
                                        reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                                        if(!(actualSize4>=0+1)){
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
                    }//close par
                    else{
                        if(joinTuples_1_3.first->size()==0-1){
                            const std::vector<const Tuple* >* tuples;
                            tuples = &pin_.getValues({});
                            const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                            if(tupleU == NULL){
                                tuplesU = &uin_.getValues({});
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
                                int C = (*tuple1)[0];
                                int X = (*tuple1)[1];
                                const Tuple * tuple2 = (wrest.find({C, X, R}));
                                if(!tuple2 && !tupleU ){
                                    tuple2 = tupleU = (urest.find({C, X, R}));
                                    tupleUNegated = false;
                                }
                                if(tuple2){
                                    const Tuple negativeTuple = Tuple({R, 1}, &_cliquesize, true);
                                    const Tuple * tuple3 = &negativeTuple;
                                    bool lTrue = (wcliquesize.find(negativeTuple)!=NULL);
                                    const Tuple * undefTuple = ucliquesize.find(negativeTuple);
                                    if((!lTrue && undefTuple == NULL) || (undefTuple && tupleU == NULL)){
                                        if(undefTuple){
                                            tuple3 = tupleU = undefTuple;
                                            tupleU->print();
                                            tupleUNegated = true;
                                        }
                                        std::pair<AuxMap*,AuxMap*> joinTuples_1_4;
                                        joinTuples_1_4.first = &p_aux_R_Y_member_Y_R_;
                                        joinTuples_1_4.second = &u_aux_R_Y_member_Y_R_;
                                        {
                                            Tuple sharedTuple(std::vector<int>({R,R}));
                                            if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                                                sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                                const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                                const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                                for(int i=0;i<trueJoinTuples->size();i++){
                                                    const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                                    if(findResult!=NULL){
                                                        sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                                                    }
                                                }
                                                for(int i=0;i<undefJoinTuples->size();i++){
                                                    const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                                    if(findResult!=NULL){
                                                        sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                                                    }
                                                }
                                            }
                                        }
                                        std::pair<AuxMap*,AuxMap*> joinTuples_1_4SharedVariables = *sharedVariables_1_ToAggregate_4[std::vector<int>({R,R})];
                                        int actualSize4 = joinTuples_1_4SharedVariables.first->size();
                                        std::set<std::vector<int>> alreadyCounted4;
                                        for(const Tuple * joinTupleUndef : u_aux_R_Y_member_Y_R_0_3_.getValues({R,R})){
                                            if(joinTuples_1_4SharedVariables.first->getValues({joinTupleUndef->at(1)}).size()==0){
                                                if(joinTuples_1_4SharedVariables.second->getValues({joinTupleUndef->at(1)}).size()>0){
                                                    std::vector<int> key({joinTupleUndef->at(1)});
                                                    alreadyCounted4.insert(key);
                                                }
                                            }
                                        }
                                        actualSize4+=alreadyCounted4.size();
                                        if(tupleU==NULL){
                                            std::vector<int> reason;
                                            const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                                            for(int i=0;i<falseTuples0->size();i++){
                                                bool joiningTupleFound=false;
                                                int R = falseTuples0->at(i)->at(0);
                                                int Y = falseTuples0->at(i)->at(1);
                                                const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                                                const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                                                const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                                                if(joiningTupleFound){
                                                    const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                                    if(it!=tupleToVar.end()){
                                                        reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                                    }//closing if
                                                }
                                            }
                                            const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                                            for(int i=0;i<falseTuples1->size();i++){
                                                bool joiningTupleFound=false;
                                                int Y = falseTuples1->at(i)->at(0);
                                                int R = falseTuples1->at(i)->at(1);
                                                const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                                                const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                                                const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                                                for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                                    const Tuple * tuple0=NULL;
                                                    if(i_0<trueTuples0->size())
                                                        tuple0=trueTuples0->at(i_0);
                                                    else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                        tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                                    else
                                                        tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                                    joiningTupleFound=true;
                                                }
                                                if(joiningTupleFound){
                                                    const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                                    if(it!=tupleToVar.end()){
                                                        reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
                                                    }//closing if
                                                }
                                            }
                                            if(!(actualSize4>=0+1)){
                                                if(tupleU ==  NULL) {
                                                    std::pair<AuxMap*,AuxMap*> joinTuples_1_3;
                                                    joinTuples_1_3.first = &p_aux_R_Y_member_Y_R_;
                                                    joinTuples_1_3.second = &u_aux_R_Y_member_Y_R_;
                                                    std::pair<AuxMap*,AuxMap*> joinTuples_1_3SharedVariables = *sharedVariables_1_ToAggregate_3[std::vector<int>({R,R})];
                                                    const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                                    for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                                        Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                                                        const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                                                        if(itaux_0!=tupleToVar.end()){
                                                            reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                                                        }//closing if
                                                        Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                                                        const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                                                        if(itmember_1!=tupleToVar.end()){
                                                            reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
                                                        }//closing if
                                                    }
                                                    const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                                                    if(it_reason1!=tupleToVar.end())
                                                        reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                                                    const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                                    if(it_reason2!=tupleToVar.end())
                                                        reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                                    const auto & it_reason3 = tupleToVar.find(Tuple(*tuple3));
                                                    if(it_reason3!=tupleToVar.end())
                                                        reason.push_back(it_reason3->second * (tuple3->isNegated() ? -1:1));
                                                    const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                                                    if(it_reason_starter!=tupleToVar.end())
                                                        reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                                                    const std::vector<const Tuple*>* undefinedTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                                    for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                        if(joinTuples_1_3SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 0){
                                                            const Tuple* aggrTupleU=NULL;
                                                            const std::vector<const Tuple*>* tuplesaux;
                                                            const std::vector<const Tuple*>* tuplesUaux=&EMPTY_TUPLES;
                                                            tuplesaux= &paux_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                            if(aggrTupleU==NULL)
                                                                tuplesUaux= &uaux_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                            for(int j2=0;j2<tuplesaux->size()+tuplesUaux->size();j2++){
                                                                const Tuple* auxTuple2=NULL;
                                                                bool addedToReason=false;
                                                                if(j2 < tuplesaux->size()){
                                                                    auxTuple2 = tuplesaux->at(j2);
                                                                    const auto & it = tupleToVar.find(*auxTuple2);
                                                                    if(it!=tupleToVar.end()){
                                                                        addedToReason=true;
                                                                        reason.push_back(it->second * (auxTuple2->isNegated() ? -1:1));
                                                                    }//closing if
                                                                    if(tuplesUaux != &EMPTY_TUPLES) {
                                                                    }
                                                                }else{
                                                                    auxTuple2 = tuplesUaux->at(j2-tuplesaux->size());
                                                                    aggrTupleU = auxTuple2;
                                                                }
                                                                const std::vector<const Tuple*>* tuplesmember;
                                                                const std::vector<const Tuple*>* tuplesUmember=&EMPTY_TUPLES;
                                                                tuplesmember= &pmember_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                                if(aggrTupleU==NULL)
                                                                    tuplesUmember= &umember_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                                for(int j4=0;j4<tuplesmember->size()+tuplesUmember->size();j4++){
                                                                    const Tuple* auxTuple4=NULL;
                                                                    bool addedToReason=false;
                                                                    if(j4 < tuplesmember->size()){
                                                                        auxTuple4 = tuplesmember->at(j4);
                                                                        const auto & it = tupleToVar.find(*auxTuple4);
                                                                        if(it!=tupleToVar.end()){
                                                                            addedToReason=true;
                                                                            reason.push_back(it->second * (auxTuple4->isNegated() ? -1:1));
                                                                        }//closing if
                                                                        if(tuplesUmember != &EMPTY_TUPLES) {
                                                                        }
                                                                    }else{
                                                                        auxTuple4 = tuplesUmember->at(j4-tuplesmember->size());
                                                                        aggrTupleU = auxTuple4;
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
                                                                if(addedToReason) reason.pop_back();
                                                            }
                                                        }
                                                    }
                                                }
                                            }//close aggregate if
                                        }//close tupleU if
                                    }//close par
                                }//close par
                            }//close par
                        }//close par
                    }//close par
                }//close par
            }//close par
        }//close if
        if(facts[i] < 0){
            bool tupleUNegated = false;
            const Tuple * tupleU = NULL;
            if(starter.getPredicateName()== &_aux || starter.getPredicateName()== &_member){
                for(const auto sharedVarTuple : sharedVariables_1_ToAggregate_4){
                    tupleU=NULL;
                    int R = sharedVarTuple.first[0];
                    std::pair<AuxMap*,AuxMap*> joinTuples_1_4;
                    joinTuples_1_4.first=sharedVarTuple.second->first;
                    joinTuples_1_4.second=sharedVarTuple.second->second;
                    int actualSize4 = joinTuples_1_4.first->size();
                    std::set<std::vector<int>> alreadyCounted;
                    for(const Tuple * joinTupleUndef : u_aux_R_Y_member_Y_R_0_3_.getValues({R,R})){
                        if(joinTuples_1_4.first->getValues({joinTupleUndef->at(1)}).size()==0){
                            if(joinTuples_1_4.second->getValues({joinTupleUndef->at(1)}).size()>0){
                                std::vector<int> key({joinTupleUndef->at(1)});
                                alreadyCounted.insert(key);
                            }
                        }
                    }
                    actualSize4+=alreadyCounted.size();
                    if(!(actualSize4>=0+1)){
                        std::vector<int> reason;
                        const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                        for(int i=0;i<falseTuples0->size();i++){
                            bool joiningTupleFound=false;
                            int R = falseTuples0->at(i)->at(0);
                            int Y = falseTuples0->at(i)->at(1);
                            const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                            const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                            const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                            if(joiningTupleFound){
                                const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                if(it!=tupleToVar.end()){
                                    reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                }//closing if
                            }
                        }
                        const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                        for(int i=0;i<falseTuples1->size();i++){
                            bool joiningTupleFound=false;
                            int Y = falseTuples1->at(i)->at(0);
                            int R = falseTuples1->at(i)->at(1);
                            const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                            const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                            const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                            for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                const Tuple * tuple0=NULL;
                                if(i_0<trueTuples0->size())
                                    tuple0=trueTuples0->at(i_0);
                                else if(i_0<trueTuples0->size()+undefTuples0->size())
                                    tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                else
                                    tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                joiningTupleFound=true;
                            }
                            if(joiningTupleFound){
                                const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                if(it!=tupleToVar.end()){
                                    reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
                                }//closing if
                            }
                        }
                        const std::vector<const Tuple* >* tuples;
                        tuples = &pin_.getValues({});
                        const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                        if(tupleU == NULL){
                            tuplesU = &uin_.getValues({});
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
                            int C = (*tuple1)[0];
                            int X = (*tuple1)[1];
                            const Tuple * tuple2 = (wrest.find({C, X, R}));
                            if(!tuple2 && !tupleU ){
                                tuple2 = tupleU = (urest.find({C, X, R}));
                                tupleUNegated = false;
                            }
                            if(tuple2){
                                const Tuple negativeTuple = Tuple({R, 1}, &_cliquesize, true);
                                const Tuple * tuple3 = &negativeTuple;
                                bool lTrue = (wcliquesize.find(negativeTuple)!=NULL);
                                const Tuple * undefTuple = ucliquesize.find(negativeTuple);
                                if((!lTrue && undefTuple == NULL) || (undefTuple && tupleU == NULL)){
                                    if(undefTuple){
                                        tuple3 = tupleU = undefTuple;
                                        tupleU->print();
                                        tupleUNegated = true;
                                    }
                                    std::pair<AuxMap*,AuxMap*> joinTuples_1_3;
                                    joinTuples_1_3.first = &p_aux_R_Y_member_Y_R_;
                                    joinTuples_1_3.second = &u_aux_R_Y_member_Y_R_;
                                    {
                                        Tuple sharedTuple(std::vector<int>({R,R}));
                                        if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                                            sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                            const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            for(int i=0;i<trueJoinTuples->size();i++){
                                                const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                                if(findResult!=NULL){
                                                    sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                                                }
                                            }
                                            for(int i=0;i<undefJoinTuples->size();i++){
                                                const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                                if(findResult!=NULL){
                                                    sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                                                }
                                            }
                                        }
                                    }
                                    std::pair<AuxMap*,AuxMap*> joinTuples_1_3SharedVariables = *sharedVariables_1_ToAggregate_3[std::vector<int>({R,R})];
                                    if(tupleU==NULL){
                                        const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                        for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                            Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                                            const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                                            if(itaux_0!=tupleToVar.end()){
                                                reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                                            }//closing if
                                            Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                                            const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                                            if(itmember_1!=tupleToVar.end()){
                                                reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
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
                                        if(tuple3!=tupleU){
                                            const auto & it_reason3 = tupleToVar.find(Tuple(*tuple3));
                                            if(it_reason3!=tupleToVar.end())
                                                reason.push_back(it_reason3->second * (tuple3->isNegated() ? -1:1));
                                        }
                                        const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                                        if(it_reason_starter!=tupleToVar.end())
                                            reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                                        if(joinTuples_1_3SharedVariables.first->size()>=0){
                                            std::cout<<"conflict detected in propagator1"<<std::endl;
                                            propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                        }else{
                                            if(joinTuples_1_3SharedVariables.first->size() == 0-1){
                                                const std::vector<const Tuple*>* undefinedTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                                for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                    if(joinTuples_1_3SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 0){
                                                        const Tuple* aggrTupleU=NULL;
                                                        const std::vector<const Tuple*>* tuplesaux;
                                                        const std::vector<const Tuple*>* tuplesUaux=&EMPTY_TUPLES;
                                                        tuplesaux= &paux_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                        if(aggrTupleU==NULL)
                                                            tuplesUaux= &uaux_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                        for(int j2=0;j2<tuplesaux->size()+tuplesUaux->size();j2++){
                                                            const Tuple* auxTuple2=NULL;
                                                            bool addedToReason=false;
                                                            if(j2 < tuplesaux->size()){
                                                                auxTuple2 = tuplesaux->at(j2);
                                                                const auto & it = tupleToVar.find(*auxTuple2);
                                                                if(it!=tupleToVar.end()){
                                                                    addedToReason=true;
                                                                    reason.push_back(it->second * (auxTuple2->isNegated() ? -1:1));
                                                                }//closing if
                                                                if(tuplesUaux != &EMPTY_TUPLES) {
                                                                }
                                                            }else{
                                                                auxTuple2 = tuplesUaux->at(j2-tuplesaux->size());
                                                                aggrTupleU = auxTuple2;
                                                            }
                                                            const std::vector<const Tuple*>* tuplesmember;
                                                            const std::vector<const Tuple*>* tuplesUmember=&EMPTY_TUPLES;
                                                            tuplesmember= &pmember_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                            if(aggrTupleU==NULL)
                                                                tuplesUmember= &umember_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                            for(int j4=0;j4<tuplesmember->size()+tuplesUmember->size();j4++){
                                                                const Tuple* auxTuple4=NULL;
                                                                bool addedToReason=false;
                                                                if(j4 < tuplesmember->size()){
                                                                    auxTuple4 = tuplesmember->at(j4);
                                                                    const auto & it = tupleToVar.find(*auxTuple4);
                                                                    if(it!=tupleToVar.end()){
                                                                        addedToReason=true;
                                                                        reason.push_back(it->second * (auxTuple4->isNegated() ? -1:1));
                                                                    }//closing if
                                                                    if(tuplesUmember != &EMPTY_TUPLES) {
                                                                    }
                                                                }else{
                                                                    auxTuple4 = tuplesUmember->at(j4-tuplesmember->size());
                                                                    aggrTupleU = auxTuple4;
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
                                                            if(addedToReason) reason.pop_back();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }else{
                                    const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                        Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                                        const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                                        if(itaux_0!=tupleToVar.end()){
                                            reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                                        }//closing if
                                        Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                                        const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                                        if(itmember_1!=tupleToVar.end()){
                                            reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
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
                                    if(tuple3!=tupleU){
                                        const auto & it_reason3 = tupleToVar.find(Tuple(*tuple3));
                                        if(it_reason3!=tupleToVar.end())
                                            reason.push_back(it_reason3->second * (tuple3->isNegated() ? -1:1));
                                    }
                                    const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                                    if(it_reason_starter!=tupleToVar.end())
                                        reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                                        if(joinTuples_1_3SharedVariables.first->size()>=0){
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
                    }//close par
                    else{
                        if(actualSize4==0+1){
                            const std::vector<const Tuple* >* tuples;
                            tuples = &pin_.getValues({});
                            const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                            if(tupleU == NULL){
                                tuplesU = &uin_.getValues({});
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
                                int C = (*tuple1)[0];
                                int X = (*tuple1)[1];
                                const Tuple * tuple2 = (wrest.find({C, X, R}));
                                if(!tuple2 && !tupleU ){
                                    tuple2 = tupleU = (urest.find({C, X, R}));
                                    tupleUNegated = false;
                                }
                                if(tuple2){
                                    const Tuple negativeTuple = Tuple({R, 1}, &_cliquesize, true);
                                    const Tuple * tuple3 = &negativeTuple;
                                    bool lTrue = (wcliquesize.find(negativeTuple)!=NULL);
                                    const Tuple * undefTuple = ucliquesize.find(negativeTuple);
                                    if((!lTrue && undefTuple == NULL) || (undefTuple && tupleU == NULL)){
                                        if(undefTuple){
                                            tuple3 = tupleU = undefTuple;
                                            tupleU->print();
                                            tupleUNegated = true;
                                        }
                                        std::pair<AuxMap*,AuxMap*> joinTuples_1_3;
                                        joinTuples_1_3.first = &p_aux_R_Y_member_Y_R_;
                                        joinTuples_1_3.second = &u_aux_R_Y_member_Y_R_;
                                        {
                                            Tuple sharedTuple(std::vector<int>({R,R}));
                                            if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                                                sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                                const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                                const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                                for(int i=0;i<trueJoinTuples->size();i++){
                                                    const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                                    if(findResult!=NULL){
                                                        sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                                                    }
                                                }
                                                for(int i=0;i<undefJoinTuples->size();i++){
                                                    const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                                    if(findResult!=NULL){
                                                        sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                                                    }
                                                }
                                            }
                                        }
                                        std::pair<AuxMap*,AuxMap*> joinTuples_1_3SharedVariables = *sharedVariables_1_ToAggregate_3[std::vector<int>({R,R})];
                                        if(tupleU==NULL){
                                            const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            std::vector<int> reason;
                                            for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                                Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                                                const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                                                if(itaux_0!=tupleToVar.end()){
                                                    reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                                                }//closing if
                                                Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                                                const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                                                if(itmember_1!=tupleToVar.end()){
                                                    reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
                                                }//closing if
                                            }
                                            if(joinTuples_1_3SharedVariables.first->size()>=0){
                                                if(tupleU ==  NULL) {
                                                    std::pair<AuxMap*,AuxMap*> joinTuples_1_4;
                                                    joinTuples_1_4.first = &p_aux_R_Y_member_Y_R_;
                                                    joinTuples_1_4.second = &u_aux_R_Y_member_Y_R_;
                                                    std::pair<AuxMap*,AuxMap*> joinTuples_1_4SharedVariables = *sharedVariables_1_ToAggregate_4[std::vector<int>({R,R})];
                                                    const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                                                    for(int i=0;i<falseTuples0->size();i++){
                                                        bool joiningTupleFound=false;
                                                        int R = falseTuples0->at(i)->at(0);
                                                        int Y = falseTuples0->at(i)->at(1);
                                                        const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                                                        const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                                                        const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                                                        if(joiningTupleFound){
                                                            const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                                            if(it!=tupleToVar.end()){
                                                                reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                                            }//closing if
                                                        }
                                                    }
                                                    const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                                                    for(int i=0;i<falseTuples1->size();i++){
                                                        bool joiningTupleFound=false;
                                                        int Y = falseTuples1->at(i)->at(0);
                                                        int R = falseTuples1->at(i)->at(1);
                                                        const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                                                        const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                                                        const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                                                        for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                                            const Tuple * tuple0=NULL;
                                                            if(i_0<trueTuples0->size())
                                                                tuple0=trueTuples0->at(i_0);
                                                            else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                                tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                                            else
                                                                tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                                            joiningTupleFound=true;
                                                        }
                                                        if(joiningTupleFound){
                                                            const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                                            if(it!=tupleToVar.end()){
                                                                reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
                                                            }//closing if
                                                        }
                                                    }
                                                    const auto & it_reason1 = tupleToVar.find(Tuple(*tuple1));
                                                    if(it_reason1!=tupleToVar.end())
                                                        reason.push_back(it_reason1->second * (tuple1->isNegated() ? -1:1));
                                                    const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                                    if(it_reason2!=tupleToVar.end())
                                                        reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                                    const auto & it_reason3 = tupleToVar.find(Tuple(*tuple3));
                                                    if(it_reason3!=tupleToVar.end())
                                                        reason.push_back(it_reason3->second * (tuple3->isNegated() ? -1:1));
                                                    const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                                                    if(it_reason_starter!=tupleToVar.end())
                                                        reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                                                    const std::vector<const Tuple*>* undefinedTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                                    for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                        if(joinTuples_1_4SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 0 && joinTuples_1_4SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 1){
                                                            {
                                                                const Tuple* aggrTupleU = uaux.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                                if(aggrTupleU != NULL){
                                                                    const auto & it = tupleToVar.find(*aggrTupleU);
                                                                    if(it != tupleToVar.end()) {
                                                                        int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                        auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                                    }
                                                                }
                                                            }
                                                            {
                                                                const Tuple* aggrTupleU = umember.find({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
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
                                            }//close aggregate if
                                        }//close tupleU if
                                    }//close par
                                }//close par
                            }//close par
                        }//close par
                    }//close par
                }//close par
            }//close par
        }//close if
        if(starter.getPredicateName() == &_in) { 
            const Tuple * tuple0 = &starter;
            if(facts[i] > 0){
                {
                    const Tuple * tupleU = NULL;
                    bool tupleUNegated = false;
                    int C = (*tuple0)[0];
                    int X = (*tuple0)[1];
                    const std::vector<const Tuple* >* tuples;
                    tuples = &prest_0_1_.getValues({C, X});
                    const std::vector<const Tuple* >* tuplesU = &EMPTY_TUPLES;
                    if(tupleU == NULL){
                        tuplesU = &urest_0_1_.getValues({C, X});
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
                        int R = (*tuple1)[2];
                        const Tuple negativeTuple = Tuple({R, 1}, &_cliquesize, true);
                        const Tuple * tuple2 = &negativeTuple;
                        bool lTrue = (wcliquesize.find(negativeTuple)!=NULL);
                        const Tuple * undefTuple = ucliquesize.find(negativeTuple);
                        if((!lTrue && undefTuple == NULL) || (undefTuple && tupleU == NULL)){
                            if(undefTuple){
                                tuple2 = tupleU = undefTuple;
                                tupleU->print();
                                tupleUNegated = true;
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_1_3;
                            joinTuples_1_3.first = &p_aux_R_Y_member_Y_R_;
                            joinTuples_1_3.second = &u_aux_R_Y_member_Y_R_;
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                                    sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                    const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    for(int i=0;i<trueJoinTuples->size();i++){
                                        const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                        if(findResult!=NULL){
                                            sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                                        }
                                    }
                                    for(int i=0;i<undefJoinTuples->size();i++){
                                        const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                        if(findResult!=NULL){
                                            sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                                        }
                                    }
                                }
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_1_3SharedVariables = *sharedVariables_1_ToAggregate_3[std::vector<int>({R,R})];
                            if(tupleU==NULL){
                                const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                std::vector<int> reason;
                                for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                    Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                                    const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                                    if(itaux_0!=tupleToVar.end()){
                                        reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                                    }//closing if
                                    Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                                    const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                                    if(itmember_1!=tupleToVar.end()){
                                        reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
                                    }//closing if
                                }
                                if(joinTuples_1_3SharedVariables.first->size()>=0){
                                    std::pair<AuxMap*,AuxMap*> joinTuples_1_4;
                                    joinTuples_1_4.first = &p_aux_R_Y_member_Y_R_;
                                    joinTuples_1_4.second = &u_aux_R_Y_member_Y_R_;
                                    {
                                        Tuple sharedTuple(std::vector<int>({R,R}));
                                        if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                                            sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                            const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            for(int i=0;i<trueJoinTuples->size();i++){
                                                const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                                if(findResult!=NULL){
                                                    sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                                                }
                                            }
                                            for(int i=0;i<undefJoinTuples->size();i++){
                                                const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                                if(findResult!=NULL){
                                                    sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                                                }
                                            }
                                        }
                                    }
                                    std::pair<AuxMap*,AuxMap*> joinTuples_1_4SharedVariables = *sharedVariables_1_ToAggregate_4[std::vector<int>({R,R})];
                                    int actualSize4 = joinTuples_1_4SharedVariables.first->size();
                                    std::set<std::vector<int>> alreadyCounted4;
                                    for(const Tuple * joinTupleUndef : u_aux_R_Y_member_Y_R_0_3_.getValues({R,R})){
                                        if(joinTuples_1_4SharedVariables.first->getValues({joinTupleUndef->at(1)}).size()==0){
                                            if(joinTuples_1_4SharedVariables.second->getValues({joinTupleUndef->at(1)}).size()>0){
                                                std::vector<int> key({joinTupleUndef->at(1)});
                                                alreadyCounted4.insert(key);
                                            }
                                        }
                                    }
                                    actualSize4+=alreadyCounted4.size();
                                    const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                                    for(int i=0;i<falseTuples0->size();i++){
                                        bool joiningTupleFound=false;
                                        int R = falseTuples0->at(i)->at(0);
                                        int Y = falseTuples0->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                                        const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                                        const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                                        if(joiningTupleFound){
                                            const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                            if(it!=tupleToVar.end()){
                                                reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                            }//closing if
                                        }
                                    }
                                    const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                                    for(int i=0;i<falseTuples1->size();i++){
                                        bool joiningTupleFound=false;
                                        int Y = falseTuples1->at(i)->at(0);
                                        int R = falseTuples1->at(i)->at(1);
                                        const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                                        const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                                        const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                                        for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                            const Tuple * tuple0=NULL;
                                            if(i_0<trueTuples0->size())
                                                tuple0=trueTuples0->at(i_0);
                                            else if(i_0<trueTuples0->size()+undefTuples0->size())
                                                tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                            else
                                                tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                            joiningTupleFound=true;
                                        }
                                        if(joiningTupleFound){
                                            const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                            if(it!=tupleToVar.end()){
                                                reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
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
                                    if(tuple2!=tupleU){
                                        const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                        if(it_reason2!=tupleToVar.end())
                                            reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                    }
                                    const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                                    if(it_reason_starter!=tupleToVar.end())
                                        reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                                    if(!(actualSize4>=0+1)){
                                        std::cout<<"conflict detected in propagatoron last aggregate starting from literal1"<<std::endl;
                                        propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                    }else{
                                        if(actualSize4 == 0+1){
                                            const std::vector<const Tuple*>* undefinedTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                if(joinTuples_1_4SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 0 && joinTuples_1_4SharedVariables.second->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 1){
                                                    {
                                                        const Tuple* aggrTupleU = uaux.find({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                        if(aggrTupleU != NULL){
                                                            const auto & it = tupleToVar.find(*aggrTupleU);
                                                            if(it != tupleToVar.end()) {
                                                                int sign = aggrTupleU->isNegated() ? 1:-1;
                                                                auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                                            }
                                                        }
                                                    }
                                                    {
                                                        const Tuple* aggrTupleU = umember.find({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
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
                                }//close ext aggregate if
                            }else{
                            std::vector<int> reason;
                            const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                            for(int i=0;i<falseTuples0->size();i++){
                                bool joiningTupleFound=false;
                                int R = falseTuples0->at(i)->at(0);
                                int Y = falseTuples0->at(i)->at(1);
                                const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                                const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                                const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                                if(joiningTupleFound){
                                    const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                    if(it!=tupleToVar.end()){
                                        reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                    }//closing if
                                }
                            }
                            const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                            for(int i=0;i<falseTuples1->size();i++){
                                bool joiningTupleFound=false;
                                int Y = falseTuples1->at(i)->at(0);
                                int R = falseTuples1->at(i)->at(1);
                                const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                                const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                                const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                                for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                    const Tuple * tuple0=NULL;
                                    if(i_0<trueTuples0->size())
                                        tuple0=trueTuples0->at(i_0);
                                    else if(i_0<trueTuples0->size()+undefTuples0->size())
                                        tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                    else
                                        tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                    joiningTupleFound=true;
                                }
                                if(joiningTupleFound){
                                    const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                    if(it!=tupleToVar.end()){
                                        reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
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
                            if(tuple2!=tupleU){
                                const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                if(it_reason2!=tupleToVar.end())
                                    reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                            }
                            const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                            for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                                const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                                if(itaux_0!=tupleToVar.end()){
                                    reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                                }//closing if
                                Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                                const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                                if(itmember_1!=tupleToVar.end()){
                                    reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
                                }//closing if
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_1_4;
                            joinTuples_1_4.first = &p_aux_R_Y_member_Y_R_;
                            joinTuples_1_4.second = &u_aux_R_Y_member_Y_R_;
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                                    sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                    const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    for(int i=0;i<trueJoinTuples->size();i++){
                                        const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                        if(findResult!=NULL){
                                            sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                                        }
                                    }
                                    for(int i=0;i<undefJoinTuples->size();i++){
                                        const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                        if(findResult!=NULL){
                                            sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                                        }
                                    }
                                }
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_1_4SharedVariables = *sharedVariables_1_ToAggregate_4[std::vector<int>({R,R})];
                            int actualSize4 = joinTuples_1_4SharedVariables.first->size();
                            std::set<std::vector<int>> alreadyCounted4;
                            for(const Tuple * joinTupleUndef : u_aux_R_Y_member_Y_R_0_3_.getValues({R,R})){
                                if(joinTuples_1_4SharedVariables.first->getValues({joinTupleUndef->at(1)}).size()==0){
                                    if(joinTuples_1_4SharedVariables.second->getValues({joinTupleUndef->at(1)}).size()>0){
                                        std::vector<int> key({joinTupleUndef->at(1)});
                                        alreadyCounted4.insert(key);
                                    }
                                }
                            }
                            actualSize4+=alreadyCounted4.size();
                                if(!(actualSize4>=0+1) && (joinTuples_1_3SharedVariables.first->size()>=0)){
                                    const auto & it = tupleToVar.find(*tupleU);
                                    if(it != tupleToVar.end()) {
                                        int sign = tupleU->isNegated() ? -1 : 1;
                                        auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
                                    }
                                }
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_1_4;
                            joinTuples_1_4.first = &p_aux_R_Y_member_Y_R_;
                            joinTuples_1_4.second = &u_aux_R_Y_member_Y_R_;
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(!sharedVariables_1_ToAggregate_4.count(sharedTuple)){
                                    sharedVariables_1_ToAggregate_4[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                    const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    for(int i=0;i<trueJoinTuples->size();i++){
                                        const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                        if(findResult!=NULL){
                                            sharedVariables_1_ToAggregate_4[sharedTuple]->first->insert2(*findResult);
                                        }
                                    }
                                    for(int i=0;i<undefJoinTuples->size();i++){
                                        const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                        if(findResult!=NULL){
                                            sharedVariables_1_ToAggregate_4[sharedTuple]->second->insert2(*findResult);
                                        }
                                    }
                                }
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_1_4SharedVariables = *sharedVariables_1_ToAggregate_4[std::vector<int>({R,R})];
                            int actualSize4 = joinTuples_1_4SharedVariables.first->size();
                            std::set<std::vector<int>> alreadyCounted4;
                            for(const Tuple * joinTupleUndef : u_aux_R_Y_member_Y_R_0_3_.getValues({R,R})){
                                if(joinTuples_1_4SharedVariables.first->getValues({joinTupleUndef->at(1)}).size()==0){
                                    if(joinTuples_1_4SharedVariables.second->getValues({joinTupleUndef->at(1)}).size()>0){
                                        std::vector<int> key({joinTupleUndef->at(1)});
                                        alreadyCounted4.insert(key);
                                    }
                                }
                            }
                            actualSize4+=alreadyCounted4.size();
                            if(tupleU==NULL){
                                std::vector<int> reason;
                                const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                                for(int i=0;i<falseTuples0->size();i++){
                                    bool joiningTupleFound=false;
                                    int R = falseTuples0->at(i)->at(0);
                                    int Y = falseTuples0->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                                    const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                                    const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                                    if(joiningTupleFound){
                                        const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                        if(it!=tupleToVar.end()){
                                            reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                        }//closing if
                                    }
                                }
                                const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                                for(int i=0;i<falseTuples1->size();i++){
                                    bool joiningTupleFound=false;
                                    int Y = falseTuples1->at(i)->at(0);
                                    int R = falseTuples1->at(i)->at(1);
                                    const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                                    const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                                    const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                                    for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                        const Tuple * tuple0=NULL;
                                        if(i_0<trueTuples0->size())
                                            tuple0=trueTuples0->at(i_0);
                                        else if(i_0<trueTuples0->size()+undefTuples0->size())
                                            tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                        else
                                            tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                        joiningTupleFound=true;
                                    }
                                    if(joiningTupleFound){
                                        const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                        if(it!=tupleToVar.end()){
                                            reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
                                        }//closing if
                                    }
                                }
                                if(!(actualSize4>=0+1)){
                                    std::pair<AuxMap*,AuxMap*> joinTuples_1_3;
                                    joinTuples_1_3.first = &p_aux_R_Y_member_Y_R_;
                                    joinTuples_1_3.second = &u_aux_R_Y_member_Y_R_;
                                    {
                                        Tuple sharedTuple(std::vector<int>({R,R}));
                                        if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                                            sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                            const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            for(int i=0;i<trueJoinTuples->size();i++){
                                                const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                                if(findResult!=NULL){
                                                    sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                                                }
                                            }
                                            for(int i=0;i<undefJoinTuples->size();i++){
                                                const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                                if(findResult!=NULL){
                                                    sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                                                }
                                            }
                                        }
                                    }
                                    std::pair<AuxMap*,AuxMap*> joinTuples_1_3SharedVariables = *sharedVariables_1_ToAggregate_3[std::vector<int>({R,R})];
                                    const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                        Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                                        const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                                        if(itaux_0!=tupleToVar.end()){
                                            reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                                        }//closing if
                                        Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                                        const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                                        if(itmember_1!=tupleToVar.end()){
                                            reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
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
                                    if(tuple2!=tupleU){
                                        const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                        if(it_reason2!=tupleToVar.end())
                                            reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                                    }
                                    const auto & it_reason_starter = tupleToVar.find(Tuple(starter));
                                    if(it_reason_starter!=tupleToVar.end())
                                        reason.push_back(it_reason_starter->second * (starter.isNegated() ? -1:1));
                                    if(joinTuples_1_3SharedVariables.first->size()>=0){
                                        std::cout<<"conflict detected in propagatoron last aggregate starting from literal1"<<std::endl;
                                        propagatedLiteralsAndReasons.insert({-1, std::vector<int>(reason)});
                                    }else{
                                        if(joinTuples_1_3SharedVariables.first->size() == 0-1){
                                            const std::vector<const Tuple*>* undefinedTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                            for(int iUndef=0;iUndef<undefinedTuples->size();iUndef++){
                                                if(joinTuples_1_3SharedVariables.first->getValues({undefinedTuples->at(iUndef)->at(1)}).size() == 0){
                                                    const Tuple* aggrTupleU=NULL;
                                                    const std::vector<const Tuple*>* tuplesaux;
                                                    const std::vector<const Tuple*>* tuplesUaux=&EMPTY_TUPLES;
                                                    tuplesaux= &paux_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                    if(aggrTupleU==NULL)
                                                        tuplesUaux= &uaux_0_1_.getValues({undefinedTuples->at(iUndef)->at(0),undefinedTuples->at(iUndef)->at(1)});
                                                    for(int j2=0;j2<tuplesaux->size()+tuplesUaux->size();j2++){
                                                        const Tuple* auxTuple2=NULL;
                                                        bool addedToReason=false;
                                                        if(j2 < tuplesaux->size()){
                                                            auxTuple2 = tuplesaux->at(j2);
                                                            const auto & it = tupleToVar.find(*auxTuple2);
                                                            if(it!=tupleToVar.end()){
                                                                addedToReason=true;
                                                                reason.push_back(it->second * (auxTuple2->isNegated() ? -1:1));
                                                            }//closing if
                                                            if(tuplesUaux != &EMPTY_TUPLES) {
                                                            }
                                                        }else{
                                                            auxTuple2 = tuplesUaux->at(j2-tuplesaux->size());
                                                            aggrTupleU = auxTuple2;
                                                        }
                                                        const std::vector<const Tuple*>* tuplesmember;
                                                        const std::vector<const Tuple*>* tuplesUmember=&EMPTY_TUPLES;
                                                        tuplesmember= &pmember_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                        if(aggrTupleU==NULL)
                                                            tuplesUmember= &umember_0_1_.getValues({undefinedTuples->at(iUndef)->at(2),undefinedTuples->at(iUndef)->at(3)});
                                                        for(int j4=0;j4<tuplesmember->size()+tuplesUmember->size();j4++){
                                                            const Tuple* auxTuple4=NULL;
                                                            bool addedToReason=false;
                                                            if(j4 < tuplesmember->size()){
                                                                auxTuple4 = tuplesmember->at(j4);
                                                                const auto & it = tupleToVar.find(*auxTuple4);
                                                                if(it!=tupleToVar.end()){
                                                                    addedToReason=true;
                                                                    reason.push_back(it->second * (auxTuple4->isNegated() ? -1:1));
                                                                }//closing if
                                                                if(tuplesUmember != &EMPTY_TUPLES) {
                                                                }
                                                            }else{
                                                                auxTuple4 = tuplesUmember->at(j4-tuplesmember->size());
                                                                aggrTupleU = auxTuple4;
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
                                                        if(addedToReason) reason.pop_back();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }//close ext aggregate if
                            }else{
                            const std::vector<const Tuple*>* trueTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                            std::vector<int> reason;
                            for(int reasonIndex=0;reasonIndex<trueTuples->size();reasonIndex++){
                                Tuple tupleaux_0(std::vector<int>({trueTuples->at(reasonIndex)->at(0),trueTuples->at(reasonIndex)->at(1)}),&_aux);
                                const auto & itaux_0 = tupleToVar.find(tupleaux_0);
                                if(itaux_0!=tupleToVar.end()){
                                    reason.push_back(itaux_0->second * (tupleaux_0.isNegated() ? -1:1));
                                }//closing if
                                Tuple tuplemember_1(std::vector<int>({trueTuples->at(reasonIndex)->at(2),trueTuples->at(reasonIndex)->at(3)}),&_member);
                                const auto & itmember_1 = tupleToVar.find(tuplemember_1);
                                if(itmember_1!=tupleToVar.end()){
                                    reason.push_back(itmember_1->second * (tuplemember_1.isNegated() ? -1:1));
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
                            if(tuple2!=tupleU){
                                const auto & it_reason2 = tupleToVar.find(Tuple(*tuple2));
                                if(it_reason2!=tupleToVar.end())
                                    reason.push_back(it_reason2->second * (tuple2->isNegated() ? -1:1));
                            }
                            const std::vector<const Tuple*>* falseTuples0 = &faux_.getValues({});
                            for(int i=0;i<falseTuples0->size();i++){
                                bool joiningTupleFound=false;
                                int R = falseTuples0->at(i)->at(0);
                                int Y = falseTuples0->at(i)->at(1);
                                const std::vector<const Tuple*>* trueTuples1 = &pmember_0_1_.getValues({Y, R});
                                const std::vector<const Tuple*>* undefTuples1 = &umember_0_1_.getValues({Y, R});
                                const std::vector<const Tuple*>* falseTuples1 = &fmember_0_1_.getValues({Y, R});
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
                                if(joiningTupleFound){
                                    const auto & it = tupleToVar.find(*falseTuples0->at(i));
                                    if(it!=tupleToVar.end()){
                                        reason.push_back(it->second * (falseTuples0->at(i)->isNegated() ? 1:-1));
                                    }//closing if
                                }
                            }
                            const std::vector<const Tuple*>* falseTuples1 = &fmember_.getValues({});
                            for(int i=0;i<falseTuples1->size();i++){
                                bool joiningTupleFound=false;
                                int Y = falseTuples1->at(i)->at(0);
                                int R = falseTuples1->at(i)->at(1);
                                const std::vector<const Tuple*>* trueTuples0 = &paux_0_1_.getValues({R, Y});
                                const std::vector<const Tuple*>* undefTuples0 = &uaux_0_1_.getValues({R, Y});
                                const std::vector<const Tuple*>* falseTuples0 = &faux_0_1_.getValues({R, Y});
                                for(int i_0=0;!joiningTupleFound && i_0 < trueTuples0->size()+undefTuples0->size()+falseTuples0->size();i_0++){
                                    const Tuple * tuple0=NULL;
                                    if(i_0<trueTuples0->size())
                                        tuple0=trueTuples0->at(i_0);
                                    else if(i_0<trueTuples0->size()+undefTuples0->size())
                                        tuple0=undefTuples0->at(i_0-trueTuples0->size());
                                    else
                                        tuple0=falseTuples0->at(i_0-trueTuples0->size()-undefTuples0->size());
                                    joiningTupleFound=true;
                                }
                                if(joiningTupleFound){
                                    const auto & it = tupleToVar.find(*falseTuples1->at(i));
                                    if(it!=tupleToVar.end()){
                                        reason.push_back(it->second * (falseTuples1->at(i)->isNegated() ? 1:-1));
                                    }//closing if
                                }
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_1_3;
                            joinTuples_1_3.first = &p_aux_R_Y_member_Y_R_;
                            joinTuples_1_3.second = &u_aux_R_Y_member_Y_R_;
                            {
                                Tuple sharedTuple(std::vector<int>({R,R}));
                                if(!sharedVariables_1_ToAggregate_3.count(sharedTuple)){
                                    sharedVariables_1_ToAggregate_3[sharedTuple] = new std::pair<AuxMap*,AuxMap*>(new AuxMap(std::vector<unsigned>({1})),new AuxMap(std::vector<unsigned>({1})));
                                    const std::vector<const Tuple*>* trueJoinTuples = &p_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    const std::vector<const Tuple*>* undefJoinTuples = &u_aux_R_Y_member_Y_R_0_3_.getValues({R,R});
                                    for(int i=0;i<trueJoinTuples->size();i++){
                                        const auto findResult = waux_R_Y_member_Y_R_.find(Tuple(*trueJoinTuples->at(i)));
                                        if(findResult!=NULL){
                                            sharedVariables_1_ToAggregate_3[sharedTuple]->first->insert2(*findResult);
                                        }
                                    }
                                    for(int i=0;i<undefJoinTuples->size();i++){
                                        const auto findResult = uaux_R_Y_member_Y_R_.find(Tuple(*undefJoinTuples->at(i)));
                                        if(findResult!=NULL){
                                            sharedVariables_1_ToAggregate_3[sharedTuple]->second->insert2(*findResult);
                                        }
                                    }
                                }
                            }
                            std::pair<AuxMap*,AuxMap*> joinTuples_1_3SharedVariables = *sharedVariables_1_ToAggregate_3[std::vector<int>({R,R})];
                                if(joinTuples_1_3SharedVariables.first->size()>=0 && (!(actualSize4>=0+1))){
                                    const auto & it = tupleToVar.find(*tupleU);
                                    if(it != tupleToVar.end()) {
                                        int sign = tupleU->isNegated() ? -1 : 1;
                                        auto & reas = propagatedLiteralsAndReasons.insert({it->second*sign, std::vector<int>(reason)}).first->second;
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
