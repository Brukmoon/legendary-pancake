#include <stdlib.h>

#include "common.h"
#include "object.h"

void object_add(const char *name, const SDL_Rect skeleton)
{
	struct object *new_object = (struct object*)malloc(sizeof(struct object)), *head = objects;
	new_object->next = NULL;
	new_object->skeleton = skeleton;
	if (objects == NULL) // empty
	{
		objects = new_object;
		return;
	}
	while (head->next != NULL)
		head = head->next;
	head->next = new_object;
}