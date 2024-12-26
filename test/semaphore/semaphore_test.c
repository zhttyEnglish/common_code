#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>


int queue_t;
sem_t p; //p信号
sem_t q; //q信号

void *p_func(void * args){ //生成随机数
    while(1){
        sem_wait(&p);
		queue_t = rand() % 1000 + 1;
        printf("p : %d \n", queue_t);
        sem_post(&q);
        usleep(10000);
    }
}

void *q_func(void * args){ //q置-1 
    int temp;
    while(1){
        sem_wait(&q);
        temp = queue_t;
        queue_t = -1;
        printf("q : %d \n", queue_t);
		printf("temp : %d \n", temp);
		sem_post(&p);
        usleep(10000);
    }
}

int main(){
    sem_init(&p, 0, 1);
    sem_init(&q, 0, 0);
    srand(time(NULL));

    pthread_t pid, qid;
    pthread_create(&pid, NULL, p_func, NULL);
    pthread_create(&qid, NULL, q_func, NULL);

    pthread_join(pid, NULL);
    pthread_join(qid, NULL);

    sem_destroy(&p);
    sem_destroy(&q);
    
    return 0;
}
