#include <dlfcn.h>
#include "conf.h"
#include "dcblock.h"
#include <android/log.h>
#include <stdbool.h>

DCBlock dcBlock;

bool (*game_window_add_mouse_button_callback)(void* window, void* user, bool (*hook)(void* user, double x, double y, int button, int action));
bool (*game_window_add_window_creation_callback)(void* user, void (*creation)(void* user));
void* (*game_window_get_primary_window)();

extern "C" void __attribute__((visibility("default"))) mod_preinit() {
}

extern "C" __attribute__((visibility("default"))) void mod_init() {
    dcBlock.init();
    void* h = dlopen("libmcpelauncher_gamewindow.so", 0);
    if(h) {
        game_window_add_mouse_button_callback = (decltype(game_window_add_mouse_button_callback))dlsym(h, "game_window_add_mouse_button_callback");
        game_window_add_window_creation_callback = (decltype(game_window_add_window_creation_callback))dlsym(h, "game_window_add_window_creation_callback");
        game_window_get_primary_window = (decltype(game_window_get_primary_window))dlsym(h, "game_window_get_primary_window");

        game_window_add_window_creation_callback(h, [](void* user) {
            void* window = game_window_get_primary_window();
            if(window) {
                game_window_add_mouse_button_callback(window, NULL, [](void* user, double x, double y, int button, int action) -> bool {
                    if(action != 0) {
                        return false;
                    }
                    return dcBlock.onMouseClick(button);
                });
            } else {
                __android_log_print(ANDROID_LOG_ERROR, "DCBlock", "Failed to initialize: game_window_get_primary_window returned null");
            }
        });

    } else {
        __android_log_print(ANDROID_LOG_ERROR, "DCBlock", "Failed to initialize: dlopen libmcpelauncher_gamewindow.so returned null");
    }
}
