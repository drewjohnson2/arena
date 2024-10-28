#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define arena_err(msg) \
	fprintf(stderr, "%s (%s:%d): %s\n", msg, __func__, __LINE__, strerror(errno))

typedef struct region 
{
	uintptr_t *data;
	size_t size;
	size_t capacity;
	struct region *next;
} Region;

typedef struct arena
{
	Region *region;
	size_t regionsAllocated; // for debugging
} Arena;

/*
 *	Creates a new arena.
 *
 *	Allocates with mmap.
 *
 *	---------------------------------------------
 *
 *	size_t		capacity	- The desired arena capacity.
*/	
Arena a_new(size_t capacity);

/*
 *	Allocates some memory in the current Region
 *	of the Arena and returns it to the user. Memory
 *	is mapped using mmap.
 *	---------------------------------------------
 *
 *	Arena		*arena		- The arena to allocate to.
 *	size_t		size		- The size of the type to allocate.
 *	size_t		alignment	- The alignment of the given type. (Use _Alignof())
 */
void * a_alloc(Arena *arena, size_t size, size_t alignment);

/*
 *	Frees memory mapped for the arena.
 *
 *	returns 0 on success and -1 on fail.
 *
 *	----------------------------------------------
 *
 *	Arena		*arena		- The arena to free
*/
int a_free(Arena *arena);

/*
 *	Maps memory for the string to be duplicated
 *	then uses memcpy to duplicate the given string.
 *
 *	----------------------------------------------
 *
 *	Arena		*a	- The arena to allocate to.
 *	const char	*str	- The string to be duplicated.
*/
char * a_strdup(Arena *a, const char *str);

#endif
