#include "array.h"
#include <string.h>

bool array_includes_string(const char *array[],
                           int arrayLen,
                           const char *targetString) {
  if (arrayLen == 0 ||
      array == NULL ||
      targetString == NULL) {
    return false;
  }

  const char *targetStrings[1] = {targetString};
  return array_includes_any_target_strings(array, arrayLen, targetStrings, 1);
}


bool array_includes_any_target_strings(const char *array[],
                                       int arrayLen,
                                       const char *targetStrings[],
                                       int targetStringsLen) {
  if (arrayLen == 0 ||
      targetStringsLen == 0 ||
      array == NULL ||
      targetStrings == NULL) {
    return false;
  }

  for (int i = 0; i < arrayLen; ++i) {
    const char *currentArrayString = array[i];

    for (int j = 0; j < targetStringsLen; ++j) {
      const char *currentTargetString = targetStrings[j];

      const u_int32_t currentStringLen = strlen(currentArrayString);
      const u_int32_t currentTargetStringLen = strlen(currentTargetString);
      const u_int32_t longestLen =
          currentStringLen > currentTargetStringLen ? currentStringLen : currentTargetStringLen;

      if (strncmp(currentArrayString, currentTargetString, longestLen) == 0) {
        return true;
      }
    }
  }
  return false;
}


bool array_includes_all_target_strings(const char *array[],
                                       int arrayLen,
                                       const char *targetStrings[],
                                       int targetStringsLen) {
  if (arrayLen == 0 ||
      targetStringsLen == 0 ||
      array == NULL ||
      targetStrings == NULL) {
    return false;
  }

  // init matchedIndexes array with zeros - on each match set that index to 1 we've
  // matched all targets strings if the matchedIndexes elements add up to targetStringsLen
  int matchedIndexes[targetStringsLen];
  memset(matchedIndexes, 0, sizeof(int) * targetStringsLen);


  for (int arrayStrIdx = 0; arrayStrIdx < arrayLen; ++arrayStrIdx) {
    const char *arrayStr = array[arrayStrIdx];

    for (int targetStrIdx = 0; targetStrIdx < targetStringsLen; ++targetStrIdx) {
      const char *targetStr = targetStrings[targetStrIdx];

      const u_int32_t currStrLen = strlen(arrayStr);
      const u_int32_t currTargetStrLen = strlen(targetStr);
      const u_int32_t longestLen = currStrLen > currTargetStrLen ? currStrLen : currTargetStrLen;

      if (strncmp(arrayStr, targetStr, longestLen) == 0) {
        matchedIndexes[targetStrIdx] = 1;
      }

      int sum = 0;
      for (int sumIndex = 0; sumIndex < targetStringsLen; sumIndex++) {
        sum = sum + matchedIndexes[sumIndex];
      }

      if (sum == targetStringsLen) {
        return true;
      }
    }
  }
  return false;
}


uint32_t count_occurrences_of_x_in_y(const char *x[],
                                     int xLen,
                                     const char *y[],
                                     int yLen) {
  int count = 0;
  // go over all strings in y
  for (int i = 0; i < yLen; ++i) {
    const char *xString = y[i];
    if (array_includes_string(x, xLen, xString)) {
      // count each time x includes a current y string
      count++;
    }
  }
  return count;
}