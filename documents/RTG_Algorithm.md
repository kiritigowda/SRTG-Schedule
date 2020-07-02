# RTG Scheduler Modes

**Input:** Aperiodic jobs compatible with GPU execution

## Mode 1 - Greedy Schedule

* Step 1: Check if the current available GCUs meet condition-1
* Step 2: If condition-1 is not met, send the job back for CPU evaluation. Else check for condition-2
* Step 3: If condition-2 is not met, send the job back for CPU evaluation. Else go to * Step 4
* Step 4: Accept Job for GPU execution. Send the Job and Data required for execution for immediate execution

````
Algorithm 1: GCU Request Satisfied
    GCU_Request_Satisfied: GCU_Available, Job ::
        if( GCU_Available >= Job.GCU_Required):
            return true
        else
            return false

Algorithm 2: Deadline Fulfilled
    Deadline_Fullfilled: Dispatch_Time, Job ::
        if( Dispatch_Time + Job.Execution_time <= Job.Deadline)
            return true
        else
            return false

Algorithm 3: Query Available GCUs
	Query_Available_GCUs:
		TT = check_total_tokens()
		TD = check_tokens_distributed()
		GCU_Available = TT - TD
        return GCU_Available

Algorithm 4: Greedy Schedule
    Mode_1: Job ::
		GCU_A = Query_Available_GCUs()
		status = check( GCU_Request_Satisfied( GCU_A, Job ) && Deadline_Fullfilled( present_time, Job ) )
			if( status == true )
				Update_GCU_Distribution_List( present_time, GCU_A, Job )
				Dispatch_Job( Job, IMMEDIATE_GPU_EXECUTION )
                return true
			else
				CPU_Evaluation( Job )
                return false
````

## Mode 2 - Event Aware Schedule

* Step 1: Check if the current available GCUs meet condition-1
* Step 2: If condition-1 is not met with current available GUCs, check if condition-1 can be meet with the first future availability of GCUs.  
* Step 3: If condition-1 is met, check if condition-2 can be met when the current or future GCU availability. Else send the job back for CPU evaluation
* Step 4: If condition-1 & condition-2 are met, accept Job for GPU execution. Go to * Step 5.
* Step 5: Send the Job and Data required for execution for immediate execution or to the smart queue for future execution.

````
Algorithm 5: Query Future Available GCUs
    Query_Future_Available_GCUs: List_ID ::
		( Future_GCU_Available, Time_GCUs_Available ) = Query_GCU_Distribution_List( List_ID )
        return ( Future_GCU_Available, Time_GCUs_Available )

Algorithm 6: Event Aware Schedule
	Mode_2: Job ::
        GCU_A = Query_Available_GCUs()
        if ( GCU_Request_Satisfied( GCU_A, Job ) )
        {
            Mode_1( Job )
        }
        else
        {
            while( GCU_Distribution_List != NULL )
            {
                ( GCU_FA, T_GCU_FA ) = Query_Future_Available_GCUs( i )
                GCU_A = GCU_A + GCU_FA
                Dispatch_Time = Present_Time + T_GCU_FA
                if ( !Deadline_Fullfilled( Dispatch_Time, Job ) )
                    CPU_Evaluation( Job )
					return false
                
                status = check( GCU_Request_Satisfied( GCU_A, Job ) )
                if( status == true )
                    Update_GCU_Distribution_List( Dispatch_Time, GCU_A, Job )
                    Dispatch_Job( Job, FUTURE_GPU_EXECUTION )
                    return true
                
                i = i->next
            }
        }
````

## Mode 3 - Event Aware Schedule with Bias

