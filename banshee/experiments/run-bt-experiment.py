#!/usr/bin/python
import sys
import os
import string
import getopt

options='c:d:p:l:o:s:heb'
long_options=['start-with=','end-with=','analysis=',"help","stop-on-build-error","ext="]

# Default values for command line options
project = "cqual"
#repository = ":pserver:anonymous@cvs.sourceforge.net:/cvsroot/" + project
repository = "~jkodumal/work/local_repositories/" + project
logfilename = "logs/" + project + ".log"
outfilename = "out/" + project 
statefilename = "state/" + project
start_with_entry = 0
end_with_entry = 100
compilescript = "./default_compile.sh"
analysis = "../cparser/parser_ns.exe"
enhanced_mod_check = False
stop_on_build_error = False
use_bitkeeper = False
extension = ".i"

# Print a usage message and exit
def usage():
    print("Usage: %s [options]\n\n\
options:\n\
  -c <script>:    run this script to compile the project\n\
  -d <cvsroot>:   specify CVS root (CVS mode only)\n\
  -p <project>:   name the repository (project) to analyze\n\
  -l <logfile>:   read commit log from logfile\n\
  -o <outfile>:   save output as outfile\n\
  -s <statefile>: read/write the analysis state from/to statefile\n\
  -e              used enhanced check to find modified files (C projects only)\n\
  -b              analyze a bitkeeper (not CVS) project\n\
  -h              show this message\n"
	  % sys.argv[0])

# Parse command line options
def parse_options():
    global project, repository, logfilename, outfilename,statefilename
    global start_with_entry, end_with_entry, analysis, extension 
    global enhanced_mod_check, compilescript, stop_on_build_error
    try:
	opts, args = getopt.getopt(sys.argv[1:],options,long_options)
    except getopt.GetoptError:
	usage()
	sys.exit(2)
    for o,a in opts:
	if (o == '-c'):
	    compilescript = a
	if (o == '-d'):
	    repository = a
	if (o == '-p'):
	    project = a
	if (o == '-l'):
	    logfilename = a
	if (o == '-o'):
	    outfilename = a
	if (o == '-s'):
	    statefilename = a
	if (o == '-e'):
	    enhanced_mod_check = a
	if (o == '--start-with'):
	    start_with_entry = int(a)
	if (o == '--end-with'):
	    end_with_entry = int(a)
	if (o in ['-h','--help']):
	    usage()
	    sys.exit(0)
        if (o == '--stop-on-build-error'):
            stop_on_build_error = True
	if (o == '--analysis'):
	    analysis = a
	if (o == '--ext'):
	    extension = a
	if (o == '-b'):
	    use_bitkeeper = True

# Get the next log entry, exit if we are past the desired stopping
# point. Each entry is a pair consisting of a date string and a list
# of (type, name, version) triples
def next_cvs_log_entry(logfile):
    result = []
    date = logfile.readline()
    if (not date):
	sys.exit(0)
    while(True):
	entry = string.split(logfile.readline())
	if (not (len(entry) == 3)):
	    break
	else:
	    result.append(entry)
    return (date,result)

def next_bk_log_entry(logfile):
    result = []
    line = logfile.readline()
    if (not line):
	sys.exit(0);
    return (string.split(line)[0],[])

def next_log_entry(logfile):
    if (use_bitkeeper):
	return next_bk_log_entry(logfile)
    else:
	return next_cvs_log_entry(logfile)

def run_checkout(repository, version, project):
    if (use_bitkeeper):
	os.system("bash -c \"bk export -r%s %s %s &>/dev/null\"" % (version, repository, project))
	os.system("bash -c \"chmod -R %s +w &>/dev/null\"" % project)
    else:
	os.system("bash -c \"cvs -d %s co -D \\\"%s\\\" %s &>/dev/null\"" % (repository, version, project))


# Get a list of the files with a given extension that have been
# modified in dir_b as compared to dir_a. The directory name will be
# the project name originally checked out
def get_modified_files_simple(dir_a, dir_b, extension):
    result = []
    b_files = os.popen("find %s -name *%s" % (dir_b,extension))
    for filewline in b_files.readlines():
	file = filewline[:-1]
	if (os.system("bash -c \"diff %s %s &>/dev/null\"" 
		      % (file, dir_a + file[len(dir_b):]))):
	    result.append(project + file[len(dir_b):])
    return result

