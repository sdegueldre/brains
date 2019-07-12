#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#define MEM_SIZE 30000

int main(int argc, char* argv[]) {
  if(argc != 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return -1;
  }

  int fd = open(argv[1], 0);
  if(fd == -1) {
    perror(argv[1]);
    return -1;
  }

  char memory[MEM_SIZE] = {0};
  int32_t dp = 0; // Data pointer

  char instruction;
  while(read(fd, &instruction, 1)) {
    // printf("Instruction: %c\nMemory: ", instruction);
    // for(uint8_t i = 0; i < MEM_SIZE; i++)
    //   printf("0x%02x ", memory[i]);
    // putchar('\n');
    switch(instruction) {
      case '>': dp++; break;
      case '<': dp--; break;
      case '+': memory[dp]++; break;
      case '-': memory[dp]--; break;
      case '.': putchar(memory[dp]); fflush(stdout); break;
      case ',': memory[dp] = (char)getchar(); break;
      case '[':
        if(memory[dp] == 0) {
          uint32_t depth = 1;
          while(depth != 0) {
            read(fd, &instruction, 1);
            switch(instruction) {
              case '[': depth++; break;
              case ']': depth--; break;
            }
          }
        }
        break;
      case ']':
      {
        // printf("Seeking matching [:\n");
        uint32_t depth = 1;
        do {
          // previous byte is current instruction,
          // the one before is the preceding instruction
          lseek(fd, -2, SEEK_CUR);
          read(fd, &instruction, 1);
          switch(instruction) {
            case '[': depth--; break;
            case ']': depth++; break;
          }
        } while(depth != 0);
        lseek(fd, -1, SEEK_CUR); // Repeat [ so that it can skip the block
      }
      break;
    }
  }
  close(fd);
}