* Step 1: Check if the job is compute-intensive or non-compute-intensive. If the job is non-compute-intensive go to * Step 2, else go to * Step 7.
* Step 2: Check if the current available GCUs meet condition-1
* Step 3: If condition-1 is not met with current available GUCs, check if condition-1 can be meet with the first future availability of GCUs.  
* Step 4: If condition-1 is met, check if condition-2 can be met when the current or future GCU availability. Else send the job back for CPU evaluation
* Step 5: If condition-1 & condition-2 are met, accept Job for GPU execution. Go to * Step 6.
* Step 6: Send the Job and Data required for execution for immediate execution or to the smart queue for future execution and return.
* Step 7: Check the latest time the job can be scheduled for GPU execution.
* Step 8: Check if condition-1 can be meet with the GCUs available at the latest schedulable time.
* Step 9: If condition-1 is not meet at the latest schedulable time, send the job for CPU evaluation. Else go to * Step 10.
* Step 10: Accept the job for GPU execution and send the job to the smart queue for future execution and return.

````
Algorithm 7: Event Aware Schedule with Bias
	Mode_3: Job ::
        Status = Query_Compute_Intensive_Job( Job )
		if( Status == False )
		{
            Mode_2( Job )
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
                    ( GCU_FA, T_GCU_FA ) = Query_Future_Available_GCUs( i )
                    GCU_A = GCU_A + GCU_FA
                    Dispatch_Time = Present_Time + T_GCU_FA
                    if ( !Deadline_Fullfilled( Dispatch_Time, Job ) )
                        CPU_Evaluation( Job )
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

## Mode 4 - Event Aware Schedule with Bias and Bias Prediction

* Step 1: Check if the job is compute-intensive or non-compute-intensive. If the job is non-compute-intensive go to * Step 2, else go to * Step 8.
* Step 2: Check if the current available GCUs meet condition-1
* Step 3: If condition-1 is not met with current available GUCs, check if condition-1 can be meet with the first future availability of GCUs.
* Step 4: If condition-1 is not meet, try to tap GCUs in between two compute intensive jobs
* Step 5: If condition-1 is met, check if condition-2 can be met when the current or future GCU availability. Else send the job back for CPU evaluation
* Step 6: If condition-1 & condition-2 are met, accept Job for GPU execution. Go to * Step 6.
* Step 7: Send the Job and Data required for execution for immediate execution or to the smart queue for future execution and return.
* Step 8: Check the latest time the job can be scheduled for GPU execution.
* Step 9: Check if condition-1 can be meet with the GCUs available at the latest schedulable time.
* Step 10: If condition-1 is not meet at the latest schedulable time, send the job for CPU evaluation. Else go to * Step 10.
* Step 11: Accept the job for GPU execution and send the job to the smart queue for future execution and return.

````
Algorithm 8: Schedule In-Between Compute Intensive Jobs
    Optimize_Non_Compute_Intensive_Job_Schedule: Job ::
        while( GCU_Distribution_List != NULL )
        {
            status = Check ( Query_List_For_Consecutive_Compute_Intensive_Jobs( i ) )
            if( status == true )
                (Job1, Job2) = List_Consecutive_Compute_Intensive_Jobs( i )
                status = check( Job.Exection_time <= Query_Time_Gap_Between_Compute_Intensive_Jobs(i) )
                if( status == true )
                    Dispatch_Time = Job1.Completion_Time
                    if ( !Deadline_Fullfilled( Dispatch_Time, Job ) )
                        CPU_Evaluation( Job )
                        return false
                    GCU_A = Job2.GCU_Required
                    status = check( GCU_Request_Satisfied( GCU_A, Job ) && Query_Time_Gap_Empty() )
					if status == true
                        Flag_Job_With_Virtual_Token( Job )
                        Update_GCU_Distribution_List( Dispatch_Time, GCU_A, Job )
                        Dispatch_Job( Job, ALAP_TIME_GAP_GPU_EXECUTION )
                        return true
                    status = check( GCU_Request_Satisfied( GCU_A, Job ) && !Query_Time_Gap_Empty() )
                    if status == true
                        TBD:: (Add evaluation on NON Compute Jobs accepted)
            
            i = i->next
        }

Algorithm 9: Event Aware Schedule with Bias and Bias Predicition
	Mode_4: Job ::
        Status = Query_Compute_Intensive_Job( Job )
		if( Status == False)
		{
            Status = Mode_2( Job )
            if( Status == false )
            {
                Cancel_CPU_Evaluation( Job )
                Optimize_Non_Compute_Intensive_Job_Schedule( Job )
            }
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
                    ( GCU_FA, T_GCU_FA ) = Query_Future_Available_GCUs( i )
                    GCU_A = GCU_A + GCU_FA
                    Dispatch_Time = Present_Time + T_GCU_FA
                    if ( !Deadline_Fullfilled( Dispatch_Time, Job ) )
                        CPU_Evaluation( Job )
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

# Mode 5 - Event Aware Schedule with Bias and Advanced Bias Prediction

* Step 1: Check if the job is compute-intensive or non-compute-intensive. If the job is non-compute-intensive go to * Step 2, else go to * Step 8.
* Step 2: Check if the current available GCUs meet condition-1
* Step 3: If condition-1 is not met with current available GUCs, check if condition-1 can be meet with the first future availability of GCUs.
* Step 4: If condition-1 is not meet, try to tap GCUs in between two compute intensive jobs and allow for moving compute intensive job for early dispatch
* Step 5: If condition-1 is met, check if condition-2 can be met when the current or future GCU availability. Else send the job back for CPU evaluation
* Step 6: If condition-1 & condition-2 are met, accept Job for GPU execution. Go to * Step 6.
* Step 7: Send the Job and Data required for execution for immediate execution or to the smart queue for future execution and return.
* Step 8: Check the latest time the job can be scheduled for GPU execution.
* Step 9: Check if condition-1 can be meet with the GCUs available at the latest schedulable time.
* Step 10: If condition-1 is not meet at the latest schedulable time, send the job for CPU evaluation. Else go to * Step 10.
* Step 11: Accept the job for GPU execution and send the job to the smart queue for future execution and return.

````
Algorithm 10: Schedule In-Between Compute Intensive Jobs And Optimize 
Optimize_Non_Compute_And_Compute_Intensive_Job_Schedule: Job ::
    while( GCU_Distribution_List != NULL )
    {
        status = Check ( Query_List_For_Consecutive_Compute_Intensive_Jobs( i ) )
        if( status == true )
            (Job1, Job2) = List_Consecutive_Compute_Intensive_Jobs( i )
            status = check( Job.Exection_time <= Query_Time_Gap_Between_Compute_Intensive_Jobs(i) )
            if( status == true )
                Dispatch_Time = Job1.Completion_Time
                if ( !Deadline_Fullfilled( Dispatch_Time, Job ) )
                    CPU_Evaluation( Job )
                    return false
                GCU_A = Job2.GCU_Required
                status = check( GCU_Request_Satisfied( GCU_A, Job ) && Query_Time_Gap_Empty() )
                if status == true
                    Flag_Job_With_Virtual_Token( Job )
                    Update_GCU_Distribution_List( Dispatch_Time, GCU_A, Job )
                    Dispatch_Job( Job, ALAP_TIME_GAP_GPU_EXECUTION )
                    return true
                status = check( GCU_Request_Satisfied( GCU_A, Job ) && !Query_Time_Gap_Empty() )
                if status == true
                    TBD:: (Add evaluation on NON Compute Jobs accepted)
            else TBD::
                Dispatch-Time = Compute Intensive J1 completion time
                Move Compute Intensive J2 to Dispatch-Time
                status = check (Scheduling Ji after J2)
                if status == true
                        Allocate Virtual Tokens
                        GCU_distributed_list update
                        dispatch job for look ahead GPU execution
        
        i = i->next
    }

Algorithm 11: Event Aware Schedule with Bias and Improved Bias Predicition
    Mode_5: Job ::
        Status = Query_Compute_Intensive_Job( Job )
        if( Status == False)
        {
            Status = Mode_2( Job )
            if( Status == false )
            {
                Cancel_CPU_Evaluation( Job )
                Optimize_Non_Compute_Intensive_Job_Schedule( Job )
            }
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
                    ( GCU_FA, T_GCU_FA ) = Query_Future_Available_GCUs( i )
                    GCU_A = GCU_A + GCU_FA
                    Dispatch_Time = Present_Time + T_GCU_FA
                    if ( !Deadline_Fullfilled( Dispatch_Time, Job ) )
                        CPU_Evaluation( Job )
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
