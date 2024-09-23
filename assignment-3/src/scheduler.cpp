#include <iostream>
#include <bits/stdc++.h>
using namespace std;

int MAX_WAIT_TIME = 0;
// process class having all the methods and props of an process
class Process
{
private:
  int pid;
  int arrival_time;
  queue<int> cpu_bursts;
  queue<int> i_o_bursts;
  int burst_no = 1;
  int ready_q_arrival;
  int ready_q_departure;
  int wait_time = 0;
public:
  int getPid()
  {
    return pid;
  }
  void setPid(int n_pid)
  {
    pid = n_pid;
  }
  int getArrivalTime()
  {
    return arrival_time;
  }
  void setArrivalTime(int n_a_t)
  {
    arrival_time = n_a_t;
    ready_q_arrival = arrival_time;
  }
  queue<int> getCpuBursts()
  {
    return cpu_bursts;
  }
  void setCpuBursts(queue<int> n)
  {
    cpu_bursts = n;
  }
  int currentCPUburst()
  {
    if (!cpu_bursts.empty())
    {
      return cpu_bursts.front();
    }
    else
    {
      return -1;
    }
  }
  int currentIOburst()
  {
    if (!i_o_bursts.empty())
    {
      return i_o_bursts.front();
    }
    else
    {
      return -1;
    }
  }
  void setCurrentCpuBurst(int n)
  {
    cpu_bursts.front() = n;
  }
  void popCpuBurst()
  {
    cpu_bursts.pop();
  }
  queue<int> getIOBursts()
  {
    return i_o_bursts;
  }
  void setIOBursts(queue<int> n)
  {
    i_o_bursts = n;
  }
  void reduceIOBurst()
  {
    i_o_bursts.front()--;
  }
  void popIOBurst()
  {
    i_o_bursts.pop();
  }
  int curr_burst_pos(){
    return burst_no;
  }
  void set_burst_pos(int n){
    burst_no = n;
  }
  void set_r_q_arrival(int n){
    ready_q_arrival = n;
  }
  void calculate_wait_time(int n){
    ready_q_departure = n;
    wait_time += (ready_q_departure - ready_q_arrival);
    MAX_WAIT_TIME = max(MAX_WAIT_TIME,wait_time);
  }
  int get_wait_time(){
    return wait_time;
  }
};

// creates process from given line
Process *createProcess(string line, int pid)
{
  Process *p = new Process;
  bool is_io = false;
  queue<int> io_bursts;
  queue<int> cpu_bursts;
  int i = 0;
  while (i < line.size())
  {
    if (i == 0)
    {
      // start
      int time = 0;
      while (line[i] != ' ')
      {
        time = time * 10 + (line[i] - '0');
        i++;
      }
      p->setArrivalTime(time);
    }
    else
    {
      // bursts
      if (line[i] == '-')
      {
        io_bursts.push(-1);
        cpu_bursts.push(-1);
        break;
      }
      else if (is_io)
      {
        while (line[i] == ' ')
        {
          i++;
        }
        int time = 0;
        while (line[i] != ' ')
        {
          time = time * 10 + (line[i] - '0');
          i++;
        }
        io_bursts.push(time);
        is_io = false;
      }
      else
      {
        while (line[i] == ' ')
        {
          i++;
        }
        int time = 0;
        while (line[i] != ' ')
        {
          time = time * 10 + (line[i] - '0');
          i++;
        }
        cpu_bursts.push(time);
        is_io = true;
      }
    }
    i++;
  }
  p->setIOBursts(io_bursts);
  p->setCpuBursts(cpu_bursts);
  p->setPid(pid);
  return p;
}

// setting up a global time
int GLOBAL_TIME = 0;
// maximum wait time until which a process can come
int MAX_WAIT = 0;
// jiffy counter
void jiffy()
{
  GLOBAL_TIME++;
#ifdef SLOW
  sleep(1);
#endif
  // cout << "GLOBAL_TIME: " << GLOBAL_TIME << endl;
}

// a counter which decreases time for those part which actually doesn't take time.
void antiJiffy()
{
  GLOBAL_TIME--;
  // cout << "REDUCED_GLOBAL_TIME: " << GLOBAL_TIME << endl;
}

