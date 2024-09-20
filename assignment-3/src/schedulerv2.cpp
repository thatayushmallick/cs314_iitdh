#include <bits/stdc++.h>
using namespace std;

// Process class that encapsulates a process's attributes and behavior
class Process
{
private:
    int pid;
    int arrival_time;
    queue<int> cpu_bursts;
    queue<int> io_bursts;

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
    queue<int> &getCpuBursts()
    {
        return cpu_bursts;
    }
    void setCpuBursts(queue<int> n)
    {
        cpu_bursts = n;
    }
    queue<int> &getIOBursts()
    {
        return io_bursts;
    }
    void setIOBursts(queue<int> n)
    {
        io_bursts = n;
    }
    void reduceCpuBurst()
    {
        if (!cpu_bursts.empty())
        {
            cpu_bursts.front()--;
        }
    }
    void popCpuBurst()
    {
        if (!cpu_bursts.empty())
        {
            cpu_bursts.pop();
        }
    }

    void reduceIOBurst()
    {
        if (!io_bursts.empty())
        {
            io_bursts.front()--;
        }
    }
    void popIOBurst()
    {
        if (!io_bursts.empty())
        {
            io_bursts.pop();
        }
    }
    bool isFinished()
    {
        return cpu_bursts.empty() && io_bursts.empty();
    }
};

// Function to parse a single integer from a string
int parseInt(const string &line, int &i)
{
    int value = 0;
    while (i < line.size() && line[i] == ' ')
    {
        i++;
    }
    while (i < line.size() && isdigit(line[i]))
    {
        value = value * 10 + (line[i] - '0');
        i++;
    }
    return value;
}

// Function to create a Process object from a given line
Process *createProcess(const string &line, int pid)
{
    Process *p = new Process;
    queue<int> io_bursts;
    queue<int> cpu_bursts;
    int i = 0;

    // Parse arrival time
    int arrival_time = parseInt(line, i);
    p->setArrivalTime(arrival_time);
    bool is_io = false;

    // Parse CPU and IO bursts
    while (i < line.size())
    {
        if (line[i] == '-')
        { // End of process bursts
            // cpu_bursts.push(-1);
            // io_bursts.push(-1);
            break;
        }
        int burst_time = parseInt(line, i);
        if (is_io)
        {
            io_bursts.push(burst_time);
        }
        else
        {
            cpu_bursts.push(burst_time);
        }
        is_io = !is_io; // Toggle between CPU and IO bursts
    }

    p->setCpuBursts(cpu_bursts);
    p->setIOBursts(io_bursts);
    p->setPid(pid);
    return p;
}

// Global time and wait time
int GLOBAL_TIME = 0;
int MAX_WAIT = 0;

// Increment global time (simulate a jiffy)
void jiffy()
{
    GLOBAL_TIME++;
    cout << "GLOBAL_TIME: " << GLOBAL_TIME << endl;
}

void antijiffy()
{
    GLOBAL_TIME--;
    cout << "antijiffy called, GLOBAL_TIME : " << GLOBAL_TIME << endl;
}

// Process initializer
void initializeProcess(queue<Process *> &processList, deque<Process *> &readyQueue)
{
    while (!processList.empty() && processList.front()->getArrivalTime() <= GLOBAL_TIME)
    {
        cout << "PID: " << processList.front()->getPid() << " INITIALIZED" << endl;
        readyQueue.push_back(processList.front());
        processList.pop();
    }
}

// CPU class to manage current process and its time left
class CPU
{
private:
    Process *running_process = nullptr;
    int time_left;

public:
    bool Empty = true;
    Process *getCurrentProcess()
    {
        return running_process;
    }
    void setCurrentProcess(Process *p)
    {
        running_process = p;
        Empty = false;
    }
    void clearCurrentProcess()
    {
        running_process = nullptr;
        Empty = true;
    }
    int getTimeLeft()
    {
        return time_left;
    }
    void setTimeLeft(int t)
    {
        time_left = t;
    }
    void reduceTime()
    {
        if (time_left > 0)
        {
            time_left--;
        }
    }
};

