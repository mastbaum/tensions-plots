#include <cstdio>
#include <unistd.h>
#include <string>
#include "Options.h"

Options::Options(int argc, char* argv[]) : Options() {
  int c;
  while ((c = getopt(argc, argv, "abc:")) != -1) {
    switch (c) {
      case 'c':
        config = optarg;
        break;
      case '?':
        if (optopt == 'c')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if(isprint(optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        valid = false;
        break;
    }
  }

  nopt = optind;
}

