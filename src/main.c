#include <stdio.h>
#include <string.h>
#include "dump.h"
#include "edit.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s -<arg> <file>\n", argv[0]);
    printf("Type %s --help for help\n", argv[0]);
    return 1;
  }

  if (argc < 3 && (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "-e") == 0)) {
    fprintf(stderr, "Error: missing filename\n");
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
  } else if (strcmp(argv[1], "-e") == 0) {
    FILE *file = fopen(argv[2], "r+b");
    if (file == NULL) {
      perror("fopen");
      return 1;
    }
    edit(file, argv[2]);
    fclose(file);
  } else if (strcmp(argv[1], "--help") == 0) {
    printf("Usage: %s -<arg> <file>\n", argv[0]);
    printf("Args: -d (dump), -e (edit)\n");
  } else {
    printf("Unknown argument: %s", argv[1]);
    return 1;
  }

  return 0;
}
