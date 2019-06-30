#ifndef _DETECTION_H 
#define _DETECTION_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "src/pattern/51Degrees.h"

static fiftyoneDegreesProvider provider;

int startDetectionProvider(int numWorksets, int cacheItems);

void runDetection(char *userAgent, int *width, int *height);

#endif