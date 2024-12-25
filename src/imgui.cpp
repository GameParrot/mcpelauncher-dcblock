#include <sys/stat.h>
#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>
#include "conf.h"
#include "imgui.h"
#include "util.h"

showwindow ImGUIOptions::mcpelauncher_show_window;
closewindow ImGUIOptions::mcpelauncher_close_window;

void ImGUIOptions::initImgui() {
    void* libmenu = dlopen("libmcpelauncher_menu.so", 0);
    if(libmenu) {
        typedef void (*addmenu)(size_t length, struct MenuEntryABI* entries);
        addmenu mcpelauncher_addmenu;
        *(void**)(&mcpelauncher_addmenu) = dlsym(libmenu, "mcpelauncher_addmenu");
        struct MenuEntryABI enabled;
        enabled.name = "Enabled";
        enabled.click = [](void* user) {
            Conf::enabled = !Conf::enabled;
            Conf::save();
            if(Conf::showLogWindow) {
                ImGUIOptions* inst = static_cast<ImGUIOptions*>(user);
                inst->updateLogWindow();
            }
        };
        enabled.selected = [](void* user) -> bool { return Conf::enabled; };
        enabled.length = 0;
        enabled.user = (void*)this;

        struct MenuEntryABI blockRightDc;
        blockRightDc.name = "Block right DC";
        blockRightDc.click = [](void* user) {
            Conf::blockRightDc = !Conf::blockRightDc;
            Conf::save();
            if(Conf::showLogWindow) {
                ImGUIOptions* inst = static_cast<ImGUIOptions*>(user);
                inst->updateLogWindow();
            }
        };
        blockRightDc.selected = [](void* user) -> bool { return Conf::blockRightDc; };
        blockRightDc.length = 0;
        blockRightDc.user = (void*)this;

        struct MenuEntryABI logClicks;
        logClicks.name = "Log clicks to console";
        logClicks.click = [](void* user) { Conf::logClicks = !Conf::logClicks; Conf::save(); };
        logClicks.selected = [](void* user) -> bool { return Conf::logClicks; };
        logClicks.length = 0;

        struct MenuEntryABI showLogWindow;
        showLogWindow.name = "Show log window";
        showLogWindow.click = [](void* user) {
            ImGUIOptions* inst = static_cast<ImGUIOptions*>(user);
            Conf::showLogWindow = !Conf::showLogWindow;
            Conf::save();
            if(Conf::showLogWindow) {
                inst->updateLogWindow();
            } else {
                inst->closeLogWindow();
            }
        };
        showLogWindow.selected = [](void* user) -> bool { return Conf::showLogWindow; };
        showLogWindow.length = 0;
        showLogWindow.user = (void*)this;

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

            mcpelauncher_show_window("Threshold", 1, user, [](void* user) { Conf::save(); }, 1, &thresholdWindow);
        };
        changeThreshold.selected = [](void* user) -> bool { return false; };
        changeThreshold.length = 0;

        struct MenuEntryABI reloadConf;
        reloadConf.name = "Reload config";
        reloadConf.click = [](void* user) {
            ImGUIOptions* inst = static_cast<ImGUIOptions*>(user);
            Conf::load(*inst);
        };
        reloadConf.user = (void*)this;
        reloadConf.selected = [](void* user) -> bool { return false; };
        reloadConf.length = 0;

        struct MenuEntryABI entry;
        struct MenuEntryABI entries[] = {enabled, blockRightDc, logClicks, showLogWindow, changeThreshold, reloadConf};
        entry.subentries = entries;
        entry.length = sizeof(entries) / sizeof(struct MenuEntryABI);
        entry.name = "DCBlock";

        mcpelauncher_addmenu(1, &entry);

        *(void**)(&mcpelauncher_show_window) = dlsym(libmenu, "mcpelauncher_show_window");
        *(void**)(&mcpelauncher_close_window) = dlsym(libmenu, "mcpelauncher_close_window");
    }
}

void ImGUIOptions::addClick(int button, bool supressed) {
    std::string newStr = (button == 1 ? "[Left] " : "[Right] ") + std::string((supressed ? "Supressed a DC" : "Mouse down"));
    clickLog = newStr + "\n" + clickLog;
    if(Conf::showLogWindow) {
        updateLogWindow();
    }
}

void ImGUIOptions::updateLogWindow() {
    struct control logWindow;
    logWindow.type = 3;
    logWindow.data.text.label = (char*)clickLog.c_str();

    std::string infoText = "Enabled: " + formatBool(Conf::enabled) + " | Block right DC: " + formatBool(Conf::blockRightDc) + " | Threshold: " + std::to_string(Conf::threshold) + "\n";
    struct control infoBox;
    infoBox.type = 3;
    infoBox.data.text.label = (char*)infoText.c_str();

    struct control entries[] = {infoBox, logWindow};

    mcpelauncher_show_window("DCBlock", 0, NULL, [](void* user) {}, 2, entries);
}

void ImGUIOptions::closeLogWindow() {
    mcpelauncher_close_window("DCBlock");
}