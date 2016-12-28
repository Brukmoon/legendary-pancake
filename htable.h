/**
 * Array/LList Hash table imlementation.
 * @author Michal H.
 *
 */
#ifndef HTABLE_H
#define HTABLE_H

#include <stdbool.h>

// djb2 hash
/*unsigned long
hash(const char *str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c;

	return hash;
}*/

#endif // HTABLE_H