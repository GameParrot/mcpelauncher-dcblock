#include <sys/stat.h>
#include <string>

int hasConfigChanged(const char *configFilePath);

void printConfig();

void onConfEdited();

long getEpochTime();

std::string getPath();