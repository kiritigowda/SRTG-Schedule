# Soft Real-Time GPU - Job Creator

**SRTG-JobCreator** is comprehensive job set creation tool. The tool generates aperiodic soft-real-time job sets and associated data based on scheduler parameters.

## Scheduler Parameters 
* Job Arrival Rate - `Lambda` 
* `Number of GCUs Available` on the system
* `Even` / `Odd` GCU Requests by jobs
* `Delay Schedule GCU Limit`

The synthetic data generation is randomized to diversify and validate the use of SRTG-scheduler. The synthetic data not only has to exercise the main functionalities but also all the critical outliers and edge cases. The randomized synthetic data also helps to validate the scheduler more thoroughly as compared to real-world workloads which could not stress the system enough to understand how it would behave in cases of transient overloads or edge cases.
