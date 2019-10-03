#numberOfVariables numberOfRules numberOfPredicates maxArity maxBodySize maxHeadSize nFacts maxConstant constantRatio
#./generateBenchmark.py 2 3 3 2 2 2 10 10
#4 4 4 3 3 2 20 10
count=0
until [[ $variable =~ FAILED ]]; do
        ./generateBenchmark.py 3 5 3 3 4 3 10 10 3
       variable=`./runTest.py`
       count=$((count+1))
       echo ${variable}
       echo "Test number "+${count}
done
