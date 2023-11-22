#include "config.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

void saferFree(void *ptr);

bool array_includes_any_target_strings(const char *array[],
                                       int arrayLen,
                                       const char *targetStrings[],
                                       int targetStringsLen);

bool array_includes_string(const char *array[],
                           int arrayLen,
                           const char *targetString);

bool array_includes_all_target_strings(const char *array[],
                                       int arrayLen,
                                       const char *targetStrings[],
                                       int targetStringsLen);

bool has_unknown_flags(const char *flags[],
                       int numFlags,
                       const char **unknownFlag);

bool count_occurences_of_y_in_x(const char *y[],
                                int yLen,
                                const char *x[],
                                int xLen);

bool has_doubled_flag(const char *cliFlags[],
                      int cliFlagsLen,
                      const char **doubledFlag);

jsonfmt_error_t get_spaces_flag_value(int argc,
                                      const char *argv[],
                                      struct jsonfmt_config *config);

void set_flags_and_paths(int argc,
                         const char *argv[],
                         struct jsonfmt_config *config);

void init_config(struct jsonfmt_config *config);


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

bool has_unknown_flags(const char *flags[],
                       int numFlags,
                       const char **unknownFlag) {
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

bool count_occurences_of_y_in_x(const char *y[],
                                int yLen,
                                const char *x[],
                                int xLen) {
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


bool has_doubled_flag(const char *cliFlags[],
                      int cliFlagsLen,
                      const char **doubledFlag) {
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
                                      struct jsonfmt_config *config) {


  int spacesValueIndex = config->spacesFlagIndex + 1;

  if (spacesValueIndex >= argc) {
//    printf("no value provided for spaces flag\n");
    return JSONFMT_ERR_NO_VALUE_PROVIDED;
  }

  const char *spacesValueStr = argv[spacesValueIndex];
  char *endPtr;
  long int value;

  errno = 0;
  value = strtol(spacesValueStr, &endPtr, 10);

  if (errno == ERANGE && (value == LONG_MAX || value == LONG_MIN)) {
//    printf("parsed value falls out of range\n");
    // error - parsed value falls out of range
    return JSONFMT_ERR_INCORRECT_ARG_TYPE;
  }

  if (errno != 0 && value == 0) {
    // an error happened & the value could not be parsed
//    printf("parsed value has incorrect type\n");
    return JSONFMT_ERR_INCORRECT_ARG_TYPE;
  }

  if (*endPtr != '\0') {
    // error - number has string on end of it eg "345fd"
//    printf("parsed value has incorrect type - string attached to number value\n");
    return JSONFMT_ERR_INCORRECT_ARG_TYPE;
  }

  if (endPtr == spacesValueStr) {
    // error - no digits found in the string "spacesValueStr"
//    printf("parsed value has incorrect type - no digits found\n");
    return JSONFMT_ERR_INCORRECT_ARG_TYPE;
  }

  config->numSpaces = value;
  return JSONFMT_OK;
};

void set_flags_and_paths(int argc,
                         const char *argv[],
                         struct jsonfmt_config *config) {
  // init memory for args & flags char pointer arrays - base size on argc
  config->paths = malloc(argc * sizeof(char *));
  config->flags = malloc(argc * sizeof(char *));

  for (int i = 0; i < argc; i++) {
    // skip first CLI arg (program name)
    if (i == 0) continue;

    const char *currentCliArg = argv[i];
    const u_int32_t argLen = strlen(currentCliArg);

    if (argLen == 0) continue;

    // must begin with '-' & be at least 2 chars long
    bool isFlag = strncmp(currentCliArg, "-", 1) == 0 && argLen >= 2;

    if (isFlag) {
      config->flags[config->numFlags] = currentCliArg;
      config->numFlags += 1;

      // check if it's a --spaces / -s flag
      const char *spacesFlags[] = {"-s", "--spaces"};
      if (array_includes_string(spacesFlags, 2, currentCliArg)) {
        config->spacesFlagIndex = i;
        // check if next cli arg after spaces is a value - if so skip over
        // the next cli arg in the loop since it's the value for the --spaces flag
        int nextCliArgIdx = i + 1;
        if (nextCliArgIdx < argc && strncmp(argv[nextCliArgIdx], "-", 1) != 0) {
          // skip what would be the next cli arg in the loop
          i += 1;
        }
      }
    } else {
      config->paths[config->numPaths] = currentCliArg;
      config->numPaths += 1;
    }
  }
}

void init_config(struct jsonfmt_config *config) {
  config->useSpaces = false;
  config->numSpaces = 0;
  config->spacesFlagIndex = -1;
  config->useTabs = false;
  config->writeToFile = false;
  config->useLF = false;
  config->useCRLF = false;
  config->useStdIn = false;
  config->flags = NULL;
  config->numFlags = 0;
  config->paths = NULL;
  config->numPaths = 0;
  config->jsonFiles = NULL;
  config->numJsonFiles = 0;
}

jsonfmt_error_t new_jsonfmt_config(int argc,
                                   const char *argv[],
                                   struct jsonfmt_config **outConfig) {


  *outConfig = (struct jsonfmt_config *) malloc(sizeof(struct jsonfmt_config));
  memset(*outConfig, 0, sizeof(struct jsonfmt_config));

  struct jsonfmt_config *config = *outConfig;

  init_config(config);

  if (argc <= 1) {
    // no args provided set defaults
    config->useSpaces = true;
    config->numSpaces = 2;
    config->useLF = true;
    config->useStdIn = true;
    return JSONFMT_OK;
  }

  set_flags_and_paths(argc, argv, config);

  const char *unknownFlag = NULL;

  if (has_unknown_flags(config->flags, config->numFlags, &unknownFlag)) {
//    printf("found unknown flag: %s \n", unknownFlag);
    return JSONFMT_ERR_UNRECOGNISED_OPTION;
  }

  const char *doubledFlag = NULL;

  if (has_doubled_flag(config->flags, config->numFlags, &doubledFlag)) {
//    printf("found repeated flag: %s \n", doubledFlag);
    return JSONFMT_ERR_REPEATED_OPTION;
  }

  const char *spacesFlags[] = {"-s", "--spaces"};
  config->useSpaces = array_includes_any_target_strings(config->flags,
                                                        config->numFlags,
                                                        spacesFlags,
                                                        2);

  const char *tabsFlags[] = {"-t", "--tabs"};
  config->useTabs = array_includes_any_target_strings(config->flags,
                                                      config->numFlags,
                                                      tabsFlags,
                                                      2);

  if (config->useSpaces && config->useTabs) {
//    printf("err - has spaces & tabs flag \n");
    return JSONFMT_ERR_CANT_SET_TABS_AND_SPACES_FLAG;
  }

  const char *lfFlags[] = {"--lf"};
  config->useLF = array_includes_any_target_strings(config->flags,
                                                    config->numFlags,
                                                    lfFlags,
                                                    1);

  const char *crlfFlags[] = {"--crlf"};
  config->useCRLF = array_includes_any_target_strings(config->flags,
                                                      config->numFlags,
                                                      crlfFlags,
                                                      1);

  if (config->useLF && config->useCRLF) {
    return JSONFMT_ERR_CANT_SET_LF_AND_CRLF_FLAG;
  }

  if (config->useSpaces) {
    jsonfmt_error_t err = get_spaces_flag_value(argc,
                                                argv,
                                                config);
    if (err != JSONFMT_OK) {
      return err;
    }

    if (config->numSpaces > 10) {
      return JSONFMT_ERR_VALUE_TOO_HIGH;
    }
  }

  const char *writeFlags[] = {"-w", "--write"};
  config->writeToFile = array_includes_any_target_strings(config->flags,
                                                          config->numFlags,
                                                          writeFlags,
                                                          2);

  if (config->writeToFile && config->numPaths < 1) {
    return JSONFMT_ERR_CANNOT_WRITE_NO_PATH_PROVIDED;
  }

  if (config->numPaths < 1) {
    config->useStdIn = true;
    return JSONFMT_OK;
  }

  // TODO: check for paths that can't be resolved

  // TODO check if args are paths or files


  // print testing =============================================================
//  printf("flags: \n");
//  for (int i = 0; i < config->numFlags; i++) {
//    printf(" %s \n", config->flags[i]);
//  }
//
//  printf("args: \n");
//  for (int i = 0; i < config->numPaths; i++) {
//    printf(" %s \n", config->paths[i]);
//  }
//
//  printf("spaced index: %d \n", config->spacesFlagIndex);
  //============================================================================
  return JSONFMT_OK;
}


jsonfmt_error_t free_jsonfmt_config(struct jsonfmt_config *config) {
  saferFree(config->paths);
  saferFree(config->flags);
  saferFree(config);
  return JSONFMT_OK;
}
