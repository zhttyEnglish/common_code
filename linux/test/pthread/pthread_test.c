#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define max_count 5000
int g_num = 0;

pthread_mutex_t mut;

void *counter1(void * args){
    int i = 1;
    while(i <= max_count / 4){
		pthread_mutex_lock(&mut);
        g_num ++;
        printf("pthread 1 num %d \n", g_num);
		pthread_mutex_unlock(&mut);
        usleep(1000);
        i ++;
    }
}

void *counter2(void * args){
    int j = 1;
    while(j <= max_count / 4){
		pthread_mutex_lock(&mut);
        g_num ++;
		printf("pthread 2 num %d \n", g_num);
		pthread_mutex_unlock(&mut);
        usleep(1000);
        j ++;
    }
}

int main(){
	pthread_mutex_init(&mut, NULL);
    pthread_t t1;
    pthread_t t2;

    pthread_create(&t1, NULL, counter1, NULL);
    pthread_create(&t2, NULL, counter2, NULL);
	
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

    return 0;
}
