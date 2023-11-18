#include <iostream>
#include <vector>
#include <tuple>
#include <random>
#include <stdlib.h>
#include <string>
#include <thread>
#include <chrono>
#include <limits.h>

using namespace std;

void delay() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void fcfs(vector<int>& at, vector<int>& bt, int start, int end) {
    vector<int> temp_at(at.begin() + start, at.begin() + end);
    vector<int> temp_bt(bt.begin() + start, bt.begin() + end);
    vector<bool> completion;
    completion.assign(end - start, false);
    int tot_time = 0;
    while (true) {
        bool check = true;
        for (int i = 0; i < end - start; i++) {
            if (tot_time <= temp_at[i] && completion[i] == false) {
                check = false;
                tot_time += temp_bt[i];
                completion[i] = true;
            }
        }
        if (check) {
            break;
        }
    }
    for (int i = 0; i < tot_time; i++) {
        delay();
    }
}

void sjf(vector<int>& at, vector<int>& bt, int start, int end) {
    vector<int> temp_at(at.begin() + start, at.begin() + end);
    vector<int> temp_bt(bt.begin() + start, bt.begin() + end);
    vector<bool> completion;
    int n = end - start;
    completion.assign(n, false);
    int tot_time = 0;
    for (int i = 0; i < n; i++) {
        int shortestIndex = -1;
        int shortestBurst = INT_MAX;

        // Find the shortest job that has arrived
        for (int j = 0; j < n; j++) {
            if (temp_at[j] <= tot_time && completion[j] == false && temp_bt[j] < shortestBurst) {
                shortestBurst = temp_bt[j];
                shortestIndex = j;
            }
        }

        // If no job is found, increment time to the next arrival
        if (shortestIndex == -1) {
            int minArrival = INT_MAX;
            for (int j = 0; j < n; j++) {
                if (temp_at[j] < minArrival && completion[j] == false) {
                    minArrival = at[j];
                    shortestIndex = j;
                }
            }
            tot_time = minArrival;
        }
        completion[shortestIndex] = true;
        tot_time += temp_bt[shortestIndex];
    }
    for (int i = 0; i < tot_time; i++) {
        delay();
    }
}

void rr(vector<int>& at, vector<int>& bt, int start, int end, int quantum) {
    vector<int> temp_at(at.begin() + start, at.begin() + end);
    vector<int> temp_bt(bt.begin() + start, bt.begin() + end);
    vector<bool> completion;
    completion.assign(end - start, false);
    int tot_time = 0;

    while (true) {
        bool allProcessesDone = true;
        for (int i = 0; i < end - start; i++) {
            if (completion[i] == false && temp_at[i] <= tot_time) {
                if (temp_bt[i] <= quantum) {
                    
                    allProcessesDone = false;
                    completion[i] = true;
                    tot_time += temp_bt[i];
                    temp_bt[i] = 0;
                }
                else{
                    tot_time += quantum;
                    temp_bt[i] -= quantum;
                }
            }
        }
        if (allProcessesDone) {
            for(int i=0;i<end-start;i++){
                if(completion[i]==false){
                    continue;
                }
            }
            for (int m = 0; m <tot_time; m++) {
                        delay();
            }
            break;
        }

    }
}

void priority(vector<int>& at, vector<int>& bt, vector<int>& priority, int start, int end) {
    vector<int> temp_at(at.begin() + start, at.begin() + end);
    vector<int> temp_bt(bt.begin() + start, bt.begin() + end);
    vector<int> temp_priority(priority.begin() + start, priority.begin() + end);
    vector<bool> completion;
    completion.assign(end - start, false);
    int tot_time = 0;
    int n = end - start;
    for (int i = 0; i < n; i++) {
        int highest_priority = -1;
        int index = -1;
        for (int j = 0; j < n; j++) {
            if (temp_at[j] <= tot_time && completion[j] == false && temp_priority[j] > highest_priority) {
                highest_priority = temp_priority[j];
                index = j;
            }
            if (highest_priority == 5) {
                break;
            }
        }

        // If no process is found, increment time to the next arrival
        if (index == -1) {
            int minArrival = INT_MAX;
            for (int j = 0; j < n; j++) {
                if (temp_at[j] < minArrival) {
                    minArrival = temp_at[j];
                    index = j;
                }
            }
            tot_time = minArrival;
        }
        tot_time += temp_bt[index];
        completion[index] = true;
    }
    for (int i = 0; i < tot_time; i++) {
        delay();
    }
}

