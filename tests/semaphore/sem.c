#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/queue.h>
#include <alchemy/timer.h>
#include <alchemy/sem.h>

#define QUEUE_SIZE 10
#define MAX_MESSAGE_SIZE 1000

RT_TASK sender_task;
RT_TASK receiver_task;

RT_SEM sem;

int running = 1;
RTIME sem_time = 0;
void sender(void *arg) {
    while(running) {
        if(rt_sem_p(&sem, TM_INFINITE) != 0) {
            printf("Queue semaphore was not signaled");
        }
        RTIME afterSemTime = rt_timer_read();
        RTIME diff = afterSemTime - sem_time;
        printf("Set to receive time: %lld ns\n\n", diff);
    }
}

int seconds_elapsed = 0;
void receiver(void *arg) {
    while(running) {
        sem_time = rt_timer_read();
        rt_sem_v(&sem);

        RTIME afterSemTime = rt_timer_read();
        RTIME diff = afterSemTime - sem_time;
        printf("Set time: %lld ns\n", diff);
        rt_task_sleep(500ll * 1000 * 1000); // 500ms
    }

}

int main() {
    mlockall(MCL_CURRENT | MCL_FUTURE);

    rt_sem_create(&sem, "QueueSemaphore", 0, S_FIFO);

    rt_task_create(&sender_task, "Sender Task", 0, 50, 0);
    rt_task_create(&receiver_task, "Receiver Task", 0, 50, 0);

    rt_task_start(&sender_task, &sender, NULL);
    rt_task_start(&receiver_task, &receiver, NULL);

    pause();

    return 0;
}