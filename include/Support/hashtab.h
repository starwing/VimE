/* hashtab.h
 * the Hash table service.
 */

#ifndef __HASHTAB_C__
#define __HASHTAB_C__

/* hash value type */
typedef unsigned long hash_t;

/* hash function */
typedef hash_t (*hash_function_t)(void *);

/* compare function */
typedef int (*hash_compare_t)(void *, void *);


/* a item used to implement hashitem.
 *
 *
 */

struct hash_item
{
    struct list_head list;
};

struct hash_table
{

#ifdef VIME_CONFIG_HASHTABLE_DEBUG
#endif /* VIME_CONFIG_HASHTABLE_DEBUG */
};

int hashtable_init(struct hashtable *hashtab);

int hashtable_drop(struct hashtable *hashtab);

int hashtable_clear(struct hashtable *hashtab);

struct hashitem *hashtable_get(struct hashtable *hashtab, void *key);

int hashtable_set(struct hashtable *hashtab, void *key, struct hashitem *value);

#define hash_entry(ptr, type, field) container_of(ptr, type, field)

#endif /* __HASHTAB_C__ */
