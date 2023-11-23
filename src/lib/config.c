#include "config.h"
#include "array.h"

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

// region declarations ------------------------------------------------------------

void safeFree(void *ptr);


bool has_unknown_flags(const char *flags[],
                       int numFlags,
                       const char **unknownFlag);


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


void new_error_format_string(struct jsonfmt_config *config, const char *formatString, ...);


// endregion

// region module functions ------------------------------------------------------------

jsonfmt_error_t new_jsonfmt_config(int argcOrigin,
                                   const char *argvOrigin[],
                                   struct jsonfmt_config **outConfig) {

  // slice argv to remove program name - we don't use it
  int argc = argcOrigin - 1;
  const char **argv = argcOrigin > 1 ? argvOrigin + 1 : NULL;

  *outConfig = calloc(1, sizeof(struct jsonfmt_config));

  struct jsonfmt_config *config = *outConfig;

  init_config(config);

  set_flags_and_paths(argc, argv, config);

  // use stdin for input if no path given by the user
  if (config->pathsLen == 0) {
    config->useStdIn = true;
  }

  const char *unknownFlag = NULL;

  if (has_unknown_flags(config->flags, config->flagsLen, &unknownFlag)) {
    jsonfmt_error_t err = JSONFMT_ERR_UNRECOGNISED_OPTION;

    new_error_format_string(config,
                            get_jsonfmt_error_string(err),
                            unknownFlag);
    return err;
  }

  const char *doubledFlag = NULL;

  if (has_doubled_flag(config->flags, config->flagsLen, &doubledFlag)) {
    jsonfmt_error_t err = JSONFMT_ERR_REPEATED_OPTION;

    new_error_format_string(config,
                            get_jsonfmt_error_string(err),
                            doubledFlag);
    return err;
  }

  const char *spacesFlags[] = {"-s", "--spaces"};
  config->useSpaces = array_includes_any_target_strings(config->flags,
                                                        config->flagsLen,
                                                        spacesFlags,
                                                        2);
  // we got a user provided spaces option - validate it
  if (config->useSpaces) {
    jsonfmt_error_t err = get_spaces_flag_value(argc, argv, config);

    if (err != JSONFMT_OK) {
      return err;
    }
    if (config->numSpaces > 10 || config->numSpaces < 1) {
      err = JSONFMT_ERR_VALUE_OUT_OF_RANGE;

      new_error_format_string(config,
                              get_jsonfmt_error_string(err),
                              "--spaces/-s",
                              "1",
                              "10");
      return err;
    }
  }

  const char *tabsFlags[] = {"-t", "--tabs"};
  config->useTabs = array_includes_any_target_strings(config->flags,
                                                      config->flagsLen,
                                                      tabsFlags,
                                                      2);
  // check user hasn't set tabs & spaces options
  if (config->useSpaces && config->useTabs) {
    jsonfmt_error_t err = JSONFMT_ERR_CANNOT_USE_FLAGS_AT_SAME_TIME;

    new_error_format_string(config,
                            get_jsonfmt_error_string(err),
                            "--spaces/-s & --tabs/-t");
    return err;
  }

  // add default spaces settings if neither spaces/tabs is set by the user
  if (!config->useSpaces && !config->useTabs) {
    config->useSpaces = true;
    config->numSpaces = 2;
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
  // check user hasn't set lf & crlf options
  if (config->useLF && config->useCRLF) {
    jsonfmt_error_t err = JSONFMT_ERR_CANNOT_USE_FLAGS_AT_SAME_TIME;

    new_error_format_string(config,
                            get_jsonfmt_error_string(err),
                            "--lf & --crlf");
    return err;
  }

  // add default line settings if neither lf/crlf is set by the user
  if (!config->useLF && !config->useCRLF) {
    config->useLF = true;
  }

  const char *writeFlags[] = {"-w", "--write"};
  config->writeToFile = array_includes_any_target_strings(config->flags,
                                                          config->flagsLen,
                                                          writeFlags,
                                                          2);
  // check user hasn't set write flag with no paths to write output to
  if (config->writeToFile && config->pathsLen < 1) {
    jsonfmt_error_t err = JSONFMT_ERR_CANNOT_WRITE_NO_PATH_PROVIDED;

    new_error_format_string(config,
                            get_jsonfmt_error_string(err),
                            NULL);
    return err;
  }

  return JSONFMT_OK;
  //TODO:  return create_json_files_array(config);
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
  ptr = NULL;
}




//jsonfmt_error_t create_json_files_array(struct jsonfmt_config *config) {
//
//  const char *realPaths[config->pathsLen];
//  const char *pathWithError = NULL;
//  jsonfmt_error_t err = JSONFMT_OK;
//
//  // build up an array of resolved paths
//  for (int i = 0; i < config->pathsLen; ++i) {
//    realPaths[i] = NULL;
//    const char *realPath = realpath(config->paths[i], NULL);
//
//    // path resolved OK - store it
//    if (realPath != NULL) {
//      realPaths[i] = realPath;
//      continue;
//    }
//
//    //  we hit an error resolving the path - convert error & exit loop
//    pathWithError = config->paths[i];
//    switch (errno) {
//      case EACCES:
//        err = JSONFMT_ERR_PATH_PERMS_CANNOT_READ;
//        break;
//      case EINVAL:
//        err = JSONFMT_ERR_PATH_IS_NULL;
//        break;
//      case EIO:
//        err = JSONFMT_ERR_PATH_IO_ERROR_OCCURRED;
//        break;
//      case ELOOP:
//        err = JSONFMT_ERR_PATH_TOO_MANY_SYMLINKS;
//        break;
//      case ENAMETOOLONG:
//        err = JSONFMT_ERR_PATH_IS_TOO_LONG;
//        break;
//      case ENOENT:
//        err = JSONFMT_ERR_PATH_FILE_DOES_NOT_EXIST;
//        break;
//      case ENOTDIR:
//        err = JSONFMT_ERR_PATH_PREFIX_IS_NOT_A_DIRECTORY;
//        break;
//    }
//    break;
//  }
//
//  // free memory for resolved paths if there was an error
//  if (err != JSONFMT_OK) {
//    for (int i = 0; i < config->pathsLen; ++i) {
//      safeFree((void *) realPaths[i]);
//    }
//
//    // generate an error string & return the error
//    const char *formatStr = get_jsonfmt_error_string(err);
//    config->errorStringLen = strlen(formatStr) + strlen(pathWithError);
//    config->errorString = calloc(config->errorStringLen, sizeof(char));
//
//    snprintf(config->errorString, config->errorStringLen, formatStr, pathWithError);
//    return err;
//  }
//}


bool has_unknown_flags(const char *flags[],
                       int numFlags,
                       const char **unknownFlag) {
  if (numFlags == 0 || flags == NULL) return false;

  enum {
    numKnownFlags = 9
  };
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


bool has_doubled_flag(const char *cliFlags[],
                      int cliFlagsLen,
                      const char **doubledFlag) {
  if (cliFlagsLen == 0 || cliFlags == NULL) return false;

  enum {
    flagsFieldSize = 2
  };
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
  if (argc == 0 || argv == NULL) return JSONFMT_OK;

  int spacesValueIndex = config->spacesFlagIndex + 1;

  if (spacesValueIndex >= argc) {
    jsonfmt_error_t err = JSONFMT_ERR_NO_VALUE_PROVIDED;

    new_error_format_string(config,
                            get_jsonfmt_error_string(err),
                            "--spaces/-s");
    return err;
  }

  const char *spacesValueStr = argv[spacesValueIndex];
  char *endPtr;
  long int value;

  errno = 0;
  value = strtol(spacesValueStr, &endPtr, 10);

  if ((errno == ERANGE && (value == LONG_MAX || value == LONG_MIN)) || // parsed value falls out of range
      (errno != 0 && value == 0) ||       // the value could not be parsed
      (*endPtr != '\0') ||                // number has string on end of it eg "345fd"
      (endPtr == spacesValueStr)          // no digits found in the string "spacesValueStr"
      ) {
    jsonfmt_error_t err = JSONFMT_ERR_INCORRECT_ARG_TYPE;

    new_error_format_string(config,
                            get_jsonfmt_error_string(err),
                            "--spaces/-s");
    return err;
  }

  config->numSpaces = value;
  return JSONFMT_OK;
};


void set_flags_and_paths(int argc,
                         const char *argv[],
                         struct jsonfmt_config *config) {
  if (argc == 0 || argv == NULL) return;

  // init memory for args & flags char pointer arrays - base size on argc
  config->paths = calloc(argc, sizeof(char *));
  config->flags = calloc(argc, sizeof(char *));

  for (int i = 0; i < argc; i++) {
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
  config->numSpaces = 0;
  config->spacesFlagIndex = -1;

  config->useSpaces = false;
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
  config->errorStringLen = 0;
}
// endregion

