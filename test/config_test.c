#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include "config.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


Test(new_jsonfmt_config, set_jsonfmt_config_defaults_when_no_flags_given,
     .description = "sets correct jsonfmt_config defaults when no input given on CLI") {

  int argc = 1;
  const char *argv[] = {
      ""
  };

  struct jsonfmt_config *config = NULL;

  jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

  cr_expect_eq(err, JSONFMT_OK);

  cr_expect_eq(config->useSpaces, true);
  cr_expect_eq(config->numSpaces, 2);
  cr_expect_eq(config->useTabs, false);
  cr_expect_eq(config->writeToFile, false);
  cr_expect_eq(config->useLF, true);
  cr_expect_eq(config->useCRLF, false);
  cr_expect_eq(config->useStdIn, true);
  cr_expect_eq(config->paths, NULL);
  cr_expect_eq(config->jsonFilePaths, NULL);

  free_jsonfmt_config(config);
}

Test(new_jsonfmt_config, set_useSpaces_and_numSpaces_fields,
     .disabled = false,
     .description = "sets useSpaces & numSpaces fields in jsonfmt_config when '-s <num>' or '--spaces <num>' flag is set") {
  enum {
    numTests = 2,
    argc = 3
  };

  const char *argvs[numTests][argc] = {
      {".../jsonfmt", "--spaces", "5"},
      {".../jsonfmt", "-s",       "5"},
  };

  for (int i = 0; i < numTests; ++i) {
    const char **argv = argvs[i];

    struct jsonfmt_config *config = NULL;

    jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

    cr_expect_eq(err, JSONFMT_OK);
    cr_expect_eq(config->useSpaces, true);
    cr_expect_eq(config->numSpaces, 5);

    free_jsonfmt_config(config);
  }
}

Test(new_jsonfmt_config, set_useTabs_field,
     .description = "sets useTabs field in jsonfmt_config when '-t' or '--tabs' flag is set") {
  enum {
    numTests = 2,
    argc = 2
  };

  const char *argvs[numTests][argc] = {
      {".../jsonfmt", "--tabs",},
      {".../jsonfmt", "-t",},
  };

  for (int i = 0; i < numTests; ++i) {
    const char **argv = argvs[i];

    struct jsonfmt_config *config = NULL;

    jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

    cr_expect_eq(err, JSONFMT_OK);
    cr_expect_eq(config->useTabs, true);

    free_jsonfmt_config(config);
  }
}

Test(new_jsonfmt_config, set_writeToFile_field,
     .description = "sets write field in jsonfmt_config when '-w' or '--write' flag is set") {
  enum {
    numTests = 2,
    argc = 2
  };

  const char *argvs[numTests][argc] = {
      {".../jsonfmt", "--write",},
      {".../jsonfmt", "-w",},
  };

  for (int i = 0; i < numTests; ++i) {
    const char **argv = argvs[i];

    struct jsonfmt_config *config = NULL;

    jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

    cr_expect_eq(err, JSONFMT_OK);
    cr_expect_eq(config->writeToFile, true);

    free_jsonfmt_config(config);
  }
}

Test(new_jsonfmt_config, set_useLF_field,
     .description = "sets useLF field in jsonfmt_config when '--lf' flag is set") {
  int argc = 2;
  const char *argv[] = {
      ".../jsonfmt",
      "--lf",
  };
  struct jsonfmt_config *config = NULL;

  jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

  cr_expect_eq(err, JSONFMT_OK);
  cr_expect_eq(config->useLF, true);

  free_jsonfmt_config(config);
}

Test(new_jsonfmt_config, set_useCRLF_field,
     .description = "sets useCRLF field in jsonfmt_config when '--crlf' flag is set") {
  int argc = 2;
  const char *argv[] = {
      ".../jsonfmt",
      "--crlf",
  };
  struct jsonfmt_config *config = NULL;

  jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

  cr_expect_eq(err, JSONFMT_OK);
  cr_expect_eq(config->useCRLF, true);

  free_jsonfmt_config(config);
}

