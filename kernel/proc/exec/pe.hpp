#pragma once

#include <internal_task.h>

#include "exec.hpp"

RetStructData ExecutePE(const char *Path, CBElevation Elevation, VMM::PageTableManager ptm);
