#include "config.h"
#include <string.h>
#include <stdio.h>

bool includesString(const char *container[], int numItems, const char *compareString) {

  for (int i = 0; i < numItems; ++i) {
    const char *containerString = container[i];
    const u_int32_t containerStringLen = strlen(containerString);

    const u_int32_t compareStringLen = strlen(compareString);

    u_int32_t compareLen = containerStringLen < compareStringLen ? containerStringLen : compareStringLen;

    if (strncmp(containerString, compareString, compareLen) == 0) {
      return true;
    }
  }

  return false;
}

bool hasUnknownFlags(char *flags[], int numFlags, char **unknownFlag) {
  const short numKnownFlags = 9;
  char *knownFlags[numKnownFlags] = {
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
    if (!includesString(knownFlags, numKnownFlags, flag)) {
      *unknownFlag = flag;
      return true;
    }
  }

  return false;
}

bool hasBothFlags(char *flags, char *shortFlag, char *longFlag) {
  return true;
}

void get_flags_and_args(int argc,
                        char *argv[],
                        char *flags[],
                        u_int32_t *numFlags,
                        char *args[],
                        u_int32_t *numArgs,
                        u_int32_t *spacesFlagIndex) {

  for (int i = 0; i < argc; i++) {
    // skip first CLI arg (program name)
    if (i == 0) continue;

    char *currentArg = argv[i];
    const u_int32_t argLen = strlen(currentArg);

    if (argLen == 0) continue;

    // must begin with '-' & be at least 2 chars long
    bool isFlag = strncmp(currentArg, "-", 1) == 0 && argLen >= 2;

    if (isFlag) {
      flags[*numFlags] = currentArg;
      (*numFlags)++;

      if (strncmp(currentArg, "-s", argLen) == 0 ||
          strncmp(currentArg, "--spaces", argLen) == 0) {
        // save index of --spaces arg
        *spacesFlagIndex = i;
      }
    } else {
      args[*numArgs] = currentArg;
      (*numArgs)++;
    }
  }
}

jsonfmt_error_t new_jsonfmt_config(int argc, char *argv[], struct jsonfmt_config **output_config) {

  const size_t jsonfmt_config_size = sizeof(struct jsonfmt_config);

  struct jsonfmt_config *config = (struct jsonfmt_config *) malloc(jsonfmt_config_size);
  memset(config, 0, jsonfmt_config_size);

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


  char *flags[argc];
  memset(flags, 0, argc * sizeof(char *));
  u_int32_t numFlags = 0;

  char *args[argc];
  memset(args, 0, argc * sizeof(char *));
  u_int32_t numArgs = 0;

  u_int32_t spacesFlagIndex = -1;

  get_flags_and_args(argc, argv, flags, &numFlags, args, &numArgs, &spacesFlagIndex);

  char *unknownFlag = NULL;

  if (hasUnknownFlags(flags, numFlags, &unknownFlag)) {
    printf("found unknown flag: %s \n", unknownFlag);
  } else {
    printf("did not find unknown flags \n");
  }

  printf("flags: \n");
  for (int i = 0; i < numFlags; i++) {
    printf(" %s \n", flags[i]);
  }

  printf("args: \n");
  for (int i = 0; i < numArgs; i++) {
    printf(" %s \n", args[i]);
  }

  printf("spaced index: %d \n", spacesFlagIndex);

  // get flags

  // get args

  return 1;
}


jsonfmt_error_t free_jsonfmt_config(struct jsonfmt_config **config) {
  free(*config);
  return JSONFMT_OK;
}
