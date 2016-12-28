#include <string.h>
#include <stdlib.h>
#include <SDL_mixer.h>

#include "common.h"
#include "sound.h"

struct music_bucket
{
	struct music_bucket *next;
	char key[MUSIC_FILE_NAME_LENGTH];
	Mix_Music *music;
};

static struct {
#define MUSIC_ARR_SIZE 10
	struct music_bucket *music[MUSIC_ARR_SIZE];
} music_container;

static unsigned long hash(const char *str)
{
	unsigned long hash = 5381;
	int c;
	while (c = *str++)
		hash = ((hash << 5) + hash) + c;
	return hash;
}

void add_music(const char *name, const char* type)
{
	// Calculate index from hash.
	unsigned long index = hash(name) % MUSIC_ARR_SIZE;
	struct music_bucket* new_bucket = (struct music_bucket*) malloc(sizeof(struct music_bucket));
	// Copy name to the new bucket.
	strcpy_s(new_bucket->key, MUSIC_FILE_NAME_LENGTH, name);
	// Create buffer, put name inside.
	char name_buffer[MUSIC_FILE_NAME_LENGTH];
	strcpy_s(name_buffer, MUSIC_FILE_NAME_LENGTH, name);
	// Append file type.
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

static Mix_Music* get_music(const char *name)
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

void play_music(const char *name)
{
	if (Mix_PlayingMusic() == 0)
	{
		//Play the music
		if (Mix_PlayMusic(get_music(name), -1) == -1)
		{
			return;
		}
	}
}