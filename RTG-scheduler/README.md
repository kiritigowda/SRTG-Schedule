[![Build Status](https://travis-ci.org/kiritigowda/SRTG-Schedule.svg?branch=master)](https://travis-ci.org/kiritigowda/SRTG-Schedule)
[![codecov](https://codecov.io/gh/kiritigowda/SRTG-Schedule/branch/master/graph/badge.svg)](https://codecov.io/gh/kiritigowda/SRTG-Schedule)

# RTGS
The current release version is 1.0.0

### Windows
```
RTG-scheduler [options] --j <jobs_file.txt> --rt <Release_Time_file.txt> --m <option> --p <option> --d <option> 
```
### Linux / MAC OS X
```
./RTG-scheduler [options] --j <jobs_file.txt> --rt <Release_Time_file.txt> --m <option> --p <option> --d <option>
```

## Scheduler Options Supported
````
        --h/--help      -- Show full help
        --v/--verbose   -- Show detailed messages
````
## Scheduler Parameters
````
        --j/--jobs                 -- Jobs to be scheduled [required]
        --rt/--releaseTimes        -- Release times for the jobs [required]
        --m/--mode                 -- Scheduler Mode [optional - default:5]
        --p/--maxProcessors        -- Max processors available on the GPU [optional - default:16]
        --d/--delayLimitPercentage -- Delay Schedule processor limit in percentage [optional - default:60]
````

- #### --j/--jobs -- The Jobs File is the list of Jobs to be scheduled: <jobs_file.txt>
```
        Jid     - Job Number
        Pn      - Processors Needed
        Texe    - Execution Time
        Td      - Deadline
        Tlts    - Lastest Time Schedulable on the GPU

        "Jid, Pn, Texe, Td, Tlts"
```

- #### --rt/--releaseTimes -- The Release Time File has the list of release times of the kernels: <Release_Time_file.txt>
```
        Tr      - Release Time
        Jr      - Number of jobs released

        "Tr, Jr"
```

- #### --m/--mode -- The Modes Supported: <mode option>
```
        1 - Greedy Schedule
        2 - Event Aware Scheduler
        3 - Event Aware Scheduler with Bias
        4 - Event Aware Scheduler with Bias and Bias Prediction
        5 - Event Aware Scheduler with Bias and Improved Bias Prediction
        N - Extended in the next release
```

## RTGS Function Summary

### DESCRIPTION
This guide provides an overview of the content and usage of the Scheduler tool. This tool is used to schedule jobs on a hetrogenous computing enviornment, with a simple, easy-to-use interface. It encapsulates most of the real time constraints, thus scheduling jobs safely.

* Scheduler Main – This function resides on the CPU and dispatches work to the GPU 
* Get Job Information – Gets the user’s jobs for dispatch 
* Get Job Release Information – Gets the user’s job release times for dispatch 
* Kernel  Book  Keeper –  When  a  kernel  is  released  the  Scheduler  calls  this  function  as  it  has  all  the  information  of  the  available  SMs and future release times 
* Queue job execution– The function safely dispatches the kernels on the GPU and allocates the kernel with required SMs 
* Retrieve  Processors –  Releases  all  the  Kernels  which  have  completed  their  execution  and  frees  up  the  Streaming  Multiprocessors (SM) 
* Processors  Unavailable –  If  processors  are  not  available  at  the  present  times,  this  function  safes  schedules  the  kernels  on  to  the future available time 
* Queue kernel  handler –  The  function  safely  queues  kernels  to  be  dispatched  on  to  the  GPU  and  allocates  the  kernel  with  required SMs 
* Dispatch Queued Kernels – Dispatches the Kernels set for future release times 
* AEAP SCHEDULER – The future scheduler calls the As Early As Possible algorithm to schedule the Kernel on to the GPU 
* AEAP ADVANCED SCHEDULER – once a kernel is scheduled for a  ALAP, this function tries to fit the kernels  before the ALAP kernel release time and after if not possible after the ALAP kernel execution.  
* ALAP SCHEDULER – If the Kernel needs a large chunk of the SMs, then they are scheduled As Late As Possible on to the GPU 
* ALAP ADVANCED SCHEDULER – if another Kernel requires a large chunk of SMs, then it is scheduled ALAP by this function. It takes into account that there are other ALAP kernels scheduled and does not allow overlap.
* ALAP  IMPROVE SCHEDULER –  if  an  ALAP  set  Kernel  can  be  scheduled  without  interfering  with  other  kernels  and  if  it  improves  response  time  of another kernel, the ALAP is called on to modify the schedule to improve system response times.
