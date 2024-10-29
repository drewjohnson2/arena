#include "../include/arena.h"
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>

static void * _map_memory(size_t capacity);

Arena a_new(size_t capacity) 
{
	void *mem = _map_memory(capacity);
	Region *region = malloc(sizeof(Region));

	region->data = mem;
	region->size = 0;
	region->capacity = capacity;
	region->next = NULL;

	Arena arena = 
	{
		.region = region,
		.regionsAllocated = 1
	};

	return arena;
}

void * a_alloc(Arena *arena, size_t size, size_t alignment)
{
	// align size for desired type.
	size = (size + alignment - 1) & ~(alignment - 1);

	Region *region = arena->region;

	assert(size < region->capacity);

	while (region->next != NULL) region = region->next;

	if (region->size + size >= region->capacity)
	{
		void *mem = _map_memory(region->capacity);
		Region *newRegion = malloc(sizeof(Region));
		
		newRegion->data = mem;
		newRegion->size = 0;
		newRegion->capacity = region->capacity;
		newRegion->next = NULL;

		region->next = newRegion;
		region = newRegion;

		arena->regionsAllocated++;
	}
	
	// align the pointer to the next address that satisfies the alignment constraint
	uintptr_t current = (uintptr_t)region->data + region->size;
	uintptr_t aligned = (current + alignment - 1) & ~(alignment - 1);

	region->size = aligned - (uintptr_t)region->data + size;

	return (void *)aligned;
}

int a_free(Arena *arena) 
{
	Region *head = arena->region;
	Region *tmp;

	while(head)
	{
		tmp = head;
		head = head->next;

		int res = munmap(tmp->data, tmp->capacity);

		if (res == -1)
		{
			arena_err("Unmap failed.");
			return res;
		}

		free(tmp);
	}

	return 0;
}

char * a_strdup(Arena *arena, const char *str)
{
	size_t n = strlen(str);
	char *dup = (char *)a_alloc(arena, n + 1, _Alignof(char *));

	memcpy(dup, str, n);

	dup[n] = '\0';

	return dup;
}

static void * _map_memory(size_t capacity)
{
	void *mem = mmap(NULL, capacity, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

	assert(mem != MAP_FAILED);

	int protect = mprotect(mem, capacity, PROT_READ | PROT_WRITE);

	assert(protect != -1);

	return mem;
}
