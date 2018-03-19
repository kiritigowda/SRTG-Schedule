import os
import getopt
import sys
import random
import collections

opts, args = getopt.getopt(sys.argv[1:], 'd:s:n:j:p:')
 
outputDirectory = ''
jobSetName = ''
numJobSet = -1
numJobsPerSet = -1
maxProcessors = -1

for opt, arg in opts:
    if opt == '-d':
        outputDirectory = arg
    elif opt == '-s':
        jobSetName = arg
    elif opt == '-n':
        numJobSet = int(arg)
    elif opt == '-j':
    	numJobsPerSet = int(arg)
    elif opt == '-p':
    	maxProcessors = int(arg)

if outputDirectory == '' or jobSetName == '' or jobSetName == '' :
    print('Invalid command line arguments. -d [output Directory] ' \
          				  '-s [job set name] ' \
          				  '-n [number of job sets to be created] ' \
          				  '-j [number of job per set to be created]  ' \
          				  '-p [max processors available]')
    exit()

if not os.path.exists(outputDirectory):
    os.makedirs(outputDirectory)

# num job sets required to be created
for s in range(numJobSet):
	fileName_Jobs= outputDirectory+'/'+jobSetName+'-'+str(s)+'-jobs.txt'
	fileName_Releases= outputDirectory+'/'+jobSetName+'-'+str(s)+'-releaseTimes.txt'
	
	# array to hold all releases
	releaseTimeArray = []
	for i in range(numJobsPerSet):
		releaseTimeArray.append(0)

	# create a job set - job_file.txt
	releaseTime = 0
	with open(fileName_Jobs,'w') as f:
	
		for x in range(numJobsPerSet):
			jobNumber = x
			numProcessors = random.randint(1,maxProcessors)
			if numProcessors % 2 != 0:
				numProcessors += 1;
			executionTime = random.randint(2,maxProcessors) + random.randint(-1,numProcessors+5)
			releaseTime = random.randint(releaseTime, releaseTime + random.randint(1,10))
			deadLine = releaseTime + executionTime + random.randint(numProcessors,50)
			lastestTimeSchedulable = releaseTime + numProcessors
			releaseTimeArray[x] = releaseTime
			releaseTime = releaseTime + 1
			f.write(str(jobNumber)+','+str(numProcessors)+','+str(executionTime)+','+str(deadLine)+','+str(lastestTimeSchedulable)+'\n')

	# create a job release set - job_releaseTime.txt
	from collections import Counter
	counter=Counter(releaseTimeArray)
	with open(fileName_Releases,'w') as f:
		
		for T,N in sorted(counter.items()):
			f.write(str(T)+','+str(N)+'\n')
