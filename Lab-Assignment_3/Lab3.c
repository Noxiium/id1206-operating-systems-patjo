#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define TLB_ENTRIES_SIZE 16
#define PAGE_SIZE 256
#define PAGE_TABLE_ENTRIES 256
#define FRAME_SIZE 256
#define PHYSICAL_MEMORY 65536

int number_of_page_faults = 0;
int number_of_TLB_hits = 0;
int number_of_addresses_read = 0;

int frame_number = 0;
uint32_t address;
uint8_t physical_memory[PHYSICAL_MEMORY];
uint8_t page_table[PAGE_TABLE_ENTRIES][2];
uint8_t TLB[TLB_ENTRIES_SIZE][2];

void update_page_table(int page_number);
void update_TLB_table(int page_number, int frame_number);
int check_TLB_return_frame_number(int page_number);
void print(int frame_number, int page_offset);
void print_statistics();

int main(int argc, char *argv[]){
    for (int i = 0; i < PAGE_SIZE; i++){
        page_table[i][1] = 0;
    }

    FILE *logical_addresses = fopen((argv[1]), "rb");
    FILE *backing_store = fopen("BACKING_STORE.bin", "rb");

    while (fscanf(logical_addresses, "%u", &address) != EOF){
        number_of_addresses_read++;

        
        int binary = address & 0xFFFF; // Mask the rightmost 16 bits
        int page_number = (binary >> 8) & 0xFF; //  
        int page_offset = binary & 0xFF;        

        int TLB_found_frame_number = check_TLB_return_frame_number(page_number);
        if (TLB_found_frame_number != -1){
            print(TLB_found_frame_number,page_offset);
            number_of_TLB_hits++;
        }

        else if (page_table[page_number][1] != 0){
            int pt_found_frame_number = page_table[page_number][0];
            print(pt_found_frame_number, page_offset);
            update_TLB_table(page_number, pt_found_frame_number);
        }
        else{
            fseek(backing_store, page_number * PAGE_SIZE, SEEK_SET);
            fread(&physical_memory[frame_number * FRAME_SIZE], sizeof(uint8_t), PAGE_SIZE, backing_store);

            update_page_table(page_number);
            update_TLB_table(page_number, frame_number);
            print(frame_number, page_offset);
            frame_number++;
            number_of_page_faults++;
        }
    }
    print_statistics();
    fclose(backing_store);
    fclose(logical_addresses);
}

void print_statistics(){
    float page_fault_rate = (float)number_of_page_faults/number_of_addresses_read;
    float TLB_hit_rate = (float)number_of_TLB_hits/number_of_addresses_read;
    printf("Page faults: %d\n", number_of_page_faults);
    printf("TLB hits: %d\n", number_of_TLB_hits);
    printf("Total addresses read: %d\n", number_of_addresses_read);
    printf("Page fault rate: %f\n", page_fault_rate);
    printf("TLB hit rate: %f\n", TLB_hit_rate);

}

void update_TLB_table(int page_number, int frame_number){

    for(int i = TLB_ENTRIES_SIZE-1; i > 0; i--){
        TLB[i][0] = TLB[i-1][0];
        TLB[i][1] = TLB[i-1][1];
    }
    TLB[0][0] = page_number;
    TLB[0][1] = frame_number;

}

void update_page_table(int page_number){
    page_table[page_number][0] = frame_number;
    page_table[page_number][1] = 1;
}

int check_TLB_return_frame_number(int page_number){
    int j = 0;
    for (int j = 0; j < TLB_ENTRIES_SIZE; j++){
        if (TLB[j][0] == page_number){
            return TLB[j][1];
        }
    }
    return -1;
}

void print(int frame_number, int page_offset){
    int physical_address = (frame_number << 8) | page_offset;
    printf("Virtual Address: %d  Physical Address: %d  Value: %d\n", address, physical_address, (int8_t)physical_memory[physical_address]);
}