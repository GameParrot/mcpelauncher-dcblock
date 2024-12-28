#include <stdbool.h>
#include "imgui.h"

class DCBlock {
private:
    long omsLeft = 0;
    long omsRight = 0;
    ImGUIOptions gui;

public:
    bool onMouseClick(int button);
    void init();
};