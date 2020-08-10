# Dynamic Schedule Management Framework For GPU

## Aperiodic Soft-Real-Time Jobs On GPU Based Architectures

The SRTG-scheduler itself resides on the CPU. When a GPU compatible job is released, it is sent to the SRTG-scheduler, which checks if this job can be executed on the GPU before its deadline. If yes, the job is offloaded to the GPU and any data that it needs is transferred to GPU memory. Otherwise, the job is rejected and sent back to the CPU scheduler immediately.

The scheduler supports multiple policies for scheduling aperiodic soft-real-time jobs on the GPU, captured via five different modes.
* **Mode 1** - Greedy Schedule
* **Mode 2** - Event Aware Schedule
* **Mode 3** - Event Aware Schedule with Bias
* **Mode 4** - Event Aware Schedule with Bias and Bias Prediction
* **Mode 5** - Event Aware Schedule with Bias and Improved Bias Prediction

**Input:** **Aperiodic Soft-Real-Time** jobs compatible with GPU execution

### Mode 1 - Greedy Schedule

* Step 1: Check if the current available GCUs meet condition-1
* Step 2: If condition-1 is not met, reject job for GPU Execution. Else check for condition-2
* Step 3: If condition-2 is not met, reject job for GPU Execution. Else go to Step 4
* Step 4: Accept Job for GPU execution. Send the Job and Data required for execution for `IMMEDIATE` execution

#### **Algorithm 1:** GCU Request Satisfied
````
GCU_Request_Satisfied: GCU_Available, Job ::
    if( GCU_Available >= Job.GCU_Required):
        return true
    else
        return false
````

#### **Algorithm 2:** Deadline Fulfilled
```
Deadline_Fullfilled: Dispatch_Time, Job ::
    if( Dispatch_Time + Job.Execution_time <= Job.Deadline)
        return true
    else
        return false
```

#### **Algorithm 3:** Query Available GCUs
````
Query_Available_GCUs:
    TT = check_total_tokens()
    TD = check_tokens_distributed()
    GCU_Available = TT - TD
    return GCU_Available
````

#### **Algorithm 4:** Greedy Schedule
```
Mode_1: Job ::
    GCU_A = Query_Available_GCUs()
    status = check( GCU_Request_Satisfied( GCU_A, Job ) && Deadline_Fullfilled( present_time, Job ) )
    if( status == true )
        Update_GCU_Distribution_List( present_time, GCU_A, Job )
        Dispatch_Job( Job, IMMEDIATE_GPU_EXECUTION )
        return true
    else
        return false
```

### Mode 2 - Event Aware Schedule

* Step 1: Check if the current available GCUs meet condition-1
* Step 2: If condition-1 is not met with current available GUCs, check if condition-1 can be meet with the first future availability of GCUs.  
* Step 3: If condition-1 is met, check if condition-2 can be met when the current or future GCU availability. Else reject job for GPU Execution
* Step 4: If condition-1 & condition-2 are met, accept Job for GPU execution. Go to Step 5.
* Step 5: Send the Job and Data required for execution for `IMMEDIATE` execution or to the smart queue for `FUTURE` execution.

#### **Algorithm 5:** Event Aware Schedule
```
Mode_2: Job ::
    GCU_A = Query_Available_GCUs()
    if ( GCU_Request_Satisfied( GCU_A, Job ) )
    {
        Status = Mode_1( Job )
        return Status
    }
    else
    {
        while( GCU_Distribution_List != NULL )
        {
            ( Future_GCU_Available, Time_GCUs_Available ) = Query_Future_Available_GCU_Distribution_List( i )
            GCU_A = GCU_A + Future_GCU_Available
            Dispatch_Time = Present_Time + T_GCU_FA

            if ( !Deadline_Fullfilled( Dispatch_Time, Job ) )
                return false
                
            status = check( GCU_Request_Satisfied( GCU_A, Job ) )
            if( status == true )
                Update_GCU_Distribution_List( Dispatch_Time, GCU_A, Job )
                Dispatch_Job( Job, FUTURE_GPU_EXECUTION )
                return true
                
            i = i->next
        }
    }
```

