#!/usr/bin/python
import sys
import os
import string

def used_fn(name):
    used_fns = ["_hash_table_insert","hash_table_insert", "_make_hash_table",
		"make_hash_table","seed_fn_ptr_table","_seed_fn_ptr_table",
	        "fail","_fail","__fail","calloc","malloc"]
    return (name in used_fns)

def get_table(libraries):
    result = []
    nm = "nm"
    nmargs = ""
    for lib in libraries:
	output = os.popen("%s %s %s" % (nm, nmargs, lib) )
	for line in output.readlines():
	    entry = string.split(line);
	    if len(entry) == 3 and ("T" in entry[1]) and not "." in entry[2] and not used_fn(entry[2]):
		if (entry[2][0] == "_"):
		    result.append(entry[2][1:])
		else:
		    result.append(entry[2])
    result.sort()
    return result

def print_preamble():
    print "#include \"linkage.h\""
    print "EXTERN_C_BEGIN"
    print "typedef void *region;"
    print "typedef void *hash_table;"
    print "typedef void (*fn_ptr)(void);"
    print "hash_table make_hash_table(region r, unsigned long, void *, void *);"
    print "int hash_table_insert(hash_table,void *,void *);"
    print "hash_table fn_ptr_table = (void *)0;"

def print_postamble():
    print "EXTERN_C_END"

def print_protos(table):
    for entry in table:
	print ("void %s(void);" % entry)

def print_array(table):
    print "fn_ptr fn_ptr_array[%d] = {" % len(table)
    for entry in table:
	print "%s," % entry
    print "};"

def print_hash(table):
    print "void seed_fn_ptr_table(region r)\n{"
    print "fn_ptr_table= make_hash_table(r, %d, ptr_hash, ptr_eq);" % len(table)
    count = 0
    for entry in table:
	print "hash_table_insert(fn_ptr_table,%s,(void *)%d);" % (entry, count)
	count = count + 1
    print "}\n"
    print "const int num_fn_ptrs= %d;" % count

def print_usage_and_exit():
    print "Usage: %s [file...]" % sys.argv[0]
    sys.exit(0)

# get all the header files in the current directory
#def get_headers():
#    output = os.popen("ls -1 *.h")
#    return map((lambda x: x[:-1]), output.readlines())

def print_c_file():
    if (len(sys.argv) <= 1):
	print_usage_and_exit()
    table = get_table(sys.argv[1:])
    print_preamble()
    print_protos(table)
    print_array(table)
    print_hash(table)
    print_postamble()

print_c_file()
