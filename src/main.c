#include <dlfcn.h>
#include <sys/time.h>
#include <stdlib.h>
#include <android/log.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

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
        exit(EXIT_FAILURE);
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
        // Configuration file not found, create a new one with default values
        fp = fopen(confPath, "w");
        if (fp) {
            fputs("Enabled=true\n", fp);
            fputs("BlockRightDc=false\n", fp);
            fputs("LogClicks=false\n", fp);
            fputs("UseLegacyMousefeed=false\n", fp);
            fputs("Threshold=50\n", fp);
            fclose(fp);
        }
    }

    __android_log_print(ANDROID_LOG_INFO, "DCBlock", "enabled: %s", enabled ? "true" : "false");
    __android_log_print(ANDROID_LOG_INFO, "DCBlock", "blockRightDc: %s", blockRightDc ? "true" : "false");
    __android_log_print(ANDROID_LOG_INFO, "DCBlock", "logClicks: %s", logClicks ? "true" : "false");
    __android_log_print(ANDROID_LOG_INFO, "DCBlock", "useLegacyMousefeed: %s", useLegacyMousefeed ? "true" : "false");
    __android_log_print(ANDROID_LOG_INFO, "DCBlock", "threshold: %d", threshold);

}

char (*Mouse_feed_org)(void*, char, signed char, short, short, short, short, bool);
long oms = 0;
long oms1 = 0;
long lastUpdateTime = 0;

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

void __attribute__ ((visibility ("default"))) mod_preinit() {
    if (!hasInited) {
        loadConfFile();

        void *h = dlopen("libmcpelauncher_mod.so", 0);
        typedef int (*preinithook)(const char *, void *, void **);
        preinithook mcpelauncher_preinithook;
        *(void **)(&mcpelauncher_preinithook) = dlsym(h, "mcpelauncher_preinithook");

        const char *mouseFeedSymbol = useLegacyMousefeed ? "_ZN11MouseDevice4feedEccssssb" : "_ZN11MouseDevice4feedEcassssb";
        mcpelauncher_preinithook(mouseFeedSymbol, (void *)&Mouse_feed, (void **)&Mouse_feed_org);
        mcpelauncher_preinithook(mouseFeedSymbol, (void *)&Mouse_feed, (void **)&Mouse_feed_org);
        hasInited = true;
    }
}

__attribute__ ((visibility ("default"))) void mod_init() {
}