// global ready queue && global waiting queue
queue<Process *> READY_Q;
priority_queue<pair<int,Process*>,vector<pair<int,Process*>>, greater<pair<int, Process*>>> WAITING_Q;
priority_queue<pair<int, Process *>, vector<pair<int, Process *>>, greater<pair<int, Process *>>> SJF_READY_Q;
// process intializer
void throwNewProcess(queue<Process *> &ProcessList)
{
  // cout << "TRY THROWING NEW PROCESS" << endl;
  if (!ProcessList.empty())
  {
    if (ProcessList.front()->getArrivalTime() == GLOBAL_TIME)
    {
      // cout << "PID: " << ProcessList.front()->getPid() << " INTIALISED" << endl;
      READY_Q.push(ProcessList.front());
      ProcessList.pop();
      throwNewProcess(ProcessList);
    }
    else
    {
      return;
    }
  }
}
// a cpu class that should show the time for which it is
// going to be busy and with which process
class CPU
{
private:
  int process_pid;
  int time_left;
  Process *running_process;

public:
  bool is_empty = true;
  int get_process_pid()
  {
    return process_pid;
  }
  void set_process_pid(int npid)
  {
    process_pid = npid;
  }
  int get_time_left()
  {
    return time_left;
  }
  void set_time_left(int n)
  {
    time_left = n;
  }
  Process *get_current_process()
  {
    return running_process;
  }
  void set_current_process(Process *n)
  {
    running_process = n;
  }
};

// an IO class is that has the time and pid of all the process
// that needs and IO and is waiting in the queue
class IO
{
public:
  bool is_sjf = false;
  /*
  DO IO steps:-
  1. waiting_q is a priority queue where
  first element is an token on when to release
  the process from IO
  2. when an IO is released priority_q is popped
  and do_io is called recursively checking if next 
  element is to be popped
  3. if token value hasn't been reached function
  is returned.
  */
  void do_io(bool cpu_empty)
  {
    if (!WAITING_Q.empty())
    {
      if(GLOBAL_TIME==WAITING_Q.top().first){
        if(is_sjf){
          // cout << WAITING_Q.top().second->getPid() << " WAITING 2 READY " << endl;
          WAITING_Q.top().second->popIOBurst();
          SJF_READY_Q.push({WAITING_Q.top().second->currentCPUburst(),WAITING_Q.top().second});
          WAITING_Q.top().second->set_r_q_arrival(GLOBAL_TIME);
          WAITING_Q.pop();
          do_io(cpu_empty);
        }else{
          WAITING_Q.top().second->popIOBurst();
          READY_Q.push(WAITING_Q.top().second);
          WAITING_Q.top().second->set_r_q_arrival(GLOBAL_TIME);
          WAITING_Q.pop();
          do_io(cpu_empty);
        }
      }
      else
      {
        return;
      }
    }
    else
    {
      return;
    }
  }
  void set_waiting_process(int token ,Process *p)
  {
    WAITING_Q.push({token,p});
  }
};


