#ifndef JSONFMT_CONFIG_H
#define JSONFMT_CONFIG_H

#include <stdbool.h>
#include "error.h"

struct jsonfmt_config {
  bool useSpaces;
  long int numSpaces;
  int spacesFlagIndex;
  bool useTabs;
  bool writeToFile;
  bool useLF;
  bool useCRLF;
  bool useStdIn;
  const char** flags;
  int numFlags;
  const char **paths;
  int numPaths;
  char **jsonFiles;
  int numJsonFiles;
  char *errorString;
};

jsonfmt_error_t new_jsonfmt_config(int argc, const char* argv[], struct jsonfmt_config **outConfig);
jsonfmt_error_t free_jsonfmt_config(struct jsonfmt_config *config);

#endif //JSONFMT_CONFIG_H
