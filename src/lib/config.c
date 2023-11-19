#include "config.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

void saferFree(void *ptr) {
  if (ptr != NULL) {
    free(ptr);
  }
}

bool array_includes_any_target_strings(const char *array[],
                                       int arrayLen,
                                       const char *targetStrings[],
                                       int targetStringsLen) {
  for (int i = 0; i < arrayLen; ++i) {
    const char *currentArrayString = array[i];

    for (int j = 0; j < targetStringsLen; ++j) {
      const char *currentTargetString = targetStrings[j];

      const u_int32_t currentStringLen = strlen(currentArrayString);
      const u_int32_t currentTargetStringLen = strlen(currentTargetString);
      const u_int32_t longestLen =
          currentStringLen > currentTargetStringLen ? currentStringLen : currentTargetStringLen;

      if (strncmp(currentArrayString, currentTargetString, longestLen) == 0) {
        return true;
      }
    }
  }
  return false;
}

bool array_includes_string(const char *array[],
                           int arrayLen,
                           const char *targetString) {
  const char *targetStrings[1] = {targetString};
  return array_includes_any_target_strings(array, arrayLen, targetStrings, 1);
}

bool array_includes_all_target_strings(const char *array[],
                                       int arrayLen,
                                       const char *targetStrings[],
                                       int targetStringsLen) {

  // init matchedIndexes array with zeros - on each match set that index to 1 we've
  // matched all targets strings if the matchedIndexes elements add up to targetStringsLen
  int matchedIndexes[targetStringsLen];
  memset(matchedIndexes, 0, sizeof(int) * targetStringsLen);


  for (int arrayStrIdx = 0; arrayStrIdx < arrayLen; ++arrayStrIdx) {
    const char *arrayStr = array[arrayStrIdx];

    for (int targetStrIdx = 0; targetStrIdx < targetStringsLen; ++targetStrIdx) {
      const char *targetStr = targetStrings[targetStrIdx];

      const u_int32_t currStrLen = strlen(arrayStr);
      const u_int32_t currTargetStrLen = strlen(targetStr);
      const u_int32_t longestLen = currStrLen > currTargetStrLen ? currStrLen : currTargetStrLen;

      if (strncmp(arrayStr, targetStr, longestLen) == 0) {
        matchedIndexes[targetStrIdx] = 1;
      }

      int sum = 0;
      for (int sumIndex = 0; sumIndex < targetStringsLen; sumIndex++) {
        sum = sum + matchedIndexes[sumIndex];
      }

      if (sum == targetStringsLen) {
        return true;
      }
    }
  }
  return false;
}

bool has_unknown_flags(const char *flags[], int numFlags, const char **unknownFlag) {
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
    const char *flag = flags[i];
    if (!array_includes_string(knownFlags, numKnownFlags, flag)) {
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
    if (array_includes_string(y, yLen, xString)) {
      // count each time y includes a current x string
      count++;
    }
  }
  return count;
}

struct args_and_flags {
  const char **args;
  int numArgs;
  const char **flags;
  int numFlags;
  int spacesFlagIndex;
};

bool has_doubled_flag(const char *cliFlags[], int cliFlagsLen, const char **doubledFlag) {
#define flagsFieldSize 2
  struct supported_flag_tuple {
    int count;
    const char *flags[flagsFieldSize];
  };

#define numSupportedFlags 5
  struct supported_flag_tuple supported_flags[numSupportedFlags] = {
      {.flags = {"-s", "--spaces"}, .count = 0},
      {.flags = {"-t", "--tabs"}, .count = 0},
      {.flags = {"-w", "--write"}, .count = 0},
      {.flags = {"", "--lf"}, .count = 0},
      {.flags = {"", "--crlf"}, .count = 0},
  };

  for (int cliFlagIdx = 0; cliFlagIdx < cliFlagsLen; ++cliFlagIdx) {
    const char *currentCliFlag = cliFlags[cliFlagIdx];

    for (int suppFlagIdx = 0; suppFlagIdx < numSupportedFlags; ++suppFlagIdx) {

      struct supported_flag_tuple *currentSuppFlag = &(supported_flags[suppFlagIdx]);

      if (array_includes_string(currentSuppFlag->flags, flagsFieldSize, currentCliFlag)) {
        currentSuppFlag->count += 1;
      }

      if (currentSuppFlag->count > 1) {
        *doubledFlag = currentCliFlag;
        return true;
      }
    }
  }
  return false;
}

