#include "Executor.h"

#include "utils/ConstantsManager.h"

#include "DLV2libs/input/InputDirector.h"

#include "parsing/AspCore2InstanceBuilder.h"

extern "C" Executor* create_object() {
    return new Executor;
}
extern "C" void destroy_object( Executor* object ) {
    delete object;
}
Executor::~Executor() {}

Executor::Executor() {}

typedef vector<const Tuple* > Tuples;
typedef vector<vector<const Tuple* > >Reasons;
typedef vector<unordered_set<Tuple, TupleHash> >NegativeReasons;
void printTuples(const string & predicateName, const Tuples & tuples) {
    for (const vector<unsigned> * tuple : tuples) {
        cout <<predicateName<< "(";
        for (unsigned i = 0; i < tuple->size(); i++) {
            if (i > 0) {
                cout << ",";
            }
            cout << ConstantsManager::getInstance().unmapConstant((*tuple)[i]);
        }
        cout << ").\n";
    }
}
void Executor::executeFromFile(const char* filename) {
    DLV2::InputDirector director;
    AspCore2InstanceBuilder* builder = new AspCore2InstanceBuilder();
    director.configureBuilder(builder);
    vector<const char*> fileNames;
    fileNames.push_back(filename);
    director.parse(fileNames);
    executeProgramOnFacts(builder->getProblemInstance());
    delete builder;
}

using PredicateWSet = std::unordered_set<Tuple, TupleHash>;

void addReasons(const Tuple * tuple, vector<const Tuple*> & outputReasons, const map<string, Reasons* > & predicateReasonsMap, const map<string, NegativeReasons* > & predicateNegativeReasonsMap) {
    const std::vector<const Tuple*> & tupleReasons = predicateReasonsMap.at(tuple->getPredicateName())->at(tuple->getId());
     if (tupleReasons.empty()) {
        outputReasons.push_back(tuple);
    }
    else {
        for (const Tuple * reason : tupleReasons) {
            addReasons(reason, outputReasons, predicateReasonsMap, predicateNegativeReasonsMap);
        }
    }
    const std::unordered_set<Tuple, TupleHash> & tupleNegativeReasons = predicateNegativeReasonsMap.at(tuple->getPredicateName())->at(tuple->getId());
    for (const Tuple & reason : tupleNegativeReasons) {
        outputReasons.push_back(&reason);
    }
}

aspc::Literal tupleToLiteral(const Tuple & tuple) {
    aspc::Literal literal(tuple.getPredicateName(), tuple.isNegated());
    for (unsigned v : tuple) {
        literal.addTerm(ConstantsManager::getInstance().unmapConstant(v));
    }
    return literal;
}
void Executor::executeProgramOnFacts(const vector<aspc::Atom*> & program) {
    const string cl = "cl";
    const string set = "set";
    failedConstraints.clear();
    map<string, PredicateWSet*> predicateWSetMap;
    map<string, Tuples* > predicateTuplesMap;
    map<string, Reasons* > predicateReasonsMap;
    map<string, NegativeReasons* > predicateNegativeReasonsMap;
    Tuples tuples_cl;
    Reasons reasons_cl;
    NegativeReasons negativeReasons_cl;
    PredicateWSet wcl;
    predicateWSetMap[cl]=&wcl;
    predicateTuplesMap[cl]=&tuples_cl;
    predicateReasonsMap[cl]=&reasons_cl;
    predicateNegativeReasonsMap[cl]=&negativeReasons_cl;
    Tuples tuples_set;
    Reasons reasons_set;
    NegativeReasons negativeReasons_set;
    PredicateWSet wset;
    predicateWSetMap[set]=&wset;
    predicateTuplesMap[set]=&tuples_set;
    predicateReasonsMap[set]=&reasons_set;
    predicateNegativeReasonsMap[set]=&negativeReasons_set;
    unordered_map <string, vector <AuxiliaryMap*> > predicateToAuxiliaryMaps;
    vector<unsigned> keyIndexesset_0_1_;
    keyIndexesset_0_1_.push_back(0);
    keyIndexesset_0_1_.push_back(1);
    AuxiliaryMap pset_0_1_(&keyIndexesset_0_1_);
    predicateToAuxiliaryMaps["set"].push_back(&pset_0_1_);
    for(unsigned i=0;i<program.size();i++) {
        map<string,PredicateWSet*>::iterator it = predicateWSetMap.find(program[i]->getPredicateName());
        if(it==predicateWSetMap.end()) {
        }
        else {
            const auto& insertResult=it->second->insert(program[i]->getTuple(it->second->size()));
            if(insertResult.second){
                Tuples & tuples = *predicateTuplesMap[program[i]->getPredicateName()];
                Reasons & reasons = *predicateReasonsMap[program[i]->getPredicateName()];
                NegativeReasons & negativeReasons = *predicateNegativeReasonsMap[program[i]->getPredicateName()];
                tuples.push_back(&(*(insertResult.first)));
                reasons.push_back(vector<const Tuple* >());
                negativeReasons.push_back(unordered_set<Tuple, TupleHash>());
                for(AuxiliaryMap* auxMap:predicateToAuxiliaryMaps[it->first]){
                    auxMap -> insert2(*tuples.back());
                }
            }
        }
    }
    unsigned index_cl=0;
    unsigned index_set=0;
    index_cl=0;
    while(index_cl!=tuples_cl.size()){
        const Tuple * tuple0 = tuples_cl[index_cl];
        {
            const vector<const Tuple* >& tuples = pset_0_1_.getValues({(*tuple0)[1],(*tuple0)[2]});
            for( unsigned i=0; i< tuples.size();i++){
                const Tuple * tuple1 = tuples[i];
                const vector<const Tuple* >& tuples = pset_0_1_.getValues({(*tuple0)[3],(*tuple0)[4]});
                for( unsigned i=0; i< tuples.size();i++){
                    const Tuple * tuple2 = tuples[i];
                    const vector<const Tuple* >& tuples = pset_0_1_.getValues({(*tuple0)[5],(*tuple0)[6]});
                    for( unsigned i=0; i< tuples.size();i++){
                        const Tuple * tuple3 = tuples[i];
                        failedConstraints.push_back(vector<aspc::Literal>());
                        vector<const Tuple *> reasons;
                        addReasons(tuple0, reasons, predicateReasonsMap, predicateNegativeReasonsMap);
                        addReasons(tuple1, reasons, predicateReasonsMap, predicateNegativeReasonsMap);
                        addReasons(tuple2, reasons, predicateReasonsMap, predicateNegativeReasonsMap);
                        addReasons(tuple3, reasons, predicateReasonsMap, predicateNegativeReasonsMap);
                        for(const Tuple * reason: reasons) {
                            failedConstraints.back().push_back(tupleToLiteral(*reason));
                        }
                    }
                }
            }
        }
        index_cl++;
    }
}
