#include "error.h"
#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

const char *const jsfmt_error_strings[] =
    {
        "no error",
        "can't use flags set at the same time: '%s'",
        "can't use --write(-w) with no set 'path'",
        "unrecognised option: '%s'",
        "incorrect type for arg: '%s'",
        "option appears more than once: '%s'",
        "expected value for option: '%s', none provided",
        "value provided for option ('%s') is out of range: min - %s, max - %s",
        "permission denied reading path '%s'",
        "permission denied writing path '%s'",
        "path does not exist: '%s'",
        "no .json files found in input path(s)",
        "path is resolved to be null: '%s'",
        "i/o error occurred resolving path: '%s'",
        "too many symlinks resolving path: '%s'",
        "path is too long to resolve: '%s'",
        "prefix in path is not a directory: '%s'",
    };


_Static_assert(ARRAY_LEN(jsfmt_error_strings) == JSONFMT_ERR_COUNT,
               "You must keep `jsfmt_err` enum & "
               "`jsfmt_error_strings` array in-sync.");

const char *get_jsonfmt_error_string(jsonfmt_error_t error_code) {

  if (error_code > JSONFMT_OK &&
      error_code < JSONFMT_ERR_COUNT) {
    return jsfmt_error_strings[error_code];
  }

  return NULL;
}

void new_error_format_string(struct jsonfmt_config *config, const char *formatString, ...) {
  va_list args;
  va_start(args, formatString);
  int errStringLen = vsnprintf(NULL, 0, formatString, args) + 1;
  va_end(args);

  char *errorStringBuffer = calloc(errStringLen, sizeof(char));

  va_start(args, formatString);
  vsnprintf(errorStringBuffer, errStringLen, formatString, args);
  va_end(args);

  config->errorString = errorStringBuffer;
}
