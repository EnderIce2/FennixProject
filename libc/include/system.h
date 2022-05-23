#ifndef _FSL_SYSTEM_H
#define _FSL_SYSTEM_H

#include <cdefs.h>

E void WriteSysDebugger(const char *Format, ...);
E void usleep(unsigned long Microseconds);
E int system(const char *Command);
E void Exit(int Status) NO_THROW NO_RETURN;

#endif
