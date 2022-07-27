#pragma once

/**
 * @brief Convert lowercase letter to uppercase
 * 
 */
#define toupper(c) ((c)-0x20 * (((c) >= 'a') && ((c) <= 'z')))

/**
 * @brief Convert uppercase letter to lowercase
 * 
 */
#define tolower(c) ((c) + 0x20 * (((c) >= 'A') && ((c) <= 'Z')))
