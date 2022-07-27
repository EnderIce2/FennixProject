#ifndef _FSL_SYSTEM_H
#define _FSL_SYSTEM_H

#include <cdefs.h>

/**
 * @brief Write to the system debugger.
 * 
 * @param Format 
 * @param ... 
 * @return void
 */
E void WriteSysDebugger(const char *Format, ...);

/**
 * @brief Sleep for a given number of microseconds.
 * 
 * @param Microseconds 
 * @return void
 */
E void usleep(unsigned long Microseconds);

/**
 * @brief Execute a command in the system shell.
 * 
 * @param Command 
 * @return int
 */
E int system(const char *Command);

/**
 * @brief Exit the program with a status code.
 * 
 * @param Status 
 * @return void
 */
E void Exit(int Status) NO_THROW NO_RETURN;

#endif