tuple<vector<int>, vector<int>, vector<int>> generateRandomDataset(int numProcesses, int maxBurstTime = 10, int maxPriority = 5) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> arrivalTimeDist(0, 100);
    uniform_int_distribution<> burstTimeDist(1, maxBurstTime);
    uniform_int_distribution<> priorityDist(1, maxPriority);

    vector<int> arrivalTime;
    vector<int> burstTime;
    vector<int> priority;

    for (int i = 0; i < numProcesses; ++i) {
        arrivalTime.push_back(arrivalTimeDist(gen));
        burstTime.push_back(burstTimeDist(gen));
        priority.push_back(priorityDist(gen));
    }

    return make_tuple(arrivalTime, burstTime, priority);
}

int main() {
    int numProcesses = 100;
    tuple<vector<int>, vector<int>, vector<int>> mytuple = generateRandomDataset(numProcesses);
    vector<int> arrivalTime = get<0>(mytuple);
    vector<int> burstTime = get<1>(mytuple);
    vector<int> priority_process = get<2>(mytuple);

    int no_cores;
    cout << "Enter Number of Cores to be used:";
    cin >> no_cores;
    int processes_per_thread = numProcesses / no_cores;

    auto starting = chrono::high_resolution_clock::now();
    vector<thread> sjf_threads;
    for (int i = 0; i < no_cores; i++) {
        sjf_threads.emplace_back(sjf, std::ref(arrivalTime), std::ref(burstTime), i * processes_per_thread, (i + 1) * processes_per_thread);
    }
    for (auto& thread : sjf_threads) {
        thread.join();
    }
    auto ending = chrono::high_resolution_clock::now();
    auto durationer = chrono::duration_cast<chrono::milliseconds>(ending - starting).count();
    cout << "Execution time: SJF " << durationer << " milliseconds" << endl;

    auto starting2 = chrono::high_resolution_clock::now();
    vector<thread> rr_threads;
    for (int i = 0; i < no_cores; i++) {
        rr_threads.emplace_back(rr, std::ref(arrivalTime), std::ref(burstTime), i * processes_per_thread, (i + 1) * processes_per_thread, 4);
    }
    for (auto& thread : rr_threads) {
        thread.join();
    }
    auto ending2 = chrono::high_resolution_clock::now();
    auto durationer2 = chrono::duration_cast<chrono::milliseconds>(ending2 - starting2).count();
    cout << "Execution time: RR " << durationer2 << " milliseconds" << endl;

    auto starting3 = chrono::high_resolution_clock::now();
    vector<thread> priority_threads;
    for (int i = 0; i < no_cores; i++) {
        priority_threads.emplace_back(priority, std::ref(arrivalTime), std::ref(burstTime), std::ref(priority_process), i * processes_per_thread, (i + 1) * processes_per_thread);
    }
    for (auto& thread : priority_threads) {
        thread.join();
    }
    auto ending3 = chrono::high_resolution_clock::now();
    auto durationer3 = chrono::duration_cast<chrono::milliseconds>(ending3 - starting3).count();
    cout << "Execution time: Priority " << durationer3 << " milliseconds" << endl;

    auto starting4 = chrono::high_resolution_clock::now();
    vector<thread> fcfs_threads;
    for (int i = 0; i < no_cores; i++) {
        fcfs_threads.emplace_back(fcfs, std::ref(arrivalTime), std::ref(burstTime), i * processes_per_thread, (i + 1) * processes_per_thread);
    }
    for (auto& thread : fcfs_threads) {
        thread.join();
    }
    auto ending4 = chrono::high_resolution_clock::now();
    auto durationer4 = chrono::duration_cast<chrono::milliseconds>(ending4 - starting4).count();
    cout << "Execution time: FCFS " << durationer4 << " milliseconds" << endl;

    return 0;
}
