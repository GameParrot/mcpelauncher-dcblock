#include <dlfcn.h>
#include "util.h"
#include <sys/time.h>
#include <stdlib.h>
#include <android/log.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include "dcblock.h"
#include "conf.h"

void DCBlock::init() {
    gui.initImgui();
    Conf::load(gui);
}

int32_t DCBlock::getButtonState(const AInputEvent* t) {
    long currentEpochTime = getEpochTime();

    int32_t i = AMotionEvent_getButtonState(t);
    if(Conf::enabled) {
        if(i & 1) {
            if(!leftPrevious) {
                if(Conf::showLogWindow) {
                    gui.addClick(1, currentEpochTime - omsLeft <= Conf::threshold);
                }
                if(currentEpochTime - omsLeft > Conf::threshold) {
                    if(Conf::logClicks) {
                        __android_log_print(0, "DCBlock", "[■ ] Mouse down");
                    }
                    omsLeft = currentEpochTime;
                } else {
                    leftBlocking = true;
                    if(Conf::logClicks) {
                        __android_log_print(0, "DCBlock", "[■ ] Suppressed a DC");
                    }
                }
            }
            leftPrevious = true;
        } else {
            if(leftPrevious) {
                leftBlocking = false;
            }
            leftPrevious = false;
        }
        if(i & 2) {
            if(!rightPrevious && Conf::blockRightDc) {
                if(Conf::showLogWindow) {
                    gui.addClick(2, currentEpochTime - omsRight <= Conf::threshold);
                }
                if(currentEpochTime - omsRight > Conf::threshold) {
                    if(Conf::logClicks) {
                        __android_log_print(0, "DCBlock", "[ ■] Mouse down");
                    }
                    omsRight = currentEpochTime;
                } else {
                    rightBlocking = true;
                    if(Conf::logClicks) {
                        __android_log_print(0, "DCBlock", "[ ■] Suppressed a DC");
                    }
                }
            }
            rightPrevious = true;
        } else {
            if(rightPrevious) {
                rightBlocking = false;
            }
            rightPrevious = false;
        }
    }
    if(leftBlocking) {
        i = i & ~1;
    }
    if(rightBlocking) {
        i = i & ~2;
    }
    return i;
}
