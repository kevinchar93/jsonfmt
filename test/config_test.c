#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include "config.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define HELLO_WORLD_C "#include <stdio.h>\n\nint main() {\nprintf(\"Hello, World!\\n\");\nreturn 0;\n}"
#define HELLO_WORLD_JS "console.log(\"hello, world\");"
#define HELLO_WORLD_JSON "{\"hello\":\"world\"}"

TestSuite(new_jsonfmt_config);

void setupTestDirAndFiles(void) {
  /**
   File structure
   ==============================
   - test-file-1.c
   - test-file-2.js
   - test-file-3.json
   - dir1
     - test-files-4.js
     - test-files-5.json
     - dir2
        - test-file-6.json
        - dir3
          - test-file-7.json
          - test-file-8.ts
   ==============================
  */

  mkdir("./dir1", 0755);
  mkdir("./dir1/dir2", 0755);
  mkdir("./dir1/dir2/dir3", 0755);

  FILE *fp = fopen("./test-file-1.c", "w+");
  cr_assert_not_null(fp);
  fputs(HELLO_WORLD_C, fp);
  fclose(fp);

  fp = fopen("./test-file-2.js", "w+");
  cr_assert_not_null(fp);
  fputs(HELLO_WORLD_JS, fp);
  fclose(fp);

  fp = fopen("./test-file-3.json", "w+");
  cr_assert_not_null(fp);
  fputs(HELLO_WORLD_JSON, fp);
  fclose(fp);

  fp = fopen("./dir1/test-file-4.js", "w+");
  cr_assert_not_null(fp);
  fputs(HELLO_WORLD_JS, fp);
  fclose(fp);

  fp = fopen("./dir1/test-file-5.json", "w+");
  cr_assert_not_null(fp);
  fputs(HELLO_WORLD_JSON, fp);
  fclose(fp);

  fp = fopen("./dir1/dir2/test-file-6.json", "w+");
  cr_assert_not_null(fp);
  fputs(HELLO_WORLD_JSON, fp);
  fclose(fp);

  fp = fopen("./dir1/dir2/dir3/test-file-7.json", "w+");
  cr_assert_not_null(fp);
  fputs(HELLO_WORLD_JSON, fp);
  fclose(fp);

  fp = fopen("./dir1/dir2/dir3/test-file-8.ts", "w+");
  cr_assert_not_null(fp);
  fputs(HELLO_WORLD_JS, fp);
  fclose(fp);
}

void teardownTestDirAndFiles(void) {
  remove("./test-file-1.c");
  remove("./test-file-2.js");
  remove("./test-file-3.json");
  remove("./dir1/test-file-4.js");
  remove("./dir1/test-file-5.json");
  remove("./dir1/dir2/test-file-6.json");
  remove("./dir1/dir2/dir3/test-file-7.json");
  remove("./dir1/dir2/dir3/test-file-8.ts");

  rmdir("./dir1/dir2/dir3");
  rmdir("./dir1/dir2");
  rmdir("./dir1");
}



// region defaults tests ----------------------------------------------------------

Test(new_jsonfmt_config, set_jsonfmt_config_defaults_when_no_flags_given,
     .disabled = false,
     .description = "new_jsonfmt_config(…) returns jsonfmt_config with correct defaults when no input given on CLI") {

  int argc = 1;
  const char *argv[] = {
      ""
  };

  struct jsonfmt_config *config = NULL;

  jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

  cr_expect_eq(err, JSONFMT_OK);

  cr_expect_eq(config->useSpaces, true);
  cr_expect_eq(config->numSpaces, 2);
  cr_expect_eq(config->spacesFlagIndex, -1);
  cr_expect_eq(config->useTabs, false);
  cr_expect_eq(config->writeToFile, false);
  cr_expect_eq(config->useLF, true);
  cr_expect_eq(config->useCRLF, false);
  cr_expect_eq(config->useStdIn, true);

  cr_expect_eq(config->flags, NULL);
  cr_expect_eq(config->flagsLen, 0);

  cr_expect_eq(config->paths, NULL);
  cr_expect_eq(config->pathsLen, 0);

  cr_expect_eq(config->jsonFiles, NULL);
  cr_expect_eq(config->jsonFilesLen, 0);

  cr_expect_eq(config->errorString, NULL);
  cr_expect_eq(config->errorStringLen, 0);

  free_jsonfmt_config(config);
}

Test(new_jsonfmt_config, set_default_useSpaces_field,
     .disabled = false,
     .description = "new_jsonfmt_config(…) returns jsonfmt_config with useSpaces DEFAULTED to TRUE with numSpaces DEFAULTED to 2 when spaces / tabs flag not set") {
  int argc = 3;
  const char *argv[] = {
      ".../jsonfmt",
      "fakePath"
  };
  struct jsonfmt_config *config = NULL;

  jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

  cr_expect_eq(err, JSONFMT_OK);
  cr_expect_eq(config->useSpaces, true);
  cr_expect_eq(config->numSpaces, 2);
  free_jsonfmt_config(config);
}

