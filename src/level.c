#include <stdio.h>
#include <stdlib.h>

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

// INTERNAL, load level textures into container
static bool level_load_textures(struct level *level, SDL_Renderer *renderer, FILE *f);
static void level_destroy_textures(struct level *level);


bool level_load(int id, SDL_Renderer *renderer)
{
	INFO("- Loading level %d.", id);
	if (g_level) // Previous level not cleaned up.
	{
		INFO("Cleaning up previously loaded level %s (#%d).", g_level->name, g_level->id);
		level_clean(&g_level);
	}
	level_init(&g_level);
	char buffer[BUFFER_SIZE];
	sprintf_s(buffer, BUFFER_SIZE, LEVEL_PATH"level%d.level", id);
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
		ERROR("Level can't be NULL. Call level_init first.");
		return false;
	}
	FILE *f = NULL;
	fopen_s(&f, file_name, "r");
	if (!f)
	{
		ERROR("Level file couldn't be opened.");
		return false;
	}
	INFO("Tilemap %s loaded.", file_name);
	// TODO: Set this to correct value.
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
	fclose(f);
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
	INFO("Level structure initialized.");
}

void level_clean(struct level **level)
{
	if (!*level)
	{
		INFO("Level not initialized. Skipping clean.");
		return;
	}
	level_free_grid(*level);
	//level_destroy_sprites();
	level_destroy_textures(*level);
	INFO("%p", *level);
	free(*level);
	*level = NULL;
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
	fscanf_s(f, "%s", file_name, BUFFER_SIZE);
	g_level->background = load_texture(renderer, file_name);
	if (!g_level->background)
	{
		INFO("Level background couldn't be loaded.");
		fclose(f);
		return false;
	}
	/*
	char buffer[BUFFER_SIZE];
	while (fgets(file_name, BUFFER_SIZE, f))
	{
		int curr_id = 0; // currently processing texture id #curr_id
		SDL_Rect temp;
		if (sscanf_s(file_name, "SPRITE: %s %d %d %d %d", buffer, BUFFER_SIZE, &temp.x, &temp.y, &temp.w, &temp.h) == 5)
		{
			SDL_Rect *target = (SDL_Rect*)malloc(sizeof(SDL_Rect));
			target->x = temp.x;
			target->y = temp.y;
			target->w = temp.w;
			target->h = temp.h;
			level_sprite_add(buffer, curr_id, target);
		}
		else if (sscanf_s(file_name, "FILE: %s", buffer, BUFFER_SIZE) == 1)
			curr_id = level_texture_add(buffer, renderer);
		else if (SDL_strcmp("OBJECTS\n", file_name) == 0)
			break;
	}
	*/
	return true;
}

void level_destroy_textures(struct level *level)
{
	SDL_DestroyTexture(level->tileset);
	SDL_DestroyTexture(level->background);
	/*
	for (int i = 0; i < LEVEL_TEXTURES_ARR_SIZE; ++i)
	{
		while (textures_container.data[i])
		{
#ifdef _DEBUG
			int ll_position = -1; // position in linked list
			ll_position++;
#endif // _DEBUG
			struct level_texture_bucket *iter = textures_container.data[i];
			textures_container.data[i] = iter->next;
#ifdef _DEBUG
			INFO("Freeing texture #%d from index %d (position %d).", iter->key, i, ll_position);
#endif // _DEBUG
			SDL_DestroyTexture(iter->texture);
			free(iter);
		}
	}
	*/
	INFO("Textures destroyed.");
}