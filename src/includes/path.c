
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "common.h"
#include "path.h"
#include "level.h"

// child index
#define LEFT_ELEMENT(i) (2*i + 1)
#define RIGHT_ELEMENT(i) (LEFT_ELEMENT(i) + 1)
// parent index
#define PARENT(i) ((i-1)/2)

typedef vec2 node;

struct pf_node
{
	bool discovered;
	// start to node
	int g_score;
	// start to goal through the node
	int f_score;
	// came from?
	node parent;
};

static void node_swap(node* const one, node* const two);
static bool node_equal(node const* one, node const* two);

static void node_swap(node* const one, node* const two)
{
	node temp = *one;
	*one = *two;
	*two = temp;
}

static bool node_equal(node const* one, node const* two)
{
	return (one->x == two->x && one->y == two->y);
}

// heap
struct open_set
{
	int size;
	node* e;
};

static void open_set_init(struct open_set** h);
static void open_set_destroy(struct open_set* h);
static void open_set_push(struct open_set* h, node const data, struct pf_node const **node_grid);
static node open_set_pop(struct open_set *h, struct pf_node **node_grid);
static bool open_set_contains(struct open_set* h, node const* current);
static bool open_set_is_empty(struct open_set const *h);
// make the heap meet its requirements
static void heapify(struct open_set *h, int i, struct pf_node const **node_grid);
// bubble the value at position up
static void bubble_up(struct open_set* h, int position, struct pf_node const **node_grid);

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
	// empty
	(*h)->size = 0;
	(*h)->e = NULL;
}

static void heapify(struct open_set *h, int i, struct pf_node const **node_grid) 
{
	// is left child smaller than current?
	int smallest = i;
	if (LEFT_ELEMENT(i) < h->size && (node_grid[h->e[LEFT_ELEMENT(i)].y][h->e[LEFT_ELEMENT(i)].x].f_score < node_grid[h->e[smallest].y][h->e[smallest].x].f_score))
		smallest = LEFT_ELEMENT(i);
	// is right child smaller than current?
	if (RIGHT_ELEMENT(i) < h->size && (node_grid[h->e[RIGHT_ELEMENT(i)].y][h->e[RIGHT_ELEMENT(i)].x].f_score < node_grid[h->e[smallest].y][h->e[smallest].x].f_score))
		smallest = RIGHT_ELEMENT(i);
	// has the smallest element changed?
	if (smallest != i) 
	{
		node_swap(&(h->e[i]), &(h->e[smallest]));
		heapify(h, smallest, node_grid);
	}
}

static void bubble_up(struct open_set* h, int position, struct pf_node const **node_grid)
{
	if (position != 0)
	{
		if (node_grid[h->e[position].y][h->e[position].x].f_score < node_grid[h->e[PARENT(position)].y][h->e[PARENT(position)].x].f_score)
		{
			node_swap(&h->e[position], &h->e[PARENT(position)]);
			bubble_up(h, PARENT(position), node_grid);
		}
	}
}

static void open_set_push(struct open_set* h, node const data, struct pf_node const **node_grid)
{
	if (h->size)
		h->e = realloc(h->e, (h->size+1)*sizeof(node));
	// first push
	else h->e = malloc(sizeof(node));
	// put it in last position
	h->e[h->size++] = data;
	// find correct position
	bubble_up(h, h->size - 1, node_grid);
}

static void open_set_destroy(struct open_set* h)
{
	free(h->e);
	free(h);
}

// returns the popped value
static node open_set_pop(struct open_set *h, struct pf_node **node_grid) 
{
	node ret = { -1, -1 };
	if (h->size) 
	{
		// select smallest val
		ret = h->e[0];
		// pop it, e.g. replace it with last value + shrink and heapify
		h->e[0] = h->e[--h->size];
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
	if (!*node_grid)
	{
		ERROR("Not enough memory!");
		return;
	}
	// build the grid
	for (int i = 0; i < g_level->tile_map.height; ++i)
	{
		node_grid[i] = malloc(sizeof(struct pf_node)*g_level->tile_map.width);
		if (!node_grid[i])
		{
			ERROR("Not enough memory!");
			return;
		}
		for (int j = 0; j < g_level->tile_map.width; ++j)
		{
			node_grid[i][j].discovered = false;
			// inf
			node_grid[i][j].f_score = INT_MAX;
			node_grid[i][j].g_score = INT_MAX;
		}
	}
	bool success = false;

	struct open_set* open_set = NULL;
	open_set_init(&open_set);
	// start node
	open_set_push(open_set, (node) { start.x, start.y}, node_grid);
	node_grid[start.y][start.x].parent = (node) { -1, -1 };
	node_grid[start.y][start.x].discovered = true;
	node_grid[start.y][start.x].g_score = 0;
	node_grid[start.y][start.x].f_score = estimate_distance(&start, &goal);
	// left, right, down, up - for neighbors
	const int p_mod[8] = { -1, 0, 1, 0, 0, -1, 0, 1 };
	while (!open_set_is_empty(open_set))
	{
		node current = open_set_pop(open_set, node_grid);
		if (node_equal(&current, &goal))
		{
			success = true;
			break;
		}
		node_grid[current.y][current.x].discovered = true;
		// handle neighbors
		for (int i = 0; i < 8; i += 2)
		{
			const node neighbor = { current.x + p_mod[i],current.y + p_mod[i + 1] };
			if (neighbor.x >= 0 && neighbor.x < g_level->tile_map.width && neighbor.y >= 0 && neighbor.y < g_level->tile_map.height)
			{
				// neighbor already processed
				if (node_grid[neighbor.y][neighbor.x].discovered || g_level->tile_map.map[TMAP_COLLISION_LAYER][neighbor.y][neighbor.x] == 1)
					continue;
				int tentative_gscore = node_grid[current.y][current.x].g_score + 1;
				bool contains = open_set_contains(open_set, &neighbor);
				if (!contains)
					;
				if (tentative_gscore >= node_grid[neighbor.y][neighbor.x].g_score)
					continue;
				// best path until now
				node_grid[neighbor.y][neighbor.x].parent = current;
				node_grid[neighbor.y][neighbor.x].g_score = tentative_gscore;
				node_grid[neighbor.y][neighbor.x].f_score = tentative_gscore + estimate_distance(&neighbor, &goal);
				if (!contains)
					open_set_push(open_set, neighbor, node_grid);
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
	INFO("cost: %d", node_grid[current->y][current->x].g_score);
	// root
	(*path) = malloc(sizeof(struct position));
	(*path)->next = NULL;
	(*path)->pos.x = current->x;
	(*path)->pos.y = current->y;
	current = &node_grid[current->y][current->x].parent;
	struct position* iter = *path;
	while (current->x != -1 && current->y != -1)
	{
		iter->next = malloc(sizeof(struct position));
		iter = iter->next;
		iter->pos.x = current->x;
		iter->pos.y = current->y;
		current = &node_grid[current->y][current->x].parent;
	}
	iter->next = NULL;
	iter = *path;
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

// linear search through the heap
static bool open_set_contains(struct open_set* h, node const* current)
{
	for (int i = 0; i < h->size; ++i)
		if (node_equal(&h->e[i], current))
			return true;
	return false;
}