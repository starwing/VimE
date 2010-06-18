/*
 * VimE - the Vim Extensible
 */


#include <defs.h>
#include <System/mem.h>


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
    void const *key;    /**< the key value of this entry. */
    hash_t hash;        /**< the hash value of the key of the entry */
};

/** the default constructor of #hash_entry. */
#define HASH_ENTRY_INIT  {NULL, NULL}

/**
 * get the entry of a hash item.
 *
 * \param ptr the pointer points to a hashitem.
 * \param type the type of the struction contains the hashitem.
 * \param field the field of hashitem in struction.
 * \return return the type struction which contains the hashitem
 *         pointered by ptr.
 */
#define HASH_ENTRY(ptr, type, field) container_of((ptr), type, field)


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
typedef int (*hash_compare_t)(void const *lhs, void const *rhs);


/** the hash item type.  */
typedef struct hash_entry *hashitem_t;

/** hash item value of emply entry. */
#define HI_NULL NULL

/** hash item value of removed entry. */
#define HI_TOMB (hashitem_t)((char*)NULL + 1)

/** whether the entry is empty. */
#define hi_is_empty(hi) ((hi) == HI_NULL || (hi) == HI_TOMB)


/**
 * hash table struction.
 *
 * use ht_init() function to initialize a hashtable (mainly
 * alloc space for array). after use, call ht_drop() function
 * to destroy this table. use ht_clear() to delete all items
 * from this table, and use ht_get(), ht_set() to access
 * the items in the table.
 */
struct hashtable
{
    size_t   capacity;  /**< mask used for hash value (nr of
                             items in array is "mask" + 1). */
    size_t   size;      /**< number of items used. */
    size_t   fillsize;  /**< number of items used + removed. */
    int      flags;     /**< counter for hash_lock(). */

    hashitem_t *array;  /**< points to the array. */
};

/** the default constructor of #hashtable */
#define HASHTABLE_INIT {0, 0, 0, HF_DEFAULT, NULL}

/** flags for default #hashtable. */
#define HF_DEFAULT          (0)

/** flags for needn't free hashtable#array. */
#define HF_NO_FREE        (1 << 0)

/** flags for #fast_hashtable that has smallarray. */
#define HF_HAS_SMALLARRAY   (1 << 1)

/** whether a #hashtable needn't free its array. */
#define ht_no_free(hashtab) (((hashtab)->flags & HF_NO_FREE) == 0)

/** whether a #hashtable hash a small array. */
#define ht_has_smallarray(hashtab) (((hashtab)->flags & HF_HAS_SMALLARRAY) == 0)


/** Initial size for a hashtable.
 *
 * Our items are relatively small and growing is expensive, thus use
 * 16 as a start.  Must be a power of 2.
 */
#define HT_INIT_SIZE (1 << 4)

/**
 * a static fast hashtable.
 *
 * needn't to allocate space for array.
 */
struct fast_hashtable
{
    /** the #hashtable of the static version hashtable. */
    struct hashtable hashtab;

    /** the pointer points to the hash table. */
    hashitem_t smallarray[HT_INIT_SIZE];
};

/** the default constructor of #fast_hashtable */
#define FAST_HASHTABLE_INIT {{0, 0, 0, HF_HAS_SMALLARRAY, NULL}, {}}

/** convert a fast_hashtable to hashtable. */
#define HASHTABLE(ptr) (&(ptr)->hashtab)

/** convert a hashtable to fast hashtable. */
#define FAST_HT_ENTRY(ptr) \
    container_of(hashtab, struct fast_hashtable, hashtab)

/** get static array of a static hashtable. */
#define ht_get_smallarray(hashtab) (FAST_HT_ENTRY(hashtab)->smallarray)


#define PERTURB_SHIFT 5

#define IS_POWER_OF_2(x) ((x & (x - 1)) == 0)


/**
 * define the function \b body for findding entries in a hashtable.
 *
 * \param hashtab the \b name of #hashtable.
 * \param item the \b name of the item that used in cmp_expr.
 * \param hash_expr the \b expression that used to compute hash value.
 * \param cmp_expr the \b expression that used to compare *item and
 *        the key used to find.
 * \return the entry pointer if found, or NULL if not found.
 * \sa ht_entry
 */
