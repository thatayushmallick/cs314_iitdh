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
  void reduceCpuBurst()
  {
    cpu_bursts.front()--;
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
  // sleep(1);
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
queue<Process *> WAITING_Q;
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
private:
  vector<Process *> waiting_process;

public:
  bool is_fifo = false;
  bool check_is_empty()
  {
    return (waiting_process.empty() == true);
  }
  void do_io()
  {
    if (!waiting_process.empty())
    {
      cout << "DOING IO" << endl;
      int index = 0;
      for (auto i = waiting_process.begin(); i != waiting_process.end(); i++)
      {
        if (!check_is_empty())
        { // there is case where while erasing process from waiting queue might empty the waiting process vector
          if (waiting_process[index]->getIOBursts().front() == 0)
          {                                       // check if while reducing IO burst have been trimmed to 0.
            waiting_process[index]->popIOBurst(); // if so then this burst must be popped
            cout << "PROCESS " << waiting_process[index]->getPid() << " ENTERING READY_Q" << endl;
            READY_Q.push(waiting_process[index]); // if not FIFO then after popping push this process in back of queue
            waiting_process.erase(i); // remove the process from waiting_q
            antiJiffy();              // as it doesn't take time to go from waiting 2 ready_q
          }
          else if (waiting_process[index]->getIOBursts().front() > 0)
          {
            cout << "PROCESS " << waiting_process[index]->getPid() << " WAITING IN IO FOR " << waiting_process[index]->getIOBursts().front() << endl;
            waiting_process[index]->reduceIOBurst(); // if io burst is not positive just reduce the Burst.
          }
          else
          {
            cout << "PROCESS " << waiting_process[index]->getPid() << " HAS EXITED!!" << endl;
            waiting_process.erase(i); // if io burst in -1 or non-negative remove process from waiting_q directly.
            antiJiffy();              // it doesn't take time to exit from waiting_q
          }
          index++;
        }
        else
        { // if no process in IO it should break; from checking loop
          break;
        }
      }
    }
    else
    {
      cout << "NO IO NEED" << endl;
    }
  }
  /*steps to do IO in SJF
  1. check if something in io if not
  do nothing else
  2. iterate through waiting process
  list
  3. if waiting time is positive then
  decrease io burst else if waiting
  time is zero burst the io from process
  io queue
  4. if waiting time is -1 then erase the
  process.
  5. if the running algroithm is non-preamptive
  then the same process should be executed in cpu
  again, so we push it into some ready q NOT SJF_READY_Q
  else if it's preamptive we push it into SJF_READY_Q*/
  void do_sjf_io()
  {
    if (!waiting_process.empty())
    {
      cout << "DOING IO" << endl;
      int index = 0;
      for (auto i = waiting_process.begin(); i != waiting_process.end(); i++)
      {
        if (!check_is_empty())
        { // there is case where while erasing process from waiting queue might empty the waiting process vector
          if (waiting_process[index]->getIOBursts().front() == 0)
          {                                       // check if while reducing IO burst have been trimmed to 0.
            waiting_process[index]->popIOBurst(); // if so then this burst must be popped
            cout << "PROCESS " << waiting_process[index]->getPid() << " ENTERING SJF_READY_Q" << endl;
            SJF_READY_Q.push({waiting_process[index]->currentCPUburst(), waiting_process[index]}); // if it's fifo then after popping the burst process should be pushed in front of READY_Q.
            waiting_process.erase(i); // remove the process from waiting_q
            antiJiffy();              // as it doesn't take time to go from waiting 2 ready_q
          }
          else if (waiting_process[index]->getIOBursts().front() > 0)
          {
            cout << "PROCESS " << waiting_process[index]->getPid() << " WAITING IN IO FOR " << waiting_process[index]->getIOBursts().front() << endl;
            waiting_process[index]->reduceIOBurst(); // if io burst is not positive just reduce the Burst.
          }
          else
          {
            cout << "PROCESS " << waiting_process[index]->getPid() << " HAS EXITED!!" << endl;
            waiting_process.erase(i); // if io burst in -1 or non-negative remove process from waiting_q directly.
            antiJiffy();              // it doesn't take time to exit from waiting_q
          }
          index++;
        }
        else
        { // if no process in IO it should break; from checking loop
          break;
        }
      }
    }
    else
    {
      cout << "NO IO NEED" << endl;
    }
  }
  vector<Process *> get_waiting_process()
  {
    return waiting_process;
  }
  void set_waiting_process(Process *p)
  {
    waiting_process.push_back(p);
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
    if (READY_Q.empty() && my_IO->check_is_empty() && GLOBAL_TIME > MAX_WAIT && my_cpu->is_empty)
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
      cout << "CPU ENGAGED!";
      if (my_cpu->get_time_left() == 0)
      {
        my_cpu->get_current_process()->popCpuBurst();
        my_cpu->is_empty = true;
        if (my_cpu->get_current_process()->getIOBursts().front() != -1)
        {
          cout << my_cpu->get_process_pid() << " GOING 2 WAITING_Q" << endl;
          my_IO->set_waiting_process(my_cpu->get_current_process());
        }
        else
        {
          cout << my_cpu->get_process_pid() << " EXITING.." << endl;
          antiJiffy(); // as it doesn't take time to go from cpu 2 exiting
        }
      }
      else
      {
        cout << my_cpu->get_process_pid() << " IS IN CPU FOR" << my_cpu->get_time_left();
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
  while (true)
  {

  }
  cout << "SJF DONE" << endl;
}

// void doSRTF(queue<Process*>&processList){
//   CPU* my_cpu = new CPU;
//   IO* my_io = new IO;
//   my_io->is_preamptive = true;
//   my_cpu->is_empty = true;
//   while(true){
//     if(GLOBAL_TIME<=MAX_WAIT){
//       throwNewProcessSJF(processList);
//     }
//     /*
//     breaking condition: sjf_waiting_q
//     is empty so is CPU so is IO and
//     all process from process list has
//     been thrown to ready_q i.e
//     GLOBAL_TIME > MAX_WAIT
//     */
//    if(SJF_READY_Q.empty() && my_cpu->is_empty && my_io->check_is_empty() && GLOBAL_TIME>MAX_WAIT){
//     cout << "BREAKING" << endl;
//    }
//    /*
//    if SJF_READY_Q is not empty then
//    there must be some process to throw.
//    */
//    if(!SJF_READY_Q.empty()){
//     /*
//     if CPU is unoccupied throw the shortest
//     task in cpu.
//     */
//       if(my_cpu->is_empty){
//         my_cpu->set_current_process(SJF_READY_Q.top().second);
//         my_cpu->set_time_left(SJF_READY_Q.top().second->currentCPUburst());
//         my_cpu->set_process_pid(SJF_READY_Q.top().second->getPid());
//         SJF_READY_Q.pop();
//       }else{
//         if(my_cpu->get_time_left()>SJF_READY_Q.top().second->currentCPUburst()){
//           SJF_READY_Q.push
//         }
//       }
//    }
//   }
// }

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
  startFIFO(process_list);
  // doSJF(process_list);
  return 0;
}