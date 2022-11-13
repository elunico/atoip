#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct options {
  int padded : 1;
  int reversed : 1;
  int extended : 1;
};

void usage() {
  fprintf(stderr, "Usage: ./atoip [ -r ] [ -p ] [ -e ] STRING [ STRING... ]");
  exit(1);
}

void try_increment(char **sptr, int index) {
  char *string = *sptr;
  if (index < 0) {
    fprintf(stderr, "Too Many Increments!\n");
    abort();
  }
  string[index]++;
  if (string[index] > 'z') {
    try_increment(sptr, index - 1);
    string[index] = 'a';
  }
}

struct options get_options(int *argc, char **argv[]) {
  char ch;
  struct options opts = {0};
  int count = 0;
  while ((ch = getopt(*argc, *argv, "pr")) != -1) {
    switch (ch) {
    case 'p':
      opts.padded = 1;
      count++;
      break;
    case 'r':
      opts.reversed = 1;
      count++;
      break;
    case 'e':
      opts.extended = 1;
      count++;
      break;
    case '?':
    default:
      usage();
    }
  }
  *argc -= count;
  *argv += count;
  return opts;
}

int main(int argc, char *argv[]) {

  if (argc < 2)
    usage();

  struct options opts = get_options(&argc, &argv);

  int maxlen = opts.extended ? 8 : 2;
  char *static_ident = opts.extended ? "aaaaaaaa" : "aa";

  char *ident = strdup(static_ident); // don't mutate a static string

  for (int i = 1; i < argc; i++) {
    char *convertee = argv[i];
    int length = strlen(convertee);
    int blocks = length / 4;
    int extra = length % 4;

    printf("int %s[] = {", ident);
    for (int i = 0; i < blocks; i++) {
      printf("0x%x%x%x%x", convertee[i * 4 + 3], convertee[i * 4 + 2],
             convertee[i * 4 + 1], convertee[i * 4 + 0]);

      if (((i + 1) < blocks) || extra)
        printf(", ");
    }

    if (extra) {
      printf("0x");
      if (opts.padded)
        for (int i = 0; i < 4 - extra; i++)
          printf("00");

      for (int i = 0; i < extra; i++)
        printf("%x", convertee[length - extra + (extra - i - 1)]);
    }

    printf("};\n");

    try_increment(&ident, maxlen - 1);
  }

  free(ident);
}
