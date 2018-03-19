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
void Executor::executeProgramOnFacts(const vector<aspc::Atom> & program) {
    failedConstraints.clear();
    map<string, PredicateWSet*> predicateWSetMap;
    map<string, Tuples* > predicateTuplesMap;
    Tuples tuples_b;
    PredicateWSet wb;
    predicateWSetMap["b"]=&wb;
    predicateTuplesMap["b"]=&tuples_b;
    Tuples tuples_c;
    PredicateWSet wc;
    predicateWSetMap["c"]=&wc;
    predicateTuplesMap["c"]=&tuples_c;
    Tuples tuples_f;
    PredicateWSet wf;
    predicateWSetMap["f"]=&wf;
    predicateTuplesMap["f"]=&tuples_f;
    unordered_map <string, vector <AuxiliaryMap*> > predicateToAuxiliaryMaps;
    vector<unsigned> keyIndexesb_;
    AuxiliaryMap pb_(&keyIndexesb_);
    predicateToAuxiliaryMaps["b"].push_back(&pb_);
    vector<unsigned> keyIndexesf_0_;
    keyIndexesf_0_.push_back(0);
    AuxiliaryMap pf_0_(&keyIndexesf_0_);
    predicateToAuxiliaryMaps["f"].push_back(&pf_0_);
    for(unsigned i=0;i<program.size();i++) {
        map<string,PredicateWSet*>::iterator it = predicateWSetMap.find(program[i].getPredicateName());
        if(it==predicateWSetMap.end()) {
            program[i].print();
            cout<<".\n";
        }
        else {
            vector<unsigned> tuple = program[i].getIntTuple();
            const auto& insertResult=it->second->insert(tuple);
            if(insertResult.second){
                Tuples & tuples = *predicateTuplesMap[program[i].getPredicateName()];
                tuples.push_back(&(*(insertResult.first)));
                for(AuxiliaryMap* auxMap:predicateToAuxiliaryMaps[it->first]){
                    auxMap -> insert2(*tuples.back());
                }
            }
        }
    }
    unsigned index_b=0;
    unsigned index_c=0;
    unsigned index_f=0;
    index_c=0;
    while(index_c!=tuples_c.size()){
        const vector<unsigned>& tuple0 = *tuples_c[index_c];
        {
            vector<unsigned> key0(0);
            const vector<const vector <unsigned>* >& tuples = pb_.getValues(key0);
            for( unsigned i=0; i< tuples.size();i++){
                const vector<unsigned>& tuple1 = *tuples[i];
                vector<unsigned> key1(1);
                key1[0]=tuple0[0];
                if(wf.find(key1)==wf.end()){
                    vector<aspc::Literal>failedConstraint;
                    vector<string> terms0;
                    for(unsigned v:tuple0) {
                        terms0.push_back(ConstantsManager::getInstance().unmapConstant(v));
                    }
                    failedConstraint.push_back(aspc::Literal(false, aspc::Atom("c", terms0)));
                    vector<string> terms1;
                    for(unsigned v:tuple1) {
                        terms1.push_back(ConstantsManager::getInstance().unmapConstant(v));
                    }
                    failedConstraint.push_back(aspc::Literal(false, aspc::Atom("b", terms1)));
                    vector<string> terms2;
                    for(unsigned v:key1) {
                        terms2.push_back(ConstantsManager::getInstance().unmapConstant(v));
                    }
                    failedConstraint.push_back(aspc::Literal(true, aspc::Atom("f", terms2)));
                    failedConstraints.push_back(failedConstraint);
                }
            }
        }
        index_c++;
    }
}
