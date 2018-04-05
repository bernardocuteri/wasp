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

typedef vector<const vector<unsigned>* > Tuples;
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

using PredicateWSet = std::unordered_set<std::vector<unsigned>, VectorHash>;
void Executor::executeProgramOnFacts(const vector<aspc::Atom*> & program) {
    failedConstraints.clear();
    map<string, PredicateWSet*> predicateWSetMap;
    map<string, Tuples* > predicateTuplesMap;
    Tuples tuples_p_0;
    PredicateWSet wp_0;
    predicateWSetMap["p_0"]=&wp_0;
    predicateTuplesMap["p_0"]=&tuples_p_0;
    Tuples tuples_p_2;
    PredicateWSet wp_2;
    predicateWSetMap["p_2"]=&wp_2;
    predicateTuplesMap["p_2"]=&tuples_p_2;
    Tuples tuples_p_4;
    PredicateWSet wp_4;
    predicateWSetMap["p_4"]=&wp_4;
    predicateTuplesMap["p_4"]=&tuples_p_4;
    Tuples tuples_p_5;
    PredicateWSet wp_5;
    predicateWSetMap["p_5"]=&wp_5;
    predicateTuplesMap["p_5"]=&tuples_p_5;
    unordered_map <string, vector <AuxiliaryMap*> > predicateToAuxiliaryMaps;
    vector<unsigned> keyIndexesp_0_0_1_2_3_;
    keyIndexesp_0_0_1_2_3_.push_back(0);
    keyIndexesp_0_0_1_2_3_.push_back(1);
    keyIndexesp_0_0_1_2_3_.push_back(2);
    keyIndexesp_0_0_1_2_3_.push_back(3);
    AuxiliaryMap pp_0_0_1_2_3_(&keyIndexesp_0_0_1_2_3_);
    predicateToAuxiliaryMaps["p_0"].push_back(&pp_0_0_1_2_3_);
    vector<unsigned> keyIndexesp_0_0_1_2_;
    keyIndexesp_0_0_1_2_.push_back(0);
    keyIndexesp_0_0_1_2_.push_back(1);
    keyIndexesp_0_0_1_2_.push_back(2);
    AuxiliaryMap pp_0_0_1_2_(&keyIndexesp_0_0_1_2_);
    predicateToAuxiliaryMaps["p_0"].push_back(&pp_0_0_1_2_);
    for(unsigned i=0;i<program.size();i++) {
        map<string,PredicateWSet*>::iterator it = predicateWSetMap.find(program[i]->getPredicateName());
        if(it==predicateWSetMap.end()) {
            program[i]->print();
            cout<<".\n";
        }
        else {
            vector<unsigned> tuple = program[i]->getIntTuple();
            const auto& insertResult=it->second->insert(tuple);
            if(insertResult.second){
                Tuples & tuples = *predicateTuplesMap[program[i]->getPredicateName()];
                tuples.push_back(&(*(insertResult.first)));
                for(AuxiliaryMap* auxMap:predicateToAuxiliaryMaps[it->first]){
                    auxMap -> insert2(*tuples.back());
                }
            }
        }
    }
    unsigned index_p_0=0;
    unsigned index_p_2=0;
    unsigned index_p_4=0;
    unsigned index_p_5=0;
    while(index_p_5!=tuples_p_5.size() || index_p_4!=tuples_p_4.size() || index_p_2!=tuples_p_2.size()){
        while(index_p_4!=tuples_p_4.size()){
            const vector<unsigned>& tuple0 = *tuples_p_4[index_p_4];
            {
                if( tuple0[0] == tuple0[1]){
                    vector<unsigned> key0(3);
                    key0[0]=tuple0[0];
                    key0[1]=tuple0[0];
                    key0[2]=tuple0[0];
                    const vector<const vector <unsigned>* >& tuples = pp_0_0_1_2_.getValues(key0);
                    for( unsigned i=0; i< tuples.size();i++){
                        const vector<unsigned>& tuple1 = *tuples[i];
                        if( tuple1[0] == tuple1[1] && tuple1[0] == tuple1[2] && tuple1[1] == tuple1[2]){
                            vector <unsigned> head(4);
                            head[0]=tuple0[1];
                            head[1]=tuple0[1];
                            head[2]=tuple0[1];
                            head[3]=tuple0[1];
                            const auto & insertResult = wp_5.insert(head);
                            if(insertResult.second){
                                tuples_p_5.push_back(&(*insertResult.first));
                            }
                        }
                    }
                }
            }
            index_p_4++;
        }
        while(index_p_2!=tuples_p_2.size()){
            const vector<unsigned>& tuple0 = *tuples_p_2[index_p_2];
            {
                vector<unsigned> key0(4);
                key0[0]=tuple0[0];
                key0[1]=tuple0[0];
                key0[2]=tuple0[1];
                key0[3]=tuple0[1];
                const vector<const vector <unsigned>* >& tuples = pp_0_0_1_2_3_.getValues(key0);
                for( unsigned i=0; i< tuples.size();i++){
                    const vector<unsigned>& tuple1 = *tuples[i];
                    if( tuple1[0] == tuple1[1] && tuple1[2] == tuple1[3]){
                        vector <unsigned> head(2);
                        head[0]=tuple0[0];
                        head[1]=tuple0[1];
                        const auto & insertResult = wp_4.insert(head);
                        if(insertResult.second){
                            tuples_p_4.push_back(&(*insertResult.first));
                        }
                    }
                }
            }
            {
                vector <unsigned> head(2);
                head[0]=tuple0[0];
                head[1]=tuple0[1];
                const auto & insertResult = wp_4.insert(head);
                if(insertResult.second){
                    tuples_p_4.push_back(&(*insertResult.first));
                }
            }
            index_p_2++;
        }
        while(index_p_5!=tuples_p_5.size()){
            const vector<unsigned>& tuple0 = *tuples_p_5[index_p_5];
            {
                if( tuple0[0] == tuple0[2] && tuple0[0] == tuple0[3] && tuple0[2] == tuple0[3]){
                    vector <unsigned> head(2);
                    head[0]=tuple0[1];
                    head[1]=tuple0[3];
                    const auto & insertResult = wp_2.insert(head);
                    if(insertResult.second){
                        tuples_p_2.push_back(&(*insertResult.first));
                    }
                }
            }
            index_p_5++;
        }
    }
    printTuples("p_0",tuples_p_0);
    printTuples("p_2",tuples_p_2);
    printTuples("p_4",tuples_p_4);
    printTuples("p_5",tuples_p_5);
}
