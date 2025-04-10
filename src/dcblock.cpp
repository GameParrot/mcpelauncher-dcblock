#include "util.h"
#include <android/log.h>
#include "dcblock.h"
#include "conf.h"

void DCBlock::init() {
    gui.initImgui();
    Conf::load(gui);
}

bool DCBlock::onMouseClick(int button) {
    if(!Conf::enabled) {
        return false;
    }
    long currentEpochTime = getEpochTime();
    switch(button) {
    case 1:
        gui.addClick(1, currentEpochTime - omsLeft <= Conf::threshold);
        if(currentEpochTime - omsLeft > Conf::threshold) {
            if(Conf::logClicks) {
                __android_log_print(0, "DCBlock", "[■ ] Mouse down");
            }
            omsLeft = currentEpochTime;
            return false;
        } else {
            if(Conf::logClicks) {
                __android_log_print(0, "DCBlock", "[■ ] Suppressed a DC");
            }
            return true;
        }
    case 2:
        if(!Conf::blockRightDc) {
            return false;
        }
        gui.addClick(2, currentEpochTime - omsRight <= Conf::threshold);
        if(currentEpochTime - omsRight > Conf::threshold) {
            if(Conf::logClicks) {
                __android_log_print(0, "DCBlock", "[ ■] Mouse down");
            }
            omsRight = currentEpochTime;
            return false;
        } else {
            if(Conf::logClicks) {
                __android_log_print(0, "DCBlock", "[ ■] Suppressed a DC");
            }
            return true;
        }
    default:
        return false;
    }
}
