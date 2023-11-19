#include "error.h"

const char *const jsfmt_error_strings[] =
    {
        "No Error.",
        "can't use --spaces(-s) & --tabs(-t) at the same time.",
        "can't use --lf & --crlf at the same time.",
        "can't use --write(-w) with no set 'path.'",
        "can't read file(s) at given path, permission denied:",
        "can't write file(s) at given path, permission denied:",
        "unrecognised option:",
        "incorrect type for arg:",
        "paths does not exist:",
        "no .json files found in input path(s):",
        "option appears more than once: %",
        "no value provided for option: %s",
        "value provided for arg ('%s') is too high:",
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