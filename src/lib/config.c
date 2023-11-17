#include "config.h"
#include <string.h>
#include <stdio.h>

void saferFree(void *ptr) {
  if (ptr != NULL) {
    free(ptr);
  }
}

bool doesArrayIncludeString(const char *array[],
                            int arrayLen,
                            const char *targetString) {

  for (int i = 0; i < arrayLen; ++i) {
    const char *currentString = array[i];

    const u_int32_t currentStringLen = strlen(currentString);
    const u_int32_t targetStringLen = strlen(targetString);
    const u_int32_t shortestLen = currentStringLen < targetStringLen ? currentStringLen : targetStringLen;

    if (strncmp(currentString, targetString, shortestLen) == 0) {
      return true;
    }
  }
  return false;
}

bool hasUnknownFlags(char *flags[], int numFlags, char **unknownFlag) {
  #define numKnownFlags 9
  const char *knownFlags[numKnownFlags] = {
      "--spaces",
      "-s",
      "--tabs",
      "-t",
      "--write",
      "-w",
      "--lf",
      "--crlf",
      "--spaces",
  };

  for (int i = 0; i < numFlags; ++i) {
    char *flag = flags[i];
    if (!doesArrayIncludeString(knownFlags, numKnownFlags, flag)) {
      *unknownFlag = flag;
      return true;
    }
  }

  return false;
}

bool count_occurences_of_y_in_x(const char *y[], int yLen, const char *x[], int xLen) {
  int count = 0;
  // go over all strings in x
  for (int i = 0; i < xLen; ++i) {
    const char *xString = x[i];
    if (doesArrayIncludeString(y, yLen, xString)) {
      // count each time y includes a current x string
      count++;
    }
  }
  return count;
}

struct args_and_flags {
  char **args;
  int numArgs;
  char **flags;
  int numFlags;
  int spacesFlagIndex;
  struct {
    int spaces;
    int tabs;
    int write;
    int lf;
    int crlF;
  } flagCount;
};

void count_each_flag(const char *flag, int flagIndex, struct args_and_flags *argsAndFlags) {
#define spacesFlagsLen 2
  const char *spacesFlags[] = {"--spaces", "-s"};

  if (doesArrayIncludeString(spacesFlags, spacesFlagsLen, flag)) {
    argsAndFlags->flagCount.spaces += 1;
    argsAndFlags->spacesFlagIndex = flagIndex;
    return;
  }

#define tabsFlagsLen 2
  const char *tabsFlags[] = {"--tabs", "-t"};

  if (doesArrayIncludeString(tabsFlags, tabsFlagsLen, flag)) {
    argsAndFlags->flagCount.tabs += 1;
    return;
  }

#define writeFlagsLen 2
  const char *writeFlags[] = {"--write", "-w"};

  if (doesArrayIncludeString(writeFlags, writeFlagsLen, flag)) {
    argsAndFlags->flagCount.write += 1;
    return;
  }

#define lfFlagsLen 2
  const char *lfFlags[] = {"--lf"};

  if (doesArrayIncludeString(lfFlags, lfFlagsLen, flag)) {
    argsAndFlags->flagCount.lf += 1;
    return;
  }

#define crlfFlagsLen 2
  const char *crlfFlags[] = {"--crlf"};

  if (doesArrayIncludeString(crlfFlags, crlfFlagsLen, flag)) {
    argsAndFlags->flagCount.crlF += 1;
    return;
  }
}

void free_args_and_flags(struct args_and_flags *argsAndFlags) {
  saferFree(argsAndFlags->args);
  saferFree(argsAndFlags->flags);
  saferFree(argsAndFlags);
}

void new_args_and_flags(int argc,
                        char *argv[],
                        struct args_and_flags **outputArgsAndFlags) {

  // init memory for struct
  struct args_and_flags *argsAndFlags = malloc(sizeof(struct args_and_flags));
  memset(argsAndFlags, 0, sizeof(struct args_and_flags));

  // init memory for args & flags char pointer arrays - base size on argc
  argsAndFlags->args = malloc(argc * sizeof(char *));
  argsAndFlags->flags = malloc(argc * sizeof(char *));

  for (int i = 0; i < argc; i++) {
    // skip first CLI arg (program name)
    if (i == 0) continue;

    char *currentArg = argv[i];
    const u_int32_t argLen = strlen(currentArg);

    if (argLen == 0) continue;

    // must begin with '-' & be at least 2 chars long
    bool isFlag = strncmp(currentArg, "-", 1) == 0 && argLen >= 2;

    if (isFlag) {
      argsAndFlags->flags[argsAndFlags->numFlags] = currentArg;
      argsAndFlags->numFlags += 1;
      count_each_flag(currentArg, i, argsAndFlags);
    } else {
      argsAndFlags->args[argsAndFlags->numArgs] = currentArg;
      argsAndFlags->numArgs += 1;
    }
  }

  *outputArgsAndFlags = argsAndFlags;
}

jsonfmt_error_t new_jsonfmt_config(int argc, char *argv[], struct jsonfmt_config **output_config) {

  struct jsonfmt_config *config = (struct jsonfmt_config *) malloc(sizeof(struct jsonfmt_config));
  memset(config, 0, sizeof(struct jsonfmt_config));

  // init config with defaults
  config->useSpaces = true;
  config->numSpaces = 2;
  config->useTabs = false;
  config->writeToFile = false;
  config->useLF = true;
  config->useCRLF = false;
  config->useStdIn = true;
  config->paths = NULL;
  config->jsonFilePaths = NULL;

  if (argc <= 1) {
    // no args provided use defaults
    *output_config = config;
    return JSONFMT_OK;
  }

  struct args_and_flags *argsAndFlags = NULL;

  new_args_and_flags(argc, argv, &argsAndFlags);

  char *unknownFlag = NULL;

  if (hasUnknownFlags(argsAndFlags->flags, argsAndFlags->numFlags, &unknownFlag)) {
    printf("found unknown flag: %s \n", unknownFlag);
    return JSONFMT_ERR_UNRECOGNISED_OPTION;
  }

  // print testing =============================================================
  printf("flags: \n");
  for (int i = 0; i < argsAndFlags->numFlags; i++) {
    printf(" %s \n", argsAndFlags->flags[i]);
  }

  printf("args: \n");
  for (int i = 0; i < argsAndFlags->numArgs; i++) {
    printf(" %s \n", argsAndFlags->args[i]);
  }

  printf("spaced index: %d \n", argsAndFlags->spacesFlagIndex);
  //============================================================================
  free_args_and_flags(argsAndFlags);

  return 1;
}


jsonfmt_error_t free_jsonfmt_config(struct jsonfmt_config **config) {
  free(*config);
  return JSONFMT_OK;
}