/*performs FIFO per cpu
burst*/
void startFIFO(queue<Process *> &processList)
{
  vector<int>wait_times(processList.size(),0);
  vector<int>completion_times(processList.size(),0);
  CPU *my_cpu = new CPU;
  IO *my_IO = new IO;
  while (true)
  {
    /*if global time is less than 
    max wait try throwing a new proccess
    to ready_q*/
    if (GLOBAL_TIME <= MAX_WAIT)
    {
      throwNewProcess(processList);
    }
    /*cases to break out:
    1.ready_q is empty,
    2. no process in IO
    3. time is greater than last 
    recievable process 
    4. cpu is empty*/
    if (READY_Q.empty() && WAITING_Q.empty() && GLOBAL_TIME > MAX_WAIT && my_cpu->is_empty)
    {
      antiJiffy();
      cout << "MAKESPAN: " << GLOBAL_TIME << endl;
      break;
    }
    my_IO->do_io(my_cpu->is_empty);
    /*if cpu is empty and we 
    have some process in ready_q
    we throw it into cpu*/
    if (my_cpu->is_empty && !READY_Q.empty())
    {
      my_cpu->set_current_process(READY_Q.front());
      my_cpu->set_process_pid(READY_Q.front()->getPid());
      my_cpu->set_time_left(READY_Q.front()->getCpuBursts().front());
      my_cpu->is_empty = false;
      my_cpu->get_current_process()->calculate_wait_time(GLOBAL_TIME);
      cout << "P" << my_cpu->get_process_pid()+1 << ": " << my_cpu->get_current_process()->curr_burst_pos() << " " << GLOBAL_TIME << " ";
      READY_Q.pop();
    }
    /*if cpu is not empty
    we reduce burst or if 
    burst is finished we throw this 
    process to do io.*/
    if (!my_cpu->is_empty)
    {
      if (my_cpu->get_time_left() == 0)
      {
        my_cpu->get_current_process()->popCpuBurst();
        my_cpu->get_current_process()->set_burst_pos(my_cpu->get_current_process()->curr_burst_pos()+1);
        my_cpu->is_empty = true;
        if(my_cpu->get_current_process()->currentCPUburst()!=-1){
          cout << GLOBAL_TIME << endl;
          int token = my_cpu->get_current_process()->currentIOburst()+GLOBAL_TIME;
          my_IO->set_waiting_process(token,my_cpu->get_current_process());
        }else{
          wait_times[my_cpu->get_process_pid()] = my_cpu->get_current_process()->get_wait_time();
          completion_times[my_cpu->get_process_pid()] = GLOBAL_TIME - my_cpu->get_current_process()->getArrivalTime();
          cout << GLOBAL_TIME << endl;
        }
        /*if there is some process in ready-q
        throw that into cpu*/
        if(!READY_Q.empty()){
          my_cpu->set_current_process(READY_Q.front());
          my_cpu->set_process_pid(READY_Q.front()->getPid());
          my_cpu->set_time_left(READY_Q.front()->getCpuBursts().front());
          my_cpu->is_empty = false;
          my_cpu->get_current_process()->calculate_wait_time(GLOBAL_TIME);
          cout << "P" << my_cpu->get_process_pid()+1 << ": " << my_cpu->get_current_process()->curr_burst_pos() << " " << GLOBAL_TIME << " ";
          READY_Q.pop();
        }
      }
      /*if cpu isn't empty do cpu work!!!*/
      if(!my_cpu->is_empty){
        my_cpu->set_time_left(my_cpu->get_time_left() - 1);
      }
    }
    jiffy();
  }
  cout << "FIFO ENDED" << endl;
  int wait_time_sum = 0;
  int completion_time_sum = 0;
  int max_completion_time = 0;
  for(int i=0; i<wait_times.size(); i++){
    wait_time_sum+=wait_times[i];
    completion_time_sum+=completion_times[i];
    max_completion_time = max(max_completion_time,completion_times[i]);
  }
  cout << "AVERAGE WAIT TIME: " << float(wait_time_sum)/wait_times.size() << endl;
  cout << "MAXIMUM WAIT TIME: " << MAX_WAIT_TIME << endl;
  cout << "AVERAGE COMPLETION TIME: " << float(completion_time_sum)/wait_times.size() << endl;
  cout << "MAXIMUM COMPLETION TIME: " << max_completion_time << endl;
}

// process intializer for SJF PREAMPTIVE & NON-PREAMPTIVE
void throwNewProcessSJF(queue<Process *> &processList)
{
  // cout << "THROWING NEW PROCESS" << endl;
  if (!processList.empty())
  {
    if (processList.front()->getArrivalTime() == GLOBAL_TIME)
    {
      // cout << "PID: " << processList.front()->getPid() << " INTIALISED" << endl;
      pair<int, Process *> frontProcess;
      frontProcess.first = processList.front()->currentCPUburst();
      frontProcess.second = processList.front();
      SJF_READY_Q.push(frontProcess);
      processList.pop();
      throwNewProcessSJF(processList);
    }
    else
    {
      return;
    }
  }
}

