#pragma once

#include <internal_task.h>

#include "exec.hpp"

RetStructData ExecuteELF(const char *Path, ELEVATION Elevation, VMM::PageTableManager ptm);
