#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <stdint.h>
#include <semaphore.h>
#include <sys/mman.h>





sem_t sem_read_counter;
sem_t sem_write_counter;

void* create_shared_memory(size_t size) {
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  return mmap(NULL, size, protection, visibility, -1, 0);
}


void reader_method(){

    sem_wait(&sem_read_counter);
    
    //read buffer
    //print out PID, valueOfBuffer
    pid_t process_id = getpid();
    printf("The reader (%d) reads the value %d\n", process_id, 0);
    sleep(1);

    if(true){
        printf("The last reader releases the lock.");
    }
    sem_post(&sem_read_counter);

}

int main(){

    sem_init(&sem_read_counter, 0, 2);
    sem_init(&sem_write_counter, 1, 1);
    pid_t pid = fork();
    if(pid == -1) {
    perror("fork");
    exit(1);
   } 

    if(pid > 0){ // writer
        
    }




    if(pid == 0){ //Create two readers

        pid_t pid = fork();

        if(pid == -1) {
            perror("fork");
            exit(1);
            }

        if(pid > 0){ //first reader
            reader_method();
        }   

        if(pid == 0){ // second reader
            reader_method();

        }

    
   }
}