#define DEFINE_HT_ENTRY_BODY(hashtab, item, hash_expr, cmp_expr) \
{                                                               \
    hash_t _hash;                                               \
    int _idx, _mask, _perturb;                                  \
    hashitem_t *item, *_freeitem = NULL;                        \
                                                                \
    assert((hashtab) != NULL);                                  \
    if (ht_may_resize((hashtab), 0) == FAIL)                    \
        return NULL;                                            \
    assert((hashtab)->size < (hashtab)->capacity);              \
                                                                \
    _mask = (hashtab)->capacity - 1;                            \
    _hash = (hash_expr);                                        \
    _idx = (int)(_hash & _mask);                                \
    item = &(hashtab)->array[_idx];                             \
                                                                \
    if (*item == HI_NULL)                                       \
        return item;                                            \
    else if (*item == HI_TOMB)                                  \
        _freeitem = item;                                       \
    else if ((*item)->hash == _hash && (cmp_expr) == 0)         \
        return item;                                            \
                                                                \
    for (_perturb = _hash; ; _perturb >>= PERTURB_SHIFT)        \
    {                                                           \
        _idx = (int)((_idx << 2) + _idx + _perturb + 1);        \
        item = &(hashtab)->array[_idx & _mask];                 \
                                                                \
        if (*item == HI_NULL)                                   \
            return _freeitem == NULL ? item : _freeitem;        \
        else if (*item == HI_TOMB && _freeitem == NULL)         \
            _freeitem = item;                                   \
        else if ((*item)->hash == _hash && (cmp_expr) == 0)     \
            return item;                                        \
    }                                                           \
                                                                \
    /* can not reach here. */                                   \
}

/**
 * define the function \b body of hashtable insert routines.
 *
 * \param hashtab the \b name of #hashtable.
 * \param value the \b name of the item that used to insert.
 * \param entry_expr the \b expression to compute the entry in
 *        the #hashtable, this expression is usually a function call
 *        to ht_entry() function or other *_entry() routines generated
 *        from DEFINE_HT_ENTRY_BODY().
 * \return value if insert success. or the elder entry if there is
 *         another entry in the table.
 * \sa ht_insert ht_set
 */
#define DEFINE_HT_INSERT_BODY(hashtab, value, entry_expr) \
{                                       \
    hashitem_t *_entry = (entry_expr);  \
    if (!hi_is_empty(*_entry))          \
        return *_entry;                 \
                                        \
    *_entry = value;                    \
    ++hashtab->size;                    \
    ++hashtab->fillsize;                \
    return *_entry;                     \
}

/**
 * define the function \b body of hashtable lookup routines.
 *
 * \param hashtab the \b name of #hashtable.
 * \param entry the \b name of the pointer to hashitem_t.
 * \param entry_expr the \b expression to compute the entry in
 *        the #hashtable, this expression is usually a function call
 *        to ht_entry() function or other *_entry() routines generated
 *        from DEFINE_HT_ENTRY_BODY().
 * \param succ_expr the \b expression that returned when find a entry.
 * \param fail_expr the \b expression that returned when can't find.
 * \return succ_expr or fail_expr.
 * \sa ht_insert ht_remove ht_get
 */
#define DEFINE_HT_LOOKUP_BODY(hashtab, entry, entry_expr, succ_expr, fail_expr) \
{   hashitem_t *entry = (entry_expr);   \
    return hi_is_empty(*entry) ? (fail_expr) : (succ_expr); }


/* public routine prototypes. */
INLINE struct hashtable *ht_init(struct hashtable *hashtab);
INLINE void ht_drop(struct hashtable *hashtab);
INLINE struct hashtable *ht_safe_init(struct hashtable *hashtab);
INLINE struct hashtable *fast_ht_init(struct fast_hashtable *hashtab);
INLINE struct hashtable *ht_clear(struct hashtable *hashtab);
INLINE int ht_may_resize(struct hashtable *hashtab, size_t minitems);
INLINE hash_t ht_default_hash(char const *key);
INLINE hashitem_t *ht_entry(struct hashtable *hashtab,
        void const *key, hash_t hash, hash_compare_t cmp_func);