void doSJF(queue<Process *> &process_lists)
{
  vector<int>wait_times(process_lists.size(),0);
  vector<int>completion_times(process_lists.size(),0);
  CPU *my_cpu = new CPU;
  IO *my_io = new IO;
  my_cpu->is_empty = true;
  my_io->is_sjf = true;
  while (true)
  {
    /*if global time is less than
    max wait you can throw new process
    */
   if(GLOBAL_TIME<=MAX_WAIT){
    throwNewProcessSJF(process_lists);
   }
   /*breaking condition is activated when
   globaltime > maxwait, cpu is free, io is 
   free, and ready_q is also empty.*/
   if(GLOBAL_TIME>MAX_WAIT && my_cpu->is_empty && WAITING_Q.empty() && SJF_READY_Q.empty()){
    antiJiffy();
    cout << "MAKESPAN: " << GLOBAL_TIME << endl;
    break;
   }
   my_io->do_io(my_cpu->is_empty);
   /*if cpu is ready and there is a job in
   sjf_ready_q then process the shortest job*/
   if(!SJF_READY_Q.empty() && my_cpu->is_empty){
    my_cpu->is_empty = false;
    my_cpu->set_current_process(SJF_READY_Q.top().second);
    my_cpu->set_process_pid(SJF_READY_Q.top().second->getPid());
    my_cpu->set_time_left(SJF_READY_Q.top().second->currentCPUburst());
    my_cpu->get_current_process()->calculate_wait_time(GLOBAL_TIME);
    cout << "P" << my_cpu->get_process_pid()+1 << ": " << my_cpu->get_current_process()->curr_burst_pos() << " " << GLOBAL_TIME << " ";
    SJF_READY_Q.pop();
   }
   if(!my_cpu->is_empty){
    // cout << "CPU ENGAGED!!" << endl;
    /*the cpu burst is finished it should
    go for IO*/
    if(my_cpu->get_time_left() == 0){
      my_cpu->is_empty = true;
      my_cpu->get_current_process()->popCpuBurst();
      my_cpu->get_current_process()->set_burst_pos(my_cpu->get_current_process()->curr_burst_pos()+1);
      if(my_cpu->get_current_process()->currentCPUburst()!=-1){
        int token = my_cpu->get_current_process()->currentIOburst() + GLOBAL_TIME;
        my_io->set_waiting_process(token,my_cpu->get_current_process());
        cout << GLOBAL_TIME << endl;
      }else{
        wait_times[my_cpu->get_process_pid()] = my_cpu->get_current_process()->get_wait_time();
        completion_times[my_cpu->get_process_pid()] = GLOBAL_TIME - my_cpu->get_current_process()->getArrivalTime();
        cout << GLOBAL_TIME << endl;
      }
      /*if there is some process in sjf-ready-q
      throw that into cpu*/
      if(!SJF_READY_Q.empty()){
        my_cpu->set_current_process(SJF_READY_Q.top().second);
        my_cpu->set_process_pid(SJF_READY_Q.top().second->getPid());
        my_cpu->set_time_left(SJF_READY_Q.top().second->currentCPUburst());
        my_cpu->is_empty = false;
        my_cpu->get_current_process()->calculate_wait_time(GLOBAL_TIME);
        cout << "P" << my_cpu->get_process_pid()+1 << ": " << my_cpu->get_current_process()->curr_burst_pos() << " " << GLOBAL_TIME << " ";
        SJF_READY_Q.pop();
      }
    }
    /*if cpu isn't empty do cpu work!!!*/
    if(!my_cpu->is_empty){
      // cout << my_cpu->get_process_pid() << " IS IN CPU FOR " << my_cpu->get_time_left() << endl;
      my_cpu->set_time_left(my_cpu->get_time_left() - 1);
    }
   }
   jiffy();
  }
  cout << "SJF DONE" << endl;
  int wait_time_sum = 0;
  int completion_time_sum = 0;
  int max_completion_time = 0;
  for(int i=0; i<wait_times.size(); i++){
    wait_time_sum+=wait_times[i];
    completion_time_sum+=completion_times[i];
    max_completion_time = max(max_completion_time,completion_times[i]);
  }
  cout << "AVERAGE WAIT TIME: " << float(wait_time_sum)/wait_times.size() << endl;
  cout << "MAXIMUM WAIT TIME: " << MAX_WAIT_TIME << endl;
  cout << "AVERAGE COMPLETION TIME: " << float(completion_time_sum)/wait_times.size() << endl;
  cout << "MAXIMUM COMPLETION TIME: " << max_completion_time << endl;
}

