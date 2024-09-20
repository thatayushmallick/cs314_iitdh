#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <chrono>
#include <numeric>

using namespace std;

// Define a structure for the process
struct Process
{
    int arrival_time;
    vector<int> cpu_bursts;
    vector<int> io_bursts;
    int pid; // process id
    int current_burst;
    int waiting_time;
    int completion_time;
    int start_time;
    bool finished;

    Process(int id, int arr_time, vector<int> cpu, vector<int> io)
        : pid(id), arrival_time(arr_time), cpu_bursts(cpu), io_bursts(io),
          current_burst(0), waiting_time(0), completion_time(0),
          start_time(-1), finished(false) {}
};

// Function to load the workload from the file
vector<Process> load_workload(const string &file_path)
{
    ifstream file(file_path);
    vector<Process> processes;
    int arrival, burst;

    while (file >> arrival)
    {
        vector<int> cpu_bursts;
        vector<int> io_bursts;
        while (file >> burst && burst != -1)
        {
            cpu_bursts.push_back(burst); // CPU burst
            if (file.peek() == ' ')
                file.ignore();
            if (file >> burst && burst != -1)
                io_bursts.push_back(burst); // I/O burst
        }
        processes.emplace_back(processes.size(), arrival, cpu_bursts, io_bursts);
    }
    file.close();
    return processes;
}

// Function to perform FIFO scheduling
void schedule_fifo(vector<Process> &processes)
{
    queue<Process *> ready_queue;
    vector<Process *> io_queue;
    int current_time = 0;
    vector<Process *> completed_processes;
    int total_processes = processes.size();
    int processes_completed = 0;

    while (processes_completed < total_processes || !ready_queue.empty() || !io_queue.empty())
    {
        // Add new processes to the ready queue if they have arrived
        for (auto &process : processes)
        {
            if (process.arrival_time == current_time && process.start_time == -1)
            {
                ready_queue.push(&process);
                process.start_time = current_time; // Mark the start time
            }
        }

        // Handle I/O queue: Move processes back to ready queue if I/O is done
        for (auto it = io_queue.begin(); it != io_queue.end();)
        {
            Process *p = *it;
            int io_time = p->io_bursts[p->current_burst - 1];
            if (io_time == 0)
            {
                ready_queue.push(p);
                it = io_queue.erase(it);
            }
            else
            {
                // Decrement the I/O burst time
                p->io_bursts[p->current_burst - 1]--;
                ++it;
            }
        }

        // If there's something in the ready queue, process the first process (FIFO)
        if (!ready_queue.empty())
        {
            Process *current_process = ready_queue.front();
            ready_queue.pop();

            // If the process has CPU bursts left
            if (current_process->current_burst < current_process->cpu_bursts.size())
            {
                cout << "Process " << current_process->pid << " starts at time " << current_time << endl;
                int burst_time = current_process->cpu_bursts[current_process->current_burst];
                current_time += burst_time; // Run the CPU burst

                cout << "Process " << current_process->pid << " finishes CPU burst "
                     << current_process->current_burst << " at time " << current_time << endl;

                // Move to the next burst (this will either be I/O or end of process)
                current_process->current_burst++;

                if (current_process->current_burst < current_process->cpu_bursts.size())
                {
                    // Process has more bursts left, move to I/O queue
                    io_queue.push_back(current_process);
                }
                else
                {
                    // Process has finished all bursts
                    current_process->finished = true;
                    current_process->completion_time = current_time;
                    completed_processes.push_back(current_process);
                    processes_completed++;
                }
            }
        }
        else
        {
            // If the ready queue is empty, increment the current time (idle time)
            current_time++;
        }
    }

    // Output metrics
    cout << "\nScheduling completed. Processes metrics:\n";
    int total_completion_time = 0, total_waiting_time = 0;
    int max_completion_time = 0, max_waiting_time = 0;

    for (const auto &process_ptr : completed_processes)
    {
        Process *process = process_ptr; // Pointer to Process
        int waiting_time = process->completion_time - process->arrival_time -
                           accumulate(process->cpu_bursts.begin(), process->cpu_bursts.end(), 0);
        total_completion_time += process->completion_time;
        total_waiting_time += waiting_time;
        max_completion_time = max(max_completion_time, process->completion_time);
        max_waiting_time = max(max_waiting_time, waiting_time);

        cout << "Process " << process->pid << ": Completion time = " << process->completion_time
             << ", Waiting time = " << waiting_time << endl;
    }

    cout << "\nMakespan: " << current_time << endl;
    cout << "Average Completion Time: " << (total_completion_time / completed_processes.size()) << endl;
    cout << "Max Completion Time: " << max_completion_time << endl;
    cout << "Average Waiting Time: " << (total_waiting_time / completed_processes.size()) << endl;
    cout << "Max Waiting Time: " << max_waiting_time << endl;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << " <scheduling-algorithm> <path-to-workload-description-file>" << endl;
        return 1;
    }

    string algorithm = argv[1];
    string file_path = argv[2];

    // Load the workload from the file
    vector<Process> processes = load_workload(file_path);

    // Start measuring the runtime of the simulator
    auto start = chrono::high_resolution_clock::now();

    if (algorithm == "FIFO")
    {
        schedule_fifo(processes);
    }
    else
    {
        cerr << "Unknown scheduling algorithm: " << algorithm << endl;
        return 1;
    }

    // Stop measuring the runtime
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> run_time = end - start;

    // Output runtime
    cout << "\nSimulator Run Time: " << run_time.count() << " seconds" << endl;

    return 0;
}
