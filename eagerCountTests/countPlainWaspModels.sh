echo $1
echo $2
echo $3

gringo --output=SMODELS $1 $2 $3 | ./wasp.bin -n 0 | grep -o '{' | wc -l

