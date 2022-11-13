#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// static const int INT_SIZE = sizeof(int);
#define INT_SIZE sizeof(int)

static const char *usage_string =
    "Usage: ./atoip [ -r ] [ -p ] [ -e ] STRING [ STRING... ]";

static const char *help_fmt =
    "\033[7m\033[1mATOIP Converter\033[0m \n"
    "\n"
    "  %s\n"
    "\n"
    "\033[1mSummary\033[0m\n"
    "  This program takes command line strings and outputs equivalent int \n"
    "  arrays.\n"
    "\n"
    "  Each STRING is converted into an int [] array with ints corresponding\n"
    "  to the chars in the string\n\n"
    "  Additional STRING arguments will output more arrays. To keep\n"
    "  everything\n"
    "  in a single array, ensure it is a single argument\n"
    "  \n"
    "\033[1mOptions\033[0m\n"
    "  The -r option reverses the endianness of the ints output in the array.\n"
    "  By default it uses x86 Little-Endian output\n"
    "\n"
    "  The -p option pads final ints to have the same length as all others.\n"
    "  Without the -p option the final int in the array\n"
    "  will only be as long as needed, but with the -p option it will have\n"
    "  additional 0s to be as long as all other ints\n"
    "\n"
    "  The -e option allows extended mode. By default you can only output 676\n"
    "  arrays (26*26) but with the -e option you can have up to \n"
    "  208,827,064,576 arrays (26^8)\n";

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
      printf(help_fmt, usage_string);
      exit(0);
    case '?':
    default:
      usage();
    }
  }
  *argc -= count;
  *argv += count;
  return opts;
}

int atoip(struct options opts, int scount, char *strings[]) {
  static char const *const DIGIT_FMT = "%02x";
  int maxlen = opts.extended ? 8 : 2;
  char *static_ident = opts.extended ? "aaaaaaaa" : "aa";

  char *ident = strdup(static_ident); // don't mutate a static string
  for (int i = 0; i < scount; i++) {
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

    try_increment(&ident, maxlen - 1);
  }
  free(ident);
}

int main(int argc, char *argv[]) {

  if (argc < 2)
    usage();

  struct options opts = get_options(&argc, &argv);

  atoip(opts, argc - 1, (char **)(argv + 1));
}
