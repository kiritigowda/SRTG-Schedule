[![Build Status](https://travis-ci.org/kiritigowda/SRTG-Schedule.svg?branch=master)](https://travis-ci.org/kiritigowda/SRTG-Schedule)
[![codecov](https://codecov.io/gh/kiritigowda/SRTG-Schedule/branch/master/graph/badge.svg)](https://codecov.io/gh/kiritigowda/SRTG-Schedule)

# RTGS
The current release verion is 0.9.9.1 (beta preview).

### Windows
```
RTG-scheduler [options] --jobs <jobs_file.txt> --releaseTimes <Release_Time_file.txt> --mode <mode option> 
```
### Linux
```
./RTG-scheduler [options] --jobs <jobs_file.txt> --releaseTimes <Release_Time_file.txt> --mode <mode option> 
```

## Scheduler Options Supported
````
        --h/--help      -- Show full help
        --v/--verbose   -- Show detailed messages
````
## Scheduler Parameters
````
        --j/--jobs              -- Jobs to be scheduled [required]
        --rt/--releaseTimes     -- Release times for the jobs [required]
        --m/--mode              -- Mode options [optional]
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
        1 - Simple GPU Scheduler
        2 - As Early As Possible mode->AEAP
        3 - AEAP with As Late As Possible mode->AEAP/ALAP
        4 - AEAP/ALAP Bin Packer mode->AEAP/ALAP Pack
        5 - AEAP/ALAP BP with APLAP improver mode->AEAP/ALAP BP Improve
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
