#include <stdbool.h>
#include <stdlib.h>
#include "imgui.h"
#include <android/input.h>

class DCBlock {
private:
    long omsLeft = 0;
    long omsRight = 0;
    bool leftPrevious = false;
    bool rightPrevious = false;
    bool leftBlocking = false;
    bool rightBlocking = false;
    ImGUIOptions gui;

public:
    int32_t getButtonState(const AInputEvent *t);
    void init();
};