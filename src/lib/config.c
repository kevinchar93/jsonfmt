#include "config.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

// region module functions ------------------------------------------------------------

void safeFree(void *ptr);

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

bool count_occurrences_of_y_in_x(const char *z[],
                                 int zLen,
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

jsonfmt_error_t create_json_files_array(struct jsonfmt_config *config);
// endregion

// region module functions ------------------------------------------------------------

jsonfmt_error_t new_jsonfmt_config(int argc,
                                   const char *argv[],
                                   struct jsonfmt_config **outConfig) {

  *outConfig = calloc(1, sizeof(struct jsonfmt_config));

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

  if (has_unknown_flags(config->flags, config->flagsLen, &unknownFlag)) {
    return JSONFMT_ERR_UNRECOGNISED_OPTION;
  }

  const char *doubledFlag = NULL;

  if (has_doubled_flag(config->flags, config->flagsLen, &doubledFlag)) {
    return JSONFMT_ERR_REPEATED_OPTION;
  }

  const char *spacesFlags[] = {"-s", "--spaces"};
  config->useSpaces = array_includes_any_target_strings(config->flags,
                                                        config->flagsLen,
                                                        spacesFlags,
                                                        2);

  const char *tabsFlags[] = {"-t", "--tabs"};
  config->useTabs = array_includes_any_target_strings(config->flags,
                                                      config->flagsLen,
                                                      tabsFlags,
                                                      2);

  if (config->useSpaces && config->useTabs) {
    jsonfmt_error_t err = JSONFMT_ERR_CANNOT_USE_FLAGS_AT_SAME_TIME;
//    const char *formatStr = get_jsonfmt_error_string(err);
//
//    const char *valueStr = "--spaces(-s) & --tabs(-t) ";
//    config->errorStringLen = strlen(formatStr) + strlen(valueStr);
//    config->errorString = calloc(config->errorStringLen, sizeof(char));
//
//    snprintf(config->errorString, config->errorStringLen, formatStr, pathWithError);
    return err;
  }

  const char *lfFlags[] = {"--lf"};
  config->useLF = array_includes_any_target_strings(config->flags,
                                                    config->flagsLen,
                                                    lfFlags,
                                                    1);

  const char *crlfFlags[] = {"--crlf"};
  config->useCRLF = array_includes_any_target_strings(config->flags,
                                                      config->flagsLen,
                                                      crlfFlags,
                                                      1);
  if (config->useLF && config->useCRLF) {
    return JSONFMT_ERR_CANNOT_USE_FLAGS_AT_SAME_TIME;
  }

  if (config->useSpaces) {
    jsonfmt_error_t err = get_spaces_flag_value(argc, argv, config);

    if (err != JSONFMT_OK) {
      return err;
    }

    if (config->numSpaces > 10) {
      return JSONFMT_ERR_VALUE_OUT_OF_RANGE;
    }
  }

  const char *writeFlags[] = {"-w", "--write"};
  config->writeToFile = array_includes_any_target_strings(config->flags,
                                                          config->flagsLen,
                                                          writeFlags,
                                                          2);
  if (config->writeToFile && config->pathsLen < 1) {
    return JSONFMT_ERR_CANNOT_WRITE_NO_PATH_PROVIDED;
  }

  if (config->pathsLen < 1) {
    config->useStdIn = true;
    return JSONFMT_OK;
  }

  return JSONFMT_OK;

//  return create_json_files_array(config);
}


jsonfmt_error_t free_jsonfmt_config(struct jsonfmt_config *config) {
  safeFree(config->errorString);
  safeFree(config->paths);
  safeFree(config->flags);
  safeFree(config);
  return JSONFMT_OK;
}
// endregion

// region internal functions ---------------------------------------------------

void safeFree(void *ptr) {
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

bool count_occurrences_of_x_in_y(const char *x[],
                                 int xLen,
                                 const char *y[],
                                 int yLen) {
  int count = 0;
  // go over all strings in y
  for (int i = 0; i < yLen; ++i) {
    const char *xString = y[i];
    if (array_includes_string(x, xLen, xString)) {
      // count each time x includes a current y string
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

jsonfmt_error_t get_spaces_flag_value(int argc,
                                      const char *argv[],
                                      struct jsonfmt_config *config) {


  int spacesValueIndex = config->spacesFlagIndex + 1;

  if (spacesValueIndex >= argc) {
    return JSONFMT_ERR_NO_VALUE_PROVIDED;
  }

  const char *spacesValueStr = argv[spacesValueIndex];
  char *endPtr;
  long int value;

  errno = 0;
  value = strtol(spacesValueStr, &endPtr, 10);

  if (errno == ERANGE && (value == LONG_MAX || value == LONG_MIN)) {
    // error - parsed value falls out of range
    return JSONFMT_ERR_INCORRECT_ARG_TYPE;
  }

  if (errno != 0 && value == 0) {
    // an error happened & the value could not be parsed
    return JSONFMT_ERR_INCORRECT_ARG_TYPE;
  }

  if (*endPtr != '\0') {
    // error - number has string on end of it eg "345fd"
    return JSONFMT_ERR_INCORRECT_ARG_TYPE;
  }

  if (endPtr == spacesValueStr) {
    // error - no digits found in the string "spacesValueStr"
    return JSONFMT_ERR_INCORRECT_ARG_TYPE;
  }

  config->numSpaces = value;
  return JSONFMT_OK;
};

void set_flags_and_paths(int argc,
                         const char *argv[],
                         struct jsonfmt_config *config) {
  // init memory for args & flags char pointer arrays - base size on argc
  config->paths = calloc(argc, sizeof(char*));
  config->flags = calloc(argc, sizeof(char*));

  //  start i @ 1 to skip 1st CLI arg (program name)
  for (int i = 1; i < argc; i++) {
    const char *currentCliArg = argv[i];
    const u_int32_t argLen = strlen(currentCliArg);

    if (argLen == 0) continue;

    // must begin with '-' & be at least 2 chars long
    bool isFlag = strncmp(currentCliArg, "-", 1) == 0 && argLen >= 2;

    if (isFlag) {
      config->flags[config->flagsLen] = currentCliArg;
      config->flagsLen += 1;
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
      config->paths[config->pathsLen] = currentCliArg;
      config->pathsLen += 1;
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
  config->flagsLen = 0;
  config->paths = NULL;
  config->pathsLen = 0;
  config->jsonFiles = NULL;
  config->jsonFilesLen = 0;
  config->errorString = NULL;
}
// endregion

