/*
 * VimE - the Vim Extensible
 */


#include <defs.h>

/**
 * \file hashtab.h
 *
 * hash table support for VimE.
 *
 * this file contains hashtable servce for VimE. hashtable is a import
 * data strction for implement option system. VimE uses a open
 * addresss method borrowed from Vim, with some modified with
 * interface.
 */

#ifndef VIME_HASHTAB_H
#define VIME_HASHTAB_H


/** hash value type. */
typedef unsigned long hash_t;


/** hash entry type.
 *
 * used to embed into the struction you want put into hashtable. note
 * that the first field of hash_entry must the hash value. so you can
 * use dereference operator to get the hash value of the entry.
 */
struct hash_entry
{
    hash_t hash; /**< the hash value of the key of the entry */
};


/**
 * get the entry of a hash item.
 *
 * \param ptr the pointer points to a hashitem.
 * \param type the type of the struction contains the hashitem.
 * \param field the field of hashitem in struction.
 * \return return the type struction which contains the hashitem
 *         pointered by ptr.
 */
#define HASH_ENTRY(ptr, type, field) \
        container_of((ptr), type, field)


/** hash function, used to make hash values.
 *
 * \remark notice that only use a hash entry pointer, you can not
 * compute the hash value (because you don't have hash key). so you
 * must use container_of() macro on the entry to get the container of
 * entry, and get the key from the container.
 */
typedef hash_t (*hash_function_t)(struct hash_entry const *entry);


/** compare function, used to compare two keys.
 *
 * \param entry is the entry in the hashtable, passed by the caller.
 * \param key is the key want to compare with the key in entry.
 *
 * \return 0 for key in the entry is equal with the key, or nonzero
 * otherwise.
 * 
 * \remark see #hash_function_t to know how to get key from the entry
 * pointer.
 */
typedef int (*hash_compare_t)
        (struct hash_entry const *entry, void *key);


/**
 * hash table struction.
 *
 * use hashtable_init() function to initialize a hashtable (mainly
 * alloc space for array). after use, call hashtable_drop() function
 * to destroy this table. use hashtable_clear() to delete all items
 * from this table, and use hashtable_get(), hashtable_set() to access
 * the items in the table.
 */
struct hashtable
{
    unsigned long   mask;   /**< mask used for hash value (nr of
                                 items in array is "mask" + 1). */
    unsigned long   used;   /**< number of items used. */
    unsigned long   filled; /**< number of items used + removed. */
    int             locked; /**< counter for hash_lock(). */
    int             error;  /**< when set growing failed, can't add more
                                 items before growing works. */

    struct hash_entry **array;  /**< points to the array. */
};


/** the default constructor of #hashtable */
#define HASHTABLE_INIT {0, 0, 0, 0, 0, NULL}


/** Initial size for a hashtable.
 *
 * Our items are relatively small and growing is expensive, thus use
 * 16 as a start.  Must be a power of 2.
 */
#define HT_INIT_SIZE 16

/**
 * a static fast hashtable.
 *
 * needn't to allocate space for array.
 */
struct static_hashtable
{
    /** the #hashtable of the static version hashtable. */
    struct hashtable hashtab;

    /** the pointer points to the hash table. */
    struct hash_entry *static_array[HT_INIT_SIZE];
};

/** convert a static_hashtable to hashtable. */
#define HASHTABLE(ptr) (&(ptr)->hashtab)

/** convert a hashtable to static hashtable. */
#define STATIC_HT_ENTRY(ptr) container_of(hashtab, struct static_hashtable, hashtab)

/**
 * this function initializes a #hashtable.
 *
 * if the hashtable#mask nonzero, use it for the size of hashtable.
 * if hashtable#mask nonzero and hashtable#array nonzero, not allocate
 * new space for array, just call hashtable_clear() to clear all item
 * in the table. so if you want use this function, you MUST clear the
 * hashtab parameter. or use hashtable_safe_init() function below.
 *
 * \param hashtab a uninitialized hash table
 * \return return OK for success, and FAIL for fail.
 *
 * \sa hashtable_safe_init
 */
