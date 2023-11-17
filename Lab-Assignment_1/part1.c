#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main() {

    //pipefd[0] == read, [1] == write
    int pipefd[2];
    pipe(pipefd); //Create the pipe

    if(pipefd[0] == -1 || pipefd[1] == -1){
        perror("pipe");
        exit(1);
    }

   pid_t pid = fork(); 

   if(pid == -1) {
    perror("fork");
    exit(1);
   } 

   if(pid > 0){ // Parent
    pid_t wait(int *status);
    close(pipefd[1]); //Close writing end of pipe
    dup2(pipefd[0],STDIN_FILENO);
    execlp("wc","wc","-l",NULL);
    close(pipefd[0]);
    exit(0);
   
   }

   if(pid == 0){ //Child
    close(pipefd[0]); // Close reading end of pipe
    dup2(pipefd[1],STDOUT_FILENO);
    execlp("ls", "ls", "/", NULL);
    close(pipefd[1]);
    exit(0);
   }
}
