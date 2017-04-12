#include "actor.h"
#include "common.h"
#include "config.h"
#include "replay.h"

static struct replay
{
	FILE* f;
	char* buffer;
	size_t curr_position;
} g_replay;

bool replay_open_write(const char* level_name)
{
	size_t replay_length = SDL_strlen(REPLAY_PATH) + SDL_strlen("replay_.log") + SDL_strlen(level_name) + 1;
	char* replay = malloc(replay_length);
	if (!replay)
		ERROR("Alloc error!"); // TODO: Replace alloc error.
	SDL_strlcpy(replay, REPLAY_PATH, replay_length);
	SDL_strlcat(replay, "replay_", replay_length);
	SDL_strlcat(replay, level_name, replay_length);
	SDL_strlcat(replay, ".log", replay_length);
	fopen_s(&g_replay.f, replay, "w");
	free(replay);
	if (!g_replay.f)
		return false;
	g_replay.buffer = NULL;
	g_replay.curr_position = 0;
	// TODO: Check stream correctness.
	return true;
}

bool replay_open_read(const char* level_name)
{
	size_t replay_length = SDL_strlen(REPLAY_PATH) + SDL_strlen("replay_.log") + SDL_strlen(level_name) + 1;
	char* replay = malloc(replay_length);
	if (!replay)
		ERROR("Alloc error!"); // TODO: Replace alloc error.
	SDL_strlcpy(replay, REPLAY_PATH, replay_length);
	SDL_strlcat(replay, "replay_", replay_length);
	SDL_strlcat(replay, level_name, replay_length);
	SDL_strlcat(replay, ".log", replay_length);
	fopen_s(&g_replay.f, replay, "r");
	free(replay);
	g_replay.curr_position = 0;
	if (!g_replay.f)
		return false;
	// TODO: Check stream correctness.

		/* Go to the end of the file. */
	if (fseek(g_replay.f, 0L, SEEK_END) == 0)
	{
		/* Get the size of the file. */
		long bufsize = ftell(g_replay.f);
		if (bufsize == -1)
			return false;

		/* Allocate our buffer to that size. */
		g_replay.buffer = malloc(sizeof(char) * (bufsize + 1));

		/* Go back to the start of the file. */
		if (fseek(g_replay.f, 0L, SEEK_SET) != 0)
			return false;

		/* Read the entire file into memory. */
		size_t newLen = fread(g_replay.buffer, sizeof(char), bufsize, g_replay.f);
		if (ferror(g_replay.f) != 0)
		{
			return false;
		}

		else {
			g_replay.buffer[newLen++] = '\0'; /* Just to be safe. */
		}
	}
	return true;
}

void replay_save(int val)
{
	if (g_replay.f)
		fprintf(g_replay.f, "%d", val);
}

void replay_close(void)
{
	free(g_replay.buffer);
	fclose(g_replay.f);
	g_replay.f = NULL;
}

bool replay_read_frame(bool action[ACTION_COUNT])
{
	for (size_t i = 0; i < ACTION_COUNT; ++i)
	{
		if (g_replay.buffer[g_replay.curr_position] != '\0')
		{
			if ((g_replay.buffer[g_replay.curr_position++] - '0') > 1)
			{
				--i;
				continue;
			}
			action[i] = (bool)(g_replay.buffer[g_replay.curr_position-1] - '0');
		}
		else
			return false;
	}
	return true;
}