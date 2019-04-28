#include "file3.h"  /* Declaration made available here */

int global_variable = 37;    /* Definition checked against declaration */
int increment(void) { return ++global_variable; }