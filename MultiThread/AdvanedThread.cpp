#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>


int common_int = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int thread_count = 0;

//These are called serializations

void *consume(void *ptr){

    do{
        pthread_mutex_lock(&mutex1);
        common_int--;
        printf("consumer: %i\n", common_int);
        pthread_mutex_unlock(&mutex1);
        //sleep(1);
    }while(common_int > -60000);

    printf("Consumer done!\n");
    pthread_exit(NULL);
}

void *produce(void *ptr){

    do{
        pthread_mutex_lock(&mutex1);
        common_int++;
        printf("producer: %i\n", common_int);
        pthread_mutex_unlock(&mutex1);
        //sleep(1);
    }while(common_int < 60000);

    printf("Producer done!\n");
    pthread_exit(NULL);

}

using namespace std;

//including the main thread, the total number of threads are 3
int main(){

    pthread_t thread1, thread2;
    int tid1, tid2;

    if(tid1 = pthread_create(&thread1, NULL, &consume, NULL)){
        exit(EXIT_FAILURE);
    }
    if(tid2 = pthread_create(&thread2, NULL, &produce, NULL)){
        exit(EXIT_FAILURE);
    }

    cout << "Thread1 ID = " << tid1 << endl;
    cout << "Thread2 ID = " << tid2 << endl;

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    exit(EXIT_SUCCESS);
}
