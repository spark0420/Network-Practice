#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>

void *printMessage(void *ptr){
    char *message;
    message = (char *)ptr;

    printf("%s \n", message);
}

using namespace std;

//including the main thread, the total number of threads are 3

int main(){

    pthread_t thread1, thread2;

    const char *message1 = "Thread 1";
    const char *message2 = "Thread 2";
    int tid1, tid2;

    if(tid1 = pthread_create(&thread1, NULL, &printMessage, (void *)message1)){
        exit(EXIT_FAILURE);
    }
    if(tid2 = pthread_create(&thread2, NULL, &printMessage, (void *)message2)){
        exit(EXIT_FAILURE);
    }

    cout << "Thread1 ID = " << tid1 << endl;
    cout << "Thread2 ID = " << tid2 << endl;

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    exit(EXIT_SUCCESS);
}

//because the argument type is void, you need to cast it
//like(char *)