#!/usr/bin/python
import sys
import os
# Run analysis on every prefix of the filelist, serialize that prefix,
# then complete the analysis. Verify that the points-to sets are
# always the same

def list_to_string(list):
    if (len(list) == 0):
	return ""
    result = list[0]
    for elt in list[1:]:
	result = result + " " + elt
    return result

files = sys.argv[1:]

correct = int(os.popen("bash -c \"../cparser/parser_ns.exe %s 2> /dev/null | grep \\\"Number of things pointed to\\\"\"" % list_to_string(files)).readlines()[0].split()[5])

for pivot in range(1,len(files)):
    print "%d / %d" % (pivot,len(files))
    os.system("bash -c \"../cparser/parser_ns.exe -frserialize-constraints %s &>/dev/null\"" % list_to_string(files[:pivot]))
    output = os.popen("bash -c \"../cparser/parser_ns.exe -frdeserialize-constraints %s 2>/dev/null | grep \\\"Number of things pointed to\\\" \"" % list_to_string(files[pivot:])).readlines()[0]
    result = int(output.split()[5])
    if (not (result == correct)):
	print "Failed andersen region persistence test: %d expected, got %d" % (correct, result)
	print "Serialized files: %s" % list_to_string(files[:pivot])
	print "Analyzed files: %s" % list_to_string(files[pivot:])
	sys.exit(1)
