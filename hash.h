#ifndef HASH_H
#define HASH_H

unsigned long hash_s(const char *str);
inline int hash_i(int key, int buffer_size) { return key%buffer_size; }

#endif // HASH_H