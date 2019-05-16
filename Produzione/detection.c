// Snippet Start
#include <stdio.h>
#include <string.h>
#include "src/pattern/51Degrees.h"
#include "detection.h"

// Global settings and properties.
static fiftyoneDegreesProvider provider;

// Function declarations.
static void reportDatasetInitStatus(
	fiftyoneDegreesDataSetInitStatus status,
	const char* fileName);

const char* getProperty(fiftyoneDegreesWorkset* ws, const char *property);

int startDetectionProvider(int numWorksets, int cacheItems){
	const char* properties = "IsMobile,ScreenPixelsHeight,ScreenPixelsWidth";
	const char* fileName = "51Degrees-LiteV3.2.dat";

	// Create a pool of 4 worksets with a cache for 1000 items.
	fiftyoneDegreesDataSetInitStatus status =
		fiftyoneDegreesInitProviderWithPropertyString(
		fileName, &provider, properties, numWorksets, cacheItems);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		reportDatasetInitStatus(status, fileName);
		fgetc(stdin);
		return 1;
	}
}


/* int main(int argc, char* argv[]) {

	startDetectionProvider(4, 1000);

	runDetection("Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 Safari/9537.53");

	// Free the pool, dataset and cache.
	fiftyoneDegreesProviderFree(&provider);
	// Wait for a character to be pressed.
	return 0;
} */

void runDetection(char *userAgent, int *width, int *height) {
	const char* isMobile, *ScreenPixelsHeight;
	fiftyoneDegreesWorkset *ws = NULL;

	ws = fiftyoneDegreesProviderWorksetGet(&provider);

	userAgent = strtok(userAgent, ":"); 
	userAgent = strtok(NULL, "\n\t");

	fiftyoneDegreesMatch(ws, userAgent);

	*width = atoi(getProperty(ws, "ScreenPixelsWidth"));
	*height = atoi(getProperty(ws, "ScreenPixelsHeight"));

	fiftyoneDegreesWorksetRelease(ws);
}

const char* getProperty(fiftyoneDegreesWorkset* ws, const char *property) {
    int32_t requiredPropertyIndex;
    const char* isMobile;
    const fiftyoneDegreesAsciiString* valueName;

    requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(ws->dataSet, property);
    fiftyoneDegreesSetValues(ws, requiredPropertyIndex);
    valueName = fiftyoneDegreesGetString(ws->dataSet, ws->values[0]->nameOffset);
    isMobile = &(valueName->firstByte);
    return isMobile;
}

static void reportDatasetInitStatus(fiftyoneDegreesDataSetInitStatus status,
	const char* fileName) {
	switch (status) {
	case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
		printf("Insufficient memory to load '%s'.", fileName);
		break;
	case DATA_SET_INIT_STATUS_CORRUPT_DATA:
		printf("Device data file '%s' is corrupted.", fileName);
		break;
	case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
		printf("Device data file '%s' is not correct version.", fileName);
		break;
	case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
		printf("Device data file '%s' not found.", fileName);
		break;
	case DATA_SET_INIT_STATUS_NULL_POINTER:
		printf("Null pointer to the existing dataset or memory location.");
		break;
	case DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS:
		printf("Allocated continuous memory containing 51Degrees data file "
			"appears to be smaller than expected. Most likely because the"
			" data file was not fully loaded into the allocated memory.");
		break;
	default:
		printf("Device data file '%s' could not be loaded.", fileName);
		break;
	}
}
// Snippet End
