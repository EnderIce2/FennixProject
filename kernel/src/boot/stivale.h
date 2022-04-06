#include <boot/protocols/stivale.h>
#include <boot/gbp.h>
#include <types.h>

EXTERNC bool init_stivale(struct stivale_struct *bootloaderdata, GlobalBootParams *params);
EXTERNC bool detect_stivale(void *data);
