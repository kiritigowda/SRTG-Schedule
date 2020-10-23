[![Build Status](https://travis-ci.org/kiritigowda/SRTG-Schedule.svg?branch=master)](https://travis-ci.org/kiritigowda/SRTG-Schedule)
[![codecov](https://codecov.io/gh/kiritigowda/SRTG-Schedule/branch/master/graph/badge.svg)](https://codecov.io/gh/kiritigowda/SRTG-Schedule)

# Dynamic Schedule Management Framework For GPUs

## Real-Time GPU Scheduler

RTG-Scheduler is a dynamic scheduler for aperiodic soft-real-time jobs on GPU based architectures, with a simple, easy-to-use command-line interface (CLI). The project is provided under the [MIT license](https://opensource.org/licenses/MIT). It is currently supported on Windows, Linux, and macOS platforms.

The RTG-scheduler itself resides on the CPU. When a GPU compatible job is released, it is sent to the SRTG-scheduler, which checks if this job can be executed on the GPU before its deadline. If yes, the job is offloaded to the GPU and any data that it needs is transferred to GPU memory. Otherwise, the job is rejected and sent back to the CPU scheduler immediately.

<p align="center"><img width="100%" src="../documents/images/RTG_Scheduler_block_diagram.png" /></p>

The scheduler supports multiple policies for scheduling aperiodic soft-real-time jobs on the GPU, captured via five different modes.

* **Mode 1** - Greedy Schedule
* **Mode 2** - Event Aware Schedule
* **Mode 3** - Event Aware Schedule with Bias
* **Mode 4** - Event Aware Schedule with Bias and Bias Prediction
* **Mode 5** - Event Aware Schedule with Bias and Improved Bias Prediction

**Input:** **Aperiodic Soft-Real-Time** jobs compatible with GPU execution

### Mode 1 - Greedy Schedule

<p align="center"><img width="60%" src="../documents/images/algo/gcu_request_satisfied.png" /></p>

<p align="center"><img width="60%" src="../documents/images/algo/deadline_fulfilled.png" /></p>

<p align="center"><img width="60%" src="../documents/images/algo/query_available_gcus.png" /></p>

<p align="center"><img width="60%" src="../documents/images/algo/gpu_offload.png" /></p>

<p align="center"><img width="60%" src="../documents/images/algo/mode_1.png"/></p>

### Mode 2 - Event Aware Schedule

<p align="center"><img width="60%" src="../documents/images/algo/mode_2.png" /></p>

### Mode 3 - Event Aware Schedule With Bias

<p align="center"><img width="60%" src="../documents/images/algo/schedule_with_bias.png" /></p>

<p align="center"><img width="60%" src="../documents/images/algo/mode_3.png" /></p>

### Mode 4 - Event Aware Schedule with Bias and Bias Prediction

<p align="center"><img width="60%" src="../documents/images/algo/schedule_bin_pack.png" /></p>

<p align="center"><img width="60%" src="../documents/images/algo/optimize_non_compute_intensive.png" /></p>

<p align="center"><img width="60%" src="../documents/images/algo/mode_4.png" /></p>

### Mode 5 - Event Aware Schedule with Bias and Improved Bias Prediction

<p align="center"><img width="60%" src="../documents/images/algo/optimize_compute_intensive.png" /></p>

<p align="center"><img width="60%" src="../documents/images/algo/mode_5.png" /></p>

## SRTG-JobCreator

We have developed a comprehensive job set creator tool, SRTG-JobCreator, that generates aperiodic soft-real-time job sets and associated data based on parameters such as job arrival rates, number of GCUs available, even/odd GCU requests and delay schedule GCU limit.

## SRTG-ResultAnalysis

Once the jobs are scheduled and the schedule data is obtained from SRTG-Scheduler, we use a result analysis tool, [SRTG-ResultAnalysis](SRTG-ResultAnalysis) to generate relevant data plots and provide insights into the results.

<p align="center"><img width="80%" src="SRTG-ResultAnalysis/srtg-results-images/srtg-result-1.png" /></p>

<p align="center"><img width="80%" src="SRTG-ResultAnalysis/srtg-results-images/srtg-result-3.png" /></p>

<p align="center"><img width="80%" src="SRTG-ResultAnalysis/srtg-results-images/srtg-result-4.png" /></p>

## RTG-Scheduler Usage

### Windows
```
RTG-scheduler [options] --j <jobs_file.csv>
                        --r <Release_Time_file.csv>
                        --m <option> 
                        --p <option> 
                        --d <option> 
                        --simulation <1/0>
                        --gpu <AMD/NVIDIA>
```
### Linux / MAC OS X
```
./RTG-scheduler [options] --j <jobs_file.csv>
                          --r <Release_Time_file.csv>
                          --m <option> 
                          --p <option> 
                          --d <option>
                          --simulation <1/0>
                          --gpu <AMD/NVIDIA>
```

### Scheduler Options Supported
````
        --h/--help      -- Show full help
        --v/--verbose   -- Show detailed messages
````

### Scheduler Parameters
````
        --j/--jobs                 -- Jobs to be scheduled [required]
        --r/--releaseTimes         -- Release times for the jobs [required]
        --m/--mode                 -- Scheduler Mode [optional - default:5]
        --p/--maxProcessors        -- Max processors available on the GPU [optional - default:16]
        --d/--delayLimitPercentage -- Delay Schedule processor limit in percentage [optional - default:60]
        --s/--simulation 	   -- simulation mode turn ON/OFF [optional - default:ON]
        --g/--gpu 	           -- Jobs Scheduled on hardware <AMD/NVIDIA> - [optional - default:OFF]
````

#### A-periodic Jobs -- The Jobs File is the list of Jobs to be scheduled: <jobs_file.csv>
```
        Jid     - Job ID
        Pn      - Processors Needed
        Texe    - Execution Time
        Td      - Deadline
        Tlts    - Lastest Time Schedulable on the GPU

        "Jid, Pn, Texe, Td, Tlts"
```

#### A-periodic Job Release Times -- The Release Time File has the list of release times of the kernels: <Release_Time_file.csv>
```
        Tr      - Release Time
        Jr      - Number of jobs released

        "Tr, Jr"
```

#### Scheduler Policies -- The Modes Supported: <mode option>
```
        1 - Greedy Schedule
        2 - Event Aware Scheduler
        3 - Event Aware Scheduler with Bias
        4 - Event Aware Scheduler with Bias and Bias Prediction
        5 - Event Aware Scheduler with Bias and Improved Bias Prediction
```
