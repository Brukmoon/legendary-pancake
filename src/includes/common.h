/**
 * Common definitions.
 * @author Michal H.
 *
 */
#ifndef COMMON_H
#define COMMON_H

// Variadic args.
#include <stdarg.h>
#include <stdio.h>

// Message type.
enum log_level
{
	L_INFO,
	L_ERROR,
	L_CRITICAL
};

// Debug utilities.
#ifdef _DEBUG
#define ERROR(format, ...) fprintf(stderr, "ERROR[%s]"##format##"\n", __FILE__, ##__VA_ARGS__)
#define INFO(format, ...) fprintf(stdout, ": "##format##"\n", ##__VA_ARGS__)
#else
// Empty, nop.
#define ERROR(format, ...)
#define INFO(format, ...)
#endif

#ifndef UNUSED_PARAMETER
// Suppress warning: unused parameter.
#define UNUSED_PARAMETER(p) (void)p
#endif // UNUSED_PARAMETER

#endif // COMMON_H