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
    if (a == 1 && b == 1) {
        if (getEpochTime() - oms > 50) {
            if (logClicks) {
                __android_log_print(0, "DCBlock", "[■ ] Mouse down");
            }
            Mouse_feed_org(a,b,c,d,e,f);
            oms = getEpochTime();
        } else if (logClicks) {
            __android_log_print(0, "DCBlock", "[■ ] Supressed a DC");
        }
    } else if (a == 2 && b == 1 && blockRightDc) {
        if (getEpochTime() - oms1 > 50) {
            if (logClicks) {
                __android_log_print(0, "DCBlock", "[ ■] Mouse down");
            }
            Mouse_feed_org(a,b,c,d,e,f);
            oms1 = getEpochTime();
        } else if (logClicks) {
            __android_log_print(0, "DCBlock", "[ ■] Supressed a DC");
        }
    } else {
        Mouse_feed_org(a,b,c,d,e,f);
    }
}
void __attribute__ ((visibility ("default"))) mod_preinit() {
    bool useLegacyMousefeed = false;
    if (!hasInited) {
        FILE * fp;
        char * line = NULL;
        size_t len = 0;
        ssize_t read;

        struct stat buffer;  
        if (stat("/data/data/com.mojang.minecraftpe/dcblock.conf", &buffer) != 0) {
            FILE * fp1;
            fp1 = fopen("/data/data/com.mojang.minecraftpe/dcblock.conf", "w"); // W
            fputs("BlockRightDc=false\n", fp1);
            fputs("LogClicks=false\n", fp1);
            fputs("UseLegacyMousefeed=false\n", fp1);
            fclose(fp1);
        }

        fp = fopen("/data/data/com.mojang.minecraftpe/dcblock.conf", "r");
        while ((read = getline(&line, &len, fp)) != -1) {
            char *token = strtok(line, "=");
            if (strcmp(token,"BlockRightDc") == 0) {
                char* token1 = strtok(NULL, "=");
                token1[strcspn(token1, "\n")] = 0;
                if (strcmp(token1,"true") == 0) {
                    blockRightDc = true;
                }
            }
            if (strcmp(token,"LogClicks") == 0) {
                char* token1 = strtok(NULL, "=");
                token1[strcspn(token1, "\n")] = 0;
                if (strcmp(token1,"true") == 0) {
                    logClicks = true;
                }
            }
            if (strcmp(token,"UseLegacyMousefeed") == 0) {
                char* token1 = strtok(NULL, "=");
                token1[strcspn(token1, "\n")] = 0;
                if (strcmp(token1,"true") == 0) {
                    useLegacyMousefeed = true;
                }
            }
        }

        fclose(fp);
        if (line)
            free(line);

        __android_log_print(ANDROID_LOG_INFO, "DCBlock", "blockRightDc: %s", blockRightDc ? "true": "false");
        __android_log_print(ANDROID_LOG_INFO, "DCBlock", "logClicks: %s", logClicks ? "true": "false");
        __android_log_print(ANDROID_LOG_INFO, "DCBlock", "useLegacyMousefeed: %s", useLegacyMousefeed ? "true": "false");
        void* h = dlopen("libmcpelauncher_mod.so", 0);
        typedef int (*preinithook)(const char*sym, void*val, void **orig);
        preinithook mcpelauncher_preinithook;
        *(void**)(&mcpelauncher_preinithook) = dlsym(h, "mcpelauncher_preinithook");
        if (useLegacyMousefeed) {
            mcpelauncher_preinithook("_ZN5Mouse4feedEccssss", (void*)&Mouse_feed, (void**)&Mouse_feed_org);
        } else {
            mcpelauncher_preinithook("_ZN5Mouse4feedEcassss", (void*)&Mouse_feed, (void**)&Mouse_feed_org);
        }
        //dlclose(h);
        hasInited = true;
    }
}
__attribute__ ((visibility ("default"))) void mod_init() {
}
