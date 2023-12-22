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
RT_QUEUE msg_queue;

RT_SEM queue_semaphore;

int running = 1;
char sendMessage[MAX_MESSAGE_SIZE];

void initMessage() {
    for(int i = 0; i < MAX_MESSAGE_SIZE - 1; i++) {
        sendMessage[i] = i;
    }
    sendMessage[MAX_MESSAGE_SIZE - 1] = '\0';
}

void sender(void *arg) {
    while(running) {
        rt_queue_write(&msg_queue, sendMessage, sizeof(sendMessage), Q_NORMAL);
        // printf("Sender: Message sent\n");

        if(rt_sem_p(&queue_semaphore, /* 500ms */ 500 * 1000 * 1000) != 0) {
            printf("Queue semaphore was not signaled in 500ms");
        }
    }
}

int seconds_elapsed = 0;
void receiver(void *arg) {
    char message[MAX_MESSAGE_SIZE];
    RTIME last = rt_timer_read();
    int success = 0;
    int errors = 0;
    int communicationErrors = 0;
    while(running) {
        ssize_t size = rt_queue_read(&msg_queue, &message, sizeof(message), TM_INFINITE);
        if (size >= 0) {
            // Verify the message
            if(strcmp(message, sendMessage) != 0) {
                communicationErrors++;
            }
            else {
                success++;
            }
        } else {
            errors++;
        }

        RTIME now = rt_timer_read();
        RTIME diff = now - last;
        if(diff > /* 1s */ 1ll * 1000ll * 1000ll * 1000ll ) {
            double sps = (success * 1000000000.0) / diff;
            double eps = (errors * 1000000000.0) / diff;
            double ceps = (communicationErrors * 1000000000.0) / diff;

            success = 0;
            eps = 0;
            ceps = 0; 
            printf("Success: %f/s\nErrors: %f/s\nCommunication errors: %f/s\n", sps, eps, ceps);

            double bytesPerSecond = sps * sizeof(sendMessage);
            printf("Speed: %fB/s\n\n\n", bytesPerSecond);
            last = rt_timer_read();

            // Exit after 5 seconds to mitigate session freeze.
            if(seconds_elapsed++ > 5) {
                running = 0;
                exit(0);
            }
        }

        rt_sem_v(&queue_semaphore);
    }

}

int main() {
    mlockall(MCL_CURRENT | MCL_FUTURE);

    initMessage();

    rt_sem_create(&queue_semaphore, "QueueSemaphore", 0, S_FIFO);

    rt_queue_create(&msg_queue, "MessageQueue", QUEUE_SIZE * MAX_MESSAGE_SIZE, QUEUE_SIZE, Q_FIFO);

    rt_task_create(&sender_task, "Sender Task", 0, 50, 0);
    rt_task_create(&receiver_task, "Receiver Task", 0, 50, 0);

    rt_task_start(&sender_task, &sender, NULL);
    rt_task_start(&receiver_task, &receiver, NULL);

    pause();

    rt_queue_delete(&msg_queue);

    return 0;
}