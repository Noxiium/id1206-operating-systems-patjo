#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>
#include <unistd.h>

#define CYLINDER_REQUESTS 1000
#define DISK_FIRST_CYLINDER_INDEX 0
#define DISK_LAST_CYLINDER_INDEX 4999
#define DISK_CYLINDERS 5000


// Function declaration
void FCFS(int head_position, int request_queue[]);
void SSTF(int head_position, int request_queue[]);
void SCAN(int head_position, int request_queue[]);
void C_SCAN(int head_position, int request_queue[]);
void LOOK(int head_position, int request_queue[]);
void C_LOOK(int head_position, int request_queue[]);
int comparefunction(const void *a, const void *b);
void copy_array(int array_to_copy[], int new_array[]);

int main(int argc, char *argv[])
{
    // Extracts the argument (initial head position) from command line, and converts it to an int.
    int head_position = atoi(argv[1]);
    printf("Head start position: %d\n", head_position);

    // Generating a request queue with unique random numbers, simulating 1000 cylinder requests. 
    int request_queue[CYLINDER_REQUESTS];
    int used_numbers[DISK_CYLINDERS] = {0};  // Array to keep track of used numbers, initialized to 0

    // Seed the random number generator with the current time to ensure a different set of random numbers in each run.
    srand(time(0)); 

    for (int i = 0; i < CYLINDER_REQUESTS; i++) {
        int new_number;
        do {
            new_number = rand() % DISK_CYLINDERS;
        } while (used_numbers[new_number] == 1);  // Check if the number has already been used

        request_queue[i] = new_number;
        used_numbers[new_number] = 1;  // Mark the number as used
    }
    
    // Function calls for various disk-scheduling algorithms.
    FCFS(head_position, request_queue);
    SSTF(head_position, request_queue);
    SCAN(head_position, request_queue);
    C_SCAN(head_position, request_queue);
    LOOK(head_position, request_queue);
    C_LOOK(head_position, request_queue);
}

// FCFS (First Come First Serve)  Disk-scheduling algorithm
// Handles the cylinder requests in the queue in the order the requests arrived.
// Counts the total head movements needed to fulfill all request.
void FCFS(int head_position, int request_queue[])
{

    int current_head_position = head_position;
    int head_movements = 0;

    for (int i = 0; i < CYLINDER_REQUESTS; i++)
    {

        int diff = abs(current_head_position - request_queue[i]);
        head_movements += diff;
        current_head_position = request_queue[i];
    }

    printf("Head movements required for FCFS: %d\n", head_movements);
}

// SSTF (Shortest seek time first) Disk scheduling algorithm
// Selects the request with the shortest distance from the current head position.
// Counts the total head movements needed to fulfill all request.
void SSTF(int head_position, int request_queue[])
{

    int handled_requests[CYLINDER_REQUESTS] = {0};
    int current_head_position = head_position;
    int head_movements = 0;

    for (int i = 0; i < CYLINDER_REQUESTS; i++)
    {
        int closest_cylinder_index = 0;
        int min_diff = INT_MAX;

        // Find the closest unhandled request
        for (int j = 0; j < CYLINDER_REQUESTS; j++)
        {
            int diff = abs(current_head_position - request_queue[j]);

            if (diff < min_diff && (handled_requests[j] == 0))
            {
                min_diff = diff;
                closest_cylinder_index = j;
            }
        }

        handled_requests[closest_cylinder_index] = 1;
        current_head_position = request_queue[closest_cylinder_index];
        head_movements += min_diff;
    }

    printf("Head movements required for SSTF: %d\n", head_movements);
}

