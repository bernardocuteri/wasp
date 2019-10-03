gringo --output=SMODELS $1 $3 | ../wasp.bin -n 0 --interpreter=cpp_eager --script-directory=. --plugins-file=$2 | grep -o '{' | wc -l

