#ifndef JSONFMT_ERROR_H
#define JSONFMT_ERROR_H

#include <stdlib.h>

#define ARRAY_LEN(array) (sizeof(array) / sizeof((array)[0]))

typedef enum {
  JSONFMT_OK = 0,
  JSONFMT_ERR_CANT_SET_TABS_AND_SPACES_FLAG,
  JSONFMT_ERR_CANT_SET_LF_AND_CRLF_FLAG,
  JSONFMT_ERR_CANNOT_WRITE_NO_PATH_PROVIDED,
  JSONFMT_ERR_PERMS_CANNOT_READ_PATH,
  JSONFMT_ERR_PERMS_CANNOT_WRITE_PATH,
  JSONFMT_ERR_UNRECOGNISED_OPTION,
  JSONFMT_ERR_INCORRECT_ARG_TYPE,
  JSONFMT_ERR_PATH_DOES_NOT_EXIST,
  JSONFMT_ERR_PATH_NO_JSON_FILES_FOUND,
  JSONFMT_ERR_REPEATED_OPTION,
  JSONFMT_ERR_NO_VALUE_PROVIDED,
  JSONFMT_ERR_VALUE_TOO_HIGH,
  // this should always be last
  JSONFMT_ERR_COUNT
} jsonfmt_error_t;



/// @brief      Get a printable string from an enum error type
/// @param[in]  error_code     error code
/// @return     A C string that maps to the given to the error code,
///             or NULL if an invalid error code is given
const char* get_jsonfmt_error_string(jsonfmt_error_t error_code);

#endif