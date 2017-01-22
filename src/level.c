#include <stdio.h>
#include <stdlib.h>

#include "actor.h"
#include "common.h"
#include "config.h"
#include "hash.h"
#include "game.h"
#include "level.h"
#include "texture.h"

#define BUFFER_SIZE 256

struct level *g_level = NULL;

// Allocate memory for the level structure.
static void level_init(struct level **level);
static void level_alloc_grid(struct level *level);
static void level_free_grid(struct level *level);

// Load file_name level data into the level structure.
static bool level_load_data(struct level *level, SDL_Renderer *renderer, const char* file_name);

// INTERNAL, load the grid into level, expects iobuf to point to the grid.
static bool level_load_tilemap(struct level *level, FILE *f);
static void level_destroy_textures(struct level *level);


bool level_load(const char *name, SDL_Renderer *renderer)
{
	INFO("- Loading level %s.", name);
	if (g_level) // Previous level not cleaned up.
	{
		INFO("Cleaning up previously loaded level %s (#%d).", g_level->name, g_level->id);
		level_clean();
	}
	level_init(&g_level);
	char buffer[BUFFER_SIZE];
	sprintf_s(buffer, BUFFER_SIZE, LEVEL_PATH"%s.level", name);
	if (level_load_data(g_level, renderer, buffer))
	{
		INFO("Level %s loaded.", name);
		return true;
	}
	//g_level = NULL;
	ERROR("Level %s not loaded.", name);
	return false;
}

bool level_load_data(struct level *level, SDL_Renderer *renderer, const char* file_name)
{
	// TODO: Validate data.
	if (!level)
	{
		ERROR("Level can't be NULL. Call level_init first.");
		return false;
	}
	FILE *f = NULL;
	char buffer[BUFFER_SIZE], 
		command[21]; // A command can be 20 characters long (+ '\0').
	fopen_s(&f, file_name, "r");
	if (f != NULL) {
		while (!feof(f)) {
			fgets(buffer, BUFFER_SIZE, f);
			// Parse first string and ignore everything else except a newline.
			sscanf_s(buffer, "%s%*[^\n]", command, 21);
			if (SDL_strcmp(command, "NAME") == 0)
			{
				// Name of the level.
				sscanf_s(buffer, "%*s%s", level->name, LEVEL_NAME_LENGTH);
				INFO("name! %s", level->name);
			}
			else if (SDL_strcmp(command, "MAP") == 0)
			{
				// Level width and height.
				sscanf_s(buffer, "%*s%d%d", &level->tile_map.width, &level->tile_map.height);
				INFO("width! %d\nheight! %d", level->tile_map.width, level->tile_map.height);
				// Load the grid.
				level_load_tilemap(level, f);
			}
			else if (SDL_strcmp(command, "SSHEET") == 0)
			{
				// Path to the spritesheet.
				char name[BUFFER_SIZE];
				sscanf_s(buffer, "%*s%s", name, BUFFER_SIZE);
				level->tileset = load_texture(renderer, name);
				if (!level->tileset)
				{
					ERROR("Tileset %s couldn't be loaded.", name);
					fclose(f);
					return false;
				}
				INFO("Tileset (file: %s) loaded.", name);
			}
			else if (SDL_strcmp(command, "BACKGROUND") == 0)
			{
				char name[BUFFER_SIZE];
				sscanf_s(buffer, "%*s%s", name, BUFFER_SIZE);
				g_level->background = load_texture(renderer, name);
				if (!g_level->background)
				{
					INFO("Level background couldn't be loaded.");
					fclose(f);
					return false;
				}
			}
			else if (SDL_strcmp(command, "TILE") == 0)
			{
				sscanf_s(buffer, "%*s%d", &level->tile_map.tile_width);
				level->tile_map.tile_height = level->tile_map.tile_width;
				INFO("tilewidth! %d", level->tile_map.tile_width);
				INFO("tileheight! %d", level->tile_map.tile_height);
			}
			else if (SDL_strcmp(command, "SPAWN") == 0)
			{
				vec2 spawn = { 0, 0 };
				sscanf_s(buffer, "%*s%d%d", &spawn.x, &spawn.y);
				player_set_spawn(&g_player, spawn);
			}
			// Clear.
			command[0] = '\0';
		}
	}
	else {
		ERROR("Level file %s couldn't be opened.", file_name);
		return false;
	}
	fclose(f);
	/* Set this to correct value! */
	level->id = 0;
	INFO("level! %d", level->id);
	return true;
}

