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
    int currentCPUburst(){
      if(!cpu_bursts.empty()){
        return cpu_bursts.front();
      }else{
        return -1;
      }
    }
    int currentIOburst(){
      if(!i_o_bursts.empty()){
        return i_o_bursts.front();
      }else{
        return -1;
      }
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
  return 0;
}