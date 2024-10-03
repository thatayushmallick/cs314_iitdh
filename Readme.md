# OPERATING-SYSTEM-LAB
## Assignment-1
#### Step-1
+ added padding as when a kernel is passed through a "HxW" matrix it shorts it to "H-2 X W-2", so a padding can change matrix to "H+2 X W+2".
#### Step-2
+ smoothening can be done when a (3x3) matrix of (1/9s) are passed over the padded image.
#### Step-3 
+ details can be extracted when smoothened image matrix is substracted from original matrix, keeping in mind that it should be between 0-255.
#### Step-4 
+ final sharpened image can be aquired by adding detailed matrix to original image.

to run the file just run the bash script.

## Assignment-2
#### Part-1
+ just needed to apply a linear search for matching the pattern. applied here using sliding window concept.
+ to execute: (1) make build-part1 (2) make run-part1
#### Part-2
+ now we need to distribute this work parallely, so we define a maximum chunk-size that is only to be searched not more than that.
+ for this we fork our parent into two childs & each child if greater than max chunk-size forks again (implemented using execv()), and each parent waits for child to complete (implemented this using waitpid()).
+ once chunk-size is adjusted we fork again to run part1-searcher in given chunk space (implemeted using execv).
+ to execute: (1) make build-part2 (2) make run-part2
#### Part-3
+ when finding parallely, if one process finds the pattern it should kill other exisiting process as running them seems a waste of resource.
+ a killpg (kill-process-group) command passes signal 15 to the current process group killing every process inside the given process group.
+ to execute: (1) make build-part3 (2) make run-part3

## Assignment-3
#### Part-1
+ We are using the continous approach to solve this problem, i.e we increase the time by 1 quant and do the required tasks.
+ making a scheduler with one cpu, and apply different scheduling algorithm like FIFO,SJF,SRTF&RR.
+ here class for process was made, so that each process has their unique property which can be changed independently.
+ The Process Class: It has attributes: pid(process id), arrival_time, cpu_burst_queue, io_burst_queue, burst_no(burst_no of currently processing burst.), ready_q_arrival(notes the time when process arrived at ready_q), ready_q_departure(notes the the time process exit the ready_q and calculates and add up waiting_time). And methods: get-set-pid, get-set-arrivaltime, get-set-cpuBurst, current_cpu_burst(returns the process's current cpu burst), current_io_burst(returns process's current io burst), setCurrentCpuBurst(sets the given integer on front of cpuBurst), popIOBurst(pops the front of io_queue), set-get-current-burst-position(which handle the current burst's position number), set_r_q_arrival(sets the time when process arrived the ready_q), calculate_wait_time(calculates the wait time when process comes out of ready_q), get_wait_time(returns wait time)
+ CPU class: *ATTRIBUTES:* process_pid of currently running process, time_left of cpu burst of current process, Process* a pointer to process itself. *METHODS:* get/set_process_pid, get/set_time_left, get/set_current_process.
+ Every algorithm has basic structure:-
 1. An always true loop is initialised 
 2. Until last process has not yet arrived try to throw new process to ready_q
 3. check for breaking condition
 4. do_io as there might be some process that might enter ready_q
 5. if cpu is empty and there is something in ready_q push the process to cpu
 6. if there is something in cpu check for following:
  1.if current process cpu burst is finished or not, if yes then push this process to waiting_q, i.e it goes for IO.
  2.if not then <<DO_CPU_WORK>>
+ FIFO: here cpu work is default
+ SJF: here if cpu burst is finished, then next process is chosen from prioritised waiting_q
+ SRTF: here everything is same as SJF, just when in DO_CPU_WORK if there is some process whose cpu_burst is smaller than current process they get replaced. 
+ RR: Here everything is same as FIFO, except while doing CPU_WORK if time_quant of given process is finished then it is replaced by process in front of ready_q

#### Part-2
+ Here everything is same as part-1 except we have two cpus.