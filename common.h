/**
 * Common macros.
 * @author Michal H.
 *
 */
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdarg.h>

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
#define ERROR(format, ...)
#define INFO(format, ...)
#endif

#endif // COMMON_H