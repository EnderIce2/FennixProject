#pragma once

#include <internal_task.h>

#include "exec.hpp"

RetStructData ExecuteMZ(const char *Path, ELEVATION Elevation, VMM::PageTableManager ptm);
