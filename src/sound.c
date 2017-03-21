#include <string.h>
#include <stdlib.h>
#include <SDL_mixer.h>

#include "common.h"
#include "config.h"
#include "hash.h"
#include "sound.h"

#define MUSIC_ARR_SIZE 5
#define SOUND_ARR_SIZE 10

struct music_bucket
{
	struct music_bucket *next;
	char *key;
	Mix_Music *music;
};
struct sound_bucket
{
	struct sound_bucket *next;
	char *key;
	Mix_Chunk *sound;
};

static struct {
	struct music_bucket *music[MUSIC_ARR_SIZE];
} music_container;

static struct {
	struct sound_bucket *sound[SOUND_ARR_SIZE];
} sound_container;

void music_set_pause(bool set)
{
#if MUSIC_ON
	switch (set)
	{
	case true:
		Mix_PauseMusic();
		break;
	case false:
		Mix_ResumeMusic();
		break;
	}
#else
	UNUSED_PARAMETER(set);
#endif // MUSIC_ON
}

static Mix_Music* music_get(const char *name)
{
	unsigned long index = hash_s(name) % MUSIC_ARR_SIZE;
	struct music_bucket* iter = music_container.music[index];
	while (iter)
	{
		if (strcmp(iter->key, name) == 0) // string match
			return iter->music;
		iter = iter->next;
	}
	return NULL;
}

void music_add(const char *name, const char* type)
{
#if MUSIC_ON
	if (music_get(name) != NULL)
	{
		INFO("Music %s already added. Skipping.", name);
		return;
	}
	// Calculate index from hash.
	unsigned long index = hash_s(name) % MUSIC_ARR_SIZE;
	struct music_bucket* new_bucket = (struct music_bucket*) malloc(sizeof(struct music_bucket));
	if (!new_bucket)
	{
		ERROR("Not enough memory!");
		return;
	}
	new_bucket->next = NULL;
	size_t str_len = SDL_strlen(name) + 1;
	new_bucket->key = malloc(str_len);
	// Copy name to the new bucket.
	strcpy_s(new_bucket->key, str_len, name);
	// Create buffer, put path to data folder inside.
	size_t name_buffer_length = SDL_strlen(SOUND_PATH) + SDL_strlen(name) + SDL_strlen(type) + 1;
	char *name_buffer = malloc(name_buffer_length);
	strcpy_s(name_buffer, name_buffer_length, SOUND_PATH);
	// Append file name and type.
	strcat_s(name_buffer, name_buffer_length, name);
	strcat_s(name_buffer, name_buffer_length, type);
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
	INFO("Music %s (%s) loaded to index %d.", name, name_buffer, index);
	free(name_buffer);
#else
	UNUSED_PARAMETER(type);
	UNUSED_PARAMETER(name);
#endif // MUSIC_ON
}

static Mix_Chunk* sound_get(const char *name)
{
	unsigned long index = hash_s(name) % SOUND_ARR_SIZE;
	struct sound_bucket* iter = sound_container.sound[index];
	while (iter)
	{
		if (strcmp(iter->key, name) == 0) // string match
			return iter->sound;
		iter = iter->next;
	}
	return NULL;
}

void sound_add(const char* name, const char* type)
{
#if SOUND_ON
	if (sound_get(name) != NULL)
	{
		INFO("Sound %s already added. Skipping.", name);
		return;
	}
	// Calculate index from hash.
	unsigned long index = hash_s(name) % SOUND_ARR_SIZE;
	struct sound_bucket* new_bucket = (struct sound_bucket*) malloc(sizeof(struct sound_bucket));
	if (!new_bucket)
	{
		ERROR("Not enough memory!");
		return;
	}
	new_bucket->next = NULL;
	size_t str_len = SDL_strlen(name) + 1;
	new_bucket->key = malloc(str_len);
	// Copy name to the new bucket.
	strcpy_s(new_bucket->key, str_len, name);
	// Create buffer, put name inside.
	size_t name_buffer_length = SDL_strlen(SOUND_PATH) + SDL_strlen(name) + SDL_strlen(type) + 1;
	char *name_buffer = malloc(name_buffer_length);
	strcpy_s(name_buffer, name_buffer_length, SOUND_PATH);
	// Append file name and type.
	strcat_s(name_buffer, name_buffer_length, name);
	strcat_s(name_buffer, name_buffer_length, type);
	// Load music.
	new_bucket->sound = Mix_LoadWAV(name_buffer);
	if (!new_bucket->sound)
	{
		ERROR("Couldn't load sound %s (%s): %s", name, name_buffer, Mix_GetError());
		free(new_bucket);
		return;
	}
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
	INFO("Sound %s (%s) loaded to index %d.", name, name_buffer, index);
	free(name_buffer);
#else
	UNUSED_PARAMETER(name);
	UNUSED_PARAMETER(type);
#endif // SOUND_ON
}

void music_play(const char *name, int fadein_ms)
{
#if MUSIC_ON
	// which music is currently playing?
	static unsigned long curr_hash = -1; // -1, make it overflow
	unsigned long new_hash = hash_s(name);
	// not already playing?
	if (new_hash != curr_hash)
	{
		curr_hash = new_hash;
		//Play the music
		Mix_Music* m = music_get(name);
		if (m == NULL)
		{
			ERROR("Music %s not found. Use function add_music first to load the music into the resource manager.", name);
			return;
		}

		if (Mix_FadeInMusic(m, -1, fadein_ms) == -1)
		{
			ERROR("Couldn't play music %s.", name);
		}
	}
#else
	UNUSED_PARAMETER(name);
	UNUSED_PARAMETER(fadein_ms);
#endif // MUSIC_ON
}

void music_stop(void)
{
#if MUSIC_ON
	Mix_HaltMusic();
#endif // MUSIC_ON
}

void sound_play(const char *name)
{
#if SOUND_ON
	Mix_Chunk* s = sound_get(name);
	if (s == NULL)
	{
		ERROR("Sound %s not found. Use function add_sound first to load the music into the resource manager.", name);
		return;
	}
	if (Mix_PlayChannel(-1, s, 0) == -1)
	{
		ERROR("Couldn't play sound %s.", name);
	}
#else
	UNUSED_PARAMETER(name);
#endif // SOUND_ON
}

void audio_destroy(void)
{
#if MUSIC_ON
	for (int i = 0; i < MUSIC_ARR_SIZE; ++i)
	{
		while (music_container.music[i])
		{
			struct music_bucket *iter = music_container.music[i];
			music_container.music[i] = iter->next;
			free(iter->key);
			Mix_FreeMusic(iter->music);
			free(iter);
		}
	}
#endif // MUSIC_ON
#if SOUND_ON
	for (int i = 0; i < SOUND_ARR_SIZE; ++i)
		while (sound_container.sound[i])
		{
			struct sound_bucket *iter = sound_container.sound[i];
			sound_container.sound[i] = iter->next;
			free(iter->key);
			Mix_FreeChunk(iter->sound);
			free(iter);
		}
#endif // SOUND_ON
}