void level_init(struct level **level)
{
	if (*level) // prevent memory leak
	{
		INFO("Level already initialized. Skipping.");
		return;
	}
	*level = malloc(sizeof(**level));
	if (!*level)
	{
		ERROR("Allocation error at level structure initialization.");
		return;
	}
	(*level)->background = NULL;
	(*level)->tileset = NULL;
	for (int i = 0; i < TMAP_LAYER_COUNT; ++i)
		(*level)->tile_map.map[i] = NULL;
	INFO("Level structure initialized.");
}

void level_clean(void)
{
	if (!g_level)
	{
		INFO("Level not initialized. Skipping clean.");
		return;
	}
	if (g_level->tile_map.map[0])
	{
		level_free_grid(g_level);
		level_destroy_textures(g_level);
	}
	free(g_level);
	g_level = NULL;
	INFO("Level structure freed.");
}

bool level_load_tilemap(struct level *level, FILE *f)
{
	level_alloc_grid(level);
	for (int i = 0; i < level->tile_map.height; ++i)
		for (int j = 0; j < level->tile_map.width; ++j)
			fscanf_s(f, "%d:%d", &level->tile_map.map[TMAP_TEXTURE_LAYER][i][j], &level->tile_map.map[TMAP_COLLISION_LAYER][i][j]);
#ifdef _DEBUG
	INFO("Map grid: ");
	for (int i = 0; i < level->tile_map.height; ++i)
	{
		for (int j = 0; j < level->tile_map.width; ++j)
			printf("%d ", level->tile_map.map[TMAP_COLLISION_LAYER][i][j]);
		printf("\n");
	}
#endif // _DEBUG
	return true;
}

void level_alloc_grid(struct level *level)
{
	if (!level)
	{
		ERROR("Level can't be NULL.");
		return;
	}
	for (int j = 0; j < TMAP_LAYER_COUNT; ++j)
	{
		level->tile_map.map[j] = malloc(level->tile_map.height * sizeof(tile*));
		for (int i = 0; i < level->tile_map.height; ++i)
			level->tile_map.map[j][i] = malloc(level->tile_map.width * sizeof(tile));
	}
	INFO("Level grid initialized.");
}

void level_free_grid(struct level *level)
{
		for (int j = 0; j < TMAP_LAYER_COUNT; ++j)
		{
			for (int i = 0; i < level->tile_map.height; ++i)
				free(level->tile_map.map[j][i]);
			free(level->tile_map.map[j]);
		}
		INFO("Level grid freed.");
}

void level_destroy_textures(struct level *level)
{
	SDL_DestroyTexture(level->tileset);
	SDL_DestroyTexture(level->background);
	INFO("Textures destroyed.");
}

bool level_save()
{
	char file_name[LEVEL_NAME_LENGTH + 7];
	SDL_strlcpy(file_name, LEVEL_PATH, LEVEL_NAME_LENGTH + 7);
	SDL_strlcat(file_name, g_level->name, LEVEL_NAME_LENGTH + 7);
	SDL_strlcat(file_name, ".level", LEVEL_NAME_LENGTH + 7);
	FILE *f = NULL;
	fopen_s(&f, file_name, "w");
	if (!f)
	{
		ERROR("Couldn't open file %s.", file_name);
		return false;
	}
	fprintf(f, "NAME %s\n", g_level->name);
	fprintf(f, "TILE %d\n", g_level->tile_map.tile_width);
	fprintf(f, "MAP %d %d\n", g_level->tile_map.width, g_level->tile_map.height);
	for (int y = 0; y < g_level->tile_map.height; ++y)
	{
		for (int x = 0; x < g_level->tile_map.width; ++x)
			fprintf(f, "%d:%d ", g_level->tile_map.map[TMAP_TEXTURE_LAYER][y][x], g_level->tile_map.map[TMAP_COLLISION_LAYER][y][x]);
		fprintf(f, "\n");
	}
	fprintf(f, "%s\n", "SSHEET assets/gfx/spritesheet.png");
	fprintf(f, "%s\n", "BACKGROUND assets/gfx/level1_background.png");
	fprintf(f, "%s %d %d\n", "SPAWN", g_player.actor.spawn.x, g_player.actor.spawn.y);
	INFO("Saving level to file %s.", file_name);
	fclose(f);
	return true;
}