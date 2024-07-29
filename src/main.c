#include <dlfcn.h>
#include "main.h"
#include "imgui.h"
#include <sys/time.h>
#include <stdlib.h>
#include <android/log.h>
#include <android/input.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>

char* confPath = "/data/data/com.mojang.minecraftpe/dcblock.conf";

bool enabled = true;
bool blockRightDc = false;
bool logClicks = false;
bool hasInited = false; // Workaround mcpelauncher mod loading bug
bool useLegacyMousefeed = false;
int threshold = 50;

time_t lastCheckTime = 0;

long getEpochTime() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

int hasConfigChanged(const char *configFilePath, time_t lastCheckTime) {
    struct stat fileStat;
    if (stat(configFilePath, &fileStat) == -1) {
        perror("stat");
        return 0;
    }
    return fileStat.st_mtime > lastCheckTime;
}

void loadConfFile() {
    FILE *fp = fopen(confPath, "r");
    if (fp) {
        char* line = NULL;
        size_t len;
        ssize_t read;
        while ((read = getline(&line, &len, fp)) != -1) {
            char *token = strtok(line, "=");
            if (token) {
                char *value = strtok(NULL, "\n");
                if (value) {
                    if (strcmp(token, "Enabled") == 0) {
                        enabled = strcmp(value, "true") == 0;
                    } else if (strcmp(token, "LogClicks") == 0) {
                        logClicks = strcmp(value, "true") == 0;
                    } else if (strcmp(token, "UseLegacyMousefeed") == 0) {
                        useLegacyMousefeed = strcmp(value, "true") == 0;
                    } else if (strcmp(token, "BlockRightDc") == 0) {
                        blockRightDc = strcmp(value, "true") == 0;
                    } else if (strcmp(token, "Threshold") == 0) {
                        threshold = atoi(value);
                    }
                }
            }
        }
        fclose(fp);
    } else {
        saveConfFile();
    }

    __android_log_print(ANDROID_LOG_INFO, "DCBlock", "enabled: %s", enabled ? "true" : "false");
    __android_log_print(ANDROID_LOG_INFO, "DCBlock", "blockRightDc: %s", blockRightDc ? "true" : "false");
    __android_log_print(ANDROID_LOG_INFO, "DCBlock", "logClicks: %s", logClicks ? "true" : "false");
    __android_log_print(ANDROID_LOG_INFO, "DCBlock", "useLegacyMousefeed: %s", useLegacyMousefeed ? "true" : "false");
    __android_log_print(ANDROID_LOG_INFO, "DCBlock", "threshold: %d", threshold);

}

void putBool(FILE *fp, bool b, char *name) {
    fprintf(fp, "%s=%s\n", name, (b ? "true" : "false"));
}

void putInt(FILE *fp, int b, char *name) {
    fprintf(fp, "%s=%d\n", name, b);
}

void saveConfFile() {
    FILE *fp = fopen(confPath, "w");
    if (fp) {
        putBool(fp, enabled, "Enabled");
        putBool(fp, blockRightDc, "BlockRightDc");
        putBool(fp, logClicks, "LogClicks");
        putBool(fp, useLegacyMousefeed, "UseLegacyMousefeed");
        putInt(fp, threshold, "Threshold");
        fclose(fp);
    }
    lastCheckTime = time(NULL);
}

char (*Mouse_feed_org)(void*, char, signed char, short, short, short, short, bool);

long oms = 0;
long oms1 = 0;
long lastUpdateTime = 0;
bool leftPrevious = false;
bool rightPrevious = false;
bool leftBlocking = false;
bool rightBlocking = false;

void Mouse_feed(void* t, char a, signed char b, short c, short d, short e, short f, bool g) {
    long currentEpochTime = getEpochTime();
    if (currentEpochTime - lastUpdateTime > 5000) {
        lastUpdateTime = currentEpochTime;
        if (hasConfigChanged(confPath, lastCheckTime)) {
            loadConfFile();

            struct stat fileStat;
            if (stat(confPath, &fileStat) == -1) {
                perror("stat");
                exit(EXIT_FAILURE);
            }
            lastCheckTime = fileStat.st_mtime;
        }
    }

    if (a == 1 && b == 1 && enabled) {
        if (currentEpochTime - oms > threshold) {
            if (logClicks) {
                __android_log_print(0, "DCBlock", "[■ ] Mouse down");
            }
            Mouse_feed_org(t, a, b, c, d, e, f, g);
            oms = currentEpochTime;
        } else if (logClicks) {
            __android_log_print(0, "DCBlock", "[■ ] Suppressed a DC");
        }
    } else if (a == 2 && b == 1 && blockRightDc && enabled) {
        if (currentEpochTime - oms1 > threshold) {
            if (logClicks) {
                __android_log_print(0, "DCBlock", "[ ■] Mouse down");
            }
            Mouse_feed_org(t, a, b, c, d, e, f, g);
            oms1 = currentEpochTime;
        } else if (logClicks) {
            __android_log_print(0, "DCBlock", "[ ■] Suppressed a DC");
        }
    } else {
        Mouse_feed_org(t, a, b, c, d, e, f, g);
    }
}

int32_t getButtonState(const void* t) {
    long currentEpochTime = getEpochTime();
    if (currentEpochTime - lastUpdateTime > 5000) {
        lastUpdateTime = currentEpochTime;
        if (hasConfigChanged(confPath, lastCheckTime)) {
            loadConfFile();

            struct stat fileStat;
            if (stat(confPath, &fileStat) == -1) {
                perror("stat");
                exit(EXIT_FAILURE);
            }
            lastCheckTime = fileStat.st_mtime;
        }
    }

    int32_t i = AMotionEvent_getButtonState(t);
    if(enabled) {
        if (i & 1) {
            if (!leftPrevious) {
                if (currentEpochTime - oms > threshold) {
                    if (logClicks) {
                        __android_log_print(0, "DCBlock", "[■ ] Mouse down");
                    }
                    oms = currentEpochTime;
                } else {
                    leftBlocking = true;
                    if (logClicks) {
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
        if (i & 2) {
            if (!rightPrevious && blockRightDc) {
                if (currentEpochTime - oms1 > threshold) {
                    if (logClicks) {
                        __android_log_print(0, "DCBlock", "[ ■] Mouse down");
                    }
                    oms1 = currentEpochTime;
                } else {
                    rightBlocking = true;
                    if (logClicks) {
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
        i = i &~1;
    }
    if(rightBlocking) {
        i = i &~2;
    }
    return i;
}



void __attribute__ ((visibility ("default"))) mod_preinit() {
    if (!hasInited) {
        loadConfFile();

        void *h = dlopen("libmcpelauncher_mod.so", 0);
        typedef int (*preinithook)(const char *, void *, void **);
        preinithook mcpelauncher_preinithook;
        *(void **)(&mcpelauncher_preinithook) = dlsym(h, "mcpelauncher_preinithook");

        const char *mouseFeedSymbol = useLegacyMousefeed ? "_ZN11MouseDevice4feedEccssssb" : "_ZN11MouseDevice4feedEcassssb";
        mcpelauncher_preinithook(mouseFeedSymbol, (void *)&Mouse_feed, (void **)&Mouse_feed_org);
        mcpelauncher_preinithook("AMotionEvent_getButtonState", getButtonState, NULL);

        initImgui();

        hasInited = true;
    }
}

__attribute__ ((visibility ("default"))) void mod_init() {
}