### Mode 3 - Event Aware Schedule with Bias

* Step 1: Check if the job is compute-intensive or non-compute-intensive. If the job is non-compute-intensive go to Step 2, else go to Step 7.
* Step 2: Check if the current available GCUs meet condition-1
* Step 3: If condition-1 is not met with current available GUCs, check if condition-1 can be meet with the first future availability of GCUs.  
* Step 4: If condition-1 is met, check if condition-2 can be met when the current or future GCU availability. Else reject job for GPU Execution
* Step 5: If condition-1 & condition-2 are met, accept Job for GPU execution. Go to Step 6.
* Step 6: Send the Job and Data required for execution for `IMMEDIATE` execution or to the smart queue for `FUTURE` execution and return.
* Step 7: Check the latest time the job can be scheduled for GPU execution.
* Step 8: Check if condition-1 can be meet with the GCUs available at the latest schedulable time.
* Step 9: If condition-1 is not meet at the latest schedulable time, reject job for GPU Execution. Else go to Step 10.
* Step 10: Accept the job for GPU execution and send the job to the smart queue for `ALAP` execution and return.

#### **Algorithm 6:** Event Aware Schedule with Bias
```
Mode_3: Job ::
    Status = Query_Compute_Intensive_Job( Job )
    if( Status == False )
    {
        Status = Mode_2( Job )
        return Status
    }
    else
    {
        GCU_A = Query_Available_GCUs()
        ALAP_Dispatch_Time = Job.latest_time_schedulable
        status = check( GCU_Request_Satisfied( GCU_A, Job ) )
        if( status == true )
            Update_GCU_Distribution_List( ALAP_Dispatch_Time, GCU_A, Job )
            Dispatch_Job( Job, ALAP_GPU_EXECUTION )
            return true
        else
        {
            while( GCU_Distribution_List != NULL )
            {
                ( GCU_FA, T_GCU_FA ) = Query_Future_Available_GCU_Distribution_List( i )
                GCU_A = GCU_A + GCU_FA
                Dispatch_Time = Present_Time + T_GCU_FA

                if ( !Deadline_Fullfilled( Dispatch_Time, Job ) )
                    return false

                status = check( GCU_Request_Satisfied( GCU_A, Job ) )
                if( status == true )
                    Update_GCU_Distribution_List( ALAP_Dispatch_Time, GCU_A, Job )
                    Dispatch_Job( Job, ALAP_GPU_EXECUTION )
                    return true
                    
                i = i->next
            }
        }
    }
```

### Mode 4 - Event Aware Schedule with Bias and Bias Prediction

* Step 1: Check if the job is compute-intensive or non-compute-intensive. If the job is non-compute-intensive go to Step 2, else go to Step 8.
* Step 2: Check if the current available GCUs meet condition-1
* Step 3: If condition-1 is not met with current available GUCs, check if condition-1 can be meet with the first future availability of GCUs.
* Step 4: If condition-1 is not meet, try to tap GCUs in between two compute intensive jobs
* Step 5: If condition-1 is met, check if condition-2 can be met when the current or future GCU availability. Else reject job for GPU Execution
* Step 6: If condition-1 & condition-2 are met, accept Job for GPU execution. Go to * Step 6.
* Step 7: Send the Job and Data required for execution for `IMMEDIATE` execution or to the smart queue for `FUTURE` execution and return.
* Step 8: Check the latest time the job can be scheduled for GPU execution.
* Step 9: Check if condition-1 can be meet with the GCUs available at the latest schedulable time.
* Step 10: If condition-1 is not meet at the latest schedulable time, reject job for GPU Execution. Else go to Step 10.
* Step 11: Accept the job for GPU execution and send the job to the smart queue for `ALAP` execution and return.

