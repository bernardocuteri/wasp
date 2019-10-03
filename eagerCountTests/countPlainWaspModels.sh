gringo --output=SMODELS $1 $2 $3 | wasp -n 0 | grep -o '{' | wc -l

