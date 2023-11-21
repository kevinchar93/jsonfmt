/**
  MIT License

  Copyright (c) 2022 Qualified

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  ------------------------------------------------------------------------------

  Original source:
    - https://github.com/codewars/criterion-hooks
    - https://github.com/codewars/criterion-hooks/blob/359591e0e8bf60d422c03d76a1d359699bf55554/criterion-hooks.c

  Original authors:
    - Kaz Yoshihara aka "kazk", https://github.com/kazk
    - "hobovsky", https://github.com/hobovsky

  Modifications made by:
    - Kevin Charles, kevinchar93, https://github.com/kevinchar93
*/


#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <criterion/criterion.h>
#include <criterion/hooks.h>


// PRE_ALL: occurs before running the tests.
ReportHook(PRE_ALL)(CR_UNUSED struct criterion_test_set *tests) {
  // Make stdout and stderr unbuffered (taken from the official samples).
  // https://github.com/Snaipe/Criterion/commit/444e39322632a2166eca1cccf1d85e6e97aacb5f
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);
}

// PRE_SUITE: occurs before a suite is initialized.
ReportHook(PRE_SUITE)(struct criterion_suite_set *set) {
  printf("\nSuite: %s\n", set->suite.name);
}

// PRE_INIT: occurs before a test is initialized.
ReportHook(PRE_INIT)(CR_UNUSED struct criterion_test *test) {
}

// PRE_TEST: occurs after the test initialization, but before the test is run.
ReportHook(PRE_TEST)(struct criterion_test *test) {
  printf("\n%s", test->data->description != NULL ? test->data->description : test->name);
}

static void print_failure_message(const char *message, const char *default_message) {
  printf("\n> Fail ");
  if (default_message && !(message && *message)) {
    printf("%s", default_message);
    return;
  }

  char *dup = strdup(message);
  char *saveptr = NULL;
  char *line = strtok_r(dup, "\n", &saveptr);
  do {
    printf("%s", line);
    line = strtok_r(NULL, "\n", &saveptr);
    if (line) printf("<:LF:>");
  } while (line);
  free(dup);
  puts("");
}

// ASSERT: occurs when an assertion is hit
ReportHook(ASSERT)(struct criterion_assert_stats *stats) {
  if (stats->passed) {
    return;
  }
  print_failure_message(stats->message, "Assertion failed");
}

// THEORY_FAIL: occurs when a theory iteration fails.
ReportHook(THEORY_FAIL)(CR_UNUSED struct criterion_theory_stats *stats) {
}

// TEST_CRASH: occurs when a test crashes unexpectedly.
ReportHook(TEST_CRASH)(struct criterion_test_stats *stats) {
  printf("\n> Error: Test Crashed, Caught unexpected signal: ");
  switch (stats->signal) {
    case SIGILL:
      printf("SIGILL (%d). %s\n", stats->signal, "Invalid instruction.");
      break;

    case SIGFPE:
      printf("SIGFPE (%d). %s\n", stats->signal, "Erroneous arithmetic operation.");
      break;

    case SIGSEGV:
      printf("SIGSEGV (%d). %s\n", stats->signal, "Invalid memory access.");
      break;

    default:
      printf("%d\n", stats->signal);
  }
}

static void process_failed_test(struct criterion_test_stats *test) {
  // If there is a failed assertion, it most probably got already printed by assertion hook.
  // Test has already been reported as failed.
  for (struct criterion_assert_stats *assert = test->asserts; assert; assert = assert->next) {
    if (!assert->passed) {
      return;
    }
  }
  print_failure_message(test->message, "Test failed");
}

// POST_TEST: occurs after a test ends, but before the test finalization.
ReportHook(POST_TEST)(struct criterion_test_stats *stats) {
  if (stats->timed_out) {
    puts("\n> Error");
  } else if (stats->test_status == CR_STATUS_PASSED) {
    puts("\n> Pass");
  } else {
    process_failed_test(stats);
  }
  printf("> Took: %.4f ms\n", stats->elapsed_time*1000);
}

// POST_FINI: occurs after a test finalization.
ReportHook(POST_FINI)(CR_UNUSED struct criterion_test_stats *stats) {
}

// POST_SUITE: occurs before a suite is finalized.
ReportHook(POST_SUITE)(struct criterion_suite_stats *stats) {
}

// POST_ALL: occurs after all the tests are done.
ReportHook(POST_ALL)(CR_UNUSED struct criterion_global_stats *stats) {
}