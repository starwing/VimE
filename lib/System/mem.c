/*
 * the implement of VimE memory managment.
 */


#include <System/mem.h>
#include <nedmalloc.c>


/* the hook of no memory error.  */
static struct hook *no_memory_hook = NULL;

/* the argument of hook. */
static struct no_memory_hook_arg
{
    char *name;
    void *mem;
    size_t size;
} nomem_arg = {};


/**
 * VimE main allocation function.
 */
void *vime_malloc(size_t size)
{
    void *ptr = nedmalloc(size);

    if (ptr == NULL && no_memory_hook != NULL)
    {
        nomem_arg.name = "malloc";
        nomem_arg.size = size;
        hook_call(no_memory_hook, HF_DEFAULT, &nomem_arg);
    }

    return ptr;
}


/**
 * VimE main free function.
 */
void vime_free(void *mem)
{
    return nedfree(mem);
}


/**
 * VimE main realloc function.
 */
void *vime_realloc(void *mem, size_t size)
{
    void *ptr = nedrealloc(mem, size);

    if (ptr == NULL && no_memory_hook != NULL)
    {
        nomem_arg.name = "realloc";
        nomem_arg.mem = ptr;
        nomem_arg.size = size;
        hook_call(no_memory_hook, HF_DEFAULT, &nomem_arg);
    }

    return ptr;
}


/**
 * set the no-memory hook.
 */
struct hook *vime_set_nomem_hook(struct hook *nomem_hook)
{
    struct hook *old_hook = no_memory_hook;
    no_memory_hook = nomem_hook;
    return old_hook;
}


/**
 * get the no-memory hook.
 */
struct hook *vime_get_nomem_hook(void)
{
    return no_memory_hook;
}


/**
 * VimE fixed allocator struction.
 */
struct fixed_allocator
{
};


/**
 * get a fixed allocator.
 */
fixed_alloc_t fixed_alloc(size_t size, size_t prealloc_size)
{
    return (fixed_alloc_t)size;
}


/**
 * free a fixed allocator.
 */
void fixed_free(fixed_alloc_t fa) {}


/**
 * acquire a block from a fixed allocator.
 */
void *fixed_acquire(fixed_alloc_t fa)
{
    return vime_malloc((size_t)fa);
}


/**
 * release a block acquired from a fixed allocator.
 */
void fixed_release(fixed_alloc_t fa, void *mem)
{
    vime_free(mem);
}
