/*
 * Global hashing functions.
 * Make sure the values are as different as possible.
 * @author Michal H.
 *
 */
#ifndef HASH_H
#define HASH_H

// Hash a string. Returns a large number.
unsigned long hash_s(const char *str);
inline int hash_i(int key) { return key; }

#endif // HASH_H