INLINE struct hash_entry *ht_set(struct hashtable *hashtab,
        struct hash_entry *value, hash_compare_t cmp_func);
INLINE struct hash_entry *ht_get(struct hashtable *hashtab,
        void const *key, hash_t hash, hash_compare_t cmp_func);
INLINE struct hash_entry *ht_del(struct hashtable *hashtab, hashitem_t *entry);
INLINE hashitem_t *ht_default_entry(struct hashtable *hashtab, void const *key, hash_t *pash);
INLINE struct hash_entry *ht_insert(struct hashtable *hashtab, struct hash_entry *value);
INLINE struct hash_entry *ht_lookup(struct hashtable *hashtab, void const *key);
INLINE struct hash_entry *ht_remove(struct hashtable *hashtab, void const *key);


#if defined(ENABLE_INLINE) || defined(DEFINE_INLINE_ROUTINES)

/**
 * this function initializes a #hashtable.
 *
 * if the hashtable#mask nonzero, use it for the size of hashtable.
 * if hashtable#mask nonzero and hashtable#array nonzero, not allocate
 * new space for array, just call ht_clear() to clear all item
 * in the table. so if you want use this function, you MUST clear the
 * hashtab parameter. or use ht_safe_init() function below.
 *
 * \param hashtab a uninitialized hash table
 * \return return the hashtable initialized.
 *
 * \sa ht_safe_init
 */
    INLINE struct hashtable*
ht_init(struct hashtable *hashtab)
{
    if (hashtab->array != NULL)
        return ht_clear(hashtab);
    if (ht_has_smallarray(hashtab))
        return fast_ht_init(FAST_HT_ENTRY(hashtab));
    return ht_safe_init(hashtab);
}


/**
 * destroy a #hashtable.
 *
 * \param hashtab a hash table to destroyed.
 */
    INLINE void
ht_drop(struct hashtable *hashtab)
{
    assert(hashtab != NULL);

    /*
     * if table hasn't DONT_FREE flags, and it's not have a small
     * table, or it has a smallarray, but array don't point to
     * smallarray: free the array.
     */
    if (!ht_no_free(hashtab)
            && (!ht_has_smallarray(hashtab)
            || hashtab->array != ht_get_smallarray(hashtab)))
        vime_free(hashtab->array);
}


/**
 * this function initializes a #hashtable safely.
 *
 * this function just like ht_init, but don't check the value
 * in the hashtab. so you can use it on a unclear (I mean, memset())
 * struction. and if the hashtab hash memeory initialized from heap,
 * the memory will leap! use ht_init() instead in this case.
 *
 * \param hashtab a uninitialized hash table
 * \return return the hashtable initialized.
 *
 * \sa ht_init
 */
    INLINE struct hashtable*
ht_safe_init(struct hashtable *hashtab)
{
    assert(hashtab != NULL);
    memset(hashtab, 0, sizeof(struct hashtable));

    return hashtab;
}


/**
 * this function initializes a #fast_hashtable. use
 * fast_hashtable#static_array to initialize the pointer in
 * #hashtable.
 *
 * \param hashtab a uninitialized hash table
 * \return return the hashtable initialized.
 */
    INLINE struct hashtable*
fast_ht_init(struct fast_hashtable *hashtab)
{
    ht_init(HASHTABLE(hashtab))->flags = HF_NO_FREE | HF_HAS_SMALLARRAY;
    HASHTABLE(hashtab)->array = hashtab->smallarray;
    HASHTABLE(hashtab)->capacity = HT_INIT_SIZE;
    return HASHTABLE(hashtab);
}


/**
 * clear a #hashtable.
 *
 * \param hashtab a hash table to cleared.
 */
    INLINE struct hashtable*
ht_clear(struct hashtable *hashtab)
{
    assert(hashtab != NULL);

    if (ht_has_smallarray(hashtab))
    {
        ht_drop(hashtab);
        ht_safe_init(hashtab);
        hashtab->array = ht_get_smallarray(hashtab);
        memset(hashtab->array, 0, sizeof(hashitem_t) * HT_INIT_SIZE);
        return hashtab;
    }

    if (hashtab->array != NULL)
    {
        hashitem_t *array = hashtab->array;
        memset(array, 0, sizeof(hashitem_t) * hashtab->capacity);
        ht_safe_init(hashtab)->array = array;
    }

    return hashtab;
}


