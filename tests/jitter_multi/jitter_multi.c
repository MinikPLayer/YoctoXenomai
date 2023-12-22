#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <math.h>


#define CLOCK_RES 1e-9 // Clock resolution is 1 ns by default
RTIME loop_period = 1e4; // 1us

#define TASKS 6
RTIME task_periods[TASKS] = { 1e5, 3e5, 5e5, 1e6, 2e6, 5e6 };

RT_TASK loop_task[TASKS];

#define DATA_SIZE 1000

RTIME data[TASKS][DATA_SIZE];
int data_ptr[TASKS];

void reset(int task) {
    data_ptr[task] = 0;
    for(int j = 0; j < DATA_SIZE; j++) {
        data[task][j] = 0;
    }
}

void print_data(int task) {
    char filePath[30];
    sprintf(filePath, "results/resullt%d.txt", task);
    FILE* file = fopen(filePath, "w");
    if(file == NULL) {
        printf("!!! Cannot open file at %s !!!\n", filePath);
        exit(2);
    }

    printf("Writing results to file %s...\n", filePath);
    fprintf(file, "data%d = [", task);
    for(int i = 1; i < DATA_SIZE; i++) {
        fprintf(file, "%lld", data[task][i]);
        if(i != DATA_SIZE - 1) 
            fprintf(file, ", ");
    }
    fprintf(file, "]\n");

    fclose(file);
    printf("Data written.\n");
}

void loop_task_proc(void* task)
{  
    int t = *((int*)task);
    printf("Started task %d\n", t);

    RT_TASK_INFO curtaskinfo;
    RT_TASK *curtask = rt_task_self();
    rt_task_inquire(curtask, &curtaskinfo);

    //Make the task periodic with a specified loop period
    rt_task_set_periodic(NULL, TM_NOW, task_periods[t]);

    RTIME tlast = rt_timer_read();

    // Start the task loop
    while(1) {
        if(data_ptr[t] == DATA_SIZE) {
            print_data(t);
            data_ptr[t]++;
        }

        RTIME now = rt_timer_read();
        RTIME diff = (now - tlast);

        if(data_ptr[t] < DATA_SIZE)
            data[t][data_ptr[t]++] = diff;

        tlast = now;
        rt_task_wait_period(NULL);
    }
}

int main(int argc, char **argv)
{
    // if(argc < 3) {
    //     printf("Usage: jitter [period in ns] [priority]\n");
    //     exit(0);
    // }

    // loop_period = atol(argv[1]) * 1000;
    // if(loop_period < 5000) {
    //     printf("Too small loop period\n");
    //     exit(1);
    // }

    // int priority = atoi(argv[2]);
    // if(priority < 0 || priority >= 100) {
    //     printf("Invalid priority. Can't be lower than 0 or higher than 99\n");
    //     exit(3);
    // }
    // printf("Using priority %d\n", priority);
    // printf("Using loop period: %lld (%.3fms)\n", loop_period, loop_period / 1000000.0);


    int priority = 50;
    // Lock the memory to avoid memory swapping for this program
    mlockall(MCL_CURRENT | MCL_FUTURE);
    for(int i = 0; i < TASKS; i++) {
        char str[20];
        reset(i);

        printf("Creating cyclic task...\n");

        //Create the real time task
        sprintf(str, "task%d", i);

        rt_task_create(&loop_task[i], str, 0, priority, 0);
    }
    
    for(int i = 0; i < TASKS; i++) {
        //Since task starts in suspended mode, start task
        int i_copy = i;
        printf("Starting cyclic task...\n");
        rt_task_start(&loop_task[i], &loop_task_proc, &i_copy);
    }


    //Wait for Ctrl-C
    pause();

    return 0;
}