Test(new_jsonfmt_config, set_useStdIn_field,
     .description = "sets useStdIn field in jsonfmt_config when NO path provided on CLI") {
  int argc = 3;
  const char *argv[] = {
      ".../jsonfmt",
      "--spaces",
      "8",
      "--lf"
  };
  struct jsonfmt_config *config = NULL;

  jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

  cr_expect_eq(err, JSONFMT_OK);
  cr_expect_eq(config->useStdIn, true);
  free_jsonfmt_config(config);
}
//
//Test(new_jsonfmt_config, set_paths_field_single,
//     .description = "sets paths & jsonFilePaths fields in jsonfmt_config when a SINGLE path is provided") {
//  int argc = 2;
//  char *argv[] = {
//      ".../jsonfmt",
//      "./test-dir",
//  };
//  struct jsonfmt_config *config = NULL;
//
//  jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);
//
//  cr_expect_eq(err, JSONFMT_OK);
//
//  cr_expect_eq((*config->paths)[0], "./test-dir");
//  cr_expect_eq((*config->jsonFilePaths)[0], "./test-dir/test-json-b.json");
//  cr_expect_eq((*config->jsonFilePaths)[0], "./test-dir/test-json-c.json");
//  free_jsonfmt_config(config);
//}
//
//Test(new_jsonfmt_config, set_paths_field_multiple,
//     .description = "sets paths & jsonFilePaths fields in jsonfmt_config when a MULTIPLE paths are provided") {
//  int argc = 3;
//  char *argv[] = {
//      ".../jsonfmt",
//      "./test-dir",
//      "./test-json-a.json",
//  };
//  struct jsonfmt_config *config = NULL;
//
//  jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);
//
//  cr_expect_eq(err, JSONFMT_OK);
//
//  cr_expect_eq((*config->paths)[0], "./test-dir");
//  cr_expect_eq((*config->jsonFilePaths)[0], "./test-dir/test-json-b.json");
//  cr_expect_eq((*config->jsonFilePaths)[1], "./test-dir/test-json-c.json");
//  cr_expect_eq((*config->jsonFilePaths)[2], "./test-json-a.json");
//
//  free_jsonfmt_config(config);
//}

Test(new_jsonfmt_config, fail_when_useSpaces_and_useTabs_is_set,
     .disabled = false,
     .description = "returns error 'JSONFMT_ERR_CANT_SET_TABS_AND_SPACES_FLAG' when -s/--spaces & -t/--tabs flags are set at the same time") {

  enum {
    numTests = 4,
    argc = 3
  };

  const char *argvs[numTests][argc] = {
      {".../jsonfmt", "--spaces", "--tabs"},
      {".../jsonfmt", "-s",       "-t"},
      {".../jsonfmt", "--tabs",   "-s"},
      {".../jsonfmt", "-t",       "--spaces"},
  };

  for (int i = 0; i < numTests; ++i) {
    const char **argv = argvs[i];

    struct jsonfmt_config *config = NULL;

    jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

    cr_expect_eq(err, JSONFMT_ERR_CANT_SET_TABS_AND_SPACES_FLAG);

    free_jsonfmt_config(config);
  }
}

Test(new_jsonfmt_config, fail_when_useLF_and_useCRLF_is_set,
     .disabled = false,
     .description = "returns error 'JSONFMT_ERR_CANT_SET_LF_AND_CRLF_FLAG' when --lf & --crlf flags are set at the same time") {

  enum {
    numTests = 2,
    argc = 3
  };

  const char *argvs[numTests][argc] = {
      {".../jsonfmt", "--lf",   "--crlf"},
      {".../jsonfmt", "--crlf", "--lf"},
  };

  for (int i = 0; i < numTests; ++i) {
    const char **argv = argvs[i];

    struct jsonfmt_config *config = NULL;

    jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

    cr_expect_eq(err, JSONFMT_ERR_CANT_SET_LF_AND_CRLF_FLAG);

    free_jsonfmt_config(config);
  }
}

//Test(new_jsonfmt_config, fail_when_writeToFile_set_with_no_path,
//     .description = "returns error 'JSONFMT_ERR_CANNOT_WRITE_NO_PATH_PROVIDED' when --write & flag is set & no path argument is set") {
//  int argc = 3;
//  char *argv[] = {
//      ".../jsonfmt",
//      "--write",
//  };
//  struct jsonfmt_config *config = NULL;
//
//  jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);
//
//  cr_expect_eq(err, JSONFMT_ERR_CANNOT_WRITE_NO_PATH_PROVIDED);
//
//  free_jsonfmt_config(config);
//};
//
Test(new_jsonfmt_config, fail_when_unrecognised_argument_given,
     .disabled = false,
     .description = "returns error 'JSONFMT_ERR_UNRECOGNISED_OPTION' when given unrecognised flag argument") {
  int argc = 3;
  const char *argv[] = {
      ".../jsonfmt",
      "--foo",
  };
  struct jsonfmt_config *config = NULL;

  jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

  cr_expect_eq(err, JSONFMT_ERR_UNRECOGNISED_OPTION);

  free_jsonfmt_config(config);
}

