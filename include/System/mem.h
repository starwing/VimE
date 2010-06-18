/*
 * VimE - the Vim Extensible
 */


#include <defs.h>
#include <Support/hook.h>


/**
 * \file mem.h
 *
 * memory managment routines of VimE.
 */


#ifndef VIME_MEM_H
#define VIME_MEM_H


void *vime_malloc(size_t size);
void vime_free(void *mem);
void *vime_realloc(void *mem, size_t size);
struct hook *vime_set_nomem_hook(struct hook *nomem_hook);
struct hook *vime_get_nomem_hook(void);


typedef struct fixed_allocator *fixed_alloc_t;

fixed_alloc_t fixed_alloc(size_t size, size_t prealloc_size);
void fixed_free(fixed_alloc_t fa);
void *fixed_acquire(fixed_alloc_t fa);
void fixed_release(fixed_alloc_t fa, void *mem);


#endif /* VIME_MEM_H */
