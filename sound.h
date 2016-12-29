#ifndef SOUND_H
#define SOUND_H

#define MUSIC_FILE_NAME_LENGTH 25

void music_add(const char* name, const char* type);
void sound_add(const char* name, const char* type);
void music_play(const char *name);
void sound_play(const char *name);
void audio_destroy(void);

#endif // SOUND_H