#ifndef JSONFMT_ERROR_H
#define JSONFMT_ERROR_H

#include <stdlib.h>

#define ARRAY_LEN(array) (sizeof(array) / sizeof((array)[0]))

typedef enum {
  JSONFMT_OK = 0,
  JSONFMT_ERR_CANNOT_USE_FLAGS_AT_SAME_TIME,
  JSONFMT_ERR_CANNOT_WRITE_NO_PATH_PROVIDED,
  JSONFMT_ERR_UNRECOGNISED_OPTION,
  JSONFMT_ERR_INCORRECT_ARG_TYPE,
  JSONFMT_ERR_REPEATED_OPTION,
  JSONFMT_ERR_NO_VALUE_PROVIDED,
  JSONFMT_ERR_VALUE_OUT_OF_RANGE,
  JSONFMT_ERR_PATH_PERMS_CANNOT_READ,
  JSONFMT_ERR_PATH_PERMS_CANNOT_WRITE,
  JSONFMT_ERR_PATH_FILE_DOES_NOT_EXIST,
  JSONFMT_ERR_PATH_NO_JSON_FILES_FOUND,
  JSONFMT_ERR_PATH_IS_NULL,
  JSONFMT_ERR_PATH_IO_ERROR_OCCURRED,
  JSONFMT_ERR_PATH_TOO_MANY_SYMLINKS,
  JSONFMT_ERR_PATH_IS_TOO_LONG,
  JSONFMT_ERR_PATH_PREFIX_IS_NOT_A_DIRECTORY,
  // this should always be last
  JSONFMT_ERR_COUNT
} jsonfmt_error_t;


/// @brief      Get a printable string from an enum error type
/// @param[in]  error_code     error code
/// @return     A C string that maps to the given to the error code,
///             or NULL if an invalid error code is given
const char* get_jsonfmt_error_string(jsonfmt_error_t error_code);

#endif