Test(new_jsonfmt_config, fail_when_options_are_repeated,
     .disabled = false,
     .description = "returns error 'JSONFMT_ERR_REPEATED_OPTION' when none variadic flags are repeated more than once") {

  enum {
    numTests = 5,
    argc = 3
  };

  const char *argvs[numTests][argc] = {
      {".../jsonfmt", "--spaces", "-s"},
      {".../jsonfmt", "--tabs",   "-t"},
      {".../jsonfmt", "--write",  "-w"},
      {".../jsonfmt", "--lf",     "--lf"},
      {".../jsonfmt", "--crlf",   "--crlf"},
  };

  for (int i = 0; i < numTests; ++i) {
    const char **argv = argvs[i];

    struct jsonfmt_config *config = NULL;

    jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

    cr_expect_eq(err, JSONFMT_ERR_REPEATED_OPTION);

    free_jsonfmt_config(config);
  }
}


Test(new_jsonfmt_config, fail_when_invalid_argument_type_given_spaces,
     .disabled = false,
     .description = "returns error 'JSONFMT_ERR_INCORRECT_ARG_TYPE' when given invalid arg type for --spaces") {
  enum {
    numTests = 5,
    argc = 3
  };

  const char *argvs[numTests][argc] = {
      {".../jsonfmt", "--spaces", "foo"},
      {".../jsonfmt", "--spaces", "--write"},
      {".../jsonfmt", "--spaces", "-w"},
      {".../jsonfmt", "--spaces", "ty56575"},
      {".../jsonfmt", "--spaces", "45pgl"},
  };

  for (int i = 0; i < numTests; ++i) {
    const char **argv = argvs[i];

    struct jsonfmt_config *config = NULL;

    jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

    cr_expect_eq(err, JSONFMT_ERR_INCORRECT_ARG_TYPE);

    free_jsonfmt_config(config);
  }
}

Test(new_jsonfmt_config, fail_when_no_value_given_for_spaces_flag,
     .disabled = false,
     .description = "returns error 'JSONFMT_ERR_NO_VALUE_PROVIDED' when no value is given for --spaces") {
  enum {
    numTests = 2,
  };

  int argcs[] = {
      3, 2
  };

  const char *argvs[numTests][3] = {
      {".../jsonfmt", "--write", "--spaces"},
      {".../jsonfmt", "--spaces"},
  };

  for (int i = 0; i < numTests; ++i) {
    int argc = argcs[i];
    const char **argv = argvs[i];

    struct jsonfmt_config *config = NULL;

    jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

    cr_expect_eq(err, JSONFMT_ERR_NO_VALUE_PROVIDED);

    free_jsonfmt_config(config);
  }
}

Test(new_jsonfmt_config, fail_when_value_given_for_spaces_flag_over_10,
     .disabled = false,
     .description = "returns error 'JSONFMT_ERR_VALUE_TOO_HIGH' when no value given for --spaces is over 10") {
  enum {
    numTests = 1,
    argc = 3
  };

  const char *argvs[numTests][argc] = {
      {".../jsonfmt", "--spaces", "11"},
  };

  for (int i = 0; i < numTests; ++i) {
    const char **argv = argvs[i];

    struct jsonfmt_config *config = NULL;

    jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

    cr_expect_eq(err, JSONFMT_ERR_VALUE_TOO_HIGH);

    free_jsonfmt_config(config);
  }
}

//Test(new_jsonfmt_config, fail_when_path_doesnt_exist,
//     .disabled = true,
//     .description = "returns error 'JSONFMT_ERR_PATH_DOES_NOT_EXIST' when given a non existent path") {
//  cr_assert(0);
//}
//
//Test(new_jsonfmt_config, fail_when_path_no_json_files_found,
//     .disabled = true,
//     .description = "returns error 'JSONFMT_ERR_PATH_NO_JSON_FILES_FOUND' when no .json files found in input path") {
//  cr_assert(0);
//}
//
//Test(new_jsonfmt_config, fail_when_writeToFile_with_no_write_permission,
//     .disabled = true,
//     .description = "returns error 'JSONFMT_ERR_PERMS_CANNOT_WRITE_PATH' when --write & flag is set one of the input files does not have WRITE permission"
//) {
//  cr_assert(0);
//}
//
//Test(new_jsonfmt_config, fail_when_paths_with_no_read_permission,
//     .disabled = true,
//     .description = "returns error 'JSONFMT_ERR_PERMS_CANNOT_READ_PATH' when one of the input files does not have READ permission") {
//  cr_assert(0);
//}
