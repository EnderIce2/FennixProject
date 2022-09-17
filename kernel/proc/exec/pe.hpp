#pragma once

#include <internal_task.h>

#include "exec.hpp"

RetStructData ExecutePE(const char *Path, ELEVATION Elevation, VMM::PageTableManager ptm);
