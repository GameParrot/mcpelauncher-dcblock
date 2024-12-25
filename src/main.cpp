#include <dlfcn.h>
#include "conf.h"
#include "dcblock.h"
#include "util.h"
#include <sys/time.h>
#include <stdlib.h>
#include <android/log.h>
#include <android/input.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>

DCBlock dcBlock;

extern "C" void __attribute__((visibility("default"))) mod_preinit() {
    void *h = dlopen("libmcpelauncher_mod.so", 0);
    typedef int (*preinithook)(const char *, void *, void **);
    preinithook mcpelauncher_preinithook;
    *(void **)(&mcpelauncher_preinithook) = dlsym(h, "mcpelauncher_preinithook");

    mcpelauncher_preinithook("AMotionEvent_getButtonState", (void *)+[](const AInputEvent *t) -> int32_t { return dcBlock.getButtonState(t); }, nullptr);

    dcBlock.init();
}

extern "C" __attribute__((visibility("default"))) void mod_init() {
}