bool has_spaces_and_tabs_flag(const char *cliFlags[], int cliFlagsLen) {
  enum {
    flagsSize = 2
  };
  const char *spacesFlags[flagsSize] = {"-s", "--spaces"};
  const char *tabsFlags[flagsSize] = {"-t", "--tabs"};

  bool hasSpacesFlag = array_includes_any_target_strings(cliFlags,
                                                         cliFlagsLen,
                                                         spacesFlags,
                                                         flagsSize);

  bool hasTabsFlag = array_includes_any_target_strings(cliFlags,
                                                       cliFlagsLen,
                                                       tabsFlags,
                                                       flagsSize);
  if (hasSpacesFlag && hasTabsFlag) {
    return true;
  }

  return false;
}

bool has_lf_and_crlf_flag(const char *cliFlags[], int cliFlagsLen) {
  enum {
    flagsSize = 1
  };
  const char *lfFlags[flagsSize] = {"--lf"};
  const char *crlfFlags[flagsSize] = {"--crlf"};


  bool hasLfFlag = array_includes_any_target_strings(cliFlags,
                                                     cliFlagsLen,
                                                     lfFlags,
                                                     flagsSize);

  bool hasCrlfFlag = array_includes_any_target_strings(cliFlags,
                                                       cliFlagsLen,
                                                       crlfFlags,
                                                       flagsSize);

  if (hasLfFlag && hasCrlfFlag) {
    return true;
  }

  return false;
}

jsonfmt_error_t get_spaces_flag_value(int argc,
                                      const char *argv[],
                                      int spacesFlagIndex,
                                      long int *outNumSpaces) {

  int spacesValueIndex = spacesFlagIndex + 1;

  if (spacesValueIndex >= argc) {
    printf("no value provided for spaces flag\n");
    return JSONFMT_ERR_NO_VALUE_PROVIDED;
  }

  const char *spacesValueStr = argv[spacesValueIndex];
  char *endPtr;
  long int value;

  errno = 0;
  value = strtol(spacesValueStr, &endPtr, 10);

  if (errno == ERANGE && (value == LONG_MAX || value == LONG_MIN)) {
    printf("parsed value falls out of range\n");
    // error - parsed value falls out of range
    return JSONFMT_ERR_INCORRECT_ARG_TYPE;
  }

  if (errno != 0 && value == 0) {
    // an error happened & the value could not be parsed
    printf("parsed value has incorrect type\n");
    return JSONFMT_ERR_INCORRECT_ARG_TYPE;
  }

  if (*endPtr != '\0') {
    // error - number has string on end of it eg "345fd"
    printf("parsed value has incorrect type - string attached to number value\n");
    return JSONFMT_ERR_INCORRECT_ARG_TYPE;
  }

  if (endPtr == spacesValueStr) {
    // error - no digits found in the string "spacesValueStr"
    printf("parsed value has incorrect type - no digits found\n");
    return JSONFMT_ERR_INCORRECT_ARG_TYPE;
  }

  *outNumSpaces = value;
  return JSONFMT_OK;
};

void free_args_and_flags(struct args_and_flags *argsAndFlags) {
  saferFree(argsAndFlags->args);
  saferFree(argsAndFlags->flags);
  saferFree(argsAndFlags);
}

void new_args_and_flags(int argc,
                        const char *argv[],
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

    const char *currentArg = argv[i];
    const u_int32_t argLen = strlen(currentArg);

    if (argLen == 0) continue;

    // must begin with '-' & be at least 2 chars long
    bool isFlag = strncmp(currentArg, "-", 1) == 0 && argLen >= 2;

    if (isFlag) {
      argsAndFlags->flags[argsAndFlags->numFlags] = currentArg;
      argsAndFlags->numFlags += 1;

      // check if it's a --spaces / -s flag
      const char *spacesFlags[] = {"-s", "--spaces"};
      if (array_includes_string(spacesFlags, 2, currentArg)) {
        argsAndFlags->spacesFlagIndex = i;
        // check if next cli arg after spaces is a value - if so skip over
        // the next cli arg in the loop since it's the value for the --spaces flag
        int nextCliArgIdx = i + 1;
        if (nextCliArgIdx < argc && strncmp(argv[nextCliArgIdx], "-", 1) != 0) {
          // skip what would be the next cli arg in the loop
          i += 1;
        }
      }
    } else {
      argsAndFlags->args[argsAndFlags->numArgs] = currentArg;
      argsAndFlags->numArgs += 1;
    }
  }
  *outputArgsAndFlags = argsAndFlags;
}

