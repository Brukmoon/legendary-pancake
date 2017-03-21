#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "common.h"
#include "path.h"
#include "level.h"
#include "collision.h"

#define MAX_JUMP 2

// child index
#define LEFT_ELEMENT(i) (2*i + 1)
#define RIGHT_ELEMENT(i) (LEFT_ELEMENT(i) + 1)
// parent index
#define PARENT(i) ((i-1)/2)

typedef vec3 node;
// deep copy
static void node_swap(node* const one, node* const two);
// two nodes are equal when their ![x;y]! coords match
static bool node_equal(node const* one, node const* two);

struct pf_node
{
	// start to node
	int g_score;
	// start to goal through the node
	int f_score;
	// came from
	node parent;
	int jump;
};

struct pf_node_l
{
	// jump dimension
	struct pf_node* z;
	size_t z_count;
};

// heap
struct open_set
{
	int size;
	node* e;
};

static void open_set_init(struct open_set** h);
static void open_set_destroy(struct open_set* h);
static void open_set_push(struct open_set* h, node const data, struct pf_node_l const **node_grid);
static node open_set_pop(struct open_set *h, struct pf_node_l **node_grid);
static bool open_set_contains(struct open_set* h, node const* current);
static bool open_set_is_empty(struct open_set const *h);
// make the heap meet its requirements
static void heapify(struct open_set *h, int i, struct pf_node_l const **node_grid);
// bubble the value at position up
static void bubble_up(struct open_set* h, int position, struct pf_node_l const **node_grid);

// heuristic distance between two nodes
static int estimate_distance(vec2 const* start, vec2 const* goal);
// reconstruct the path
static void reconstruct_path(node* current, struct pf_node_l const** node_grid, struct waypoint** path);

static void node_swap(node* const one, node* const two)
{
	node temp = *one;
	*one = *two;
	*two = temp;
}

// we only match x and y coords, ignore z
static bool node_equal(node const* one, node const* two)
{
	return (one->x == two->x && one->y == two->y);
}

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

static void heapify(struct open_set *h, int i, struct pf_node_l const **node_grid) 
{
	// is left child smaller than current?
	int smallest = i;
	if (LEFT_ELEMENT(i) < h->size && (node_grid[h->e[LEFT_ELEMENT(i)].y][h->e[LEFT_ELEMENT(i)].x].z[h->e[LEFT_ELEMENT(i)].z].f_score < node_grid[h->e[smallest].y][h->e[smallest].x].z[h->e[smallest].z].f_score))
		smallest = LEFT_ELEMENT(i);
	// is right child smaller than current?
	// Check overflow && match right element f_score with smallest f_score.
	if (RIGHT_ELEMENT(i) < h->size && (node_grid[h->e[RIGHT_ELEMENT(i)].y][h->e[RIGHT_ELEMENT(i)].x].z[h->e[RIGHT_ELEMENT(i)].z].f_score < node_grid[h->e[smallest].y][h->e[smallest].x].z[h->e[smallest].z].f_score))
		smallest = RIGHT_ELEMENT(i);
	// has the smallest element changed?
	if (smallest != i) 
	{
		node_swap(&(h->e[i]), &(h->e[smallest]));
		heapify(h, smallest, node_grid);
	}
}

static void bubble_up(struct open_set* h, int position, struct pf_node_l const **node_grid)
{
	if (position != 0)
	{
		if (node_grid[h->e[position].y][h->e[position].x].z[h->e[position].z].f_score < node_grid[h->e[PARENT(position)].y][h->e[PARENT(position)].x].z[h->e[PARENT(position)].z].f_score)
		{
			node_swap(&h->e[position], &h->e[PARENT(position)]);
			bubble_up(h, PARENT(position), node_grid);
		}
	}
}

