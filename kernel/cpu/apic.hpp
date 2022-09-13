#pragma once

#if defined(__amd64__)
#include "../arch/amd64/cpu/apic.hpp"
#elif defined(__i386__)
#include "../arch/i686/cpu/apic.hpp"
#elif defined(__aarch64__)
#include "../arch/aarch64/cpu/apic.hpp"
#endif
