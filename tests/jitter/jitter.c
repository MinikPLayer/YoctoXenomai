#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <math.h>


#define CLOCK_RES 1e-9 //Clock resolution is 1 ns by default
RTIME loop_period = 1e4; // 1ms

RT_TASK loop_task;

#define DATA_SIZE 10000

RTIME data[DATA_SIZE];
int data_ptr;

void reset() {
    data_ptr = 0;
    for(int j = 0; j < DATA_SIZE; j++) {
        data[j] = 0;
    }
}

void print_data(int priority) {
    char filePath[30];
    sprintf(filePath, "results/resullt%d.txt", priority);
    FILE* file = fopen(filePath, "w");
    if(file == NULL) {
        printf("!!! Cannot open file at %s !!!\n", filePath);
        exit(2);
    }

    printf("Writing results to file %s...\n", filePath);
    fprintf(file, "data%d = [", priority);
    for(int i = 1; i < DATA_SIZE; i++) {
        fprintf(file, "%lld", data[i]);
        if(i != DATA_SIZE - 1) 
            fprintf(file, ", ");
    }
    fprintf(file, "]\n");

    fclose(file);
    printf("Data written.\n");
}

void loop_task_proc(void* priority)
{  
    int p = *((int*)priority);

    RT_TASK_INFO curtaskinfo;
    RT_TASK *curtask = rt_task_self();
    rt_task_inquire(curtask, &curtaskinfo);

    //Make the task periodic with a specified loop period
    rt_task_set_periodic(NULL, TM_NOW, loop_period);

    RTIME tlast = rt_timer_read();

    // Start the task loop
    while(1) {
        if(data_ptr == DATA_SIZE) {
            print_data(p);
            reset(p);
            exit(0);
        }

        RTIME now = rt_timer_read();
        RTIME diff = (now - tlast);
        data[data_ptr++] = diff;

        tlast = now;
        rt_task_wait_period(NULL);
    }
}

int main(int argc, char **argv)
{
    if(argc < 3) {
        printf("Usage: jitter [period in ns] [priority]\n");
        exit(0);
    }

    loop_period = atol(argv[1]) * 1000;
    if(loop_period < 5000) {
        printf("Too small loop period\n");
        exit(1);
    }

    int priority = atoi(argv[2]);
    if(priority < 0 || priority >= 100) {
        printf("Invalid priority. Can't be lower than 0 or higher than 99\n");
        exit(3);
    }
    printf("Using priority %d\n", priority);
    printf("Using loop period: %lld (%.3fms)\n", loop_period, loop_period / 1000000.0);

    // Init priorities
    reset();

    char str[20];

    //Lock the memory to avoid memory swapping for this program
    mlockall(MCL_CURRENT | MCL_FUTURE);

    printf("Starting cyclic task...\n");

    //Create the real time task
    sprintf(str, "cyclic_task");

    rt_task_create(&loop_task, str, 0, priority, 0);

    //Since task starts in suspended mode, start task
    rt_task_start(&loop_task, &loop_task_proc, &priority);

    //Wait for Ctrl-C
    pause();

    return 0;
}