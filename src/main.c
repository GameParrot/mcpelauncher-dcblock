#include <dlfcn.h>
#include <sys/time.h>
#include <stdlib.h>
#include <android/log.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

bool blockRightDc = false;
bool logClicks = false;
bool hasInited = false; // Workaround mcpelauncher mod loading bug

long getEpochTime() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

char (*Mouse_feed_org)(char, signed char, short, short, short, short);
long oms = 0;
long oms1 = 0;

void Mouse_feed(char a, signed char b, short c, short d, short e, short f) {
    long currentEpochTime = getEpochTime();

    if (a == 1 && b == 1) {
        if (currentEpochTime - oms > 50) {
            if (logClicks) {
                __android_log_print(0, "DCBlock", "[■ ] Mouse down");
            }
            Mouse_feed_org(a, b, c, d, e, f);
            oms = currentEpochTime;
        } else if (logClicks) {
            __android_log_print(0, "DCBlock", "[■ ] Suppressed a DC");
        }
    } else if (a == 2 && b == 1 && blockRightDc) {
        if (currentEpochTime - oms1 > 50) {
            if (logClicks) {
                __android_log_print(0, "DCBlock", "[ ■] Mouse down");
            }
            Mouse_feed_org(a, b, c, d, e, f);
            oms1 = currentEpochTime;
        } else if (logClicks) {
            __android_log_print(0, "DCBlock", "[ ■] Suppressed a DC");
        }
    } else {
        Mouse_feed_org(a, b, c, d, e, f);
    }
}

void __attribute__ ((visibility ("default"))) mod_preinit() {
    bool useLegacyMousefeed = false;

    if (!hasInited) {
        FILE *fp = fopen("/data/data/com.mojang.minecraftpe/dcblock.conf", "r");
        if (fp) {
            char* line = NULL;
            size_t len;
            ssize_t read;
            while ((read = getline(&line, &len, fp)) != -1) {
                char *token = strtok(line, "=");
                if (token) {
                    char *value = strtok(NULL, "\n");
                    if (value) {
                        if (strcmp(token, "BlockRightDc") == 0) {
                            blockRightDc = strcmp(value, "true") == 0;
                        } else if (strcmp(token, "LogClicks") == 0) {
                            logClicks = strcmp(value, "true") == 0;
                        } else if (strcmp(token, "UseLegacyMousefeed") == 0) {
                            useLegacyMousefeed = strcmp(value, "true") == 0;
                        }
                    }
                }
            }
            fclose(fp);
        } else {
            // Configuration file not found, create a new one with default values
            fp = fopen("/data/data/com.mojang.minecraftpe/dcblock.conf", "w");
            if (fp) {
                fputs("BlockRightDc=false\n", fp);
                fputs("LogClicks=false\n", fp);
                fputs("UseLegacyMousefeed=false\n", fp);
                fclose(fp);
            }
        }

        __android_log_print(ANDROID_LOG_INFO, "DCBlock", "blockRightDc: %s", blockRightDc ? "true" : "false");
        __android_log_print(ANDROID_LOG_INFO, "DCBlock", "logClicks: %s", logClicks ? "true" : "false");
        __android_log_print(ANDROID_LOG_INFO, "DCBlock", "useLegacyMousefeed: %s", useLegacyMousefeed ? "true" : "false");

        void *h = dlopen("libmcpelauncher_mod.so", 0);
        typedef int (*preinithook)(const char *, void *, void **);
        preinithook mcpelauncher_preinithook;
        *(void **)(&mcpelauncher_preinithook) = dlsym(h, "mcpelauncher_preinithook");

        const char *mouseFeedSymbol = useLegacyMousefeed ? "_ZN5Mouse4feedEccssss" : "_ZN5Mouse4feedEcassss";
        mcpelauncher_preinithook(mouseFeedSymbol, (void *)&Mouse_feed, (void **)&Mouse_feed_org);

        hasInited = true;
    }
}

__attribute__ ((visibility ("default"))) void mod_init() {
}
