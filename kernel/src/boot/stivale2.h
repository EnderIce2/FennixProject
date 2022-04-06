#include <boot/protocols/stivale2.h>
#include <boot/gbp.h>
#include <types.h>

EXTERNC bool init_stivale2(struct stivale2_struct *bootloaderdata, GlobalBootParams *params);
EXTERNC bool detect_stivale2(void *data);
