#include "conf.h"
#include "util.h"
#include <properties/property_list.h>
#include <properties/property.h>
#include <fstream>

bool Conf::enabled = true;
bool Conf::blockRightDc = true;
bool Conf::logClicks = false;
bool Conf::showLogWindow = false;
int Conf::threshold = 50;

bool Conf::locked = false;

static properties::property_list conf('=');
static properties::property<bool> enabled(conf, "Enabled", true);
static properties::property<bool> blockRightDc(conf, "BlockRightDc", true);
static properties::property<bool> logClicks(conf, "LogClicks", false);
static properties::property<bool> showLogWindow(conf, "ShowLogWindow", false);
static properties::property<int> threshold(conf, "Threshold", 50);

void Conf::load(ImGUIOptions opts) {
    std::ifstream propertiesFile(getPath());
    if(propertiesFile) {
        conf.load(propertiesFile);
    } else {
        Conf::save();
    }
    Conf::enabled = ::enabled.get();
    Conf::blockRightDc = ::blockRightDc.get();
    Conf::logClicks = ::logClicks.get();
    Conf::showLogWindow = ::showLogWindow.get();
    Conf::threshold = ::threshold.get();

    if(Conf::showLogWindow) {
        opts.updateLogWindow();
    } else {
        opts.closeLogWindow();
    }

    printConfig();
}

void Conf::save() {
    ::enabled.set(Conf::enabled);
    ::blockRightDc.set(Conf::blockRightDc);
    ::logClicks.set(Conf::logClicks);
    ::showLogWindow.set(Conf::showLogWindow);
    ::threshold.set(Conf::threshold);
    std::ofstream propertiesFile(getPath());
    if(propertiesFile) {
        conf.save(propertiesFile);
    }
}
