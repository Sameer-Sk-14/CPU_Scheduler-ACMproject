#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <float.h>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Task {
    int task_id;
    int arrival_time;
    int burst_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
};

void fcfs_schedule(vector<Task>& tasks, vector<pair<int, int>>& schedule) {
    sort(tasks.begin(), tasks.end(), [](Task a, Task b) {
        return a.arrival_time < b.arrival_time;
    });

    int current_time = 0;
    for (auto& task : tasks) {
        if (current_time < task.arrival_time) {
            current_time = task.arrival_time;
        }
        schedule.push_back({current_time, task.task_id});
        current_time += task.burst_time;
        task.completion_time = current_time;
        task.turnaround_time = task.completion_time - task.arrival_time;
        task.waiting_time = task.turnaround_time - task.burst_time;
    }
}

void sjf_schedule(vector<Task>& tasks, vector<pair<int, int>>& schedule) {
    int n = tasks.size();
    vector<int> rt(n);

    for (int i = 0; i < n; i++)
        rt[i] = tasks[i].burst_time;

    int complete = 0, t = 0, minm = INT_MAX;
    int shortest = 0, finish_time;
    bool check = false;

    while (complete != n) {
        for (int j = 0; j < n; j++) {
            if ((tasks[j].arrival_time <= t) && (rt[j] < minm) && rt[j] > 0) {
                minm = rt[j];
                shortest = j;
                check = true;
            }
        }

        if (!check) {
            t++;
            continue;
        }

        if (schedule.empty() || schedule.back().second != tasks[shortest].task_id) {
            schedule.push_back({t, tasks[shortest].task_id});
        }

        rt[shortest]--;
        minm = rt[shortest];
        if (minm == 0)
            minm = INT_MAX;

        if (rt[shortest] == 0) {
            complete++;
            check = false;
            finish_time = t + 1;
            tasks[shortest].waiting_time = finish_time - tasks[shortest].burst_time - tasks[shortest].arrival_time;
            if (tasks[shortest].waiting_time < 0)
                tasks[shortest].waiting_time = 0;
        }
        t++;
    }

    for (int i = 0; i < n; i++) {
        tasks[i].turnaround_time = tasks[i].burst_time + tasks[i].waiting_time;
    }
}

void rr_schedule(vector<Task>& tasks, vector<pair<int, int>>& schedule, int quantum) {
    sort(tasks.begin(), tasks.end(), [](Task a, Task b) {
        return a.arrival_time < b.arrival_time;
    });

    queue<Task*> task_queue;
    int current_time = 0;
    size_t index = 0;

    while (index < tasks.size() || !task_queue.empty()) {
        while (index < tasks.size() && tasks[index].arrival_time <= current_time) {
            task_queue.push(&tasks[index]);
            ++index;
        }

        if (task_queue.empty()) {
            current_time = tasks[index].arrival_time;
            continue;
        }

        Task* current_task = task_queue.front();
        task_queue.pop();

        schedule.push_back({current_time, current_task->task_id});

        if (current_task->burst_time <= quantum) {
            current_time += current_task->burst_time;
            current_task->completion_time = current_time;
            current_task->turnaround_time = current_task->completion_time - current_task->arrival_time;
            current_task->waiting_time = current_task->turnaround_time - current_task->burst_time;
            current_task->burst_time = 0;
        } else {
            current_time += quantum;
            current_task->burst_time -= quantum;
            task_queue.push(current_task);
        }

        while (index < tasks.size() && tasks[index].arrival_time <= current_time) {
            task_queue.push(&tasks[index]);
            ++index;
        }
    }
}

