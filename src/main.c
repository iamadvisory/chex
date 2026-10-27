#include <stdio.h>
#include <string.h>
#include "dump.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s -d <file>\n", argv[0]);
    return 1;
  }

  if (strcmp(argv[1], "-d") == 0) {
    FILE *file = fopen(argv[2], "rb");
    if (file == NULL) {
      perror("fopen");
      return 1;
    }
    dump(file);
    fclose(file);
  } else {
    printf("Other modes are in progress");
  }

  return 0;
}
