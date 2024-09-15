#include <iostream>
#include <bits/stdc++.h>
using namespace std;

// process class having all the methods and props of an process
class Process{
  private:
    int pid;
    int arrival_time;
    queue<int>cpu_bursts;
    queue<int>i_o_bursts;
  public:
    int getPid(){
      return pid;
    }
    void setPid(int n_pid){
      pid = n_pid;
    }
    int getArrivalTime(){
      return arrival_time;
    }
    void setArrivalTime(int n_a_t){
      arrival_time = n_a_t;
    }
    queue<int> getCpuBursts(){
      return cpu_bursts;
    }
    void setCpuBursts(queue<int>n){
      cpu_bursts = n;
    }
    void reduceCpuBurst(){
      cpu_bursts.front()--;
    }
    void popCpuBurst(){
      cpu_bursts.pop();
    }
    queue<int> getIOBursts(){
      return i_o_bursts;
    }
    void setIOBursts(queue<int>n){
      i_o_bursts = n;
    }
    void reduceIOBurst(){
      i_o_bursts.front()--;
    }
    void popIOBurst(){
      i_o_bursts.pop();
    }
};

// creates process from given line
Process* createProcess(string line, int pid){
  Process* p = new Process;
  bool is_io = false;
  queue<int> io_bursts;
  queue<int> cpu_bursts;
  int i = 0;
  while(i < line.size()){
    if(i == 0){
      // start
      int time = 0;
      while(line[i]!=' '){
        time = time*10 + (line[i] - '0');
        i++;
      }
      p->setArrivalTime(time);
    }else{
      // bursts
      if(line[i]=='-'){
        io_bursts.push(-1);
        cpu_bursts.push(-1);
        break;
      }else if(is_io){
        while(line[i]==' '){
          i++;
        }
        int time = 0;
        while(line[i]!=' '){
          time = time*10 + (line[i]-'0');
          i++;
        }
        io_bursts.push(time);
        is_io = false;
      }else{
        while(line[i]==' '){
          i++;
        }
        int time = 0;
        while(line[i]!=' '){
          time = time*10 + (line[i]-'0');
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
void jiffy(){
  GLOBAL_TIME++;
  // sleep(1);
  cout << "GLOBAL_TIME: " << GLOBAL_TIME << endl;
}

// global ready queue && global waiting queue
queue<Process*>READY_Q;
deque<Process*>READY_Q_FIFO;
queue<Process*>WAITING_Q;
// process intializer
void throwNewProcess(queue<Process*>&ProcessList, bool is_fifo){
  cout << "THROWING NEW PROCESS" << endl;
  if(!ProcessList.empty()){
    if(ProcessList.front()->getArrivalTime()==GLOBAL_TIME){
      cout << "PID: " << ProcessList.front()->getPid() << " INTIALISED" << endl;
      if(is_fifo){
        READY_Q_FIFO.push_back(ProcessList.front());
      }else{
        READY_Q.push(ProcessList.front());
      }
      ProcessList.pop();
    }
  }
}

// a cpu class that should show the time for which it is 
// going to be busy and with which process
class CPU{
  private:
    int process_pid;
    int time_left;
    Process* running_process;
  public:
    bool is_empty = true;
    int get_process_pid(){
      return process_pid;
    }
    void set_process_pid(int npid){
      process_pid = npid;
    }
    int get_time_left(){
      return time_left;
    }
    void set_time_left(int n){
      time_left = n;
    }
    Process* get_current_process(){
      return running_process;
    }
    void set_current_process(Process* n){
      running_process = n;
    }
};

// an IO class is that has the time and pid of all the process
// that needs and IO and is waiting in the queue
class IO{
  private:
    vector<Process*> waiting_process;
  public:
    bool is_fifo = false;
    bool check_is_empty(){
      return (waiting_process.empty() == true);
    }
    void do_io(){
      cout << "lere";
      if(!waiting_process.empty()){
        cout << "DOING IO" << endl;
        int index = 0;
        for(auto i=waiting_process.begin(); i!=waiting_process.end(); i++){
          if(!check_is_empty()){
            if(waiting_process[index]->getIOBursts().front()==0){
              waiting_process[index]->popIOBurst();
              if(is_fifo){
                cout << "PROCESS " << waiting_process[index]->getPid() << " ENTERING READY_Q_FIFO" << endl;
                READY_Q_FIFO.push_front(waiting_process[index]);
              }else{
                cout << "PROCESS " << waiting_process[index]->getPid() << " ENTERING READY_Q" << endl;
                READY_Q.push(waiting_process[index]);
              }
              waiting_process.erase(i);
            }else if(waiting_process[index]->getIOBursts().front() > 0){
              cout << "PROCESS " << waiting_process[index]->getPid() << " WAITING IN IO FOR " << waiting_process[index]->getIOBursts().front() << endl;
              waiting_process[index]->reduceIOBurst();
            }else{
              cout << "PROCESS " << waiting_process[index]->getPid() << " HAS EXITED!!" << endl;
              waiting_process.erase(i);
            }
            index++;
          }else{
            break;
          }
        }
      }else{
        cout << "NO IO NEED" << endl;
      }
    }
    vector<Process*> get_waiting_process(){
      return waiting_process;
    }
    void set_waiting_process(Process* p){
      waiting_process.push_back(p);
    }
};

void startFIFO(queue<Process*>&processList){
  CPU* my_cpu = new CPU;
  IO* my_IO = new IO;
  my_IO->is_fifo = true;
  while(true){
    if(GLOBAL_TIME<=MAX_WAIT){
      throwNewProcess(processList,true);
    }
    if(READY_Q_FIFO.empty() && my_IO->check_is_empty() && GLOBAL_TIME>MAX_WAIT && my_cpu->is_empty){
      cout << "BREAKING OUT" << endl;
      break;
    }
    if(!READY_Q_FIFO.empty()){
      if(my_IO->check_is_empty()){
        if(my_cpu->is_empty){
          my_cpu->set_current_process(READY_Q_FIFO.front());
          my_cpu->set_process_pid(READY_Q_FIFO.front()->getPid());
          my_cpu->set_time_left(READY_Q_FIFO.front()->getCpuBursts().front());
          my_cpu->is_empty = false;
          cout << my_cpu->get_process_pid() << " GOING 2 CPU" << endl;
          READY_Q_FIFO.pop_front();
        }else{
          cout << "CPU ENGAGED!";
          if(my_cpu->get_time_left() == 0){
            my_cpu->get_current_process()->popCpuBurst();
            my_cpu->is_empty = true;
            if(my_cpu->get_current_process()->getIOBursts().front()!=-1){
              cout << my_cpu->get_process_pid() << " GOING 2 WAITING_Q" << endl;
              my_IO->set_waiting_process(my_cpu->get_current_process());
            }else{
              cout << my_cpu->get_process_pid() << " EXITING.." << endl;
            }
          }else{
            cout << my_cpu->get_process_pid() << " IS IN CPU FOR" << my_cpu->get_time_left();
            my_cpu->set_time_left(my_cpu->get_time_left()-1);
            jiffy();
          }
        }
      }else{
        my_IO->do_io();
        jiffy();
      }
    }else if(!my_cpu->is_empty){
      if(my_IO->check_is_empty()){
        cout << "CPU ENGAGED!";
        if(my_cpu->get_time_left() == 0){
          my_cpu->get_current_process()->popCpuBurst();
          my_cpu->is_empty = true;
          if(my_cpu->get_current_process()->getIOBursts().front()!=-1){
            cout << my_cpu->get_process_pid() << " GOING 2 WAITING_Q" << endl;
            my_IO->set_waiting_process(my_cpu->get_current_process());
          }else{
            cout << my_cpu->get_process_pid() << " EXITING.." << endl;
          }
        }else{
          cout << my_cpu->get_process_pid() << " IS IN CPU FOR" << my_cpu->get_time_left();
          my_cpu->set_time_left(my_cpu->get_time_left()-1);
          jiffy();
        }
      }else{
        my_IO->do_io();
        jiffy();
      }
    }else if(!my_IO->check_is_empty()){
      my_IO->do_io();
      jiffy();
    }
  }
  cout << "ENDED" << endl;
}

int main(int argc, char* argv[]){
  
  // get file name from argument
  char* file_name = argv[1];
  // open file
  ifstream inputFile(file_name);
  // check for open condition
  if(!inputFile.is_open()){
    cerr << "Error opening file!" << endl;
    return 1;
  }
  string line;
  // process queue to store all process from file
  queue<Process*>process_list;
  // read and create all processes
  int line_no = 0;
  while (getline(inputFile,line))
  {
    if(isdigit(line[0])){
      process_list.push(createProcess(line,line_no));
      line_no++;
    }
  }
  MAX_WAIT = process_list.back()->getArrivalTime();
  cout << "MAX WAIT: " << MAX_WAIT << endl;
  inputFile.close();
  startFIFO(process_list);
  return 0;
}