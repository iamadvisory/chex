#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <file>\n", argv[0]);
    return 1;
  }

  FILE *file = fopen(argv[1], "rb");
  if (file == NULL) {
    perror("fopen");
    return 1;
  }
  
  unsigned char buffer[16];
  size_t read_byts = fread(buffer, sizeof(buffer[0]), 16, file);

  size_t offset = 0;

  printf("OFFSET    HEX                                               ASCII\n");
  while (read_byts != 0) {
    printf("%08zX  ", offset);

    for (size_t i = 0; i < read_byts; i++) {
      printf("%02X ", buffer[i]);
    }

    offset += read_byts;

    size_t spaces = 16 - read_byts;
    for (size_t i = spaces; i > 0; i--) {
      printf("   ");
    }
    printf("  ");

    for (size_t i = 0; i < read_byts; i++) {
      if (isprint(buffer[i])) {
        putchar(buffer[i]);
      } else {
        putchar('.');
      }
    }
    read_byts = fread(buffer, sizeof(buffer[0]), 16, file);
    printf("\n");
  }
  
  fclose(file);

  return 0;
}