// SCAN Disk scheduling algorithm
// Starts from the head position and move towards one end of the disk, servicing request along the way. 
// Once it reaches the end, it reverses direction.
// Counts the total head movements needed to fulfill all request.
void SCAN(int head_position, int request_queue[]){

    // Creates a copy of the request queue
    int copy_queue[CYLINDER_REQUESTS];
    copy_array(request_queue,copy_queue);

    // Sort the copied queue for easier processing
    qsort(copy_queue, CYLINDER_REQUESTS, sizeof(int), comparefunction);
    
    
    bool left = true;
    int head_movements = 0;
    int index;

    // Finding the index where the head position is located in the request queue
    for (int i = 0; i < CYLINDER_REQUESTS; i++){
        if (head_position <= copy_queue[i]){
            index = i;
            break;
        }
    }
    
    int current_head_position = head_position;

    // Servicing request to the left of the initial head position, until the first cylinder of the disk is reached.
    while (left){
        int diff = 0;
        if (head_position == copy_queue[index - 1])
        {
            diff = abs(current_head_position - copy_queue[index - 1]);
            head_movements += diff;
            current_head_position = head_position;
            copy_queue[index - 1] = -1;
            index--;
        }
        if (head_position == DISK_FIRST_CYLINDER_INDEX){
            diff = abs(current_head_position - head_position);
            head_movements += diff;
            current_head_position = head_position;
            left = false;
            break;
        }
        head_position--;
    }

    // The head position changes direction and processes requests to the right.
    while (!left){

        while (copy_queue[index] == -1)
            index++;
        int diff = 0;
        
        if (head_position == copy_queue[index])
        {
            diff = abs(current_head_position - copy_queue[index]);
            head_movements += diff;
            current_head_position = head_position;
            copy_queue[index] = -1;
            index++;
        }
         if (head_position == DISK_LAST_CYLINDER_INDEX)
            break;

        head_position++;
    }

    printf("Head movements required for SCAN: %d\n", head_movements);
}

// C-SCAN (Circular SCAN) Disk scheduling algorithm
// Starts from the head position and move towards the end of the disk, servicing request along the way. 
// Once it reaches the end, it jumps to the beginning of the disk.
// Counts the total head movements needed to fulfill all request.
void C_SCAN(int head_position, int request_queue[]){

    // Creates a copy of the request queue
    int copy_queue[CYLINDER_REQUESTS];
    copy_array(request_queue,copy_queue);

    // Sort the copied queue for easier processing
    qsort(copy_queue, CYLINDER_REQUESTS, sizeof(int), comparefunction);

    bool right = true;
    int head_movements = 0;
    int index;
    int diff = 0;

    // Finding the index where the head position is located in the request queue
    for (int i = 0; i < CYLINDER_REQUESTS; i++){
        if (head_position <= copy_queue[i]){
            index = i;
            break;
        }

        if(i == CYLINDER_REQUESTS-1){
            index = 0;
        }
    }
    int current_head_position = head_position;

    // Servicing request to the right of the initial head position, until the last cylinder of the disk is reached.
    while (right){
        
        if (head_position == copy_queue[index])
        {
            diff = abs(current_head_position - copy_queue[index]);
            head_movements += diff;
            current_head_position = head_position;
            copy_queue[index] = -1;
            index++;
        }
        if (head_position == DISK_LAST_CYLINDER_INDEX){
            diff = abs(current_head_position - head_position);
            head_movements += diff;
            right = false;
            head_position = 0;
            current_head_position = 0;
            index  = 0; //JUMP
            break;
        }
        head_position++;
    }

    // Jumps to the first cylinder of the disk and then handles request to the right of the current head position.
    while (!right){
       
        if (head_position == copy_queue[index])
        {
            int diff = abs(current_head_position - copy_queue[index]);
            head_movements += diff;
            current_head_position = head_position;
            copy_queue[index] = -1;
            index++;
        }
         if (copy_queue[index] == -1 || head_position == DISK_LAST_CYLINDER_INDEX)
            break;

        head_position++;
    }


    printf("Head movements required for C_SCAN: %d\n", head_movements);
}