// IO class to manage processes waiting for IO
class IO
{
private:
    vector<Process *> waiting_process;

public:
    void doIO(deque<Process *> &readyQueue)
    {
        int index = 0;
        for (auto wp = waiting_process.begin(); wp != waiting_process.end();)
        {
            if (waiting_process[index]->getIOBursts().front() == 0)
            {
                waiting_process[index]->popIOBurst();
                readyQueue.push_back(waiting_process[index]);
                wp = waiting_process.erase(wp);
                antijiffy();
            }
            else if (waiting_process[index]->getIOBursts().front() == -1)
            {
                cout << "Process " << waiting_process[index]->getPid() << " has exited." << endl;
                wp = waiting_process.erase(wp);
            }
            else
            {
                cout << "Process " << waiting_process[index]->getPid() << " waiting in IO, time left: " << waiting_process[index]->getIOBursts().front() << endl;
                waiting_process[index]->reduceIOBurst();
                ++wp;
            }
            index++;
        }
    }
    void addProcessToIO(Process *p)
    {
        waiting_process.push_back(p);
    }
    bool isEmpty()
    {
        return waiting_process.empty();
    }
};

// Simulation of First-Come-First-Serve scheduling
void startFIFO(queue<Process *> &processList)
{
    CPU cpu;
    IO io;
    deque<Process *> readyQueue;

    while (!processList.empty() || !readyQueue.empty() || !io.isEmpty() || !cpu.Empty)
    {
        // Increment global time and initialize processes arriving at this time
        jiffy();
        initializeProcess(processList, readyQueue);

        // Check if the CPU is idle and a process is ready to run
        if (cpu.Empty && !readyQueue.empty())
        {
            Process *p = readyQueue.front();
            readyQueue.pop_front();
            cpu.setCurrentProcess(p);
            cout << "Process " << p->getPid() << " is running on CPU." << endl;
        }

        // Execute the current process on the CPU
        if (!cpu.Empty)
        {
            Process *p = cpu.getCurrentProcess();
            p->reduceCpuBurst();

            // Check if the current CPU burst is finished
            if (p->getCpuBursts().front() == 0)
            {
                p->popCpuBurst();
                if (!p->isFinished())
                {
                    cpu.clearCurrentProcess();
                    io.addProcessToIO(p); // Move to IO queue
                    cout << "Process " << p->getPid() << " moved to IO." << endl;
                }
            }
            else if (p->isFinished())
            {
                cout << "Process " << p->getPid() << " has completed execution." << endl;
                cpu.clearCurrentProcess(); // Process has completed
            }
        }

        // Perform IO for processes in the IO queue
        io.doIO(readyQueue);
    }

    cout << "Simulation finished at GLOBAL_TIME: " << GLOBAL_TIME << endl;
}

// Main function: reads process data from file and starts simulation
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <process_file>" << endl;
        return 1;
    }

    // Open input file
    ifstream inputFile(argv[1]);
    if (!inputFile.is_open())
    {
        cerr << "Error: Cannot open file!" << endl;
        return 1;
    }

    // Process queue to store all processes from file
    queue<Process *> process_list;
    string line;
    int line_no = 0;

    // Read and create all processes from file
    while (getline(inputFile, line))
    {
        if (isdigit(line[0]))
        {
            process_list.push(createProcess(line, line_no));
            line_no++;
        }
    }

    // Set the maximum wait time for processes
    if (!process_list.empty())
    {
        MAX_WAIT = process_list.back()->getArrivalTime();
    }

    inputFile.close();

    cout << "max wait :" << MAX_WAIT << endl;
    // Start the First-Come-First-Serve (FIFO) scheduling simulation
    startFIFO(process_list);

    return 0;
}