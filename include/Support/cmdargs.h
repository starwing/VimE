/*
 * VimE - the Vim Extensible
 */


#include <defs.h>
#include <Support/hook.h>
#include <Support/hashtab.h>


/**
 * \file cmdargs.h
 *
 * the command line argument parser routines for VimE.
 */


#ifndef VIME_CMDARGS_H
#define VIME_CMDARGS_H


/** long name length of cmdargs. */
#define CA_LONG_NAME_LEN 15


/**
 * the hook struction of a single cmdarg.
 */
struct cmdarg_hook
{
    char *long_name;
    char *document;
    int short_name;
    flag_t flags;
    struct hook action;
    struct hash_entry long_name_node;
    struct hash_entry short_name_node;
};


/** the default static constructor of #cmdarg_hook. */
#define CMDARG_HOOK_INIT \
    {NULL, NULL, '\0', 0, HOOK_INIT, HASH_ENTRY_INIT, HASH_ENTRY_INIT}

/** convert long name hash_entry to cmdarg_hook struction. */
#define CA_LONG_NAME_ENTRY(ptr) \
    HASH_ENTRY((ptr), struct cmdarg_hook, long_name_node)

/** convert short name hash_entry to cmdarg_hook struction. */
#define CA_SHORT_NAME_ENTRY(ptr) \
    HASH_ENTRY((ptr), struct cmdarg_hook, short_name_node)

/** default flags for #cmdarg_hook. */
#define CAH_FLAGS_DEFAULT 0x00000000

/** the cmdarg has a arguments. */
#define CAH_FLAGS_HAVE_ARGS 0x00000001


/**
 * the cmdarg parse table of all cmdargs.
 */
struct cmdarg_table
{
    struct hashtable long_name_ht;
    struct hashtable short_name_ht;

    struct hook unknow_arg_hook;
    struct hook invalid_arg_hook;
};

/** the default static constructor of #cmdarg_table. */
#define CMDARG_TABLE_INIT \
    {HASHTABLE_INIT, HASHTABLE_INIT, HOOK_INIT, HOOK_INIT}


/**
 * initialize a cmdarg table.
 */
#ifndef ENABLE_INLINE
struct cmdarg_table *cmdarg_table_init(struct cmdarg_table *table);
#else /* ENABLE_INLINE */

    INLINE struct cmdarg_table*
cmdarg_table_init(struct cmdarg_table *table)
{
    hashtable_init(&table->long_name_ht);
    hashtable_init(&table->short_name_ht);
    hook_init(&table->unknow_arg_hook);
    hook_init(&table->invalid_arg_hook);

    return table;
}

#endif /* ENABLE_INLINE */


/**
 * parse cmd args.
 * 
 * \param table the cmd argument hook table.
 * \param argc the count of arguments.
 * \param argv the arguments list.
 * \return the remain count of argument. and now argv only remain
 *         non-option arguments.
 * \remark this function will modify argv argument, if on which system
 *         that can't nodify argv pass from main, you must copy then
 *         into a wriatable buffer before call this function.
 */
#ifndef ENABLE_INLINE
int cmdarg_parse(struct cmdarg_table *table, int argc, char **argv)
#else /* ENABLE_INLINE */

    INTERNAL char **
process_shortarg(struct cmdarg_table *table, char **iter)
{
    int i;
    char *cur_arg = *iter;

    for (i = 0; iter[i] != '\0'; ++i)
    {
        struct cmdarg_hook *ca = NULL;
        char *arg = NULL;
        hash_t hash = default_integer_hash(cur_arg[1]);

        struct hash_entry *entry = hashtable_get(&table->short_name_ht, 
                &cur_arg[1], hash, (hash_compare_t)strcmp);

        if (entry == NULL)
        {
            hook_call(&table->unknow_arg_hook, HOOK_DEFAULT, cur_arg);
            continue;
        }

        if (cur_arg[2] != '\0')
            arg = &cur_arg[2];
        else if (iter[1] != NULL && iter[1][0] != '-')
            arg = *++iter;

        ca = CA_SHORT_NAME_ENTRY(entry);
        if (ca->flags & CAH_FLAGS_HAVE_ARGS != 0 && arg == NULL)
        {
            hook_call(&table->invalid_arg_hook, HOOK_DEFAULT, cur_arg);
            continue;
        }

        hook_call(&ca->action, HOOK_DEFAULT, arg);

        if (ca->flags & CAH_FLAGS_HAVE_ARGS)
            break;
    }

    return iter;
}


    INTERNAL char**
process_longarg(struct cmdarg_table *table, char **iter)
{
    struct cmdarg_hook *ca = NULL;
    char **new_iter = iter;
    char *cur_arg = *iter;
    char *arg = strchr(&cur_arg[2], '=');

    if ((arg = strchr(&cur_arg[2], '=')) != NULL)
        *arg++ = '\0';
    else if (iter[1][0] != '-')
        arg = *++new_iter;

    struct hash_entry *entry = hashtable_get(&table->long_name_ht, 
            &cur_arg[2], default_string_hash(&cur_arg[2]), (hash_compare_t)strcmp);

    if (entry == NULL)
    {
        hook_call(&table->unknow_arg_hook, HOOK_DEFAULT, cur_arg);
        return iter;
    }

    ca = CA_LONG_NAME_ENTRY(entry);
    if (ca->flags & CAH_FLAGS_HAVE_ARGS != 0 && arg == NULL
        || ca->flags & CAH_FLAGS_HAVE_ARGS == 0 && arg != NULL && arg != *iter)
    {
        hook_call(&table->invalid_arg_hook, HOOK_DEFAULT, cur_arg);
        return iter;
    }

    hook_call(&ca->action, HOOK_DEFAULT, arg);

    return new_iter;
}


    INLINE int
cmdarg_parse(struct cmdarg_table *table, int argc, char **argv)
{
    char **iter, **fname_arg = &argv[0];

    for (iter = argv; iter != NULL; ++iter)
    {
        char *cur_arg = *iter;
        if (cur_arg[0] == '-')
        {
            if (cur_arg[1] == '-')
                iter = process_longarg(table, iter);
            else
                iter = process_shortarg(table, iter);
        }
        else
            *fname_arg++ = cur_arg;
    }

    *fname_arg = NULL;
    return fname_arg - argv;
}

#endif /* ENABLE_INLINE */


#endif /* VIME_CMDARGS_H */
