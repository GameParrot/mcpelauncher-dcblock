#include <stdbool.h>
#include <stdlib.h>
#include <android/input.h>

class DCBlock {
private:
    long omsLeft = 0;
    long omsRight = 0;
    long lastUpdateTime = 0;
    bool leftPrevious = false;
    bool rightPrevious = false;
    bool leftBlocking = false;
    bool rightBlocking = false;

public:
    int32_t getButtonState(const AInputEvent *t);
};