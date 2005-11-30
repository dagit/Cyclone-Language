#!/usr/bin/python

# This is a test script for multidelta that flags a test suite when
# backtracking to some point followed by analyzing forword to the
# endpoint leads to a discrepancy in points-to set sizes

# The list of input files is taken from the environment variable
# multidelta_all_files. The path to the parser command is hardcoded,
# too

import os
import sys
import string
import re

def list_to_string(thelist):
    if (len(thelist) == 0):
	return ""
    result = thelist[0]
    for elt in thelist[1:]:
        result = result + " " + elt
    return result

def print_list_w_lf(thelist):
    for elt in thelist:
	print elt

# first, run the analysis, and return the total points-to set size and
# a list of (file, clock) pairs
def run_analysis(analysis, files, extraflags):
    clocks = []
    ptsetsize = 0
    cmd = "%s %s %s 2>/dev/null" % (analysis, extraflags, list_to_string(files))
    output = os.popen(cmd)
    try:
	for line in output.readlines():
	    clockmatch = re.compile("file: (.*) clock: (.*)").match(line)
	    if (clockmatch):
		clocks.append((clockmatch.group(1),clockmatch.group(2)))
	    else:
		ptsetmatch = re.compile("Number of things pointed to: ([0-9]*)").match(line)
		if (ptsetmatch):
		    ptsetsize = string.atoi(ptsetmatch.group(1))
	return (ptsetsize, clocks)
    except:
	sys.exit(1) # not interesting, a test failed

def run_subsequent(analysis, files, ptsetsize, clocks):
    endtime = clocks[-1][1]
    for i in range(2,len(clocks)+1):
	backtime = clocks[-i][1]
	first = lambda (x,y): x
	files_to_analyze = map(first, clocks) + map(first, clocks[-i+1:])
	(btsetsize, _) = run_analysis(analysis, files_to_analyze, "-fto%s -fback%s" % (endtime,backtime))
	print (btsetsize, ptsetsize)
	if (not (btsetsize == ptsetsize)):
	    print i, backtime, btsetsize, ptsetsize, clocks[-i+1][0]
	    sys.exit(0) # interesting!

analysis = "~/work/banshee/cparser/parser.exe"

filelist = string.split(os.environ['multidelta_all_files'])
print_list_w_lf(filelist)
(ptsetsize, clocks) = run_analysis(analysis, filelist,"")
run_subsequent(analysis, files, ptsetsize, clocks)
sys.exit(1)
