import os
import getopt
import sys
import random
import collections

opts, args = getopt.getopt(sys.argv[1:], 'd:s:n:j:p:l:a:')
 
outputDirectory = ''
jobSetName = ''
numJobSet = -1
numJobsPerSet = -1
maxProcessors = -1
lambdaVar = -1
ALAP_limit = ''
alapLimit = -1

for opt, arg in opts:
    if opt == '-d':
        outputDirectory = arg
    elif opt == '-s':
        jobSetName = arg;
    elif opt == '-n':
        numJobSet = int(arg);
    elif opt == '-j':
    	numJobsPerSet = int(arg);
    elif opt == '-p':
    	maxProcessors = int(arg);
    elif opt == '-l':
    	lambdaVar = float(arg);
    elif opt == '-a':
		ALAP_limit = arg;

if outputDirectory == '' or jobSetName == '' or numJobSet == -1 or numJobsPerSet == -1 or maxProcessors == -1 or lambdaVar == -1:
    print('Invalid command line arguments.	-d [output Directory - required] ' \
						'-s [job set name - required] ' \
						'-n [number of job sets to be created - required] ' \
						'-j [number of job per set to be created - required]  ' \
						'-p [max processors available - required] ' \
						'-l [lambda job arrival rate [range:0.001 to 1] - required] '\
						'-a [Schedule ALAP processor limit - optional [default: 0.75 * max processors]] ')
    exit()

if not os.path.exists(outputDirectory):
    os.makedirs(outputDirectory);

if lambdaVar <= 0:
	print ("lambda Varible should be in the range 0.001 to 1");
	exit();

if ALAP_limit == '':
	alapLimit = int(0.75*maxProcessors);
else:
	ALAP_limit.strip();
	alapLimit = int(ALAP_limit);


# num job sets required to be created
for s in range(numJobSet):
	fileName_Jobs= outputDirectory+'/'+jobSetName+'-'+str(s)+'-jobs.txt';
	fileName_Releases= outputDirectory+'/'+jobSetName+'-'+str(s)+'-releaseTimes.txt';
	
	# array to hold all releases
	releaseTimeArray = [];
	for i in range(numJobsPerSet):
		releaseTimeArray.append(0);

	# create a job set - job_file.txt
	releaseTime = 0;
	releaseDistribution = 0;
	with open(fileName_Jobs,'w') as f:
	
		for x in range(numJobsPerSet):
			jobNumber = x;
			numProcessors = random.randrange(2,maxProcessors,2);
			numProcessorInverseVar = float(1)/numProcessors;
			executionTime = random.randint(numProcessors, max( numProcessors, int(random.expovariate(numProcessorInverseVar)) ));
			releaseDistribution = int(random.expovariate(lambdaVar));
			releaseTime = releaseTime + releaseDistribution;
			lastestTimeSchedulable = releaseTime + executionTime;
			if(alapLimit < numProcessors):
				deadlineVar = numProcessorInverseVar;
			else:
				deadlineVar = numProcessorInverseVar/10;
			deadLine = lastestTimeSchedulable + int(random.expovariate(deadlineVar));
			releaseTimeArray[x] = releaseTime
			releaseTime = releaseTime + 1
			f.write(str(jobNumber)+','+str(numProcessors)+','+str(executionTime)+','+str(deadLine)+','+str(lastestTimeSchedulable)+'\n')

	# create a job release set - job_releaseTime.txt
	from collections import Counter
	counter=Counter(releaseTimeArray)
	with open(fileName_Releases,'w') as f:
		
		for T,N in sorted(counter.items()):
			f.write(str(T)+','+str(N)+'\n')
