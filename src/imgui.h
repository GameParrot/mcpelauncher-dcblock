#include <string>
#pragma once

typedef void (*showwindow)(const char* title, int isModal, void* user, void (*onClose)(void* user), int count, struct control* controls);
typedef void (*closewindow)(const char* title);

class ImGUIOptions {
private:
    std::string clickLog;
    static showwindow mcpelauncher_show_window;
    static closewindow mcpelauncher_close_window;

public:
    void initImgui();
    void addClick(int button, bool supressed);
    void updateLogWindow();
    void closeLogWindow();
};

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
