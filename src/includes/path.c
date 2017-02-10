
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "common.h"
#include "path.h"
#include "level.h"

// child index
#define LEFT_ELEMENT(i) 2*i + 1
#define RIGHT_ELEMENT(i) 2*i + 2
// parent index
#define PARENT(i) (i/2)

typedef vec2 node;

struct pf_node
{
	bool discovered;
	int g_score, f_score;
	node parent;
};

static void node_swap(node* one, node* two)
{
	node temp = *one;
	*one = *two;
	*two = temp;
}

// heap
struct open_set
{
	int size;
	node* e;
};

static bool open_set_is_empty(struct open_set const *h)
{
	return !h->size;
}

// initialize a min-heap. h should be NULL
static void open_set_init(struct open_set** h)
{
	*h = malloc(sizeof(struct open_set));
	if (!*h)
	{
		ERROR("Not enough memory!");
		return;
	}
	(*h)->size = 0;
	(*h)->e = NULL;
}

static void open_set_push(struct open_set* h, node const data, struct pf_node **node_grid)
{
	INFO("Pushing %d;%d", data.x, data.y);
	if (h->size)
		h->e = realloc(h->e, (h->size + 1)*sizeof(node));
	// first push
	else h->e = malloc(sizeof(node));
	int i = 0;
	// find position where to push it (must meet heap req)
	for (i = h->size++; i && node_grid[data.y][data.x].f_score < node_grid[h->e[PARENT(i)].y][h->e[PARENT(i)].x].f_score; i = PARENT(i))
		h->e[i] = h->e[PARENT(i)];
	h->e[i] = data;
}

static void open_set_destroy(struct open_set* h)
{
	free(h->e);
	free(h);
}

// fix heap
static void heapify(struct open_set *h, int i, struct pf_node const **node_grid) {
	// is left child smaller than current?
	int smallest = (LEFT_ELEMENT(i) < h->size && node_grid[h->e[LEFT_ELEMENT(i)].y][h->e[LEFT_ELEMENT(i)].x].f_score < node_grid[h->e[i].y][h->e[i].x].f_score) ? LEFT_ELEMENT(i) : i;
	// is right child smaller than current?
	if (RIGHT_ELEMENT(i) < h->size && node_grid[h->e[RIGHT_ELEMENT(i)].y][h->e[RIGHT_ELEMENT(i)].x].f_score < node_grid[h->e[smallest].y][h->e[smallest].x].f_score)
		smallest = RIGHT_ELEMENT(i);
	// has the smallest element changed?
	if (smallest != i) {
		node_swap(&(h->e[i]), &(h->e[smallest]));
		heapify(h, smallest, node_grid);
	}
}

// returns the popped value
node open_set_pop(struct open_set *h, struct pf_node **node_grid) {
	node ret = { -1, -1 };
	if (h->size) {
		// select smallest val
		ret = h->e[0];
		// pop it, e.g. replace it with last value + shrink and heapify
		h->e[0] = h->e[--(h->size)];
		h->e = realloc(h->e, h->size * sizeof(node));
		heapify(h, 0, node_grid);
	}
	// heap is empty
	else free(h->e);
	return ret;
}

// heuristic distance between two nodes
static int estimate_distance(node const* start, node const* goal);
static void reconstruct_path(const node* current, struct pf_node const** node_grid, struct position** path);

void path_find(vec2 start, vec2 goal, struct position** path)
{
	struct pf_node **node_grid = malloc(sizeof(struct pf_node*)*g_level->tile_map.height);
	bool success = false;
	for (int i = 0; i < g_level->tile_map.height; ++i)
	{
		node_grid[i] = malloc(sizeof(struct pf_node)*g_level->tile_map.width);
		for (int j = 0; j < g_level->tile_map.width; ++j)
		{
			node_grid[i][j].discovered = false;
			node_grid[i][j].f_score = INT_MAX;
			node_grid[i][j].g_score = INT_MAX;
		}
	}
	struct open_set* open_set = NULL;
	open_set_init(&open_set);
	open_set_push(open_set, start, node_grid);
	node_grid[start.y][start.x].parent = (node) { -1, -1 };
	node_grid[start.y][start.x].discovered = true;
	node_grid[start.y][start.x].g_score = 0;
	node_grid[start.y][start.x].f_score = estimate_distance(&start, &goal);
	const int p_mod[8] = { -1, 0, 1, 0, 0, -1, 0, 1 };
	while (!open_set_is_empty(open_set))
	{
		node current = open_set_pop(open_set, node_grid);
		if (current.x == goal.x && current.y == goal.y)
		{
			success = true;
			break;
		}
		node_grid[current.y][current.x].discovered = true;
		if (g_level->tile_map.map[TMAP_COLLISION_LAYER][current.y][current.x] == 1)
			continue;
		// handle neighbors
		for (int i = 0; i < 8; i += 2)
		{
			const node neighbor = { current.x + p_mod[i],current.y + p_mod[i + 1] };
			if (neighbor.x >= 0 && neighbor.x < g_level->tile_map.width && neighbor.y >= 0 && neighbor.y < g_level->tile_map.height)
			{
				// neighbor already processed
				if (node_grid[neighbor.y][neighbor.x].discovered)
					continue;
				open_set_push(open_set, neighbor, node_grid);
				int tentative_gscore = node_grid[neighbor.y][neighbor.x].g_score + (p_mod[i + 1] == 1 ? 2 : 1);
				if (tentative_gscore >= node_grid[neighbor.y][neighbor.x].g_score)
					continue;
				node_grid[neighbor.y][neighbor.x].parent = current;
				node_grid[neighbor.y][neighbor.x].g_score = tentative_gscore;
				node_grid[neighbor.y][neighbor.x].f_score = tentative_gscore + estimate_distance(&neighbor, &goal);
			}
		}
	}
	if (success)
		reconstruct_path(&goal, node_grid, path);
	else
		*path = NULL;
	open_set_destroy(open_set);
	for (int i = 0; i < g_level->tile_map.height; ++i)
		free(node_grid[i]);
	free(node_grid);
}

static int estimate_distance(node const* start, node const* goal)
{
	return abs(start->x - goal->x) + abs(start->y - goal->y);
}

static void reconstruct_path(const node* current, struct pf_node const** node_grid, struct position** path)
{
	// root
	(*path) = malloc(sizeof(struct position));
	(*path)->next = NULL;
	(*path)->pos = *current;
	current = &node_grid[current->y][current->x].parent;
	struct position* iter = *path;
	while (current->x != -1 && current->y != -1)
	{
		iter->next = malloc(sizeof(struct position));
		iter = iter->next;
		iter->pos = *current;
		current = &node_grid[current->y][current->x].parent;
	}
	iter->next = NULL;
	iter = *path;
	while (iter)
	{
		INFO("%d,%d", iter->pos.x, iter->pos.y);
		iter = iter->next;
	}
}

void path_destroy(struct position** path)
{
	struct position* temp = NULL;
	while (*path)
	{
		temp = *path;
		*path = (*path)->next;
		free(temp);
	}
	*path = NULL;
}