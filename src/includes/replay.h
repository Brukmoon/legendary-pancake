#ifndef REPLAY_H
#define REPLAY_H

#include <stdbool.h>

/*
 * Open the replay stream.
 */
bool replay_open_write(const char* level_name);
bool replay_open_read(const char* level_name);

/*
 * Close replay stream.
 */
void replay_close(void);
bool replay_read_frame(bool action[ACTION_COUNT]);
// Save into the replay file.
void replay_save(int val);

#endif // REPLAY_H