def get_modified_files_enhanced(dir_a, dir_b, extension):
    result = []
    b_files = os.popen("find %s -name *.o" % dir_b)
    for filewline in b_files.readlines():
	file = filewline[:-1]
	if (os.system("bash -c \"diff %s %s &>/dev/null\""
		      % (file, dir_a + file[len(dir_b):]))):
	    srcfile = os.popen("strings %s | grep CANON_IDENT").readlines()[0][len("CANON_IDENT_"):-3] + extension
	    srcfound = os.popen("find %s -name %s" % (dir_b, srcfile)).readlines()[0][:-1]
	    result.append(project + srcfound[len(dir_b):])
    return result

def get_modified_files(dir_a, dir_b, extension):
    if (enhanced_mod_check):
	return get_modified_files_enhanced(dir_a, dir_b, extension)
    else:
	return get_modified_files_simple(dir_a, dir_b, extension)

def get_removed_files(modified, candidates):
    result = []
    for file in candidates:
	if (not (file in modified)):
	    result.append(file)
    return result

# Read the list of banshee times 
def get_banshee_state(statefile):
    result = []
    while(True):
	line = statefile.readline()
	if (not line):
	    break
	temp = string.split(line)
	result.append((temp[1],int(temp[3])))
    return result

def get_filelist(dirname,extension):
    files = os.popen("find %s -name *%s" % (dirname, extension))
    return files.readlines()

# Take a string list and return it as a space separated list, also
# chop off newlines
def list_to_string(list):
    if (len(list) == 0):
	return ""
    result = list[0][:-1]
    for elt in list[1:]:
	result = result + " " + elt[:-1]
    return result

def list_to_string_nolf(list):
    if (len(list) == 0):
	return ""
    result = list[0]
    for elt in list[1:]:
	result = result + " " + elt
    return result

def list_to_pretty_string_nolf(list):
    count = 0
    if (len(list) == 0):
	return "  ** NONE"
    result = "\n  ** " + str(count) + "" + list[0]
    for elt in list[1:]:
	count = count + 1
	result = result + "\n  ** " + str(count) +  " " + elt
    return result

# Take a string list (the output of running the analysis) and process
# it into two output lists
def process_andersen_output(current, output, prefix):
    found = False
    statefile = open(statefilename + str(current),"w")
    outfile = open(outfilename + str(current),"w")
    for entry in prefix:
	statefile.write("file: %s clock: %d\n" % entry)
    for line in output:
	if (line == '##################\n'):
	    found = True
	if found:
	    outfile.write(line)
	else:
	    statefile.write(line)
    statefile.close()
    outfile.close()

# Compute the rollback time from the previous analysis state, given
# that we are rolling back to file
def compute_time(file, state):
    time = 0
    for (nextfile,next_time) in state:
	if (file == nextfile):
	    return time
	time = next_time
    return time

# Compute the new list of files to analyze, given that we are rolling
# back to file
def compute_stack(modified, removed, file, state, time):
    # First, take the prefix of the list up to file
    try:
	pivot = state.index((file,time))
    except:
	print ("File %s not found in stack" % file)
    # Next, take the suffix of filelist starting with file, but filtering out
    # any modifieds
	prefix = state[:state.index((file,time))]
    suffix = [elem for elem,_ in state[state.index((file,time)):] if (not (elem in modified or elem in removed))]
    return (suffix + modified),prefix

# Given the list of modified files, the new file list, and the old
# analysis state, compute a new filelist (stack) and the rollback time
def get_new_stack_and_time(modified, removed, state):
    for file,nexttime in state:
	if (file in modified) or (file in removed):
	    time = compute_time(file, state)
	    stack,prefix = compute_stack(modified, removed, file, state, nexttime)
	    return (time,stack, prefix)
    # TODO-- what if we just added a file, so it's not in state?
    print "Warning: failed to compute rollback time and new stack; presumably the commit just added some files or didn't modify any sources..."
    return (state[-1][1], modified, state)

def write_simulation_data(version, modified, files, prefix, simfile, modfile):
    prefiles = []
    for (file,_) in prefix:
	prefiles.append(file)
    if (len(files) > 0):
	reanalysis_size = int(os.popen("du -sck %s | grep total" % list_to_string_nolf(files)).readlines()[0].split()[0])
    else:
	reanalysis_size = 0
    if (len(prefiles) > 0):
	preanalyzed_size = int(os.popen("du -sck %s | grep total" % list_to_string_nolf(prefiles)).readlines()[0].split()[0])
    else:
	preanalyzed_size = 0
    modfile.write("version: %s all files : %s\n#####" % (version, list_to_pretty_string_nolf(prefiles + files )))
    modfile.write("modified files: %s\n#####\n" % list_to_pretty_string_nolf(modified) )
    modfile.write("analyzed files: %s\n##########\n" % list_to_pretty_string_nolf(files) )
    if (reanalysis_size + preanalyzed_size == 0):
	simfile.write("analyzed: 0 total: 0")
    else:
	simfile.write("analyzed: %d total: %d percent: %f\n" % (reanalysis_size,reanalysis_size + preanalyzed_size, float(reanalysis_size) / float(reanalysis_size + preanalyzed_size)))

