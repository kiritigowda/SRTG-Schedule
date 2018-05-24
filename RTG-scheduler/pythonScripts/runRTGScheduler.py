import os
import getopt
import sys
import random
import collections

opts, args = getopt.getopt(sys.argv[1:], 'd:s:n:r:o:')
 
JobSetDirectory = ''
jobSetName = ''
numJobSet = -1
RTGSDirectory = ''
OutputDirectory = ''

for opt, arg in opts:
    if opt == '-d':
        JobSetDirectory = arg
    elif opt == '-s':
        jobSetName = arg
    elif opt == '-n':
        numJobSet = int(arg)
    elif opt == '-r':
    	RTGSDirectory = arg
    elif opt == '-o':
    	OutputDirectory = arg

if JobSetDirectory == '' or jobSetName == '' or numJobSet == -1 or RTGSDirectory == '' or OutputDirectory == '':
    print('Invalid command line arguments. -d [testSet Directory] ' \
          				                        '-s [job set name] ' \
          				                        '-n [number of jobs in the set] ' \
          				                        '-r [RTG Scheduler Location]  ' \
          				                        '-o [output directory]')
    exit()

if not os.path.exists(JobSetDirectory):
    print "ERROR No Job Directory"
if not os.path.exists(RTGSDirectory):
    print "ERROR No RTG Scheduler Directory"
if not os.path.exists(OutputDirectory):
    print "ERROR No Output Directory"

jobs_dir = os.path.expanduser(JobSetDirectory)
exe_dir = os.path.expanduser(RTGSDirectory)
output_dir = os.path.expanduser(OutputDirectory)

# num job sets required to be created
for s in range(numJobSet):
	os.system(exe_dir+'RTG-scheduler.exe --j '+jobs_dir+jobSetName+'-'+str(s)+'-jobs.txt --rt '+jobs_dir+jobSetName+'-'+str(s)+'-releaseTimes.txt --m 99 >> '+output_dir+'outputSummary-'+str(s)+'.txt')
	os.system('mv '+exe_dir+'RTGS-Summary '+output_dir+'RTGS-Summary-'+str(s));