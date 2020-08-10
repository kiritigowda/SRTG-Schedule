[![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://travis-ci.org/kiritigowda/SRTG-Schedule.svg?branch=master)](https://travis-ci.org/kiritigowda/SRTG-Schedule)
[![codecov](https://codecov.io/gh/kiritigowda/SRTG-Schedule/branch/master/graph/badge.svg)](https://codecov.io/gh/kiritigowda/SRTG-Schedule)
[![CodeFactor](https://www.codefactor.io/repository/github/kiritigowda/srtg-schedule/badge)](https://www.codefactor.io/repository/github/kiritigowda/srtg-schedule)
<a href="https://codeclimate.com/github/kiritigowda/SRTG-Schedule/maintainability"><img src="https://api.codeclimate.com/v1/badges/dc43dc1b470535e927ac/maintainability" /></a>

# Dynamic Schedule Management Framework For GPUs

## Soft Real-Time GPU Scheduler

A schedule management framework for soft-real-time jobs that may be used by a CPU - GPU system designer/integrator to select, configure, and deploy a suitable architectural platform and to perform concurrent scheduling of these jobs. 

[Real-Time GPU Scheduler](RTG-scheduler) (RTG-Scheduler) is a dynamic scheduler for aperiodic soft-real-time jobs on GPU based architectures, with a simple, easy-to-use command-line interface (CLI). The SRTG-Scheduler is provided under the [MIT license](https://opensource.org/licenses/MIT). It is currently supported on Windows, Linux, and macOS platforms.

### Dynamic schedule management framework for aperiodic soft-real-time jobs on GPU based architectures

Graphics Processing Units (GPUs) are computational powerhouses that were originally designed for accelerating graphics applications. However, in recent years, there has been a tremendous increase in support for general-purpose computing on GPUs (GPGPU). GPU based architectures provide unprecedented magnitudes of computation at a fraction of the power used by traditional CPU based architectures. As real-time systems integrate more and more functionality, GPU based architectures are very attractive for their deployment.

Based on where they are located and how they are used in a computing system, GPUs may be broadly classified into discrete GPUs (dGPUs), integrated GPUs (iGPUs), virtual GPUs (vGPU), and external GPUs (eGPUs). Personal computers generally use either discrete, also known as dedicated, GPUs (dGPUs) or integrated, also known as shared or unified memory architecture, GPUs (iGPUs).<a href="#note1" id="note1ref"><sup>[1]</sup></a>

<p align="center"><img width="60%" src="documents/images/RTGS-ConcurrentJobExecution.PNG" /></p>

However, in a real-time system, predictability and meeting temporal requirements are much more important than raw performance. While some realtime jobs may benefit from the performance that all cores of the GPU can provide, most jobs may require only a subset of cores to successfully meet their temporal requirements.  In this work, we implement concurrent scheduling of a-periodic soft-real-time jobs on a GPU based platform, while optimizing the memory usage on the GPGPU.

In real-time jobs, the concept of a deadline is fundamental, i,e., a real-time compute workload must finish its execution and generate the results to the consumed by another real-time compute workload. These jobs are generally termed as hard or soft real-time jobs based on their deadline fulfillment requirement. This concept of the deadline is achievable because these jobs have a very reliable execution time. To achieve reliable execution time one needs to get accurate estimates of the worst-case execution time (WCET) of the job for specific hardware architectures. Another major differentiating characteristic of the hard or soft real-time job is the priority of jobs, which means that a job with a higher priority has the right to execute before the job with a lower priority. These characteristics of the real-time jobs must be meet to deploy them safely on a GPU based architecture.

Jobs, also known as **kernels**, that are dispatched to the GPU for execution are usually jobs that involve performing the same set of operations on large volumes of data, i.e., jobs that map to a Single Instruction stream, Multiple Data stream (SIMD) model of parallelism. The GPU executes such jobs by launching many typically independent threads that perform a sequence of operations on a single data element. This feature allows GPUs to get unprecedented speed-ups when compared to other types of processors. 

However, this feature also makes it challenging to safely preempt a job. When a job is preempted, its context needs to be stored so that the job can resume execution from the point at which it was preempted. However, on GPUs, there is no known method to safely storing the context of each executing thread. This makes preemption of a GPU job infeasible --- the job would essentially have to be restarted and intermediate results discarded. In early versions of GPU platforms, only one instruction stream or function represented by a kernel could execute on a GPU at any given time, regardless of GPU utilization.

Our motivation is to use the RTG-Scheduler in a system which has both CPU and GPU, which could handle the large compute workload generated by a plethora of sensors used in today's real-time applications effectively by off-loading as many jobs are possible on to the GPU and keeping the CPU open for additional compute.

<p align="center"><img width="70%" src="documents/images/RTG-Scheduler.PNG" /></p>

**The above image depicts our framework. As seen in the figure, our scheduling framework resides on a CPU core and dispatches kernels to the GPU.**

<p align="center"><img width="70%" src="documents/images/RTGS-Thread_life_Cycle.PNG" /></p>

**note:**

<a id="note1" href="#note1ref"><sup>[1]</sup></a>The disparity in the nomenclature of these GPUs is due to the fact that naming conventions of GPUs are dependent on their manufacturers.

