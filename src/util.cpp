#include <sys/stat.h>
#include <stdio.h>
#include "util.h"
#include "conf.h"
#include <android/log.h>
#include <time.h>
#include <unistd.h>

void printConfig() {
    __android_log_print(ANDROID_LOG_INFO, "DCBlock", "enabled: %s", Conf::enabled ? "true" : "false");
    __android_log_print(ANDROID_LOG_INFO, "DCBlock", "blockRightDc: %s", Conf::blockRightDc ? "true" : "false");
    __android_log_print(ANDROID_LOG_INFO, "DCBlock", "logClicks: %s", Conf::logClicks ? "true" : "false");
    __android_log_print(ANDROID_LOG_INFO, "DCBlock", "threshold: %d", Conf::threshold);
}

long getEpochTime() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

std::string getPath() {
    return "/data/data/com.mojang.minecraftpe/dcblock.conf";
}