Test(new_jsonfmt_config, set_default_useLF_field,
     .disabled = false,
     .description = "new_jsonfmt_config(…) returns jsonfmt_config with useLF DEFAULTED to TRUE when lf / crlf flag not set") {
  int argc = 3;
  const char *argv[] = {
      ".../jsonfmt",
      "fakePath"
  };
  struct jsonfmt_config *config = NULL;

  jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

  cr_expect_eq(err, JSONFMT_OK);
  cr_expect_eq(config->useLF, true);
  free_jsonfmt_config(config);
}

Test(new_jsonfmt_config, set_default_useStdIn_field,
     .disabled = false,
     .description = "new_jsonfmt_config(…) returns jsonfmt_config with useStdIn DEFAULTED to TRUE when NO path provided on CLI") {
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

// endregion


// region basic tests ----------------------------------------------------------

Test(new_jsonfmt_config, fail_when_unrecognised_argument_given,
     .disabled = false,
     .description = "new_jsonfmt_config(…) returns error 'JSONFMT_ERR_UNRECOGNISED_OPTION' when given unrecognised flag argument") {
  int argc = 2;
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
     .description = "new_jsonfmt_config(…) returns error 'JSONFMT_ERR_REPEATED_OPTION' when none variadic flags are repeated more than once") {

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
// endregion


// region spaces & tabs tests --------------------------------------------------
Test(new_jsonfmt_config, set_useSpaces_and_numSpaces_fields,
     .disabled = false,
     .description = "new_jsonfmt_config(…) returns jsonfmt_config with useSpaces set to true & correct value in numSpaces when '-s <num>' or '--spaces <num>' flag is set") {
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
     .disabled = false,
     .description = "new_jsonfmt_config(…) returns jsonfmt_config with useTabs set to TRUE when '-t' or '--tabs' flag is set") {
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

Test(new_jsonfmt_config, fail_when_useSpaces_and_useTabs_is_set,
     .disabled = false,
     .description = "new_jsonfmt_config(…) returns error 'JSONFMT_ERR_CANNOT_USE_FLAGS_AT_SAME_TIME' when -s/--spaces & -t/--tabs flags are set at the same time") {

  enum {
    numTests = 4,
    argc = 4
  };

  const char *argvs[numTests][argc] = {
      {".../jsonfmt", "--spaces", "4",        "--tabs"},
      {".../jsonfmt", "-s",       "4",        "-t"},
      {".../jsonfmt", "--tabs",   "-s",       "4"},
      {".../jsonfmt", "-t",       "--spaces", "4"},
  };

  for (int i = 0; i < numTests; ++i) {
    const char **argv = argvs[i];

    struct jsonfmt_config *config = NULL;

    jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

    cr_expect_eq(err, JSONFMT_ERR_CANNOT_USE_FLAGS_AT_SAME_TIME);

    free_jsonfmt_config(config);
  }
}

Test(new_jsonfmt_config, fail_when_invalid_argument_type_given_spaces,
     .disabled = false,
     .description = "new_jsonfmt_config(…) returns error 'JSONFMT_ERR_INCORRECT_ARG_TYPE' when given invalid arg type for --spaces") {
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
     .description = "new_jsonfmt_config(…) returns error 'JSONFMT_ERR_NO_VALUE_PROVIDED' when no value is given for --spaces") {
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
     .description = "new_jsonfmt_config(…) returns  error 'JSONFMT_ERR_VALUE_OUT_OF_RANGE' when no value given for --spaces is over 10") {
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

    cr_expect_eq(err, JSONFMT_ERR_VALUE_OUT_OF_RANGE);

    free_jsonfmt_config(config);
  }
}
// endregion


// region write tests ----------------------------------------------------------
Test(new_jsonfmt_config, set_writeToFile_field,
     .disabled = false,
     .description = "new_jsonfmt_config(…) returns jsonfmt_config with writeToFile set to TRUE when '-w' or '--write' flag is set") {
  enum {
    numTests = 2,
    argc = 3
  };

  const char *argvs[numTests][argc] = {
      {".../jsonfmt", "--write", "./fake-path"},
      {".../jsonfmt", "-w",      "./fake-path"},
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

Test(new_jsonfmt_config, fail_when_writeToFile_set_with_no_path,
     .disabled = false,
     .description = "new_jsonfmt_config(…) returns  error 'JSONFMT_ERR_CANNOT_WRITE_NO_PATH_PROVIDED' when --write & flag is set & no path argument is set") {
  int argc = 2;
  const char *argv[] = {
      ".../jsonfmt",
      "--write",
  };
  struct jsonfmt_config *config = NULL;

  jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

  cr_expect_eq(err, JSONFMT_ERR_CANNOT_WRITE_NO_PATH_PROVIDED);

  free_jsonfmt_config(config);
};
// endregion

// region lf / crlf tests ------------------------------------------------------
Test(new_jsonfmt_config, set_useLF_field,
     .disabled = false,
     .description = "new_jsonfmt_config(…) returns jsonfmt_config with useLF set to TRUE in jsonfmt_config when '--lf' flag is set") {
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
     .disabled = false,
     .description = "new_jsonfmt_config(…) returns jsonfmt_config with useCRLF set to TRUE when '--crlf' flag is set") {
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

Test(new_jsonfmt_config, fail_when_useLF_and_useCRLF_is_set,
     .disabled = false,
     .description = "new_jsonfmt_config(…) returns error 'JSONFMT_ERR_CANNOT_USE_FLAGS_AT_SAME_TIME' when --lf & --crlf flags are set at the same time") {

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

    cr_expect_eq(err, JSONFMT_ERR_CANNOT_USE_FLAGS_AT_SAME_TIME);

    free_jsonfmt_config(config);
  }
}
// endregion

// region path tests -----------------------------------------------------------

Test(new_jsonfmt_config, set_paths_field_correctly,
     .disabled = false,
     .description = "new_jsonfmt_config(…) returns jsonfmt_config with valid paths & pathsLen fields when paths are provided on CLI",
     .init = setupTestDirAndFiles,
     .fini = teardownTestDirAndFiles) {
  enum {
    numTests = 3,
    argc = 5,
    argvPathIndex = 2
  };

  int argcs[] = {3, 4, 5};
  int expectedNumPathsAll[] = {1, 2, 3};

  const char *argvs[numTests][argc] = {
      {".../jsonfmt", "--write", "./test-file-2.json",},
      {".../jsonfmt", "--write", "./test-file-2.json", "test-file-3.json"},
      {".../jsonfmt", "--write", "./test-file-3.json", "test-file-3.json", "./dir1/test-file-5.json"},
  };

  for (int i = 0; i < numTests; ++i) {
    const int expectedNumPaths = expectedNumPathsAll[i];

    const char **argv = argvs[i];
    const int argc = argcs[i];

    struct jsonfmt_config *config = NULL;

    jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

    cr_expect_eq(err, JSONFMT_OK);
    cr_expect_eq(config->pathsLen, expectedNumPaths);

    for (int j = 0; j < config->pathsLen; ++j) {
      const int argsOffset = 2;
      const char *expectedPath = strdup(argvs[i][argsOffset + j]);

      cr_expect_str_eq(config->paths[j], expectedPath);
      free((void *) expectedPath);
    }

    free_jsonfmt_config(config);
  }
}

//Test(new_jsonfmt_config, set_jsonFiles_field_from_paths_and_directories,
//     .disabled = false,
//     .description = "new_jsonfmt_config(…) returns jsonfmt_config with jsonFiles fields set when paths to files & directories are given",
//     .init = setupTestDirAndFiles,
//     .fini = teardownTestDirAndFiles) {
//  enum {
//    argc = 3,
//  };
//
//  const char argv[argc] = {".../jsonfmt", "--write", "./test-file-3.json"};
//
//  struct jsonfmt_config *config = NULL;
//
//  jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);
//
//  cr_expect_eq(err, JSONFMT_OK);
//  cr_expect_eq(config->pathsLen, 1);
//  cr_expect_eq(config->paths[0], argv[argvPathIndex]);
//  cr_expect_eq(config->paths[1], argv[argvPathIndex + 1]);
//  cr_expect_eq(config->paths[2], argv[argvPathIndex + 2]);
//  free_jsonfmt_config(config);
//}
// endregion


//Test(new_jsonfmt_config, fail_when_path_doesnt_exist,
//     .disabled = false,
//     .description = "new_jsonfmt_config(…) returns  error 'JSONFMT_ERR_PATH_DOES_NOT_EXIST' when given a non existent path") {
//  cr_assert(0);
//}
//
//Test(new_jsonfmt_config, fail_when_path_no_json_files_found,
//     .disabled = false,
//     .description = "new_jsonfmt_config(…) returns  error 'JSONFMT_ERR_PATH_NO_JSON_FILES_FOUND' when no .json files found in input path") {
//  cr_assert(0);
//}
//

//Test(new_jsonfmt_config, fail_when_writeToFile_with_no_write_permission,
//     .disabled = false,
//     .description = "new_jsonfmt_config(…) returns  error 'JSONFMT_ERR_PATH_PERMS_CANNOT_WRITE' when --write & flag is set one of the input files does not have WRITE permission"
//) {
//  cr_assert(0);
//}
//
//Test(new_jsonfmt_config, fail_when_paths_with_no_read_permission,
//     .disabled = false,
//     .description = "new_jsonfmt_config(…) returns  error 'JSONFMT_ERR_PATH_PERMS_CANNOT_READ' when one of the input files does not have READ permission") {
//  cr_assert(0);
//}
