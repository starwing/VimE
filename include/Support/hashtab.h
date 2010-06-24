/*
 * VimE - the Vim Extensible
 */


/**
 * @file
 *
 * hash table support for VimE.
 *
 * this file contains hashtable servce for VimE. hashtable is a import
 * data strction for implement option system. VimE uses a open
 * addresss method borrowed from Vim, with some modified with
 * interface.
 */

#ifndef __HASHTAB_C__
#define __HASHTAB_C__


/** hash value type. */
typedef unsigned long hash_t;

/** hash function, used to make hash values. */
typedef hash_t (*hash_function_t)(void *key);

/** compare function, used to compare two keys.
 *
 * @return -1, 0 or 1 for left hand side key is less/equal/greater
 * than right hand side key.
 */
typedef int (*hash_compare_t)(void *lhs, void *rhs);


/**
 * a item used to implement hashitem.
 *
 * this item used to embeded to another structions. hashtables just
 * hold a table of pointers points to the hash item embeded in other
 * struction.  you can use #hash_entry macro to get the struction
 * contains the item from the item pointer.
 */
struct hash_item
{
    /** hash value of the hash item. */
    hash_t hash;

    /** the key of hashitem. */
    void *key;
};


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

    struct hash_item **array; /**< points to the array. */
};


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
    struct hash_item static_array[HT_INIT_SIZE];
};

/** convert a static_hashtable to hashtable. */
#define HASHTABLE(ptr) ((struct hashtable*)ptr)


/**
 * this function initializes a #hashtable.
 *
 * if the hashtable#mask nonzero, use it for the size of hashtable.
 * if hashtable#mask nonzero and hashtable#array nonzero, not allocate
 * new space for array, just call hashtable_clear() to clear all item
 * in the table. so if you want use this function, you MUST clear the
 * hashtab parameter. or use hashtable_safe_init() function below.
 *
 * @param hashtab a uninitialized hash table
 * @return return OK for success, and FAIL for fail.
 *
 * @sa hashtable_safe_init
 */
int hashtable_init(struct hashtable *hashtab);


/**
 * this function initializes a #hashtable safely.
 *
 * this function just like hashtable_init, but don't check the value
 * in the hashtab. so you can use it on a unclear (I mean, memset())
 * struction. and if the hashtab hash memeory initialized from heap,
 * the memory will leap! use hashtable_init() instead in this case.
 *
 * @param hashtab a uninitialized hash table
 * @return return OK for success, and FAIL for fail.
 *
 * @sa hashtable_init
 */
int hashtable_safe_init(struct hashtable *hashtab);


/**
 * this function initializes a static #hashtable. use
 * static_hashtable#static_array to initialize the pointer in
 * #hashtable.
 *
 * @param hashtab a uninitialized hash table
 * @return return OK for success, and FAIL for fail.
 */
int hashtable_static_init(struct static_hashtable *hashtab);


/**
 * destroy a #hashtable.
 *
 * @param hashtab a hash table to destroyed.
 */
void hashtable_drop(struct hashtable *hashtab);


/**
 * clear a #hashtable.
 *
 * @param hashtab a hash table to cleared.
 * @return a value OK for success, and FAIL for fail.
 */
int hashtable_clear(struct hashtable *hashtab);


/**
 * get a element from hashtable.
 *
 * @param hashtab   a hash table contain element we need.
 * @param key       the key value of the element.
 * @return a hashitem, or NULL if non found.
 */
struct hashitem *hashtable_get(struct hashtable *hashtab, void *key);


/**
 * get a element from hashtable, without compute a hash value.
 *
 * @param hashtab   a hash table contain element we need.
 * @param key       the key value of the element.
 * @param hash      the hash value of the key.
 * @return a hashitem, or NULL if non found.
 */
struct hashitem *hashtable_get_withhash(struct hashtable *hashtab,
        void *key, hash_t hash);


/**
 * set a element from hashtable.
 *
 * @param hashtab   a hash table we want to insert the value in.
 * @param value     the item need to insert.
 * @return a hashitem, or NULL if non found.
 */
int hashtable_set(struct hashtable *hashtab, struct hashitem *value);


/**
 * set a element from hashtable, without compute a hash value.
 *
 * @param hashtab   a hash table we want to insert the value in.
 * @param value     the item need to insert.
 * @return a hashitem, or NULL if non found.
 */
int hashtable_set_withhash(struct hashtable *hashtab, struct hashitem *value);


/**
 * get the entry of a hash item.
 *
 * @param ptr the pointer points to a hashitem.
 * @param type the type of the struction contains the hashitem.
 * @param field the field of hashitem in struction.
 * @return return the type struction which contains the hashitem
 *         pointered by ptr.
 */
#define hash_entry(ptr, type, field) container_of(ptr, type, field)

#endif /* __HASHTAB_C__ */
