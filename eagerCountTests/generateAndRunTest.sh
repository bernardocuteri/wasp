#!/bin/bash
#numberOfVariables numberOfRules numberOfPredicates maxArity maxBodySize maxHeadSize nFacts maxConstant constantRatio maxArithmeticExpressionsPerRule
#./generateBenchmark.py 2 3 3 2 2 2 10 10 5 2
#4 4 4 3 3 2 20 10
count=0
until [[ $variable =~ FAILED ]]; do
        ./generateBenchmark.py 5 6 5 3 4 7 10 5 4 0
       variable=`./runTest.py`
       count=$((count+1))
       echo ${variable}
       echo "Test number "+${count}
done
