#include <dlfcn.h>
#include <sys/time.h>
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
            Mouse_feed_org(a,b,c,d,e,f);
            oms = getEpochTime();
        }
    } else if (a == 2 && b == 1) {
        if (getEpochTime() - oms1 > 50) {
            Mouse_feed_org(a,b,c,d,e,f);
            // Uncomment the line below to enable blocking right double clicks
            //oms1 = getEpochTime();
        }
    } else {
        Mouse_feed_org(a,b,c,d,e,f);
    }
}
void __attribute__ ((visibility ("default"))) mod_preinit() {
    void* h = dlopen("libmcpelauncher_mod.so", 0);
    typedef int (*preinithook)(const char*sym, void*val, void **orig);
    preinithook mcpelauncher_preinithook;
    *(void**)(&mcpelauncher_preinithook) = dlsym(h, "mcpelauncher_preinithook");
    mcpelauncher_preinithook("_ZN5Mouse4feedEcassss", (void*)&Mouse_feed, (void**)&Mouse_feed_org);
    //dlclose(h);
}
__attribute__ ((visibility ("default"))) void mod_init() {
}
