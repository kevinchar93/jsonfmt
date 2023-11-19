#include <stdio.h>
#include "config.h"

int main(int argc, const char *argv[]) {
//  printf("Hello, World!\n");
//
//  int i;
//  printf("%d\n", argc);
//
//  for (i = 0; i < argc - 1; i++) {
//    printf(" %s \n", argv[i]);
//  }

  struct jsonfmt_config *config = NULL;

  jsonfmt_error_t err = new_jsonfmt_config(argc, argv, &config);

  free_jsonfmt_config(config);


  return 0;
}

