#!/usr/bin/python
import sys
import os
import string




def main():
    filename = sys.argv[1]
    file = open(filename, "r")
    printnext = False
    for line in file.readlines():
	if (printnext):
	    print line[:-1]
	    printnext = False
	if (line == "ChangeSet\n"):
	    printnext = True
	else:
	    printnext = False

if __name__ == "__main__":
    main()
