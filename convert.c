#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// static const int INT_SIZE = sizeof(int);
#define INT_SIZE sizeof(int)

static const char *usage_string =
    "Usage: ./atoip [ -r ] [ -p ] [ -e ] STRING [ STRING... ]";

char *help() {
  static char help_fmt[1143];
  FILE *help = fopen("help.txt", "r");
  if (help == NULL) {
    fputs("Failed to open ./help.txt! Aborting\n", stderr);
    abort();
  }
  fread(help_fmt, 1142, 1142, help);
  return help_fmt;
}

struct options {
  int padded : 1;
  int reversed : 1;
  int extended : 1;
};

void usage() {
  fprintf(stderr, "%s\n", usage_string);
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
  while ((ch = getopt(*argc, *argv, "prhe")) != -1) {
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
    case 'h':
      printf(help(), usage_string);
      exit(0);
    case '?':
    default:
      usage();
    }
  }
  *argc -= (optind - 1);
  *argv += (optind - 1);
  return opts;
}

int atoip(struct options opts, int scount, char *strings[]) {
  static char const *const DIGIT_FMT = "%02x";
  int maxlen = opts.extended ? 8 : 2;
  char *static_ident = opts.extended ? "aaaaaaa`" : "a`";

  char *ident = strdup(static_ident); // don't mutate a static string
  if (ident == NULL) {
    fputs("strdup failed! Aborting\n", stderr);
    abort();
  }
  for (int i = 0; i < scount; i++) {
    try_increment(&ident, maxlen - 1);

    char *convertee = strings[i];
    int length = strlen(convertee); // need the NUL byte
    int blocks = length / INT_SIZE;
    int extra = length % INT_SIZE;

    printf("int %s[] = {", ident);
    for (int i = 0; i < blocks; i++) {
      printf("0x");
      for (int j = 0; j < INT_SIZE; j++) {
        int block_offset = !opts.reversed ? INT_SIZE - j - 1 : j;
        printf(DIGIT_FMT, convertee[i * INT_SIZE + block_offset]);
      }
      if (((i + 1) < blocks) || extra)
        printf(", ");
    }

    if (extra) {
      printf("0x");
      if (opts.padded && !opts.reversed)
        for (int i = 0; i < INT_SIZE - extra; i++)
          printf("00");

      for (int i = 0; i < extra; i++) {
        int block_offset = !opts.reversed ? extra - i - 1 : i;
        printf(DIGIT_FMT, convertee[length - extra + (block_offset)]);
      }

      if (opts.reversed)
        for (int i = 0; i < INT_SIZE - extra; i++)
          printf("00");
    } else {
      printf(", 0x0");
    }

    printf("};\n");
  }
  free(ident);
  return 0;
}

int main(int argc, char *argv[]) {

  if (argc < 2)
    usage();

  struct options opts = get_options(&argc, &argv);

  if (argc - 1 > 676 && !opts.extended) {
    fputs("Error! Too many arguments. Use -e for extended mode\n", stderr);
    return 2;
  }

  atoip(opts, argc - 1, (char **)(argv + 1));
}
