#ifndef SOUND_H
#define SOUND_H

#define MUSIC_FILE_NAME_LENGTH 25

void add_music(const char* name, const char* type);
void add_sound(const char* name, const char* type);
void play_music(const char *name);
void play_sound(const char *name);
void destroy_sound(void);

#endif // SOUND_H