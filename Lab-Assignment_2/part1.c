#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <stdint.h> // Include this header for intptr_t

int buffer = 0;
pthread_mutex_t mutex;

void *method(){
    int counter = 0;

    while(buffer < 15){
        pthread_mutex_lock(&mutex);

        pthread_t thread_id = pthread_self();
        pid_t process_id = getpid();

        if (buffer < 15){
            printf("Thread ID: %lu, Process ID: %d, Buffer: %d\n", thread_id, process_id, buffer);
            buffer = buffer + 1;
            counter++;
        }

        pthread_mutex_unlock(&mutex);  
        usleep(10); 
    }

    int *result = malloc(sizeof(int));
    *result = counter;
    return (void *)result;
}

int main(){
    pthread_mutex_init(&mutex, NULL);

    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;

    

    pthread_create(&thread1, NULL, &method, NULL);
    pthread_create(&thread2, NULL, &method, NULL);
    pthread_create(&thread3, NULL, &method, NULL);

    void *t_count1;
    void *t_count2;
    void *t_count3;

    pthread_join(thread1, &t_count1);
    pthread_join(thread2, &t_count2);
    pthread_join(thread3, &t_count3);

    printf("Thread %d changed buffer %d times.\n", 1, *(int *)t_count1);
    printf("Thread %d changed buffer %d times.\n", 2, *(int *)t_count2);
    printf("Thread %d changed buffer %d times.\n", 3, *(int *)t_count3);
    
    return 0;
}