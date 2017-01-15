#include <SDL.h>

// Level texture data --> objects, actors (enemies) etc.
struct sprite_bucket
{
#define RESOURCE_LENGTH 30
	struct sprite_bucket *next;
	char key[RESOURCE_LENGTH];

	struct {
		// If NULL, full.
		SDL_Rect *target;
		// ID of the texture in the internal texture manager.
		int texture_id;
	} sprite;
};
// 2D array
SDL_Texture **texture_container;

static int texture_add(const char* name, SDL_Renderer *renderer);
static SDL_Texture* texture_get(int id);
// textures_container resources are freed in destroy_textures()

static struct
{
#define SPRITE_ARR_SIZE 20
	struct sprite_bucket *data[SPRITE_ARR_SIZE];
} sprites_container;
static void sprite_add(const char* name, int texture_id, SDL_Rect* target);
static SDL_Texture *sprite_get(const char* name, SDL_Rect** target);
static void destroy_sprites(void);