# Entry point 
def main():
    entrynum = 0
    parse_options()
    simfilename = "simulations/" + project + ".sim"
    modfilename = "simulations/" + project + ".mod"
    simfile = open(simfilename, "w")
    modfile = open(modfilename, "w")
    logfile = open(logfilename, "r")
    #skip the initial blank
    logfile.readline()
    project_prev = project + "_prev" 
    # skip the specified entries
    for _ in range(0,start_with_entry):
	next_log_entry(logfile)
        entrynum = entrynum + 1
    # run one entry to prime the pump
    os.system("rm -rf %s" % project)
    version,_ = next_log_entry(logfile)
    print "Checking out version %s" % version
    run_checkout(repository, version, project)
    build_error = os.system("%s %s %d" % (compilescript, project, entrynum))
    entrynum = entrynum + 1
    if (build_error):
	print "Build error"
	sys.exit(1)
    # run Andersen's analysis, save the state and output 
    files = list_to_string(get_filelist(project,extension))
    cmd = "%s -frserialize-constraints %s 2>/dev/null" % (analysis,files)
    cmd_all = "bash -c \"%s -frserialize-constraints %s &> out/%s_all_%s.out\"" % (analysis, files, project, entrynum)
    print cmd_all
    os.system(cmd_all)
    output = os.popen(cmd).readlines()
    process_andersen_output(start_with_entry,output,[])
    # move the analysis to the _prev directory
    os.system("rm -rf %s" % project_prev) 
    os.system("mv %s %s" % (project, project_prev))

    # for each entry, do the following:
    # 1. run cvs co -d -D version
    # 2. mv to either project_odd or project_even
    # 3. compile it by running compilescript
    # 4. get the list of modified .i files with get_modified_files
    # 5. find the earliest modified file/time by comparing with banshee state
    # 6. compute the new list of files to analyze, putting recently modded
    #    files on the top of the stack
    # 7. run the alias analysis, rolling back to the specified time 
    # 8. save the new statefile and analysis output
    for current in range(start_with_entry+1,end_with_entry+1):
	os.system("rm -rf %s" % project)
	statefile = open(statefilename + str(current-1), "r")
	banshee_state = get_banshee_state(statefile)
 	version,_ = next_log_entry(logfile)
	print "Checking out %s" % version
	run_checkout(repository, version, project)
	build_error = os.system("%s %s" % (compilescript, project))
	if (build_error):
	    print "Build error -- skipping this commit"
	    os.system("cp %s %s" % (statefilename + str(current-1) , statefilename + str(current)) )
	    os.system("echo Build error > %s" % (outfilename + str(current)))
            if (stop_on_build_error):
                sys.exit(1)
	    os.system("rm -rf %s" % project)
	    continue
	modified = get_modified_files(project_prev,project,extension)
	removed = get_removed_files(modified, get_modified_files(project, project_prev,extension))
	time,files,prefix = get_new_stack_and_time(modified,removed, banshee_state)
	print "Backtracking to : %s" % time
	prefiles = []
	for (file,_) in prefix:
	    prefiles.append(file)
	cmd_all = "bash -c \"%s -frserialize-constraints %s &> out/%s_all_%d.out\"" % (analysis, list_to_string_nolf(prefiles + files), project, current)
	cmd = "%s -frserialize-constraints -frdeserialize-constraints -fback%s %s 2>/dev/null" % (analysis, time, list_to_string_nolf(files))
	#print cmd
	output = os.popen(cmd).readlines()
	process_andersen_output(current,output,prefix)
	print cmd_all
	os.system(cmd_all)
	write_simulation_data(version, modified, files, prefix, simfile, modfile)
  	os.system("rm -rf %s" % project_prev)
	os.system("mv %s %s" % (project, project_prev))

    logfile.close()
    simfile.close()
    modfile.close()
    os.system("rm -rf %s" % project)
    os.system("rm -rf %s" % project_prev)
    os.system("rm -f andersen.out")

if __name__ == "__main__":
    main()