#### **Algorithm 7:** Schedule In-Between Compute Intensive Jobs
```
Optimize_Non_Compute_Intensive_Job_Schedule: Job ::
    while( GCU_Distribution_List != NULL )
    {
        status = Check ( Query_List_For_Consecutive_Compute_Intensive_Jobs( i ) )
        if(status == true)
            (Job1, Job2) = List_Consecutive_Compute_Intensive_Jobs( i )
            status = check( Job.Exection_time <= Query_Time_Gap_Between_Compute_Intensive_Jobs(i) )
            if(status == true)
                Dispatch_Time = Job1.Completion_Time

                if( !Deadline_Fullfilled( Dispatch_Time, Job ) )
                    return false

                GCU_A = Job2.GCU_Required
                status = check( GCU_Request_Satisfied( GCU_A, Job ) && Query_Time_Gap_Empty() )
                if(status == true)
                    Flag_Job_With_Virtual_Token( Job )
                    Update_GCU_Virtual_Distribution_List( Dispatch_Time, GCU_A, Job )
                    Dispatch_Job( Job, ALAP_TIME_GAP_GPU_EXECUTION )
                    return true

                status = check( GCU_Request_Satisfied( GCU_A, Job ) && !Query_Time_Gap_Empty() )
                if(status == true)
                    GCU_V = Query_Virtual_Tokens_Inbtw_Jobs(Job1, Job2)
                    GCU_A = GCU_A - GCU_V
                    status = GCU_Request_Satisfied( GCU_A, Job )
                    if(status == true)
                        Flag_Job_With_Virtual_Token( Job )
                        Update_GCU_Virtual_Distribution_List( Dispatch_Time, GCU_A, Job )
                        Dispatch_Job( Job, ALAP_TIME_GAP_GPU_EXECUTION )
                        return true
                    else
                    {
                        Virtual_Distribution_List = Query_Virtual_List_Inbtw_Jobs( Job1, Job2 )
                        while( Virtual_Distribution_List != NULL )
                        {
                            ( GCU_FA, T_GCU_FA ) = Query_Future_Available_Virtual_Distribution_List( i )
                            GCU_A = GCU_A + GCU_FA
                            Dispatch_Time = Dispatch_Time + T_GCU_FA
                            if ( !Deadline_Fullfilled( Dispatch_Time, Job ) )
                                return false                
                            status = check( GCU_Request_Satisfied( GCU_A, Job ) )
                            if( status == true )
                                Flag_Job_With_Virtual_Token( Job )
                                Update_GCU_Virtual_Distribution_List( Dispatch_Time, GCU_A, Job )
                                Dispatch_Job( Job, ALAP_TIME_GAP_GPU_EXECUTION )
                                return true
                            i = i->next
                        }                       
                    }
        i = i->next
    }
```

#### **Algorithm 8:** Event Aware Schedule with Bias and Bias Predicition
```
Mode_4: Job ::
    Status = Query_Compute_Intensive_Job( Job )
    if( Status == False)
    {
        Status = Mode_2( Job )
        if( Status == false )
        {
            Status = Optimize_Non_Compute_Intensive_Job_Schedule( Job )
        }
        return Status
    }
    else
    {
        GCU_A = Query_Available_GCUs()
        ALAP_Dispatch_Time = Job.latest_time_schedulable
        status = check( GCU_Request_Satisfied( GCU_A, Job ) )
        if( status == true )
            Update_GCU_Distribution_List( ALAP_Dispatch_Time, GCU_A, Job )
            Dispatch_Job( Job, ALAP_GPU_EXECUTION )
            return true
        else
        {
            while( GCU_Distribution_List != NULL )
            {
                ( GCU_FA, T_GCU_FA ) = Query_Future_Available_GCU_Distribution_List( i )
                GCU_A = GCU_A + GCU_FA
                Dispatch_Time = Present_Time + T_GCU_FA
                if ( !Deadline_Fullfilled( Dispatch_Time, Job ) )
                    return false                
                status = check( GCU_Request_Satisfied( GCU_A, Job ) )
                if( status == true )
                    Update_GCU_Distribution_List( ALAP_Dispatch_Time, GCU_A, Job )
                    Dispatch_Job( Job, ALAP_GPU_EXECUTION )
                    return true                       
                i = i->next
            }
        }
    }
```

### Mode 5 - Event Aware Schedule with Bias and Advanced Bias Prediction