#ifndef ENABLE_INLINE
struct hashtable *hashtable_init(struct hashtable *hashtab);
#else /* ENABLE_INLINE */

    INLINE struct hashtable*
hashtable_init(struct hashtable *hashtab)
{
    hashtab->mask = 0;
    hashtab->used = 0;
    hashtab->filled = 0;
    hashtab->locked = 0;
    hashtab->error = 0;
    hashtab->array = NULL;

    return hashtab;
}

#endif /* ENABLE_INLINE */


/**
 * this function initializes a #hashtable safely.
 *
 * this function just like hashtable_init, but don't check the value
 * in the hashtab. so you can use it on a unclear (I mean, memset())
 * struction. and if the hashtab hash memeory initialized from heap,
 * the memory will leap! use hashtable_init() instead in this case.
 *
 * \param hashtab a uninitialized hash table
 * \return return OK for success, and FAIL for fail.
 *
 * \sa hashtable_init
 */
struct hashtable *hashtable_safe_init(struct hashtable *hashtab);


/**
 * this function initializes a static #hashtable. use
 * static_hashtable#static_array to initialize the pointer in
 * #hashtable.
 *
 * \param hashtab a uninitialized hash table
 * \return return OK for success, and FAIL for fail.
 */
#ifndef ENABLE_INLINE
struct hashtable *hashtable_static_init(struct hashtable *hashtab);
#else /* ENABLE_INLINE */

    INLINE struct hashtable*
hashtable_static_init(struct hashtable *hashtab)
{
    hashtab->mask = HT_INIT_SIZE - 1;
    hashtab->used = 0;
    hashtab->filled = 0;
    hashtab->locked = 0;
    hashtab->error = 0;
    hashtab->array = STATIC_HT_ENTRY(hashtab)->static_array;

    return hashtab;
}

#endif /* ENABLE_INLINE */


/**
 * destroy a #hashtable.
 *
 * \param hashtab a hash table to destroyed.
 */
void hashtable_drop(struct hashtable *hashtab);


/**
 * clear a #hashtable.
 *
 * \param hashtab a hash table to cleared.
 * \return a value OK for success, and FAIL for fail.
 */
int hashtable_clear(struct hashtable *hashtab);


/**
 * compute a hash value for string.
 *
 * \param key the str used to compute.
 * \return hash the hash value of the string.
 */
hash_t default_string_hash(char const *key);


/**
 * compute a hash value for a integer.
 *
 * \param key the integer used to compute.
 * \return hash the hash value of the string.
 */
hash_t default_integer_hash(int key);


/**
 * get a element from hashtable.
 *
 * \param hashtab   a hash table contain element we need.
 * \param key       the key value of the element.
 * \param hash      the hash value of the key.
 * \param cmp_func  the compare function for compare entry and key.
 * \return a hashitem, or NULL if non found.
 *
 * \remark you must compute the hash value yourself before you use
 * this function.
 */
struct hash_entry *hashtable_get(struct hashtable *hashtab,
        void *key, hash_t hash, hash_compare_t cmp_func);


/**
 * set a element from hashtable.
 *
 * \param hashtab   a hash table we want to insert the value in.
 * \param value     the item used to insert.
 * \param key       the key value of the element.
 * \param hash      the hash value of the key.
 * \param cmp_func  the compare function for compare entry and key.
 *
 * \return the hash item if success, or NULL if fail.
 */
struct hash_entry *hashtable_set(struct hashtable *hashtab, struct hash_entry *value,
        void *key, hash_t hash, hash_compare_t cmp_func);


/**
 * remove a element from hashtable.
 */
struct hash_entry *hashtable_remove(struct hashtable *hashtab,
        void *key, hash_t hash, hash_compare_t cmp_func);


#endif /* VIME_HASHTAB_H */
