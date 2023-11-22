#include <stdio.h>
#include "config.h"

int main(int argc, const char *argv[]) {

  struct jsonfmt_config *config = NULL;

  jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

  if (err != JSONFMT_OK) {
    printf("jsonfmt: %s", config->errorString);
    free_jsonfmt_config(config);
    exit(1);
  }

  free_jsonfmt_config(config);
  return 0;
}

