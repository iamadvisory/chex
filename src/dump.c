#include "dump.h"
#include <stdio.h>
#include <ctype.h>

void dump(FILE *filename) {
  unsigned char buffer[16];
  size_t bytes_read = fread(buffer, sizeof(buffer[0]), sizeof(buffer), filename);

  size_t offset = 0;

  printf("OFFSET    HEX                                               ASCII\n");
  while (bytes_read != 0) {
    printf("%08zX  ", offset);

    for (size_t i = 0; i < bytes_read; i++) {
      printf("%02X ", buffer[i]);
    }

    offset += bytes_read;

    size_t spaces = sizeof(buffer) - bytes_read;
    for (size_t i = spaces; i > 0; i--) {
      printf("   ");
    }
    printf("  ");

    for (size_t i = 0; i < bytes_read; i++) {
      if (isprint(buffer[i])) {
        putchar(buffer[i]);
      } else {
        putchar('.');
      }
    }

    bytes_read = fread(buffer, sizeof(buffer[0]), sizeof(buffer), filename);

    printf("\n");
  }  
}
