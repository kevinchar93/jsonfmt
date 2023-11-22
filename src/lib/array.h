#ifndef JSONFMT_ARRAY_H
#define JSONFMT_ARRAY_H

#include <stdlib.h>
#include <stdbool.h>

bool array_includes_string(const char *array[],
                           int arrayLen,
                           const char *targetString);

bool array_includes_any_target_strings(const char *array[],
                                       int arrayLen,
                                       const char *targetStrings[],
                                       int targetStringsLen);

bool array_includes_all_target_strings(const char *array[],
                                       int arrayLen,
                                       const char *targetStrings[],
                                       int targetStringsLen);

uint32_t count_occurrences_of_y_in_x(const char *z[],
                                 int zLen,
                                 const char *x[],
                                 int xLen);

#endif //JSONFMT_ARRAY_H
