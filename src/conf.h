#pragma once

struct Conf {
    static bool enabled;
    static bool blockRightDc;
    static bool logClicks;
    static bool useLegacyMousefeed;
    static int threshold;

    static void load();
    static void save();
};