#!/usr/bin/python
import sys
import os
import string
import random
import md5

# A file is a C source file if it ends in '.c', doesn't start with
# '-', and corresponds to a file on disk
def is_c_file(nextarg):
    return (os.path.isfile(nextarg) and nextarg[-2:] == '.c' and not nextarg[0] == '-')

# Convert a list to a space-separated string
def list_to_string_nolf(list):
    if (len(list) == 0):
	return ""
    result = list[0]
    for elt in list[1:]:
	result = result + " " + elt
    return result

# Given a filename, return a canonical string suitable for use as a C
# identifier
def get_canon_ident(filename):
    m = md5.new()
    m.update(os.path.abspath(os.curdir) + "/" + filename)
    return m.hexdigest()
#    output = os.popen("md5 -s \"%s\"" % filename)
#    return output.readlines()[0].split()[3]
    
# Return true if the line 'appendline' was already appended to the file
def already_added(appendline, filename):
    output = os.popen("tail -1 %s" % filename)
    lastline = output.readlines()[0][:-1]
    if (lastline == appendline):
	return True
    return False

def gcc_cmd():
    try:
	return os.environ["GCC_SUBST"]
    except KeyError:
	return "gcc"

# Add a string variable declaration to the end of a file. If the file
# already ends with such a declaration, this won't add another
# TODO -- md5 the FULL filename, including ${pwd}
def modify_file(filename):
    appendline = "const char CANON_IDENT_%s[] = \"CANON_IDENT_%s\";" % (get_canon_ident(filename), os.path.abspath(os.curdir) + "/" + filename)
    if (not already_added(appendline,filename)):
	os.system("echo \"const char CANON_IDENT_%s[] = \\\"CANON_IDENT_%s\\\";\" >> %s" % (get_canon_ident(filename), os.path.abspath(os.curdir) + "/" + filename, filename))

def main():
    for arg in sys.argv[1:]:
	if (is_c_file(arg)):
	    modify_file(arg)
    os.system("%s %s" % (gcc_cmd(),list_to_string_nolf(sys.argv[1:])))

if __name__ == "__main__":
    main()
