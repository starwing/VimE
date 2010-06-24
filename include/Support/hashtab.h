/*
 * VimE - the Vim Expand
 */


/**
 * @file
 *
 * hash table support for VimE.
 */

#ifndef __HASHTAB_C__
#define __HASHTAB_C__

/** hash value type */
typedef unsigned long hash_t;

/** hash function */
typedef hash_t (*hash_function_t)(void *);

/** compare function */
typedef int (*hash_compare_t)(void *, void *);


/**
 * a item used to implement hashitem.
 *
 *
 */
struct hashitem
{
    /** hash value of the hash item */
    hash_t hash;

    /** the key of hashitem */
    void *key;
};


/**
 * hash table struction.
 */
struct hashtable
{
    /** the pointer points to the hash table */
    struct hashitem **table;
};


/**
 *  the init function of hashtable.
 *
 * @param hashtab a uninitialized hash table
 * @return return OK for success, and FAIL for fail.
 */
int hashtable_init(struct hashtable *hashtab);


/**
 *  destroy a hashtable.
 *
 * @param hashtab a hash table to destroyed.
 */
void hashtable_drop(struct hashtable *hashtab);


/**
 *  clear a hash table
 *
 * @param hashtab a hash table to cleared.
 * @return a value OK for success, and FAIL for fail.
 */
int hashtable_clear(struct hashtable *hashtab);


/**
 *  get a element from hashtable.
 *
 * @param hashtab   a hash table contain element we need.
 * @param key       the key value of the element.
 * @return a hashitem, or NULL if non found.
 */
struct hashitem *hashtable_get(struct hashtable *hashtab, void *key);


/**
 *  get a element from hashtable, without compute a hash value.
 *
 * @param hashtab   a hash table contain element we need.
 * @param key       the key value of the element.
 * @param hash      the hash value of the key.
 * @return a hashitem, or NULL if non found.
 */
struct hashitem *hashtable_get_withhash(struct hashtable *hashtab, void *key, hash_t hash);


/**
 *  set a element from hashtable.
 *
 * @param hashtab   a hash table we want to insert the value in.
 * @param value     the item need to insert.
 * @return a hashitem, or NULL if non found.
 */
int hashtable_set(struct hashtable *hashtab, struct hashitem *value);


/**
 *  set a element from hashtable, without compute a hash value.
 *
 * @param hashtab   a hash table we want to insert the value in.
 * @param value     the item need to insert.
 * @return a hashitem, or NULL if non found.
 */
int hashtable_set_withhash(struct hashtable *hashtab, struct hashitem *value);


/**
 *  get the entry of a hash item.
 *
 * @param ptr the pointer points to a hashitem.
 * @param type the type of the struction contains the hashitem.
 * @param field the field of hashitem in struction.
 * @return return the type struction which contains the hashitem
 *         pointered by ptr.
 */
#define hash_entry(ptr, type, field) container_of(ptr, type, field)

#endif /* __HASHTAB_C__ */