void fuzzy_rr_schedule(vector<Task>& tasks, vector<pair<int, int>>& schedule, int quantum, int fuzz_range) {
    sort(tasks.begin(), tasks.end(), [](Task a, Task b) {
        return a.arrival_time < b.arrival_time;
    });

    queue<Task*> task_queue;
    int current_time = 0;
    size_t index = 0;

    srand(time(0)); // Initialize random seed

    while (index < tasks.size() || !task_queue.empty()) {
        while (index < tasks.size() && tasks[index].arrival_time <= current_time) {
            task_queue.push(&tasks[index]);
            ++index;
        }

        if (task_queue.empty()) {
            current_time = tasks[index].arrival_time;
            continue;
        }

        Task* current_task = task_queue.front();
        task_queue.pop();

        schedule.push_back({current_time, current_task->task_id});

        int effective_quantum = quantum + (rand() % (2 * fuzz_range + 1)) - fuzz_range; // Random quantum within range [quantum - fuzz_range, quantum + fuzz_range]
        effective_quantum = max(1, effective_quantum); // Ensure quantum is at least 1

        if (current_task->burst_time <= effective_quantum) {
            current_time += current_task->burst_time;
            current_task->completion_time = current_time;
            current_task->turnaround_time = current_task->completion_time - current_task->arrival_time;
            current_task->waiting_time = current_task->turnaround_time - current_task->burst_time;
            current_task->burst_time = 0;
        } else {
            current_time += effective_quantum;
            current_task->burst_time -= effective_quantum;
            task_queue.push(current_task);
        }

        while (index < tasks.size() && tasks[index].arrival_time <= current_time) {
            task_queue.push(&tasks[index]);
            ++index;
        }
    }
}

void ratio_based_schedule(vector<Task>& tasks, vector<pair<int, int>>& schedule) {
    int n = tasks.size();
    vector<int> rt(n);

    // Copy the burst time into rt[]
    for (int i = 0; i < n; i++)
        rt[i] = tasks[i].burst_time;

    int complete = 0, t = 0;
    int finish_time;
    bool check = false;

    while (complete != n) {
        double min_ratio = DBL_MAX;
        int shortest = 0;

        // Find the process with the minimum ratio
        for (int j = 0; j < n; j++) {
            if ((tasks[j].arrival_time <= t) && rt[j] > 0) {
                double ratio = static_cast<double>(rt[j]+t) / (t - tasks[j].arrival_time + 1);
                if (ratio < min_ratio) {
                    min_ratio = ratio;
                    shortest = j;
                    check = true;
                }
            }
        }

        if (!check) {
            t++;
            continue;
        }

        if (schedule.empty() || schedule.back().second != tasks[shortest].task_id) {
            schedule.push_back({t, tasks[shortest].task_id});
        }

        rt[shortest]--;

        if (rt[shortest] == 0) {
            complete++;
            check = false;
            finish_time = t + 1;
            tasks[shortest].waiting_time = finish_time - tasks[shortest].burst_time - tasks[shortest].arrival_time;
            if (tasks[shortest].waiting_time < 0)
                tasks[shortest].waiting_time = 0;
        }
        t++;
    }

    for (int i = 0; i < n; i++) {
        tasks[i].turnaround_time = tasks[i].burst_time + tasks[i].waiting_time;
    }
}

void print_schedule(const vector<pair<int, int>>& schedule) {
    for (const auto& entry : schedule) {
        cout << entry.first << "ms" << "--(Task:" << entry.second << ")--";
    }
    cout << "end" << endl;
}

void print_stats(const vector<Task>& tasks) {
    int total_turnaround_time = 0;
    int total_waiting_time = 0;

    for (const auto& task : tasks) {
        total_turnaround_time += task.turnaround_time;
        total_waiting_time += task.waiting_time;
    }

    int n = tasks.size();
    cout << "Average Turnaround Time: " << (double)total_turnaround_time / n << " ";
    cout << "Average Waiting Time: " << (double)total_waiting_time / n << endl;
}

int main() {
    vector<Task> tasks;
    Task task;

    while (cin >> task.task_id >> task.arrival_time >> task.burst_time) {
        tasks.push_back(task);
    }

    vector<Task> ratio_tasks = tasks;
    vector<pair<int, int>> ratio_schedule_list;
    ratio_based_schedule(ratio_tasks, ratio_schedule_list);
    print_schedule(ratio_schedule_list);
    print_stats(ratio_tasks);

    vector<Task> fcfs_tasks = tasks;
    vector<pair<int, int>> fcfs_schedule_list;
    fcfs_schedule(fcfs_tasks, fcfs_schedule_list);
    print_schedule(fcfs_schedule_list);
    print_stats(fcfs_tasks);


    vector<Task> rr_tasks = tasks;
    vector<pair<int, int>> rr_schedule_list;
    rr_schedule(rr_tasks, rr_schedule_list, 2);
    print_schedule(rr_schedule_list);
    print_stats(rr_tasks);

    vector<Task> sjf_tasks = tasks;
    vector<pair<int, int>> sjf_schedule_list;
    sjf_schedule(sjf_tasks, sjf_schedule_list);
    print_schedule(sjf_schedule_list);
    print_stats(sjf_tasks);

    

    return 0;
}