#include "error.h"

const char *const jsfmt_error_strings[] =
    {
        "no error",
        "can't use --spaces(-s) & --tabs(-t) at the same time",
        "can't use --lf & --crlf at the same time",
        "can't use flags set at the same time: %s",
        "can't use --write(-w) with no set 'path'",
        "unrecognised option: %s",
        "incorrect type for arg: %s",
        "option appears more than once: %",
        "expected value for option: %s",
        "value provided for option ('%s') is out of range: (%s)",
        "permission denied reading path '%s'",
        "permission denied writing path '%s'",
        "path does not exist: %s",
        "no .json files found in input path(s)",
        "path is resolved to be null: %s",
        "i/o error occurred resolving path: %s",
        "too many symlinks resolving path: %s",
        "path is too long to resolve: %s ",
        "prefix in path is not a directory: %s",
    };


_Static_assert(ARRAY_LEN(jsfmt_error_strings) == JSONFMT_ERR_COUNT,
               "You must keep `jsfmt_err` enum & "
               "`jsfmt_error_strings` array in-sync.");


const char* get_jsonfmt_error_string(jsonfmt_error_t error_code)
{
  const char* err_str = NULL;

  if (error_code > JSONFMT_OK && error_code < JSONFMT_ERR_COUNT )
  {
    return NULL;
  }

  err_str = jsfmt_error_strings[error_code];
  return err_str;
}