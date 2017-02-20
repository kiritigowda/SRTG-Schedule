# RTGS
The current release verion is 0.9 (beta preview).

## RTGS USER GUIDE

### DESCRIPTION
This guide provides an overview of the content and usage of the Scheduler tool. This tool is used to schedule jobs on a hetrogenous computing enviornment, with a simple, easy-to-use interface. It encapsulates most of the real time constraints, thus scheduling jobs safely.

####Scheduler Functions for Stage - 1 
* Scheduler Main – This function resides on the CPU and dispatches work to the GPU 
* Get Kernel Information & Get Kernel Release Times– Gets the user’s Kernels for dispatch 
* Kernel  Book  Keeper –  When  a  kernel  is  released  the  Scheduler  calls  this  function  as  it  has  all  the  information  of  the  available  SMs and future release times 
* Queue kernel execution– The function safely dispatches the kernels on the GPU and allocates the kernel with required SMs 
* Retrieve  Processors –  Releases  all  the  Kernels  which  have  completed  their  execution  and  frees  up  the  Streaming  Multiprocessors (SM) 

####Scheduler Functions for Stage - 2 
* Processors  Unavailable –  If  processors  are  not  available  at  the  present  times,  this  function  safes  schedules  the  kernels  on  to  the future available time 
* AEAP SCHEDULER – The future scheduler calls the As Early As Possible algorithm to schedule the Kernel on to the GPU 
* Queue kernel  handler –  The  function  safely  queues  kernels  to  be  dispatched  on  to  the  GPU  and  allocates  the  kernel  with  required SMs 
* Dispatch Queued Kernels – Dispatches the Kernels set for future release times  

####Scheduler Functions for Stage - 3 
* ALAP SCHEDULER– If the Kernel needs a large chunk of the SMs, then they are scheduled As Late As Possible on to the GPU 

####Scheduler Functions for Stage - 4 
* AEAP Flagged– once a kernel is scheduled for a  ALAP, this function tries to fit the kernels  before the ALAP kernel release time and after if not possible after the ALAP kernel execution. 
* ALAP Flagged– if another Kernel requires a large chunk of SMs, then it is scheduled ALAP by this function. It takes into account that there are other ALAP kernels scheduled and does not allow overlap.

####Scheduler Functions for Stage - 5
* ALAP  improve–  if  an  ALAP  set  Kernel  can  be  scheduled  without  interfering  with  other  kernels  and  if  it  improves  response  time  of another kernel, the ALAP is called on to modify the schedule to improve system response times.
