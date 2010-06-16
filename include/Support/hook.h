/*
 * VimE - the Vim Extensible
 */


#include <defs.h>
#define VIME_ONLY_PROTOTYPE
#include <Support/list.h>
#undef VIME_ONLY_PROTOTYPE


/**
 * \file hook.h
 *
 * the hook routines for VimE.
 *
 * hook is a function list waiting to call. you can add, remove,
 * modify  function in this list, when needed, you can use hook_call()
 * to call all function in list.
 *
 * if a function return a value OK, then the list will break, unless
 * pass HOOK_FORCE flag to hook_call, or all the function return FAIL.
 */


#ifndef VIME_HOOK_H
#define VIME_HOOK_H


/**
 * hook entry struction.
 */
struct hook_entry
{
    /** the list node. */
    struct list_entry node;

    /** the hook function. */
    int (*hook_func)(struct hook_entry *self, void *args);
};

/** the default static constructor of #hook_entry. */
#define HOOK_ENTRY_INIT {LIST_ENTRY_INIT, NULL}

/** get the hook struction from list node. */
#define HOOK_ENTRY(ptr) LIST_ENTRY((ptr), struct hook_entry, node)

/**
 * the return value for hook function.
 *
 * if return value is negtive and not pass HOOK_FORCE to hook_init()
 * function, the remain hooks will ignore.
 */
#define HOOK_BREAK  -1


/**
 * hook struction.
 */
struct hook
{
    struct list_entry *hook_list;
};

/** the default static constructor of #hook_entry. */
#define HOOK_INIT {NULL}


/** the default flag. */
#define HOOK_DEFAULT 0x00000000

/** the flags force call all function in hook list. */
#define HOOK_FORCE 0x00000001

/**
 * call the hook.
 */
#if !defined(ENABLE_INLINE) || defined(VIME_ONLY_PROTOTYPE)
int hook_call(struct hook *hook,  flag_t flags, void *args);
#else /* !defined(ENABLE_INLINE) || defined(VIME_ONLY_PROTOTYPE) */

    INLINE int
hook_call(struct hook *hook, flag_t flags, void *args)
{
    int retv = OK;
    struct list_entry *iter, *next;

    if (hook->hook_list == NULL)
        return retv;

    list_for_each_safe(iter, next, hook->hook_list)
    {
        struct hook_entry *entry = HOOK_ENTRY(iter);

        if ((retv = entry->hook_func(entry, args)) < 0
                && flags & HOOK_FORCE == 0)
            break;
    }

    return retv;
}

#endif /* !defined(ENABLE_INLINE) || defined(VIME_ONLY_PROTOTYPE) */


#endif /* VIME_HOOK_H */
