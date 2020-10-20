# Copyright (c) 2017 - 2020 Kiriti Nagesh Gowda, Inc. All rights reserved.
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
__copyright__ = "Copyright 2018 - 2020, Kiriti Nagesh Gowda - SRTG-Scheduler"
__license__ = "MIT"
__version__ = "1.0.1"
__maintainer__ = "Kiriti Nagesh Gowda"
__email__ = "Kiritigowda@gmail.com"
__status__ = "Shipping"

# import arguments
parser = argparse.ArgumentParser()
parser.add_argument('--jobs_directory', type=str, default='',
                    help='Jobs Directory - jobs file directory')
parser.add_argument('--jobset_name', type=str, default='',
                    help='Job set prefix name')
parser.add_argument('--num_jobset', type=int, default=-1,
                    help='Number of job [type:INT range:1 to N] - optional (default:1)')
parser.add_argument('--scheduler_directory', type=str, default='',
                    help='Scheduler Directory - directory with RTG-scheduler')
parser.add_argument('--output_directory', type=str, default='',
                    help='Output Directory - directory to save scheduler summary')
args = parser.parse_args()

JobSetDirectory = args.jobs_directory
jobSetName = args.jobset_name
numJobSet = args.num_jobset
RTGSDirectory = args.scheduler_directory
OutputDirectory = args.output_directory

if JobSetDirectory == '' or jobSetName == '' or numJobSet == -1 or RTGSDirectory == '' or OutputDirectory == '':
    print("ERROR - NO Arguments Passed, use --h option")
    exit()

jobs_dir = os.path.expanduser(JobSetDirectory)
exe_dir = os.path.expanduser(RTGSDirectory)
output_dir = os.path.expanduser(OutputDirectory)

if not os.path.exists(jobs_dir):
    print("ERROR No Job Directory")
    exit()
if not os.path.exists(exe_dir):
    print("ERROR No RTG Scheduler Directory")
    exit()
if not os.path.exists(output_dir):
    print("ERROR No Output Directory")
    exit()

# num job sets required to be created
for s in range(numJobSet):
    os.system('./'+exe_dir+'RTG-scheduler --j '+jobs_dir+'/'+jobSetName+'-'+str(s)+'-syntheticJobs.csv --r '+jobs_dir +
              jobSetName+'-'+str(s)+'-syntheticJobReleaseTimes.csv --m 99 >> '+output_dir+'/outputSummary-'+str(s)+'.txt')
