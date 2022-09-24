#pragma once

#include <internal_task.h>

#include "exec.hpp"

RetStructData ExecuteMZ(const char *Path, CBElevation Elevation, VMM::PageTableManager ptm);
