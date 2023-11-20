
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <stdint.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <asm-generic/fcntl.h> //remove this if on a Mac.
#include <sys/stat.h>
#include <sys/wait.h>

// struct Buffer is shares between the different processes
struct Buffer{
int VAR;
int active_readers;    
sem_t *sem_read;
sem_t *sem_write;
};

const int MAX = 15; //constant representing the amximum value for the shared variable VAR

//Declare methods for readers and writer
void reader_method(struct Buffer *buffer_ptr);
void writer_method(struct Buffer *buffer_ptr);


int main(){

    

    //Create a shared memory for the processes
    int shm_fd = shm_open("/shared_memory", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(shm_fd == -1){
        perror("shm_open");
        exit(0);
    }
    //Set the size of the shared memory
    if((ftruncate(shm_fd, sizeof(struct Buffer))) == -1){
        perror("ftruncate");
        exit(0);
    }

    // Map the shared memory to the struct Buffer
    struct Buffer *buffer_ptr = mmap(NULL, sizeof(struct Buffer), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    //Create semaphores for locking the shared variables during critical parts
    buffer_ptr -> sem_read = sem_open("/sem_read", O_CREAT, S_IRUSR | S_IWUSR, 1);
    buffer_ptr -> sem_write = sem_open("/sem_write", O_CREAT, S_IRUSR | S_IWUSR, 1);
    //TODO - handle mmapp error

    //Fork a child process
    pid_t pid = fork();
    if(pid == -1) {
    perror("fork");
    exit(1);
   } 

    if(pid >  0){ // Writer-process
        
        writer_method(buffer_ptr);

        //Wait for the Child process to finish
        wait(NULL);

        //Print the final value of VAR in the shared Buffer.
        printf("VAR value: %d\n", buffer_ptr->VAR);

        //Close and unlink the semaphores
        sem_close(buffer_ptr->sem_read);
        sem_close(buffer_ptr->sem_write);
        sem_unlink("/sem_read");
        sem_unlink("/sem_write");

        //Unmap,close and unlink the shared memory
        munmap(buffer_ptr, sizeof(struct Buffer));
        close(shm_fd);
        shm_unlink("/shared_memory");

        exit(EXIT_SUCCESS);
        
    }else{ //Reader processes

        //Fork a Child process
        pid_t pid2 = fork();

        if(pid2 == -1) {
            perror("fork");
            exit(1);
            }

        if(pid2 > 0){ //First reader process
            reader_method(buffer_ptr);
            
            //Wait for the Child process to finish before exiting process.
            wait(NULL);
            exit(EXIT_SUCCESS);

        } else {  // Second reader process
           reader_method(buffer_ptr);
             exit(EXIT_SUCCESS);
        }

    
   }
}

//Reader method executed by two reader processes
void reader_method(struct Buffer *buffer_ptr){
    
    while(buffer_ptr->VAR < MAX){
  
    //Aquire the reader semaphore to enter critical section,
    //and then increment the ative_readers variable by one.
    sem_wait(buffer_ptr -> sem_read); 
    buffer_ptr -> active_readers++;  

    // The first reader waits for the write semaphore to be available.
    if(buffer_ptr -> active_readers == 1) 
        sem_wait(buffer_ptr -> sem_write); 
        printf("The first reader acquires the lock.\n");
    
    //Release the read semaphore to allow other readers.
    sem_post(buffer_ptr -> sem_read); 
 
    
    //Reads the value of VAR from the shared buffer, and prints out the value together with PID
    pid_t process_id = getpid();
    printf("Reader (%d) reads the value %d\n", process_id, buffer_ptr->VAR);

    //Decrement active_readers variable.
    buffer_ptr -> active_readers--;    

    //If this is the last reader, release the write semaphore to allow wother processes.
    if(buffer_ptr -> active_readers == 0)
        printf("The last reader releases the lock.\n");
        sem_post(buffer_ptr->sem_write);
        
    //Sleep
    usleep(200);
    }

}

//Writer method executed by one writer process.
void writer_method(struct Buffer *buffer_ptr){
    pid_t process_id = getpid();

    while(buffer_ptr->VAR < MAX){

    //Aquire the writer semaphore to enter critical section,
    //and then increment the VAR variable from the shared buffer by one.
    sem_wait(buffer_ptr->sem_write);
    printf("The writer acquires the lock.\n");
    buffer_ptr->VAR++;

    //Print the new VAR value together with the PID.
    printf("The writer (%d) writes the value %d\n", process_id, buffer_ptr->VAR);

    //Release the write semaphore to allow other processes.
    printf("The writer releases the lock.\n");
    sem_post(buffer_ptr->sem_write);

    //Sleep
    usleep(200);
    }
}