* Step 1: Check if the job is compute-intensive or non-compute-intensive. If the job is non-compute-intensive go to Step 2, else go to Step 8.
* Step 2: Check if the current available GCUs meet condition-1
* Step 3: If condition-1 is not met with current available GUCs, check if condition-1 can be meet with the first future availability of GCUs.
* Step 4: If condition-1 is not meet, try to tap GCUs in between two compute intensive jobs and allow for moving compute intensive job for early dispatch
* Step 5: If condition-1 is met, check if condition-2 can be met when the current or future GCU availability. Else reject job for GPU Execution
* Step 6: If condition-1 & condition-2 are met, accept Job for GPU execution. Go to Step 6.
* Step 7: Send the Job and Data required for execution for `IMMEDIATE` execution or to the smart queue for `FUTURE` execution and return.
* Step 8: Check the latest time the job can be scheduled for GPU execution.
* Step 9: Check if condition-1 can be meet with the GCUs available at the latest schedulable time.
* Step 10: If condition-1 is not meet at the latest schedulable time, reject job for GPU Execution. Else go to Step 10.
* Step 11: Accept the job for GPU execution and send the job to the smart queue for `ALAP` execution and return.

#### **Algorithm 9:** Reschedule Compute Intensive Jobs And Optimize
````
Optimize_Compute_Intensive_Job_Schedule:
    while( GCU_Distribution_List != NULL )
    {
        status = Check ( Query_List_For_Consecutive_Compute_Intensive_Jobs( i ) )
        if(status == true)
            (Job1, Job2) = List_Consecutive_Compute_Intensive_Jobs( i )
            status = Consecutive_Compute_Intensive_Jobs_Analyzed( Job1, Job2 )
            if(status == true)
                if(Query_Time_Gap_Empty(Job1, Job2))
                    Dispatch_Time = Job1.Completion_Time
                    Update_Distribution_List( Dispatch_Time, Job2 )
                    Update_Dispatched_Job( Job2, ALAP_GPU_EXECUTION )
                    return
                else
                    Virtual_Distribution_List = Query_Virtual_List_Inbtw_Jobs( Job1, Job2 )
                    Dispatch_Time = Query_Completion_time(Virtual_Distribution_List)
                    Update_Distribution_List( Dispatch_Time, Job2 )
                    Update_Dispatched_Job( Job2, ALAP_GPU_EXECUTION )
                    return
        i = i->next
    }
    return
````


#### **Algorithm 10:** Event Aware Schedule with Bias and Improved Bias Predicition
```
Mode_5: Job ::
    Status = Query_Compute_Intensive_Job( Job )
    if( Status == False)
    {
        Status = Mode_2( Job )
        if( Status == false )
        {
            Status = Optimize_Non_Compute_Intensive_Job_Schedule( Job )
            if( Status == false )
            {
                Optimize_Compute_Intensive_Job_Schedule()
            }
        }
        return Status
    }
    else
    {
        GCU_A = Query_Available_GCUs()
        ALAP_Dispatch_Time = Job.latest_time_schedulable
        status = check( GCU_Request_Satisfied( GCU_A, Job ) )
        if( status == true )
            Update_GCU_Distribution_List( ALAP_Dispatch_Time, GCU_A, Job )
            Dispatch_Job( Job, ALAP_GPU_EXECUTION )
            return true
        else
        {
            while( GCU_Distribution_List != NULL )
            {
                ( GCU_FA, T_GCU_FA ) = Query_Future_Available_GCU_Distribution_List( i )
                GCU_A = GCU_A + GCU_FA
                Dispatch_Time = Present_Time + T_GCU_FA

                if ( !Deadline_Fullfilled( Dispatch_Time, Job ) )
                    return false
                        
                status = check( GCU_Request_Satisfied( GCU_A, Job ) )
                if( status == true )
                    Update_GCU_Distribution_List( ALAP_Dispatch_Time, GCU_A, Job )
                    Dispatch_Job( Job, ALAP_GPU_EXECUTION )
                    return true
            
                i = i->next
            }
        }
    }
````