static void open_set_push(struct open_set* h, node const data, struct pf_node_l const **node_grid)
{
	if (h->size)
	{
		h->e = realloc(h->e, (h->size + 1)*sizeof(node));
	}
	// first push
	else h->e = malloc(sizeof(node));
	if (!h->e)
	{
		ERROR("Not enough memory!");
		return;
	}
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
static node open_set_pop(struct open_set *h, struct pf_node_l **node_grid) 
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

void path_find(vec2 start, vec2 goal, struct waypoint** path)
{
	// TODO: Perhaps move the grid to global scope and perform a heap cleanup via a stack.
	// allocate array of pointers
	struct pf_node_l **node_grid = malloc(sizeof(struct pf_node_l*)*g_level->tile_map.height);
	if (!*node_grid)
	{
		ERROR("Not enough memory!");
		return;
	}
	// build the grid
	for (int i = 0; i < g_level->tile_map.height; ++i)
	{
		node_grid[i] = malloc(sizeof(struct pf_node_l)*g_level->tile_map.width);
		if (!node_grid[i])
		{
			ERROR("Not enough memory!");
			return;
		}
		// zero-initialize to save precious memory
		for (int j = 0; j < g_level->tile_map.width; ++j)
		{
			node_grid[i][j].z = NULL;
			node_grid[i][j].z_count = 0;
		}
	}
	// path found
	bool success = false;

	struct open_set* open_set = NULL;
	open_set_init(&open_set);
	// start node
	open_set_push(open_set, (node) { start.x, start.y, 0 }, node_grid);
	node_grid[start.y][start.x].z = realloc(node_grid[start.y][start.x].z, (++node_grid[start.y][start.x].z_count)*sizeof(struct pf_node));
	if (!node_grid[start.y][start.x].z)
	{
		ERROR("Not enough memory!");
		return;
	}
	// dummy parent node
	node_grid[start.y][start.x].z[0].parent = (vec3) { -1, -1, -1 };
	// start to start, zero cost
	node_grid[start.y][start.x].z[0].g_score = 0;
	// at the beginning, the jump value is zero - let's not overcomplicate things
	node_grid[start.y][start.x].z[0].jump = 0;
	node_grid[start.y][start.x].z[0].f_score = estimate_distance(&start, &goal);
	// left, right, down, up - for neighbors
	int const p_mod[8] = { -1, 0, 1, 0, 0, -1, 0, 1 };
	node goal_node = (node){ goal.x, goal.y, 0};
	while (!open_set_is_empty(open_set))
	{
		// pop node with lowest fscore
		node current = open_set_pop(open_set, node_grid);
		// TODO: Check if in closed list.
		if (node_equal(&current, &goal_node))
		{
			success = true;
			break;
		}
		// handle neighbors
		for (int i = 0; i < 8; i += 2)
		{
			// TODO: set z to correct value
			node neighbor = { current.x + p_mod[i],current.y + p_mod[i + 1], 0 };
			// check for map overflow
			// TODO: Move into a define.
			if (neighbor.x >= 0 && neighbor.x < g_level->tile_map.width && neighbor.y >= 0 && neighbor.y < g_level->tile_map.height)
			{
				// neighbor is a collision
				if (g_level->tile_map.map[TMAP_COLLISION_LAYER][neighbor.y][neighbor.x] == T_IMPASSABLE)
					continue;
				// TODO: Add ladder
				bool on_ground = false, on_ceiling = false,
					on_ladder = false;
				// one tile above
				if ((neighbor.y - 1) >= 0) // beware overflow
					on_ceiling = (g_level->tile_map.map[TMAP_COLLISION_LAYER][neighbor.y - 1][neighbor.x] == 1);
				// one tile below
				if(neighbor.y + 1 < g_level->tile_map.height)
					on_ground = (g_level->tile_map.map[TMAP_COLLISION_LAYER][neighbor.y + 1][neighbor.x] == TILE_COLLISION);
				on_ladder = (g_level->tile_map.map[TMAP_COLLISION_LAYER][neighbor.y][neighbor.x] == LADDER_COLLISION);
				// old jump value
				int jump = node_grid[current.y][current.x].z[current.z].jump;
				int new_jump = jump;
				// if on ground, no jump
				if (on_ground || on_ladder)
					new_jump = 0;
				// character hugs ceiling
				else if (on_ceiling)
				{
					// horizontal
					if (current.x != neighbor.x)
						new_jump = (2 * MAX_JUMP + 1) > (jump + 1) ? 2 * MAX_JUMP + 1 : jump + 1;
					else // vertical
						new_jump = (2 * MAX_JUMP) > (jump + 2) ? 2 * MAX_JUMP : jump + 2	;
				}
				
				// neither on ground nor at ceiling, new node is above
				else if (current.y < neighbor.y)
				{
					// first jump is costly
					if (jump < 2)
						new_jump = 3;
					// even
					else if (jump % 2 == 0)
						new_jump = jump + 2;
					else // odd
						new_jump = jump + 1;
				}
				// below
				else if (current.y > neighbor.y)
				{
					if (jump % 2 == 0)
						new_jump = new_jump = MAX_JUMP * 2 > (jump + 2) ? MAX_JUMP * 2 : jump + 2;
					else
						new_jump = new_jump = MAX_JUMP * 2 > (jump + 1) ? MAX_JUMP * 2 : jump + 1;
				}
				else if (!on_ground && current.x != neighbor.x)
					new_jump = jump + 1;
				if (jump % 2 != 0 && current.x != neighbor.x)
					continue;
				// max jumps and new node above the current one
				if (jump >= MAX_JUMP * 2 && current.y < neighbor.y)
					continue;
				if (new_jump >= MAX_JUMP * 2 + 6 && neighbor.x != current.x && (new_jump - (MAX_JUMP * 2 + 6)) % 8 != 3)
					continue;
				int tentative_gscore = node_grid[current.y][current.x].z[current.z].g_score + 1 + (int)new_jump/4;
				// skip?
				if (node_grid[neighbor.y][neighbor.x].z_count > 0)
				{
					int lowest_jump = INT_MAX;
					bool can_move_side = false;
					for (size_t j = 0; j < node_grid[neighbor.y][neighbor.x].z_count; ++j)
					{
						if (node_grid[neighbor.y][neighbor.x].z[j].jump < lowest_jump)
							lowest_jump = node_grid[neighbor.y][neighbor.x].z[j].jump;

						if (node_grid[neighbor.y][neighbor.x].z[j].jump % 2 == 0 && node_grid[neighbor.y][neighbor.x].z[j].jump < (MAX_JUMP * 2 + 6))
							can_move_side = true;
					}
					if (lowest_jump <= new_jump && (new_jump % 2 != 0 || new_jump >= MAX_JUMP * 2 + 6 || can_move_side))
						continue;
				}
				// init z
				bool z_found = false;
				for (size_t j = 0; j < node_grid[neighbor.y][neighbor.x].z_count; ++j)
				{
					if (node_grid[neighbor.y][neighbor.x].z[j].jump == new_jump)
					{
						neighbor.z = j;
						z_found = true;
						break;
					}
				}
				if (!z_found)
				{
					// allocate a new one and set to default vals
					node_grid[neighbor.y][neighbor.x].z = realloc(node_grid[neighbor.y][neighbor.x].z, ++node_grid[neighbor.y][neighbor.x].z_count*sizeof(struct pf_node));
					neighbor.z = node_grid[neighbor.y][neighbor.x].z_count - 1;

					node_grid[neighbor.y][neighbor.x].z[neighbor.z].g_score = INT_MAX;
					node_grid[neighbor.y][neighbor.x].z[neighbor.z].f_score = INT_MAX;
				}
				//if (tentative_gscore > node_grid[neighbor.y][neighbor.x].z[neighbor.z].g_score)
				//	continue;
				node_grid[neighbor.y][neighbor.x].z[neighbor.z].jump = new_jump;
				// best path until now
				node_grid[neighbor.y][neighbor.x].z[neighbor.z].parent.x = current.x;
				node_grid[neighbor.y][neighbor.x].z[neighbor.z].parent.y = current.y;
				node_grid[neighbor.y][neighbor.x].z[neighbor.z].parent.z = current.z;
				node_grid[neighbor.y][neighbor.x].z[neighbor.z].g_score = tentative_gscore;
				node_grid[neighbor.y][neighbor.x].z[neighbor.z].f_score = tentative_gscore + estimate_distance(&start, &goal);
				open_set_push(open_set, neighbor, node_grid);
			}
		}
	}
	if (success)
		reconstruct_path(&goal_node, node_grid, path);
	else
		*path = NULL;
	open_set_destroy(open_set);
	for (int i = 0; i < g_level->tile_map.height; ++i)
	{
		for (int j = 0; j < g_level->tile_map.width; ++j)
			free(node_grid[i][j].z);
		free(node_grid[i]);
	}
	free(node_grid);
}

static int estimate_distance(vec2 const* start, vec2 const* goal)
{
	return abs(start->x - goal->x) + abs(start->y - goal->y);
}

static void reconstruct_path(node* current, struct pf_node_l const** node_grid, struct waypoint** path)
{
	//INFO("cost: %d", node_grid[current->y][current->x].z[current->z].g_score);
	// root
	(*path) = malloc(sizeof(struct waypoint));
	(*path)->next = NULL;
	(*path)->prev = NULL;
	(*path)->pos.x = current->x;
	(*path)->pos.y = current->y;
	node *prev = current;
	current = &node_grid[current->y][current->x].z[current->z].parent;
	if (current->x == -1 && current->y == -1 && current->z == -1)
		return;
	node *next = &node_grid[current->y][current->x].z[current->z].parent;
	struct waypoint* iter = *path;
	while (current->x != -1 && current->y != -1)
	{
		next = &node_grid[current->y][current->x].z[current->z].parent;
		if ((next->y == -1 && next->x == -1 && next->z == -1) || // end node
			// about to jump
			(node_grid[next->y][next->x].z[next->z].jump != 0 && node_grid[current->y][current->x].z[current->z].jump == 0) ||
			// jumping
			(node_grid[current->y][current->x].z[current->z].jump == 3 && node_grid[prev->y][prev->x].z[prev->z].jump != 0) ||
			// jump end
			(node_grid[current->y][current->x].z[current->z].jump == 0 && node_grid[prev->y][prev->x].z[prev->z].jump != 0) ||
			// high point
		    (current->y > iter->pos.y && current->y > next->y) ||
			// changing direction
			(prev->x != next->x && prev->y != next->y) ||
			// around obstacle
			((g_level->tile_map.map[TMAP_COLLISION_LAYER][current->y][current->x - 1] == TILE_COLLISION || g_level->tile_map.map[TMAP_COLLISION_LAYER][current->y][current->x + 1] == TILE_COLLISION)
				&& current->y != iter->pos.y && current->x != iter->pos.x)
			)
		{
			iter->next = malloc(sizeof(struct waypoint));
			iter->next->prev = iter;
			iter = iter->next;
			iter->pos.x = current->x;
			iter->pos.y = current->y;
		}
		prev = current;
		current = &node_grid[current->y][current->x].z[current->z].parent;
	}
	iter->next = NULL;
}

void path_destroy(struct waypoint** path)
{
	struct waypoint* temp = NULL;
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