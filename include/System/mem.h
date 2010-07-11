/*
 * VimE - the Vim Extensible
 */


#include <defs.h>
#include <nedmalloc.h>
#include <Support/hook.h>


/**
 * \file mem.h
 *
 * memory managment routines of VimE.
 */


#ifndef VIME_MEM_H
#define VIME_MEM_H


/**
 * VimE main allocation function.
 */
void *vime_malloc(size_t size);


/**
 * VimE main free function.
 */
void vime_free(void *mem);


/**
 * VimE main realloc function.
 */
void *vime_realloc(void *mem, size_t size);


/**
 * set the no-memory hook.
 */
struct hook *vime_set_nomem_hook(struct hook *nomem_hook);


/**
 * get the no-memory hook.
 */
struct hook *vime_get_nomem_hook(void);


/**
 * VimE fixed allocator routhine.
 */
typedef struct fixed_allocator *fixed_alloc_t;


/**
 * get a fixed allocator.
 */
fixed_alloc_t fixed_alloc(size_t size, size_t prealloc_size);


/**
 * free a fixed allocator.
 */
void fixed_free(fixed_alloc_t fa);


/**
 * acquire a block from a fixed allocator.
 */
void *fixed_acquire(fixed_alloc_t fa);


/**
 * release a block acquired from a fixed allocator.
 */
void fixed_release(fixed_alloc_t fa, void *mem);


#endif /* VIME_MEM_H */
