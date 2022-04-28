# Copyright (c) 2017 - 2022 Kiriti Nagesh Gowda, Inc. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

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
        jobSetName = arg
    elif opt == '-n':
        numJobSet = int(arg)
    elif opt == '-j':
        numJobsPerSet = int(arg)
    elif opt == '-p':
        maxProcessors = int(arg)
    elif opt == '-l':
        lambdaVar = float(arg)
    elif opt == '-a':
        ALAP_limit = arg

if outputDirectory == '' or jobSetName == '' or numJobSet == -1 or numJobsPerSet == -1 or maxProcessors == -1 or lambdaVar == -1:
    print('Invalid command line arguments.\n'
          '\t\t\t\t-d [output Directory - required]\n'
          '\t\t\t\t-s [job set name - required]\n'
          '\t\t\t\t-n [number of job sets to be created - required]\n'
          '\t\t\t\t-j [number of job per set to be created - required]\n'
          '\t\t\t\t-p [max processors available - required]\n'
          '\t\t\t\t-l [lambda job arrival rate [range:0.001 to 1] - required]\n'
          '\t\t\t\t-a [Delay Schedule processor limit in percentage - optional [default: 60% -- 0.6 * max processor]]\n')
    exit()

if not os.path.exists(outputDirectory):
    os.makedirs(outputDirectory)

if lambdaVar <= 0:
    print("lambda Varible should be in the range 0.001 to 1")
    exit()

if ALAP_limit == '':
    alapLimit = int(0.60*maxProcessors)
else:
    ALAP_limit.strip()
    alapLimit = int(ALAP_limit*maxProcessors)


# num job sets required to be created
for s in range(numJobSet):
    fileName_Jobs = outputDirectory+'/' + \
        jobSetName+'-'+str(s)+'-syntheticJobs.csv'
    fileName_Releases = outputDirectory+'/' + \
        jobSetName+'-'+str(s)+'-syntheticJobReleaseTimes.csv'

    # array to hold all releases
    releaseTimeArray = []
    for i in range(numJobsPerSet):
        releaseTimeArray.append(0)

    # create a job set - job_file.txt
    releaseTime = 0
    releaseDistribution = 0
    with open(fileName_Jobs, 'w') as f:
        f.write(
            'Job ID,GCUs Required,Execution Time,DeadLine,lastest Time Schedulable\n')
        for x in range(numJobsPerSet):
            jobNumber = x
            numProcessors = random.randrange(2, maxProcessors, 2)
            numProcessorInverseVar = float(1)/numProcessors
            executionTime = random.randint(numProcessors, max(
                numProcessors, int(random.expovariate(numProcessorInverseVar))))
            releaseDistribution = int(random.expovariate(lambdaVar))
            releaseTime = releaseTime + releaseDistribution
            lastestTimeSchedulable = releaseTime + executionTime
            if(alapLimit < numProcessors):
                deadlineVar = numProcessorInverseVar
            else:
                deadlineVar = numProcessorInverseVar/10
            deadLine = lastestTimeSchedulable + \
                int(random.expovariate(deadlineVar))
            releaseTimeArray[x] = releaseTime
            releaseTime = releaseTime + 1
            f.write(str(jobNumber)+','+str(numProcessors)+','+str(executionTime) +
                    ','+str(deadLine)+','+str(lastestTimeSchedulable)+'\n')

    # create a job release set - job_releaseTime.txt
    from collections import Counter
    counter = Counter(releaseTimeArray)
    with open(fileName_Releases, 'w') as f:
        f.write('Job Release Time,Num Jobs Released\n')
        for T, N in sorted(counter.items()):
            f.write(str(T)+','+str(N)+'\n')
