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

import collections
import random
import os
import sys
import argparse
__author__ = "Kiriti Nagesh Gowda"
__copyright__ = "Copyright 2018 - 2022, Kiriti Nagesh Gowda - SRTG-Scheduler"
__license__ = "MIT"
__version__ = "1.0.1"
__maintainer__ = "Kiriti Nagesh Gowda"
__email__ = "Kiritigowda@gmail.com"
__status__ = "Shipping"

# import arguments
parser = argparse.ArgumentParser()
parser.add_argument('--directory',  	type=str, 	default='~/SRTG_jobCreator',
                    help='Output Directory - optional (default:~/SRTG_jobCreator)')
parser.add_argument('--jobset_name',	type=str, 	default='aperiodic-set',
                    help='Job set prefix name - optional (default:aperiodic-set)')
parser.add_argument('--num_jobset', 	type=int, 	default=1,
                    help='Number of job sets to be created [type:INT range:1 to N] - optional (default:1)')
parser.add_argument('--num_jobs',   	type=int, 	default=100,
                    help='Number of jobs in each job sets [type:INT range:1 to N] - optional (default:100)')
parser.add_argument('--max_gcu',    	type=int, 	default=16,
                    help='Maximum GCUs available on the system [type:INT range:1 to N] - optional (default:16)')
parser.add_argument('--job_lambda',     type=float, default=0.5,
                    help='Job arrival rate: lambda [type:FLOAT range:0.001 to 5.0] - optional (default:0.5)')
parser.add_argument('--schedule_bias',	type=int, 	default=60,
                    help='Delay schedule GCU limit percentage [type:INT range:1 to 100] - optional (default:60)')
parser.add_argument('--job_bias',   	type=str, 	default='even',
                    help='Job GCU request bias [even, odd, or mixed] - optional (default:even)')
parser.add_argument('--release_bias',   type=str, 	default='single',
                    help='Job Release bias [single, or multiple] - optional (default:single)')
args = parser.parse_args()

# get arguments
outputDirectory = args.directory
jobSetName = args.jobset_name
numJobSet = args.num_jobset
numJobsPerSet = args.num_jobs
maxGCUs = args.max_gcu
lambdaVar = args.job_lambda
scheduleBias = args.schedule_bias
jobBias = args.job_bias
releaseBias = args.release_bias
scheduleBiasVar = int((scheduleBias/100) * maxGCUs)

# validate arguments
if lambdaVar <= 0 or lambdaVar > 5:
    print("ERROR: Job arrival rate - lambda [type:FLOAT range:0.001 to 1.0]")
    exit()

if scheduleBias <= 0 or scheduleBias > 100:
    print(
        "ERROR: Delay schedule GCU limit percentage [type:INT range:1 to 100]")
    exit()

if jobBias not in ('even', 'odd', 'mixed'):
    print("ERROR: Job GCU request bias [options: even, odd, or mixed]")
    exit()

if releaseBias not in ('single', 'multiple'):
    print("ERROR: Job Release bias [options: single, or multiple]")
    exit()

# setup output directory
jobCreatorDir = os.path.expanduser(outputDirectory)
if not os.path.exists(jobCreatorDir):
    os.makedirs(jobCreatorDir)

# create num job sets
for s in range(numJobSet):
    fileName_Jobs = jobCreatorDir+'/' + \
        jobSetName+'-'+str(s)+'-syntheticJobs.csv'
    fileName_Releases = jobCreatorDir+'/'+jobSetName + \
        '-'+str(s)+'-syntheticJobReleaseTimes.csv'

    # array to hold all releases
    releaseTimeArray = []
    for i in range(numJobsPerSet):
        releaseTimeArray.append(0)

    # create a job set - jobs.csv
    releaseTime = 0
    releaseTime_prev = 0
    releaseDistribution = 0
    with open(fileName_Jobs, 'w+') as f:
        f.write(
            'Job ID,GCUs Required,Execution Time,DeadLine,lastest Time Schedulable\n')
        for x in range(numJobsPerSet):
            # Job Number
            jobNumber = x
            # Job GCUs Requested - with Job GCU Bias
            numGCUs = 0
            if jobBias == 'even':
                numGCUs = random.randrange(2, maxGCUs, 2)
            elif jobBias == 'odd':
                numGCUs = random.randrange(1, maxGCUs)
                if (numGCUs % 2) == 0:
                    numGCUs = numGCUs - 1
            else:
                numGCUs = random.randrange(1, maxGCUs)
            # Job Execution Time
            numGCUsInverseVar = float(1)/numGCUs
            executionTime = random.randint(numGCUs, max(
                numGCUs, int(random.expovariate(numGCUsInverseVar))))
            # Job Release Time - with Release Time Lambda considerations
            releaseDistribution = int(random.expovariate(lambdaVar))
            releaseTime = releaseTime + releaseDistribution
            releaseTimeArray[x] = releaseTime
            # Job Deadline
            best_QOS_Time = releaseTime + executionTime
            if(scheduleBiasVar < numGCUs):
                deadlineVar = numGCUsInverseVar
            else:
                deadlineVar = numGCUsInverseVar/10
            deadLine = best_QOS_Time + int(random.expovariate(deadlineVar))
            # Job latest time schedulable on the GPU
            lastestTimeSchedulable = deadLine - executionTime
            # Write Job Info
            f.write(str(jobNumber)+','+str(numGCUs)+','+str(executionTime) +
                    ','+str(deadLine)+','+str(lastestTimeSchedulable)+'\n')
            # max of 2 jobs released at any given release time - Scheduler Support for N releases TBD
            if releaseBias == 'single':
                releaseTime = releaseTime + 1
            else:
                if releaseTime == releaseTime_prev:
                    releaseTime = releaseTime + 1
                releaseTime_prev = releaseTime

    # create a job release set - jobReleaseTimes.csv
    from collections import Counter
    counter = Counter(releaseTimeArray)
    with open(fileName_Releases, 'w+') as f:
        f.write('Job Release Time,Num Jobs Released\n')
        for releaseTime, numJobsReleased in sorted(counter.items()):
            f.write(str(releaseTime)+','+str(numJobsReleased)+'\n')
