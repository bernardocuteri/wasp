#!/usr/bin/python3
import subprocess
import os
import argparse
import re

HEADER = '\033[95m'
OKBLUE = '\033[94m'
OKGREEN = '\033[92m'
WARNING = '\033[93m'
FAIL = '\033[91m'
ENDC = '\033[0m'
BOLD = '\033[1m'
UNDERLINE = '\033[4m'

def parseArguments():
    parser = argparse.ArgumentParser(description='This application is used to test model counts for eager wasp which are compared with plain wasp')

    parser.add_argument('single_folder', type=str, nargs='?',
                        help='An optional integer positional argument')
    parser.add_argument('-p', action='store_true')

    return parser.parse_args()
    
def lastNumberInString(string):
    return re.findall(r'\d+', string)[-1]
def main():
    args = parseArguments()

    if args.single_folder is None:
        dirs = next(os.walk('.'))[1]
    else:
        dirs = [args.single_folder]
        
    failures = 0
    total = 0

    for dir in dirs:
        if dir == "ignored":
            continue
        print("testing benchmark " + dir)
        for file in os.listdir("./"+dir+"/testInstances"):
            instance = "./"+dir+"/testInstances/"+file
            encoding = "./"+dir+"/encoding"
            constraint = "./"+dir+"/constraint"
            parameters = encoding+" "+constraint+" "+instance
            out1 = subprocess.getoutput("./countPlainWaspModels.sh "+parameters)
            if args.p:
                print(out1)
            plainModelsCount = lastNumberInString(out1)  
            print("plain wasp count is "+plainModelsCount)
            out2 = subprocess.getoutput("./countEagerWaspModels.sh "+parameters)
            if args.p:
                print(out2)
            eagerModelsCount = lastNumberInString(out2)
            print("eager wasp count is "+eagerModelsCount)
            total+=1
            if plainModelsCount!=eagerModelsCount:
                failures+=1
                print(FAIL+"MISMATCH MODEL COUNT ON "+file+ENDC)
    print("\nsuccessfull tests "+str(total-failures) +"/"+str(total)+"\n")
    if failures>0:
       print(FAIL+"TEST FAILED"+ENDC)
    else:
       print(OKGREEN+"TEST SUCCEDED"+ENDC)
    
main()