void doSRTF(queue<Process*>&process_lists){
  cout << "STARTING SRTF" << endl;
  vector<int>wait_times(process_lists.size(),0);
  vector<int>completion_times(process_lists.size(),0);
  CPU *my_cpu = new CPU;
  IO *my_io = new IO;
  my_cpu->is_empty = true;
  my_io->is_sjf = true;
  while(true){
    /*if global time is less than
    max wait you can throw new process
    */
   if(GLOBAL_TIME<=MAX_WAIT){
    throwNewProcessSJF(process_lists);
   }
    /*breaking condition is activated when
   globaltime > maxwait, cpu is free, io is 
   free, and ready_q is also empty.*/
   if(GLOBAL_TIME>MAX_WAIT && my_cpu->is_empty && WAITING_Q.empty() && SJF_READY_Q.empty()){
    break;
   }
   my_io->do_io(my_cpu->is_empty);
   /*if cpu is ready and there is a job in
   sjf_ready_q then process the shortest job*/
   if(!SJF_READY_Q.empty() && my_cpu->is_empty){
    my_cpu->is_empty = false;
    my_cpu->set_current_process(SJF_READY_Q.top().second);
    my_cpu->set_process_pid(SJF_READY_Q.top().second->getPid());
    my_cpu->set_time_left(SJF_READY_Q.top().second->currentCPUburst());
    my_cpu->get_current_process()->calculate_wait_time(GLOBAL_TIME);
    cout << "P" << my_cpu->get_process_pid()+1 << ": " << my_cpu->get_current_process()->curr_burst_pos() << " " << GLOBAL_TIME << " ";
    SJF_READY_Q.pop();
   }
   if(!my_cpu->is_empty){
    /*if cpu burst is finished 
    the process should go to IO*/
    if(my_cpu->get_time_left()==0){
      my_cpu->is_empty = true;
      my_cpu->get_current_process()->popCpuBurst();
      my_cpu->get_current_process()->set_burst_pos(my_cpu->get_current_process()->curr_burst_pos()+1);
      if(my_cpu->get_current_process()->currentCPUburst()!=-1){
        int token = my_cpu->get_current_process()->currentIOburst()+GLOBAL_TIME;
        my_io->set_waiting_process(token,my_cpu->get_current_process());
        cout << GLOBAL_TIME << endl;
      }else{
        /*just exit gracefully*/
        wait_times[my_cpu->get_process_pid()] = my_cpu->get_current_process()->get_wait_time();
        completion_times[my_cpu->get_process_pid()] = GLOBAL_TIME - my_cpu->get_current_process()->getArrivalTime();
        cout << GLOBAL_TIME << endl;
      }
      /*as the current process have might 
      have exited or went for IO, so cpu must
      be occupied from sjf-ready-q top process.*/
      if(!SJF_READY_Q.empty()){
        my_cpu->set_current_process(SJF_READY_Q.top().second);
        my_cpu->set_process_pid(SJF_READY_Q.top().second->getPid());
        my_cpu->set_time_left(SJF_READY_Q.top().second->currentCPUburst());
        my_cpu->is_empty = false;
        my_cpu->get_current_process()->calculate_wait_time(GLOBAL_TIME);
        cout << "P" << my_cpu->get_process_pid()+1 << ": " << my_cpu->get_current_process()->curr_burst_pos() << " " << GLOBAL_TIME << " ";
        // cout << my_cpu->get_process_pid() << " GOING 2 CPU" << endl;
        SJF_READY_Q.pop();
      }
    }
    /*if there is something in 
    in sjf-ready-q that should be processed*/
    if(!SJF_READY_Q.empty()){
      /*it need to be checked if remaining cpu time
      is larger than some process in ready q, the process in 
      ready q shall enter cpu and vice versa*/
      if(my_cpu->get_time_left()>SJF_READY_Q.top().second->currentCPUburst()){
        cout << GLOBAL_TIME << endl;
        my_cpu->get_current_process()->setCurrentCpuBurst(my_cpu->get_time_left());
        my_cpu->get_current_process()->set_r_q_arrival(GLOBAL_TIME);
        SJF_READY_Q.push({my_cpu->get_time_left(),my_cpu->get_current_process()});
        my_cpu->is_empty = false;
        my_cpu->set_current_process(SJF_READY_Q.top().second);
        my_cpu->set_process_pid(SJF_READY_Q.top().second->getPid());
        my_cpu->set_time_left(SJF_READY_Q.top().second->currentCPUburst());
        my_cpu->get_current_process()->calculate_wait_time(GLOBAL_TIME);
        SJF_READY_Q.pop();
        cout << "P" << my_cpu->get_process_pid()+1 << ": " << my_cpu->get_current_process()->curr_burst_pos() << " " << GLOBAL_TIME << " ";
      }
    }
    /*cpu does it's work if not empty*/
    if(!my_cpu->is_empty){
      my_cpu->set_time_left(my_cpu->get_time_left()-1);
    }
   }
   jiffy();
  }
  cout << "SRTF DONE" << endl;
  int wait_time_sum = 0;
  int completion_time_sum = 0;
  int max_completion_time = 0;
  for(int i=0; i<wait_times.size(); i++){
    wait_time_sum+=wait_times[i];
    completion_time_sum+=completion_times[i];
    max_completion_time = max(max_completion_time,completion_times[i]);
  }
  cout << "MAKESPAN: " << GLOBAL_TIME << endl;
  cout << "AVERAGE WAIT TIME: " << float(wait_time_sum)/wait_times.size() << endl;
  cout << "MAXIMUM WAIT TIME: " << MAX_WAIT_TIME << endl;
  cout << "AVERAGE COMPLETION TIME: " << float(completion_time_sum)/wait_times.size() << endl;
  cout << "MAXIMUM COMPLETION TIME: " << max_completion_time << endl;
}

