#!/usr/bin/python

# This is a test script for multidelta that flags instances where
# Steensgaard's analysis reports a smaller points-to set size than
# Andersen's analysis. Such behavior is clearly a bug in one of the
# analyses

import os
import sys
import string

def list_to_string(list):
    if (len(list) == 0):
	return ""
    result = list[0]
    for elt in list[1:]:
        result = result + " " + elt
    return result

def run_pta(analysis,files):
    output = os.popen("%s %s 2>/dev/null | grep \"Number of things pointed to:\"" % (analysis,list_to_string(files)))
    try:
        ptsetsize = string.atoi(string.split(output.readlines()[0][:-1])[5])
        return ptsetsize
    except IndexError:
        print ("Analysis %s failed" % analysis)
        sys.exit(1) # not interesting, a test failed
        return 0

ptset1 = run_pta("/moa/sc1/jkodumal/work/banshee/cparser/parser_sg.exe",["types.c"]);
ptset2 = run_pta("/moa/sc1/jkodumal/work/banshee/cparser/parser.exe",["types.c"]);

if (ptset1 < ptset2):
#    print "Interesting!"
    sys.exit(0) # interesting!
sys.exit(1) # not interesting, expected result
