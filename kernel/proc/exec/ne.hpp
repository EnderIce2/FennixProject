#pragma once

#include <internal_task.h>

#include "exec.hpp"

RetStructData ExecuteNE(const char *Path, CBElevation Elevation, VMM::PageTableManager ptm);
