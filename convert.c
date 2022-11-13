#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