void doRR(queue<Process*>&process_lists, int time_quant){
  cout << "STARTING ROUND ROBIN" << endl;
  vector<int>wait_times(process_lists.size(),0);
  vector<int>completion_times(process_lists.size(),0);
  CPU* my_cpu = new CPU;
  IO* my_io = new IO;
  my_cpu->is_empty = true;
  while(true){
    /*if global time is less than
    max wait you can throw new process
    */
   if(GLOBAL_TIME<=MAX_WAIT){
    throwNewProcess(process_lists);
   }
   /*breaking condn:-
   if ready_q, cpu, waiting, process_list is empty*/
   if(READY_Q.empty() && my_cpu->is_empty && WAITING_Q.empty() && GLOBAL_TIME > MAX_WAIT){
    antiJiffy();
    cout << "MAKESPAN: " << GLOBAL_TIME << endl;
    break;
   }
   my_io->do_io(my_cpu->is_empty);
   /*if cpu is ready and there is a job
   in ready_q then process the top job.*/
   if(!READY_Q.empty() && my_cpu->is_empty){
    my_cpu->is_empty = false;
    my_cpu->set_current_process(READY_Q.front());
    my_cpu->set_process_pid(READY_Q.front()->getPid());
    my_cpu->set_time_left(READY_Q.front()->currentCPUburst());
    my_cpu->get_current_process()->calculate_wait_time(GLOBAL_TIME);
    cout << "P" << my_cpu->get_process_pid()+1 << ": " << my_cpu->get_current_process()->curr_burst_pos() << " " << GLOBAL_TIME << " ";
    READY_Q.pop();
   }
   if(!my_cpu->is_empty){
    /*if cpu burst is finished then 
    process must go to IO*/
    if(my_cpu->get_time_left()==0){
      my_cpu->is_empty = true;
      my_cpu->get_current_process()->popCpuBurst();
      my_cpu->get_current_process()->set_burst_pos(my_cpu->get_current_process()->curr_burst_pos()+1);
      if(my_cpu->get_current_process()->currentCPUburst()!=-1){
        int token = my_cpu->get_current_process()->currentIOburst() + GLOBAL_TIME;
        my_io->set_waiting_process(token,my_cpu->get_current_process());
        cout << GLOBAL_TIME << endl;
      }else{
        /*exit gracefully.*/
        wait_times[my_cpu->get_process_pid()] = my_cpu->get_current_process()->get_wait_time();
        completion_times[my_cpu->get_process_pid()] = GLOBAL_TIME - my_cpu->get_current_process()->getArrivalTime();
        cout << GLOBAL_TIME << endl;
      }
      /*last running process might have gone 
      for IO or might have exited so new process
      must be introduced to cpu.*/
      if(!READY_Q.empty()){
        my_cpu->set_current_process(READY_Q.front());
        my_cpu->set_process_pid(READY_Q.front()->getPid());
        my_cpu->set_time_left(READY_Q.front()->getCpuBursts().front());
        my_cpu->is_empty = false;
        my_cpu->get_current_process()->calculate_wait_time(GLOBAL_TIME);
        cout << "P" << my_cpu->get_process_pid()+1 << ": " << my_cpu->get_current_process()->curr_burst_pos() << " " << GLOBAL_TIME << " ";
        READY_Q.pop();
      }
    }

    if(!my_cpu->is_empty){
      /*if the current process has been executed 
      for given time quant it should be replaced*/
      int run_time = my_cpu->get_current_process()->currentCPUburst() - my_cpu->get_time_left();
      if(run_time >= time_quant){
        my_cpu->get_current_process()->setCurrentCpuBurst(my_cpu->get_time_left());
        my_cpu->get_current_process()->set_r_q_arrival(GLOBAL_TIME);
        READY_Q.push(my_cpu->get_current_process());
        cout << GLOBAL_TIME << endl;
        my_cpu->set_current_process(READY_Q.front());
        my_cpu->set_process_pid(READY_Q.front()->getPid());
        my_cpu->set_time_left(READY_Q.front()->currentCPUburst());
        my_cpu->get_current_process()->calculate_wait_time(GLOBAL_TIME);
        READY_Q.pop();
        cout << "P" << my_cpu->get_process_pid()+1 << ": " << my_cpu->get_current_process()->curr_burst_pos() << " " << GLOBAL_TIME << " ";
      }
      my_cpu->set_time_left(my_cpu->get_time_left()-1);
    }
   }
    jiffy();
  }
  cout << "RR DONE" << endl;
  int wait_time_sum = 0;
  int completion_time_sum = 0;
  int max_completion_time = 0;
  for(int i=0; i<wait_times.size(); i++){
    wait_time_sum+=wait_times[i];
    completion_time_sum+=completion_times[i];
    max_completion_time = max(max_completion_time,completion_times[i]);
  }
  cout << "TIME QUANT: " << time_quant << endl;
  cout << "AVERAGE WAIT TIME: " << float(wait_time_sum)/wait_times.size() << endl;
  cout << "MAXIMUM WAIT TIME: " << MAX_WAIT_TIME << endl;
  cout << "AVERAGE COMPLETION TIME: " << float(completion_time_sum)/wait_times.size() << endl;
  cout << "MAXIMUM COMPLETION TIME: " << max_completion_time << endl;
}

int main(int argc, char *argv[])
{

  // get file name from argument
  char *file_name = argv[1];
  // open file
  ifstream inputFile(file_name);
  // check for open condition
  if (!inputFile.is_open())
  {
    cerr << "Error opening file!" << endl;
    return 1;
  }
  string line;
  // process queue to store all process from file
  queue<Process *> process_list;
  // read and create all processes
  int line_no = 0;
  while (getline(inputFile, line))
  {
    if (isdigit(line[0]))
    {
      process_list.push(createProcess(line, line_no));
      line_no++;
    }
  }
  MAX_WAIT = process_list.back()->getArrivalTime();
  cout << "MAX WAIT: " << MAX_WAIT << endl;
  inputFile.close();
#ifdef FIFO
  startFIFO(process_list);
#elif SJF
  doSJF(process_list);
#elif SRTF
  doSRTF(process_list);
#elif RR
  int time_quant = stoi(argv[2]);
  doRR(process_list,time_quant);
#endif
  return 0;
}