#ifndef SOUND_H
#define SOUND_H

// Load sound name.type into audio container.
void music_add(const char* name, const char* type);
void sound_add(const char* name, const char* type);
void music_play(const char *name, int fadein_ms);
void music_stop(void);
void music_set_pause(bool set);
void sound_play(const char *name);
// Cleanup audio resources.
void audio_destroy(void);

#endif // SOUND_H