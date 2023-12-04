#include <stdio.h>
#include <stdlib.h>

#define CYLINDER_REQUESTS 1000

// Function declaration
void FCFS(int head_position, int request_queue[]);
void SSTF(int head_position, int request_queue[]);
void SCAN(int head_position, int request_queue[]);
void C_SCAN(int head_position, int request_queue[]);
void LOOK(int head_position, int request_queue[]);
void C_LOOK(int head_position, int request_queue[]);



int main(int argc, char *argv[])
{
    //Extracts the argument from command line, and converts it to an int. 
    int head_position = atoi(argv[1]);
    printf("Head start position: %d\n", head_position);

    
    int request_queue[CYLINDER_REQUESTS];
}




// FCFS
void FCFS(int head_position, int request_queue[])
{

    printf("Head movements required for FCFS: ");
}

// SSTF
void SSTF(int head_position, int request_queue[])
{

    printf("Head movements required for SSTF: ");
}

// SCAN
void SCAN(int head_position, int request_queue[])
{

    printf("Head movements required for SCAN: ");
}

// C-SCAN
void C_SCAN(int head_position, int request_queue[])
{
    printf("Head movements required for C_SCAN: ");
}

// LOOK
void LOOK(int head_position, int request_queue[])
{
    printf("Head movements required for LOOK: ");
}

// C-LOOK
void C_LOOK(int head_position, int request_queue[])
{
    printf("Head movements required for C_LOOK: ");
}
