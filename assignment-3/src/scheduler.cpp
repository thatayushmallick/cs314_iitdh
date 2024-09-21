#include <iostream>
#include <bits/stdc++.h>
using namespace std;

// process class having all the methods and props of an process
class Process
{
private:
  int pid;
  int arrival_time;
  queue<int> cpu_bursts;
  queue<int> i_o_bursts;

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
  cout << "GLOBAL_TIME: " << GLOBAL_TIME << endl;
}

// a counter which decreases time for those part which actually doesn't take time.
void antiJiffy()
{
  GLOBAL_TIME--;
  cout << "REDUCED_GLOBAL_TIME: " << GLOBAL_TIME << endl;
}

// global ready queue && global waiting queue
queue<Process *> READY_Q;
priority_queue<pair<int,Process*>,vector<pair<int,Process*>>, greater<pair<int, Process*>>> WAITING_Q;
priority_queue<pair<int, Process *>, vector<pair<int, Process *>>, greater<pair<int, Process *>>> SJF_READY_Q;
// process intializer
void throwNewProcess(queue<Process *> &ProcessList, bool is_fifo)
{
  cout << "TRY THROWING NEW PROCESS" << endl;
  if (!ProcessList.empty())
  {
    if (ProcessList.front()->getArrivalTime() == GLOBAL_TIME)
    {
      cout << "PID: " << ProcessList.front()->getPid() << " INTIALISED" << endl;
      READY_Q.push(ProcessList.front());
      ProcessList.pop();
      throwNewProcess(ProcessList, is_fifo);
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
  void do_io()
  {
    if (!WAITING_Q.empty())
    {
      if(GLOBAL_TIME==WAITING_Q.top().first){
        if(is_sjf){
          cout << WAITING_Q.top().second->getPid() << " WAITING 2 READY " << endl;
          WAITING_Q.top().second->popIOBurst();
          SJF_READY_Q.push({WAITING_Q.top().second->currentCPUburst(),WAITING_Q.top().second});
          WAITING_Q.pop();
          if(WAITING_Q.empty()){
            /*if no other process is in waiting_q
            then it shouldn't take any time 2 move 
            from waiting 2 ready q;
            */
            antiJiffy();
          }
          do_io();
        }else{
          cout << WAITING_Q.top().second->getPid() << " WAITING 2 READY " << endl;
          WAITING_Q.top().second->popIOBurst();
          READY_Q.push(WAITING_Q.top().second);
          WAITING_Q.pop();
          if(WAITING_Q.empty()){
            /*if no other process is in waiting_q
            then it shouldn't take any time 2 move 
            from waiting 2 ready q;
            */
            antiJiffy();
          }
          do_io();
        }
      }
      else if(WAITING_Q.top().second->currentIOburst() == -1){
        cout << WAITING_Q.top().second->getPid() << " EXITING" << endl;
        WAITING_Q.pop();
        if(WAITING_Q.empty()){
          /*if no other process is in waiting_q
          then it shouldn't take any time 2 move 
          from waiting 2 ready q;
          */
          antiJiffy();
        }
        do_io();
      }
      else
      {
        cout << WAITING_Q.top().second->getPid() << " will be released @ " << WAITING_Q.top().first << endl;
        return;
      }
    }
    else
    {
      cout << "NO IO NEED" << endl;
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
  CPU *my_cpu = new CPU;
  IO *my_IO = new IO;
  while (true)
  {
    /*if global time is less than 
    max wait try throwing a new proccess
    to ready_q*/
    if (GLOBAL_TIME <= MAX_WAIT)
    {
      throwNewProcess(processList, true);
    }
    /*cases to break out:
    1.ready_q is empty,
    2. no process in IO
    3. time is greater than last 
    recievable process 
    4. cpu is empty*/
    if (READY_Q.empty() && WAITING_Q.empty() && GLOBAL_TIME > MAX_WAIT && my_cpu->is_empty)
    {
      cout << "BREAKING OUT" << endl;
      break;
    }
    /*if cpu is empty and we 
    have some process in ready_q
    we throw it into ready_q*/
    if (my_cpu->is_empty && !READY_Q.empty())
    {
      my_cpu->set_current_process(READY_Q.front());
      my_cpu->set_process_pid(READY_Q.front()->getPid());
      my_cpu->set_time_left(READY_Q.front()->getCpuBursts().front());
      my_cpu->is_empty = false;
      cout << my_cpu->get_process_pid() << " GOING 2 CPU" << endl;
      READY_Q.pop();
    }
    /*if cpu is not empty
    we reduce burst or if 
    burst is finished we throw this 
    process to do io.*/
    if (!my_cpu->is_empty)
    {
      cout << "CPU ENGAGED!" << endl;
      if (my_cpu->get_time_left() == 0)
      {
        my_cpu->get_current_process()->popCpuBurst();
        my_cpu->is_empty = true;
        cout << my_cpu->get_process_pid() << " GOING 2 WAITING_Q" << endl;
        int token = my_cpu->get_current_process()->currentIOburst()+GLOBAL_TIME;
        my_IO->set_waiting_process(token,my_cpu->get_current_process());
      }
      else
      {
        cout << my_cpu->get_process_pid() << " IS IN CPU FOR " << my_cpu->get_time_left() << endl;
        my_cpu->set_time_left(my_cpu->get_time_left() - 1);
      }
    }
    my_IO->do_io();
    jiffy();
  }
  cout << "ENDED" << endl;
}

// process intializer for SJF PREAMPTIVE & NON-PREAMPTIVE
void throwNewProcessSJF(queue<Process *> &processList)
{
  cout << "THROWING NEW PROCESS" << endl;
  if (!processList.empty())
  {
    if (processList.front()->getArrivalTime() == GLOBAL_TIME)
    {
      cout << "PID: " << processList.front()->getPid() << " INTIALISED" << endl;
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
  cout << "STARTING SJF" << endl;
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
    cout << "BREAKING" << endl;
    break;
   }
   /*if cpu is ready and there is a job in
   sjf_ready_q then process the shortest job*/
   if(!SJF_READY_Q.empty() && my_cpu->is_empty){
    my_cpu->is_empty = false;
    my_cpu->set_current_process(SJF_READY_Q.top().second);
    my_cpu->set_process_pid(SJF_READY_Q.top().second->getPid());
    my_cpu->set_time_left(SJF_READY_Q.top().second->currentCPUburst());
    SJF_READY_Q.pop();
   }
   if(!my_cpu->is_empty){
    cout << "CPU ENGAGED!!" << endl;
    /*the cpu burst is finished it should
    go for IO*/
    if(my_cpu->get_time_left() == 0){
      my_cpu->is_empty = true;
      my_cpu->get_current_process()->popCpuBurst();
      int token = my_cpu->get_current_process()->currentIOburst() + GLOBAL_TIME;
      my_io->set_waiting_process(token,my_cpu->get_current_process());
    }else{
      /*process is being processed ;>*/
      cout << my_cpu->get_process_pid() << " in CPU for " << my_cpu->get_time_left() << endl;
      my_cpu->set_time_left(my_cpu->get_time_left()-1);
    }
   }
   my_io->do_io();
   jiffy();
  }
  cout << "SJF DONE" << endl;
}

void doSRTF(queue<Process*>&process_lists){
  cout << "STARTING SRTF" << endl;
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
    cout << "BREAKING" << endl;
    break;
   }
   /*if cpu is ready and there is a job in
   sjf_ready_q then process the shortest job*/
   if(!SJF_READY_Q.empty() && my_cpu->is_empty){
    my_cpu->is_empty = false;
    my_cpu->set_current_process(SJF_READY_Q.top().second);
    my_cpu->set_process_pid(SJF_READY_Q.top().second->getPid());
    my_cpu->set_time_left(SJF_READY_Q.top().second->currentCPUburst());
    SJF_READY_Q.pop();
   }
   if(!my_cpu->is_empty){
    cout << "CPU ENGAGED" << endl;
    /*if cpu burst is finished 
    the process should go to IO*/
    if(my_cpu->get_time_left()==0){
      my_cpu->is_empty = true;
      my_cpu->get_current_process()->popCpuBurst();
      int token = my_cpu->get_current_process()->currentIOburst()+GLOBAL_TIME;
      my_io->set_waiting_process(token,my_cpu->get_current_process());
    }else{
      /*it need to be checked if remaining cpu time
      islarger than some process in ready q, the process in 
      ready q shall enter cpu and vice versa*/
      if(my_cpu->get_time_left()>SJF_READY_Q.top().second->currentCPUburst()){
        cout << my_cpu->get_process_pid() << " WILL BE REPLACED BY " << SJF_READY_Q.top().second->getPid() << endl;
        my_cpu->get_current_process()->setCurrentCpuBurst(my_cpu->get_time_left());
        SJF_READY_Q.push({my_cpu->get_time_left(),my_cpu->get_current_process()});
        my_cpu->set_current_process(SJF_READY_Q.top().second);
        my_cpu->set_process_pid(SJF_READY_Q.top().second->getPid());
        my_cpu->set_time_left(SJF_READY_Q.top().second->currentCPUburst());
        SJF_READY_Q.pop();
      }
      /*if previous condition isn't satisfied the 
      process currently occupying cpu shall keep
      going*/
      cout << my_cpu->get_process_pid() << " IN CPU FOR " << my_cpu->get_time_left() << endl;
      my_cpu->set_time_left(my_cpu->get_time_left()-1);
    }
   }
   my_io->do_io();
   jiffy();
  }
  cout << "SRTF DONE" << endl;
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
  // startFIFO(process_list);
  // doSJF(process_list);
  doSRTF(process_list);
  return 0;
}