#include <string.h>
#include <stdlib.h>
#include <SDL_mixer.h>

#include "common.h"
#include "config.h"
#include "sound.h"

struct music_bucket
{
	struct music_bucket *next;
	char key[MUSIC_FILE_NAME_LENGTH];
	Mix_Music *music;
};
struct sound_bucket
{
	struct sound_bucket *next;
	char key[MUSIC_FILE_NAME_LENGTH];
	Mix_Chunk *sound;
};

static struct {
#define MUSIC_ARR_SIZE 10
	struct music_bucket *music[MUSIC_ARR_SIZE];
} music_container;

static struct {
#define SOUND_ARR_SIZE 10
	struct sound_bucket *sound[SOUND_ARR_SIZE];
} sound_container;

static unsigned long hash(const char *str)
{
	unsigned long hash = 5381;
	int c;
	while (c = *str++)
		hash = ((hash << 5) + hash) + c;
	return hash;
}

void music_add(const char *name, const char* type)
{
	// Calculate index from hash.
	unsigned long index = hash(name) % MUSIC_ARR_SIZE;
	struct music_bucket* new_bucket = (struct music_bucket*) malloc(sizeof(struct music_bucket));
	if (!new_bucket)
	{
		ERROR("Not enough memory!");
		return;
	}
	new_bucket->next = NULL;
	// Copy name to the new bucket.
	strcpy_s(new_bucket->key, MUSIC_FILE_NAME_LENGTH, name);
	// Create buffer, put path to data folder inside.
	char name_buffer[MUSIC_FILE_NAME_LENGTH];
	strcpy_s(name_buffer, MUSIC_FILE_NAME_LENGTH, SOUND_PATH);
	// Append file name and type.
	strcat_s(name_buffer, MUSIC_FILE_NAME_LENGTH, name);
	strcat_s(name_buffer, MUSIC_FILE_NAME_LENGTH, type);
	// Load music.
	new_bucket->music = Mix_LoadMUS(name_buffer);
	struct music_bucket* iter = music_container.music[index], *prev = NULL;
	while (iter)
	{
		prev = iter;
		iter = iter->next;
	}
	if (!prev)
	{
		music_container.music[index] = new_bucket;
	}
	else
		prev->next = new_bucket;
}

void sound_add(const char* name, const char* type)
{
	// Calculate index from hash.
	unsigned long index = hash(name) % SOUND_ARR_SIZE;
	struct sound_bucket* new_bucket = (struct sound_bucket*) malloc(sizeof(struct sound_bucket));
	if (!new_bucket)
	{
		ERROR("Not enough memory!");
		return;
	}
	new_bucket->next = NULL;
	// Copy name to the new bucket.
	strcpy_s(new_bucket->key, MUSIC_FILE_NAME_LENGTH, name);
	// Create buffer, put name inside.
	char name_buffer[MUSIC_FILE_NAME_LENGTH];
	strcpy_s(name_buffer, MUSIC_FILE_NAME_LENGTH, SOUND_PATH);
	// Append file name and file type.
	strcat_s(name_buffer, MUSIC_FILE_NAME_LENGTH, name);
	strcat_s(name_buffer, MUSIC_FILE_NAME_LENGTH, type);
	// Load music.
	new_bucket->sound = Mix_LoadWAV(name_buffer);
	struct sound_bucket* iter = sound_container.sound[index], *prev = NULL;
	while (iter)
	{
		prev = iter;
		iter = iter->next;
	}
	if (!prev)
	{
		sound_container.sound[index] = new_bucket;
	}
	else
		prev->next = new_bucket;
}
static Mix_Music* music_get(const char *name)
{
	unsigned long index = hash(name) % MUSIC_ARR_SIZE;
	struct music_bucket* iter = music_container.music[index];
	while (iter)
	{
		if (strcmp(iter->key, name) == 0) // string match
			return iter->music;
		iter = iter->next;
	}
	ERROR("Music %s not found. Use function add_music first to load the music into the resource manager.", name);
	return NULL;
}

static Mix_Chunk* sound_get(const char *name)
{
	unsigned long index = hash(name) % SOUND_ARR_SIZE;
	struct sound_bucket* iter = sound_container.sound[index];
	while (iter)
	{
		if (strcmp(iter->key, name) == 0) // string match
			return iter->sound;
		iter = iter->next;
	}
	ERROR("Sound %s not found. Use function add_sound first to load the sound into the resource manager.", name);
	return NULL;
}

void music_play(const char *name)
{
	if (!Mix_PlayingMusic())
	{
		//Play the music
		if (Mix_PlayMusic(music_get(name), -1) == -1)
		{
			ERROR("Couldn't play music %s.", name);
		}
	}
}

void sound_play(const char *name)
{
	if (Mix_PlayChannel(-1, sound_get(name), 0) == -1)
	{
		ERROR("Couldn't play sound %s.", name);
	}
}

void audio_destroy(void)
{
	for (int i = 0; i < MUSIC_ARR_SIZE; ++i)
	{
		while (music_container.music[i])
		{
			struct music_bucket *iter = music_container.music[i];
			music_container.music[i] = iter->next;
			Mix_FreeMusic(iter->music);
			free(iter);
		}
	}
	for (int i = 0; i < SOUND_ARR_SIZE; ++i)
		while (sound_container.sound[i])
		{
			struct sound_bucket *iter = sound_container.sound[i];
			sound_container.sound[i] = iter->next;
			Mix_FreeChunk(iter->sound);
			free(iter);
		}
}