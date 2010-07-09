/*
 * VimE - the Vim Extensible
 */


#include <defs.h>
#include <System/mem.h>


/**
 * \file array.h
 *
 * the flexiable array for VimE.
 *
 * from list.h we implement a #list struction used to implement
 * sequence. but it's not enough. list can insert/remove item very
 * fast, but find/indexing is very slow. if you always want to
 * iterate all item in a sequence, then the list is the best choice,
 * or you can try #array here.
 *
 * #array is a flexiable random access data struction. it can grow if
 * nedded.
 */


#ifndef VIME_ARRAY_H
#define VIME_ARRAY_H


/** the vime array type. */
typedef void *array_t;

/** convert other pointer to array type. */
#define ARRAY_PTR(ptr) ((array_t)(ptr))


/**
 * the array struction.
 */
struct array
{
    size_t size;       /**< the count of the used item in the array. */ 
    size_t capacity;   /**< the count of all the item in the array. */
    size_t itemsize;   /**< the size of a single item in array. */
    size_t growsize;   /**< the count of additional item in grown. */

    char array[1];  /**< the array memory, it usally bigger than 1,
                         because we will allocate the array struction
                         dynmaicly. */
};

/** convert void* into a #array struction. */
#define ARRAY(ptr) container_of((ptr), struct array, array)


/**
 * alloc a new array struction.
 */
#ifndef ENABLE_INLINE
array_t array_alloc(size_t itemsize, size_t initsize, size_t growsize);
#else /* ENABLE_INLINE */

    INLINE array_t
array_alloc(size_t itemsize, size_t initsize, size_t growsize)
{
    struct array *arrptr = vime_malloc(sizeof(struct array) - 1
            + initsize * itemsize);

    if (arrptr == NULL)
        return NULL;

    arrptr->size = 0;
    arrptr->capacity = initsize;
    arrptr->itemsize = itemsize;
    arrptr->growsize = growsize;

    return ARRAY_PTR(arrptr->array);
}

#endif /* ENABLE_INLINE */


/**
 * free a unused array struction.
 */
#ifndef ENABLE_INLINE
void array_free(array_t array);
#else /* ENABLE_INLINE */

    INLINE void
array_free(array_t array)
{
    vime_free(ARRAY(array));
}

#endif /* ENABLE_INLINE */


/**
 * grow a array.
 */
#ifndef ENABLE_INLINE
array_t array_grow(array_t *parray, int force);
#else /* ENABLE_INLINE */

    INLINE array_t
array_grow(array_t *parray, int force)
{
    struct array *arrptr = ARRAY(*parray);

    if (arrptr->size == arrptr->capacity || force)
    {
        struct array *new_arr = vime_realloc(array,
                (arrptr->capacity + arrptr->growsize) * arrptr->itemsize);

        if (new_arr == NULL)
            return NULL;

        new_arr->capacity += new_arr->growsize;
        *parray = ARRAY_PTR(new_arr->array);
    }

    return *parray;
}

#endif /* ENABLE_INLINE */


/** get a item of array. */
#define array_item(ptr, type) ((type*)(ptr))

/** get the size of a array.  */
#define array_size(ptr) (ARRAY(ptr)->size)

/** get the capacity of a array.  */
#define array_capacity(ptr) (ARRAY(ptr)->capacity)

/** get the item size of a array.  */
#define array_itemsize(ptr) (ARRAY(ptr)->itemsize)

/** get the grow size of a array.  */
#define array_growsize(ptr) (ARRAY(ptr)->growsize)


#endif /* VIME_ARRAY_H */
