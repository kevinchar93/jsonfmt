#ifndef JSONFMT_CONFIG_H
#define JSONFMT_CONFIG_H

#include <stdbool.h>
#include "error.h"

struct jsonfmt_config {
  bool useSpaces;
  short numSpaces;
  bool useTabs;
  bool writeToFile;
  bool useLF;
  bool useCRLF;
  bool useStdIn;
  char **paths;
  int numPaths;
  char **jsonFilePaths;
  int numJsonFilePaths;
};

jsonfmt_error_t new_jsonfmt_config(int argc, char* argv[], struct jsonfmt_config **output_config);
jsonfmt_error_t free_jsonfmt_config(struct jsonfmt_config **config);

#endif //JSONFMT_CONFIG_H
