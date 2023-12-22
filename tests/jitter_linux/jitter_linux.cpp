#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include <chrono>
#include <thread>
#include <iostream>

using namespace std::literals;
using clock_type = std::chrono::high_resolution_clock;

#define CLOCK_RES 1e-9 //Clock resolution is 1 ns by default
RTIME loop_period = 1e4; // 1ms

RT_TASK loop_task;

#define DATA_SIZE 1000

long long data[DATA_SIZE];
int data_ptr;

void reset() {
    data_ptr = 0;
    for(int j = 0; j < DATA_SIZE; j++) {
        data[j] = 0;
    }
}

void print_data() {
    char filePath[30];
    sprintf(filePath, "results/resullt.txt");
    FILE* file = fopen(filePath, "w");
    if(file == NULL) {
        printf("!!! Cannot open file at %s !!!\n", filePath);
        exit(2);
    }

    printf("Writing results to file %s...\n", filePath);
    fprintf(file, "data = [");
    for(int i = 2; i < DATA_SIZE; i++) {
        fprintf(file, "%lld", data[i]);
        if(i != DATA_SIZE - 1) 
            fprintf(file, ", ");
    }
    fprintf(file, "]\n");

    fclose(file);
    printf("Data written.\n");
}

// void loop_task_proc(void* priority)
// {  
//     int p = *((int*)priority);

//     RT_TASK_INFO curtaskinfo;
//     RT_TASK *curtask = rt_task_self();
//     rt_task_inquire(curtask, &curtaskinfo);

//     //Make the task periodic with a specified loop period
//     rt_task_set_periodic(NULL, TM_NOW, loop_period);

//     RTIME tlast = rt_timer_read();

//     // Start the task loop
//     while(1) {
//         if(data_ptr == DATA_SIZE) {
//             print_data(p);
//             reset(p);
//             exit(0);
//         }

//         RTIME now = rt_timer_read();
//         RTIME diff = (now - tlast);
//         data[data_ptr++] = diff;

//         tlast = now;
//         rt_task_wait_period(NULL);
//     }
// }

void my_ev_handler(int signo) {
    clock_t tlast;
    tlast = clock();
    switch (signo) 
    {
        case SIGTSTP:
            if(data_ptr == DATA_SIZE) {
                print_data();
                exit(0);
            }

            clock_t now = clock();
            data[data_ptr++] = (now - tlast) / CLOCKS_PER_SEC;

            tlast = now;
            break;
    }
}

int main(int argc, char **argv)
{
    if(argc < 2) {
        printf("Usage: jitter [period in us]\n");
        exit(0);
    }

    loop_period = atol(argv[1]);
    if(loop_period < 5) {
        printf("Too small loop period\n");
        exit(1);
    }

    // int priority = atoi(argv[2]);
    // if(priority < 0 || priority >= 100) {
    //     printf("Invalid priority. Can't be lower than 0 or higher than 99\n");
    //     exit(3);
    // }
    printf("Using loop period: %lld (%.3fms)\n", loop_period, loop_period / 1000.0);

    // Init priorities
    reset();

    auto when_started = clock_type::now(); 

    auto delay = 100us;
    auto target_time = when_started + delay;

    auto tlast = clock_type::now();
    while(true) {
        if(data_ptr == DATA_SIZE) {
            print_data();
            exit(0);
        }

        auto now = clock_type::now();
        data[data_ptr++] = std::chrono::duration_cast<std::chrono::nanoseconds>(now - tlast).count();
        tlast = now;

        std::this_thread::sleep_until(target_time);
        target_time += delay;
    }

    //Wait for Ctrl-C
    pause();

    return 0;
}