#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <errno.h>

void errorHandling(const char *errorMessage);

int main() {
    mqd_t message_queue;
    const char *queue_name = "/mes";
    mq_unlink(queue_name);
    char end_of_message[] = "__END_OF_MESSAGES__";

    //Create the shared message_queue
    message_queue = mq_open(queue_name, O_CREAT | O_RDWR, 0666, NULL);
    if (message_queue == -1)
        errorHandling("mq_open");

    pid_t pid = fork();
    if (pid == -1) 
        errorHandling("fork");
    
    //Child process (Producer)
    if(pid == 0) { 

        //Open the existing message queue with write-only access
        message_queue = mq_open(queue_name, O_WRONLY);
        if (message_queue == -1)
            errorHandling("mq_open");
        
        //Open file to read from
        FILE *file = fopen("textFile.txt", "r");
        if (file == NULL) {
            errorHandling("fopen");
            exit(EXIT_FAILURE);
        }
        // Handle each word separately from the file and sends it to the message queue.
        // The use of %ms format specifier dynamically allocates memory to store the read word.
        char *message = NULL;
        while(fscanf(file, "%ms", &message) != EOF){
            mq_send(message_queue, message, strlen(message) + 1, 0);
            if(errno == EAGAIN){
                //If message queue is full, wait 
                sleep(10000); // sleep 10 ms
            }
            free(message);
        }
        // Signal the end of communication.
        mq_send(message_queue,end_of_message, sizeof(end_of_message), 0);


        fclose(file);
        mq_close(message_queue);
        exit(EXIT_SUCCESS);
    }  

    //Parent process (Consumer)
    if(pid > 0){ 

        int count = 0;

        // Open the existing message queue with read-only access
        message_queue = mq_open(queue_name, O_RDONLY);
        if (message_queue == -1)
            errorHandling("mq_open");
        
        // Continuously receive messages from the message queue  while there are still messages,
        // and count the number of words received.
        while(1){

            // Get attributes of the message queue
            struct mq_attr attr;
            if(mq_getattr(message_queue, &attr) == -1)
                errorHandling("mq_getattr");

            
            //Allocate a buffer large enough for the next message in the message queue,
            //and ensure it is empty by initializing it with zeros.
            char *buffer = (char *)malloc(attr.mq_msgsize);
            if(buffer == NULL)
                errorHandling("malloc");

            memset(buffer, 0, attr.mq_msgsize);

            // Receive message from the message queue
            ssize_t received = mq_receive(message_queue,buffer,attr.mq_msgsize, NULL);
            if(received == -1){
                errorHandling("mq_receive");
                free(buffer);
                break;
            }
            //Check for end of communication.
            if(strcmp(buffer, end_of_message) == 0){
                break;
            }

            //Count the number of words in the message.    
            count = count + 1;   

            free(buffer);
    
        }

        //Wait for Child-process to end, then print no of words in file & close the message queue.
        pid = wait(NULL);
        printf("Total # of words in file: %d\n", count);
        mq_close(message_queue);
        exit(EXIT_SUCCESS);
        
    }
    // Remove the shared message queue.
    mq_unlink(queue_name);
    return 0;
}


void errorHandling(const char *errorMessage){
    perror(errorMessage);
    exit(EXIT_FAILURE);
}

