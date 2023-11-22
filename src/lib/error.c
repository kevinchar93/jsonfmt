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
        "expected value for option: '%s'",
        "value provided for option ('%s') is out of range: %s",
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
  struct smart_string {
    const char *value;
    uint32_t length;
  };

  va_list args;
  va_start(args, formatString);

  uint32_t valueStringsCapacity = 1;
  uint32_t valueStringsSize = 0;
  const char **valueStrings = calloc(valueStringsCapacity, sizeof(char *));

  const char *fmtStrStart = formatString;
  uint32_t fmtStrLen = 0;

  uint32_t outputBufferLen = 0;

  while (*formatString != '\0') {
    fmtStrLen++;

    // if we find a %s char in the error format string
    if (*formatString == 's' &&
        formatString != fmtStrStart &&
        *(formatString - 1) == '%') {

      // dynamically grow size of valueStrings array
      if (valueStringsSize == valueStringsCapacity) {
        const char **temp = realloc(valueStrings,
                                    (valueStringsCapacity + 1) * sizeof(struct smart_string *));
        if (temp == NULL) {
          // handle memory alloc fail
        }
        valueStrings = temp;
        valueStringsCapacity++;
      }

      // get next variadic arg, save the string & add its length to the outputBufferLen
      char *currValueString = va_arg(args, char *);
      outputBufferLen += strlen(currValueString);

      valueStrings[valueStringsSize] = currValueString;
      valueStringsSize++;
    }

    // advance to next char
    formatString++;
  }

  outputBufferLen += fmtStrLen;
  char *outputBuffer = calloc(outputBufferLen, sizeof(char));

  // write initial format into the buffer with no substitution
  snprintf(outputBuffer, outputBufferLen, "%s", fmtStrStart);

  for (int i = 0; i < valueStringsSize; ++i) {
    char * format = strdup(outputBuffer);
    memset(outputBuffer, 0, outputBufferLen * sizeof(char));
    snprintf(outputBuffer, outputBufferLen, format, valueStrings[i]);
    free(format);
  }

  config->errorString = outputBuffer;
  free(valueStrings);
}