jsonfmt_error_t new_jsonfmt_config(int argc,
                                   const char *argv[],
                                   struct jsonfmt_config **output_config) {

  struct jsonfmt_config *config = (struct jsonfmt_config *) malloc(sizeof(struct jsonfmt_config));
  memset(config, 0, sizeof(struct jsonfmt_config));

  // init config
  config->useSpaces = false;
  config->numSpaces = 0;
  config->useTabs = false;
  config->writeToFile = false;
  config->useLF = false;
  config->useCRLF = false;
  config->useStdIn = false;
  config->paths = NULL;
  config->jsonFilePaths = NULL;

  if (argc <= 1) {
    // no args provided set defaults
    config->useSpaces = true;
    config->numSpaces = 2;
    config->useTabs = false;
    config->writeToFile = false;
    config->useLF = true;
    config->useCRLF = false;
    config->useStdIn = true;
    config->paths = NULL;
    config->jsonFilePaths = NULL;

    *output_config = config;
    return JSONFMT_OK;
  }

  struct args_and_flags *argsAndFlags = NULL;

  new_args_and_flags(argc, argv, &argsAndFlags);

  const char *unknownFlag = NULL;

  if (has_unknown_flags(argsAndFlags->flags, argsAndFlags->numFlags, &unknownFlag)) {
    printf("found unknown flag: %s \n", unknownFlag);
    return JSONFMT_ERR_UNRECOGNISED_OPTION;
  }

  const char *doubledFlag = NULL;

  if (has_doubled_flag(argsAndFlags->flags, argsAndFlags->numFlags, &doubledFlag)) {
    printf("found repeated flag: %s \n", doubledFlag);
    return JSONFMT_ERR_REPEATED_OPTION;
  }

  const char *spacesFlags[] = {"-s", "--spaces"};
  config->useSpaces = array_includes_any_target_strings(argsAndFlags->flags,
                                                        argsAndFlags->numFlags,
                                                        spacesFlags,
                                                        2);

  const char *tabsFlags[] = {"-t", "--tabs"};
  config->useTabs = array_includes_any_target_strings(argsAndFlags->flags,
                                                      argsAndFlags->numFlags,
                                                      tabsFlags,
                                                      2);

  if (config->useSpaces && config->useTabs) {
    printf("err - has spaces & tabs flag \n");
    return JSONFMT_ERR_CANT_SET_TABS_AND_SPACES_FLAG;
  }

  const char *lfFlags[] = {"--lf"};
  config->useLF = array_includes_any_target_strings(argsAndFlags->flags,
                                                    argsAndFlags->numFlags,
                                                    lfFlags,
                                                    1);

  const char *crlfFlags[] = {"--crlf"};
  config->useCRLF = array_includes_any_target_strings(argsAndFlags->flags,
                                                      argsAndFlags->numFlags,
                                                      crlfFlags,
                                                      1);

  if (config->useLF && config->useCRLF) {
    printf("err - has lf & crlf flag \n");
    return JSONFMT_ERR_CANT_SET_LF_AND_CRLF_FLAG;
  }

  if (config->useSpaces) {
    jsonfmt_error_t err = get_spaces_flag_value(argc,
                                                argv,
                                                argsAndFlags->spacesFlagIndex,
                                                &config->numSpaces);
    if (err != JSONFMT_OK) {
      return err;
    }

    if (config->numSpaces > 10) {
      printf("spaces value too high: %ld \n", config->numSpaces);
      return JSONFMT_ERR_VALUE_TOO_HIGH;
    }

    printf("spaces set to %ld: \n", config->numSpaces);
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


jsonfmt_error_t free_jsonfmt_config(struct jsonfmt_config *config) {
  saferFree(config);
  return JSONFMT_OK;
}