/**
 * detect whether a hashtable need resize, and resize it if needed.
 *
 * \param hashtab the hashtable need to resize.
 * \param minitems the notify value of mininal items in the hashtab.
 * \return OK for success, and FAIL for no memory error.
 */
    INLINE int
ht_may_resize(struct hashtable *hashtab, size_t minitems)
{
    size_t minsize, newsize;
    hashitem_t *newarray, *oldarray, temparray[HT_INIT_SIZE];
    hashitem_t *iter, *item;
    int idx, todo, mask, perturb;

    assert(hashtab != NULL
            && IS_POWER_OF_2(hashtab->capacity));

    if (minitems == 0)
    {
        /*
         * quickly return for fast hashtable.
         * NOTICE that the lookup algorithm in ht_entry() required
         * hashtable has mininally one empty entry, so we need make sure
         * fillsize small than HT_INIT_SIZE - 1.
         */
        if (ht_has_smallarray(hashtab)
                && hashtab->fillsize < HT_INIT_SIZE - 1
                && ht_get_smallarray(hashtab) == hashtab->array)
            return OK;

        if (hashtab->fillsize * 3 < hashtab->capacity * 2
                && hashtab->size > hashtab->capacity / 5)
            return OK;

        if (hashtab->size > 1000)
            minsize = hashtab->size * 2;
        else
            minsize = hashtab->size * 4;
    }
    else
    {
        if (minitems < hashtab->size)
            minitems = hashtab->size;
        minsize = minitems * 3 / 2;
    }

    newsize = HT_INIT_SIZE;
    while (newsize < minsize)
    {
        newsize <<= 1;

        /* over flow. */
        if (newsize == 0)
            return FAIL;
    }

    if (ht_has_smallarray(hashtab) && newsize == HT_INIT_SIZE)
    {
        newarray = ht_get_smallarray(hashtab);

        /* from newarray to newarray! this can remove HI_TOMB items. */
        if (hashtab->array == newarray)
        {
            memcpy(temparray, newarray, sizeof(hashitem_t) * HT_INIT_SIZE);
            oldarray = temparray;
        }
        else
            oldarray = hashtab->array;
    }
    else
    {
        newarray = vime_malloc(sizeof(hashitem_t) * newsize);
        if (newarray == NULL)
            return (hashtab->fillsize < hashtab->capacity) ? OK : FAIL;
        oldarray = hashtab->array;
    }
    memset(newarray, 0, sizeof(hashitem_t) * newsize);

    hashtab->capacity = newsize;
    hashtab->fillsize = hashtab->size;
    hashtab->array = newarray;
    if (oldarray == NULL)
        return OK;

    /* Move all items from the old array to the new array.  */
    todo = hashtab->size;
    mask = newsize - 1;
    for (iter = oldarray; todo > 0; ++iter)
    {
        if (*iter == HI_NULL)
            continue;

        /*
         * this algorithm is same as the algorithm in ht_entry,
         * but simpler than it. because we only need find a HI_NULL
         * entry.
         */
        idx = (int)((*iter)->hash & mask);
        item = &newarray[idx];
        
        for (perturb = (*iter)->hash; item != NULL; perturb >>= PERTURB_SHIFT)
        {
            idx = (int)((idx << 2) + idx + perturb + 1);
            item = &newarray[idx & mask];
        }
        *item = *iter;
        --todo;
    }

    return OK;
}


/**
 * compute a hash value for string.
 *
 * \param key the str used to compute.
 * \return hash the hash value of the string.
 */
    INLINE hash_t
ht_default_hash(char const *key)
{
    hash_t hash;

    assert(key != NULL && *key != '\0');

    hash = *key;
    while (*++key != '\0')
        hash = (hash << 5) + hash + *key;

    return hash;
}


