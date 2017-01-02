#include <stdio.h>
#include <stdlib.h>

#include "common.h"
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

// Level sprite manager
static void level_texture_add(const char* name, const char* type)
{

}
static SDL_Texture* level_texture_get(const char *name, SDL_Rect *target)
{

}

// Level texture data --> objects, actors (enemies) etc.
struct level_data_bucket
{
#define LEVEL_RESOURCE_LENGTH 30
	struct level_data_bucket *next;
	char key[LEVEL_RESOURCE_LENGTH];

	struct {
		// If NULL, full.
		SDL_Rect *target;
		// ID of the texture in the internal texture manager.
		int texture_id;
	} sprite;
};

static struct
{
#define LEVEL_SPRITE_ARR_SIZE 20
	struct level_data_bucket *data[LEVEL_SPRITE_ARR_SIZE];
} *g_sprites;

// INTERNAL, load level textures into container
static bool level_load_textures(struct level *level, SDL_Renderer *renderer, FILE *f);
static void level_destroy_textures(struct level *level);


bool level_load(int id, SDL_Renderer *renderer)
{
	INFO("- Loading level.");
	if (g_level)
	{
		ERROR("Previous level not cleaned");
		level_clean(&g_level);
	}
	level_init(&g_level);
	char buffer[BUFFER_SIZE];
	sprintf_s(buffer, BUFFER_SIZE, "data/map/level%d.level", id);
	if (level_load_data(g_level, renderer, buffer))
	{
		INFO("Level %d loaded.", id);
		return true;
	}
	ERROR("Level %d not loaded.", id);
	return false;
}

bool level_load_data(struct level *level, SDL_Renderer *renderer, const char* file_name)
{
	// TODO: Validate data.
	if (!level)
	{
		ERROR("level can't be NULL");
		return false;
	}
	FILE *f = NULL;
	fopen_s(&f, file_name, "r");
	INFO("Tilemap %s loaded.", file_name);
	if (!f)
	{
		ERROR("Level file couldn't be opened.");
		return false;
	}
	level->id = 0;
	INFO("level! %d", level->id);
	fscanf_s(f, "%s", level->name, LEVEL_NAME_LENGTH);
	INFO("name! %s", level->name);
	fscanf_s(f, "%d%*c%d", &level->tile_map.width, &level->tile_map.height);
	INFO("width! %d", level->tile_map.width);
	INFO("height! %d", level->tile_map.height);
	fscanf_s(f, "%d%*c%d", &level->tile_map.tile_width, &level->tile_map.tile_height);
	INFO("tilewidth! %d", level->tile_map.tile_width);
	INFO("tileheight! %d", level->tile_map.tile_height);
	if (!level_load_tilemap(level, f) || !level_load_textures(level, renderer, f))
	{
		fclose(f);
		return false;
	}
	g_level->background = load_texture(renderer, "data/background1.png");
	if (!g_level->background)
	{
		INFO("Level background couldn't be loaded.");
		fclose(f);
		return false;
	}
	fclose(f);
	return true;
}

void level_init(struct level **level)
{
	*level = malloc(sizeof(**level));
	if (!*level)
	{
		ERROR("Allocation error at level structure initialization.");
		return;
	}
	INFO("Level structure initialized.");
}

void level_clean(struct level **level)
{
	if (!*level)
		return;
	level_free_grid(*level);
	level_destroy_textures(*level);
	free(*level);
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

bool level_load_textures(struct level *level, SDL_Renderer *renderer, FILE *f)
{
	INFO("Loading textures...");
	char file_name[BUFFER_SIZE];
	fscanf_s(f, "%s", file_name, BUFFER_SIZE);
	level->tileset = load_texture(renderer, file_name);
	if (!level->tileset)
	{
		ERROR("Tileset %s couldn't be loaded.", file_name);
		return false;
	}
	INFO("Tileset (file: %s) loaded.", file_name);
	return true;
}

void level_destroy_textures(struct level *level)
{
	SDL_DestroyTexture(level->tileset);
	SDL_DestroyTexture(level->background);
	INFO("Textures destroyed.");
}

// Level sprite manager
