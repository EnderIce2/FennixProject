#include <boot/protocols/limine.h>
#include <boot/gbp.h>
#include <types.h>

EXTERNC bool init_limine(GlobalBootParams *params);
EXTERNC bool detect_limine();
