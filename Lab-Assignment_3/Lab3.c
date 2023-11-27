#include <stdio.h>
#include <stdint.h>

#define PAGE_SIZE 256
#define PHYSICAL_MEMORY 65536

int main()
{

    // int number = 62493; // logical address
    int frame_number = 0;
    uint32_t address;
    uint8_t physical_memory[PHYSICAL_MEMORY];

    FILE *logical_addresses = fopen("addresses.txt", "rb"); 
    FILE *backing_store = fopen("BACKING_STORE.bin", "rb");
   

    int i = 0;
    while (i < 10)
    {
        fscanf(logical_addresses, "%u", &address);
        int binary = address & 0xFFFF;           // maskningen
        int first_8_bits = (binary >> 8) & 0xFF; // page number
        int second_8_bits = binary & 0xFF;       // page offset

        //printf("Rightmost 16 bits: %d\n", binary);
        //printf("First 8 bits: %d\n", first_8_bits); // page number
        //printf("last 8 bits: %d\n", second_8_bits); // page offset

        int physical_address = (frame_number << 8) | second_8_bits;

        fseek(backing_store, first_8_bits * PAGE_SIZE, SEEK_SET);
        fread(&physical_memory[frame_number * PAGE_SIZE], sizeof(uint8_t), PAGE_SIZE, backing_store);

        
        printf("Virtual Address: %d  Physical Address: %d  Value: %d\n", address, physical_address, (int8_t)physical_memory[physical_address]);
        i++;
        frame_number++;
    }

    fclose(backing_store);
    fclose(logical_addresses);
}