// LOOK Disk scheduling algorithm
// Similar to SCAN, starts from head position and moves towards one end of the disk, handles cylinder request along the way.
// Doesn't go all the way to the end, when the last request in that direction is handled - it reverses direction. 
// Counts the total head movements needed to fulfill all request.
void LOOK(int head_position, int request_queue[]){

    // Creates a copy of the request queue
    int copy_queue[CYLINDER_REQUESTS];
    copy_array(request_queue,copy_queue);

    qsort(copy_queue, CYLINDER_REQUESTS, sizeof(int), comparefunction);
    
    bool left = true;
    int head_movements = 0;
    int index;

     // Finding the index where the head position is located in the request queue
    for (int i = 0; i < CYLINDER_REQUESTS; i++){
        if (head_position < copy_queue[i]){
            index = i;
            break;
        }
    }
    
    int current_head_position = head_position;

    // Servicing request to the left of the initial head position, until the last request in that direction is reached.
    while (left){
        int diff = 0;
        if (head_position == copy_queue[index-1])
        {
            diff = abs(current_head_position - copy_queue[index - 1]);
            head_movements += diff;
            current_head_position = head_position;
            copy_queue[index - 1] = -1;
            index--;
        }
        if (index == 0){
            left = false;
            break;
        }
        head_position--;
    }

    // The head position changes direction and handles requests to the right.
    while (!left){

        // Skipping over allready serviced requests
        while (copy_queue[index] == -1)
            index++;
        int diff = 0;
        
        if (head_position == copy_queue[index])
        {
            diff = abs(current_head_position - copy_queue[index]);
            head_movements += diff;
            current_head_position = head_position;
            copy_queue[index] = -1;
            index++;
        }
         if (index == CYLINDER_REQUESTS || head_movements == DISK_LAST_CYLINDER_INDEX)
            break;

        head_position++;
    }
    printf("Head movements required for LOOK: %d\n", head_movements);
}

// C-LOOK (Circular LOOK) Disk scheduling algorithm
// Similar to C-SCAN , starts from head position and moves towards one end of the disk, handles cylinder request along the way.
// Doesn't go all the way to the end, when the last request in that direction is handled - it jumps to the first request from the beginning of the disk.
// Counts the total head movements needed to fulfill all request.
void C_LOOK(int head_position, int request_queue[])
{
    int copy_queue[CYLINDER_REQUESTS];
    copy_array(request_queue,copy_queue);

     // Sort the copied queue for easier processing
    qsort(copy_queue, CYLINDER_REQUESTS, sizeof(int), comparefunction);

    bool right = true;
    int head_movements = 0;
    int index;
    int diff = 0;

     // Finding the index where the head position is located in the request queue
    for (int i = 0; i < CYLINDER_REQUESTS; i++){
        if (head_position <= copy_queue[i]){
            index = i;
            break;
        }

        if(i == CYLINDER_REQUESTS-1){
            index = 0;
        }
    }
    int current_head_position = head_position;

    // Handling requests to the right of the initial head position, until it reaches the last request in that direction.
    while (right){
        
        if (head_position == copy_queue[index])
        {
            diff = abs(current_head_position - copy_queue[index]);
            head_movements += diff;
            current_head_position = head_position;
            copy_queue[index] = -1;
            index++;
        }
        if (index == CYLINDER_REQUESTS){
            right = false;
            head_position = copy_queue[0];
            current_head_position = copy_queue[0];
            index  = 0; //JUMP
            break;
        }
        head_position++;
    }

    // Jumps to the first request from the beginning of the disk and then handles request to the right of the current head position.
    while (!right){
       
        if (head_position == copy_queue[index])
        {
            int diff = abs(current_head_position - copy_queue[index]);
            head_movements += diff;
            current_head_position = head_position;
            copy_queue[index] = -1;
            index++;
        }
         if (copy_queue[index] == -1 || index == CYLINDER_REQUESTS)
            break;

        head_position++;
    }

    printf("Head movements required for C_LOOK: %d\n", head_movements);
}

// Function is used by the qsort function to sort integers in ascending order.
int comparefunction(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

// Takes two arrays and copy all elements from array_to_copy into new_array.
void copy_array(int array_to_copy[], int new_array[]){

    for (int i = 0; i < CYLINDER_REQUESTS; i++)
    {
        new_array[i] = array_to_copy[i];
    }
}


