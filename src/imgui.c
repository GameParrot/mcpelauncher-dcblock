#include "main.h"
#include <sys/stat.h>
#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>

struct MenuEntryABI {
    const char* name;
    void * user;
    bool (*selected)(void* user);
    void (*click)(void* user);
    size_t length;
    struct MenuEntryABI* subentries;
};

void toggleEnabled() {
    enabled = !enabled;
    saveConfFile();
}
bool isEnabled() {
    return enabled;
}

void toggleBlockRightDc() {
    blockRightDc = !blockRightDc;
    saveConfFile();
}
bool isBlockRightDc() {
    return blockRightDc;
}

void toggleLogClicks() {
    logClicks = !logClicks;
    saveConfFile();
}
bool isLogClicks() {
    return logClicks;
}

void initImgui() {
        void *libmenu = dlopen("libmcpelauncher_menu.so", 0);
        if (libmenu) {
            typedef void (*addmenu)(size_t length, struct MenuEntryABI* entries);
            addmenu mcpelauncher_addmenu;
            *(void **)(&mcpelauncher_addmenu) = dlsym(libmenu, "mcpelauncher_addmenu");            
            struct MenuEntryABI enabled;
            enabled.name = "Enabled";
            enabled.click = toggleEnabled;
            enabled.selected = isEnabled;
            enabled.length = 0;

            struct MenuEntryABI blockRightDc;
            blockRightDc.name = "Block right DC";
            blockRightDc.click = toggleBlockRightDc;
            blockRightDc.selected = isBlockRightDc;
            blockRightDc.length = 0;

            struct MenuEntryABI logClicks;
            logClicks.name = "Log clicks";
            logClicks.click = toggleLogClicks;
            logClicks.selected = isLogClicks;
            logClicks.length = 0;

            struct MenuEntryABI entry;
            struct MenuEntryABI entries[] = {enabled, blockRightDc, logClicks};
            entry.subentries = entries;
            entry.length = 3;
            entry.name = "DCBlock";


            mcpelauncher_addmenu(1, &entry);
        }
}