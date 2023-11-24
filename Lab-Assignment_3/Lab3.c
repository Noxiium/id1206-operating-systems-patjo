#include <stdio.h> 
#define PAGE_SIZE 256

int main(){

int number = 62493; // logical address

int binary = number & 0xFFFF; //maskningen
int first_8_bits = (binary >> 8) & 0xFF;
int second_8_bits = binary & 0xFF;


printf("Rightmost 16 bits: %d\n", binary);

printf("First 8 bits: %d\n", first_8_bits);  // page number
printf("last 8 bits: %d\n", second_8_bits);  // page offset 

int frame_number;
FILE *backing_store = fopen("BACKING_STORE.bin", "rb");


// Läs in sidan från backing store
fseek(backing_store, first_8_bits * PAGE_SIZE, SEEK_SET);
fread(&frame_number, sizeof(int), 1, backing_store);

printf("%d", frame_number);

fclose(backing_store);
}