/**
 * get a entry used to insert/retrieve from hashtable.
 *
 * \param hashtab   the #hashtable.
 * \param key       the key value should in the entry.
 * \param hash      the hash value of the key.
 * \param cmp_func  the compare function for compare entry and key.
 * \return the pointer to entry we need.
 *
 * \remark the hashtable must have empty entries, or this function
 *         will never return.
 */
    INLINE hashitem_t*
ht_entry(
        struct hashtable *hashtab,
        void const *key,
        hash_t hash,
        hash_compare_t cmp_func)
DEFINE_HT_ENTRY_BODY(hashtab, item, hash, cmp_func((*item)->key, key))


/**
 * get the hashtable entry, but use default hash function and default
 * compare function.
 *
 * \param hashtab   the #hashtable.
 * \param key       the key value should in the entry.
 * \param phash     points to the variable received the hash value
 *                  computed form key.
 * \return the pointer to entry we need.
 *
 * \remark the hashtable must have empty entries, or this function
 *         will never return.
 */
    INLINE hashitem_t*
ht_default_entry(struct hashtable *hashtab, void const *key, hash_t *phash)
DEFINE_HT_ENTRY_BODY(hashtab, item,
        phash == NULL ? ht_default_hash(key)
            : (*phash = ht_default_hash(key)),
        strcmp((*item)->key, key))


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
    INLINE hashitem_t
ht_get(
        struct hashtable *hashtab,
        void const *key,
        hash_t hash,
        hash_compare_t cmp_func)
DEFINE_HT_LOOKUP_BODY(hashtab, entry,
        ht_entry(hashtab, key, hash, cmp_func), *entry, NULL)


/**
 * set a element to hashtable, use key and hash in the entry.
 *
 * \param hashtab   a hash table we want to insert the value in.
 * \param value     the item used to insert, the key and hash field
 *                  \b must set up before call this function.
 * \param cmp_func  the compare function for compare entry and key.
 *
 * \return the hash item if success, or NULL if fail.
 */
    INLINE hashitem_t
ht_set(
        struct hashtable *hashtab,
        struct hash_entry *value,
        hash_compare_t cmp_func)
DEFINE_HT_INSERT_BODY(hashtab, value,
        ht_entry(hashtab, value->key, value->hash, cmp_func))


/**
 * remove a element from hashtable.
 */
    INLINE hashitem_t
ht_del(struct hashtable *hashtab, hashitem_t *entry)
{
    hashitem_t del_entry = *entry;
    assert(entry - hashtab->array < hashtab->capacity);
    --hashtab->size;
    *entry = HI_TOMB;
    return del_entry;
}


/**
 * set a item to hashtable, use ht_default_hash() to compute hash
 * value.
 *
 * \param hashtab   a hash table we want to insert the value in.
 * \param value     the item used to insert.
 * \param cmp_func  the compare function for compare entry and key.
 *
 * \return the hash item if success, or NULL if fail.
 * \sa ht_set
 */
    INLINE hashitem_t
ht_insert(struct hashtable *hashtab, struct hash_entry *value)
DEFINE_HT_INSERT_BODY(hashtab, value,
        ht_default_entry(hashtab, value->key, &value->hash))


/**
 * lookup a item from hashtable, use ht_default_hash() function for
 * compute hash value.
 *
 * \param hashtab the hashtable.
 * \param key the key which look up in the hashtable.
 * \param cmp_func the compare function between #hash_entry and key.
 * \return the entry if found, or NULL if not found.
 * \sa ht_get
 */
    INLINE struct hash_entry*
ht_lookup(struct hashtable *hashtab, void const *key)
DEFINE_HT_LOOKUP_BODY(hashtab, entry,
        ht_default_entry(hashtab, key, NULL), *entry, NULL)


/**
 * remove a item from hashtable, use ht_default_hash() for default
 * hash value.
 */
    INLINE struct hash_entry*
ht_remove(struct hashtable *hashtab, void const *key)
DEFINE_HT_LOOKUP_BODY(hashtab, entry,
        ht_default_entry(hashtab, key, NULL), ht_del(hashtab, entry), NULL)

#endif /* defined(ENABLE_INLINE) || defined(DEFINE_INLINE_FUNCS) */


#endif /* VIME_HASHTAB_H */
/* vim: set ft=c: */
