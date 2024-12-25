#pragma once
#include "imgui.h"

struct Conf {
    static bool enabled;
    static bool blockRightDc;
    static bool logClicks;
    static bool showLogWindow;
    static bool useLegacyMousefeed;
    static int threshold;

    static void load(ImGUIOptions opts);
    static void save();
};