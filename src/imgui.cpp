#include <sys/stat.h>
#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>
#include "conf.h"
#include "imgui.h"

struct MenuEntryABI {
    const char* name;
    void* user;
    bool (*selected)(void* user);
    void (*click)(void* user);
    size_t length;
    struct MenuEntryABI* subentries;
};

struct control {
    int type;
    union {
        struct {
            const char* label;
            void* user;
            void (*onClick)(void* user);
        } button;
        struct {
            const char* label;
            int min;
            int def;
            int max;
            void* user;
            void (*onChange)(void* user, int value);
        } sliderint;
        struct {
            const char* label;
            float min;
            float def;
            float max;
            void* user;
            void (*onChange)(void* user, float value);
        } sliderfloat;
        struct {
            char* label;
            int size;  // 0 normal/ 1 small titel...
        } text;
        struct {
            const char* label;
            const char* def;
            const char* placeholder;
            void* user;
            void (*onChange)(void* user, const char* value);
        } textinput;
    } data;
};

typedef void (*showwindow)(const char* title, int isModal, void* user, void (*onClose)(void* user), int count, struct control* controls);
showwindow mcpelauncher_show_window;

void ImGUIOptions::initImgui() {
    void* libmenu = dlopen("libmcpelauncher_menu.so", 0);
    if(libmenu) {
        typedef void (*addmenu)(size_t length, struct MenuEntryABI* entries);
        addmenu mcpelauncher_addmenu;
        *(void**)(&mcpelauncher_addmenu) = dlsym(libmenu, "mcpelauncher_addmenu");
        struct MenuEntryABI enabled;
        enabled.name = "Enabled";
        enabled.click = [](void* user) { Conf::enabled = !Conf::enabled; Conf::save(); };
        enabled.selected = [](void* user) -> bool { return Conf::enabled; };
        enabled.length = 0;

        struct MenuEntryABI blockRightDc;
        blockRightDc.name = "Block right DC";
        blockRightDc.click = [](void* user) { Conf::blockRightDc = !Conf::blockRightDc; Conf::save(); };
        blockRightDc.selected = [](void* user) -> bool { return Conf::blockRightDc; };
        blockRightDc.length = 0;

        struct MenuEntryABI logClicks;
        logClicks.name = "Log clicks";
        logClicks.click = [](void* user) { Conf::logClicks = !Conf::logClicks; Conf::save(); };
        logClicks.selected = [](void* user) -> bool { return Conf::logClicks; };
        logClicks.length = 0;

        struct MenuEntryABI changeThreshold;
        changeThreshold.name = "Change threshold";
        changeThreshold.click = [](void* user) {
            struct control thresholdWindow;
            thresholdWindow.data.sliderint.min = 1;
            thresholdWindow.data.sliderint.max = 150;
            thresholdWindow.data.sliderint.label = "Threshold (ms)";
            thresholdWindow.data.sliderint.def = Conf::threshold;
            thresholdWindow.data.sliderint.onChange = [](void* user, int value) {
                Conf::threshold = value;
            };
            thresholdWindow.type = 1;

            mcpelauncher_show_window("Threshold", 1, NULL, [](void* user) { Conf::save(); }, 1, &thresholdWindow);
        };
        changeThreshold.selected = [](void* user) -> bool { return false; };
        changeThreshold.length = 0;

        struct MenuEntryABI reloadConf;
        reloadConf.name = "Reload config";
        reloadConf.click = [](void* user) {
            Conf::load();
        };
        reloadConf.selected = [](void* user) -> bool { return false; };
        reloadConf.length = 0;

        struct MenuEntryABI entry;
        struct MenuEntryABI entries[] = {enabled, blockRightDc, logClicks, changeThreshold, reloadConf};
        entry.subentries = entries;
        entry.length = sizeof(entries) / sizeof(struct MenuEntryABI);
        entry.name = "DCBlock";

        mcpelauncher_addmenu(1, &entry);

        *(void**)(&mcpelauncher_show_window) = dlsym(libmenu, "mcpelauncher_show_window");
    }
}
