#!/usr/bin/python
import sys
import os
import string

#print sys.argv


frontend = "../cparser/parser.exe"
flags = "-fprint-results"
pta_cmd = "%s %s" % (frontend, flags)

prefix = ""
prefix_size = int(sys.argv[1])

for x in sys.argv[2:(2+prefix_size)]:
    prefix = " %s %s" % (prefix, x)

filelist = ""
for x in sys.argv[2:]:
    filelist = "%s %s" % (filelist, x)

# this computes the points to set size for the prefix list
output = os.popen(pta_cmd + prefix + " 2> /dev/null | grep \"Number of things pointed to:\" ")
try: 
    total = int(string.split((output.readlines()[0]))[5])
except:
    sys.stderr.write("Warning: failed to analyze some files\n")
    exit

output = os.popen(pta_cmd + filelist + (" -fbt%d" %prefix_size) + " 2> /dev/null | grep \"Number of things pointed to:\" ")
try:
    total2 = int(string.split((output.readlines()[0]))[5])
except:
    sys.stderr.write("Warning: failed to analyze some files\n")
    exit

print "Total number of things pointed to (%d files): %d" % (prefix_size,total)
print "Total number of things pointed to (%d files): with backtracking: %d" % (prefix_size,total2)
if (total != total2):
    print "A backtracking test failed"
    sys.exit(1)
else:
    sys.exit(0)
