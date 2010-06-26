/*
 * VimE - the Vim Expand
 */


/**
 * \file sbtree.h
 *
 * Size Balanced Tree routines.
 *
 * Size Balanced Tree is a kind of advance balanced search tree. used
 * for quickly find items with compareable keys. to use sbtree, you
 * must embed sbtree_entry struction into your structions. there is
 * not a key field in the sbtree_entry, so you must put keys into your
 * struction.
 */

#ifndef VIME_SBTREE_H
#define VIME_SBTREE_H


/**
 * compare function, used to compare two keys.
 *
 * \param entry is the entry of the sbtree, passed by the caller.
 * \param key is the key want to compare with the key in entry.
 *
 * \return 0 for key in the entry is equal with the key, or nonzero
 * otherwise.
 *
 * \remark see #hash_function_t to know how to get key from the entry
 * pointer.
 */
typedef int (*sbtree_compare_t)
    __ARGS((struct sbtree_entry const *entry, void const *key));


/**
 * the entry struction of sbtree
 *
 * this entry used to embed into the struction used the sbtree. note
 * that the left field must be the first field of the entry.
 */
struct sbtree_entry
{
    size_t size;                /**< the size of the subtree of this
                                     node (contain self). */

    struct sbtree_entry *parent; /**< the parent node of current node. */
    struct sbtree_entry *left;  /**< the left child of current node. */
    struct sbtree_entry *right; /**< the right child of current node. */
};


/** the default static initiallizer for struct #sbtree_entry. */
#define SBTREE_INIT {1, NULL, NULL, NULL}


/**
 * maintain #sbtree_entry.
 *
 * call it after insert items.
 *
 * \param node tree node need to maintained.
 * \param care_left if it's true, the left branch of node may voilate
 *        the rule of sbtree. or the right branch of mode may voilate.
 *
 * \remark #sbtree_entry hash a size field, this field used to decide
 *         how to make the #sbtree_entry balance. so after insert
 *         items, we need call this function in *every* ancestor of
 *         the item inserted.  if you aren't sure which branch has
 *         voilated, call this routine twice to make sure all things
 *         right.
 */
#ifndef ENABLE_INLINE
void sbtree_maintain __ARGS((struct sbtree_entry *node, int care_left));
#else /* ENABLE_INLINE */


/*
 * left rotate node.
 */
    INLINE void
sbtree_left_rotate(pnode)
    struct sbtree_entry **pnode;
{
    struct sbtree_entry *new_top = (*pnode)->left;
    new_top->parent = (*pnode)->parent;

    (*pnode)->right = new_top->left;
    new_top->left->parent = (*pnode)->right;

    new_top->left = (*pnode);
    (*pnode)->parent = new_top->left;

    new_top->size = (*pnode)->size;
    (*pnode)->size = (*pnode)->left->size + (*pnode)->right->size + 1;

    (*pnode) = new_top;
}


/*
 * right rotate node.
 */
    INLINE void
sbtree_right_rotate(pnode)
    struct sbtree_entry **pnode;
{
    struct sbtree_entry *new_top = (*pnode)->left;
    new_top->parent = (*pnode)->parent;

    (*pnode)->left = new_top->right;
    new_top->right->parent = (*pnode)->left;

    new_top->right = (*pnode);
    (*pnode)->parent = new_top->right;

    new_top->size = (*pnode)->size;
    (*pnode)->size = (*pnode)->left->size + (*pnode)->right->size + 1;

    (*pnode) = new_top;
}


    INLINE void
sbtree_maintain(pnode, care_left)
    struct sbtree_entry **pnode;
    int care_left;
{
    if (care_left)
    {
        if ((*pnode)->left->left->size > (*pnode)->right->size)
            sbtree_right_rotate(pnode);
        else if ((*pnode)->left->right->size > (*pnode)->right->size)
        {
            sbtree_left_rotate(&(*pnode)->left);
            sbtree_right_rotate(pnode);
        }
        else return;
    }
    else
    {
        if ((*pnode)->right->right->size > (*pnode)->left->size)
            sbtree_left_rotate(pnode);
        else if ((*pnode)->right->left->size > (*pnode)->left->size)
        {
            sbtree_right_rotate(&(*pnode)->right);
            sbtree_left_rotate(pnode);
        }
        else return;
    }

    sbtree_maintain(&(*pnode)->left, TRUE);
    sbtree_maintain(&(*pnode)->right, FALSE);
    sbtree_maintain(t, TRUE);
    sbtree_maintain(t, FALSE);
}

#endif /* ENABLE_INLINE */


/**
 * defined a new insert function that use cmp for compare.
 *
 * \param pnode the \b name of tree root pointer used to inserted.
 * \param new_node the \b name of new node to insert to pnode.
 * \param cmp the compare \b expression used in the function body.
 *
 * see the implement of #sbtree_insert for details usage.
 */
#define DEFINE_SBTREE_INSERT_BODY(pnode, new_node, cmp)        \
{                                                              \
    int cmp_res;                                               \
    struct sbtree_entry **pchild;                              \
                                                               \
    if (*pnode == NULL)                                        \
    {                                                          \
        new_node->parent = NULL;                               \
        return *pnode = new_node;                              \
    }                                                          \
                                                               \
    for (;;)                                                   \
    {                                                          \
        pchild = (cmp_res = (cmp)) < 0 ?                       \
            &(*pnode)->left : &(*pnode)->right;                \
                                                               \
        if (cmp_res == 0)                                      \
            return NULL;                                       \
                                                               \
        if (*pchild == NULL)                                   \
            break;                                             \
                                                               \
        pnode = pchild;                                        \
    }                                                          \
    *pchild = new_node;                                        \
    new_node->parent = *pnode;                                 \
                                                               \
    for (; *pnode != NULL;                                     \
            pchild = pnode, pnode = &(*pnode)->parent)         \
        sbtree_maintain(pnode, *pchild == &(*pnode)->left);    \
    return new_node;                                           \
}


/**
 * insert node into #sbtree_entry.
 *
 * \param pnode     tree node need to insert.
 * \param new_node  new node will be inserted into node.
 * \param key       the key of the new node inserted.
 * \param cmp_func  the compare function used to compare nodes.
 * \return NULL if there is a dupicate node in the sbtree. or new_node
 *         if insert has done.
 *
 * \remark this function don't allocate memory, so the insert must
 *         success. and this function don't return anything.
 */
#ifndef ENABLE_INLINE
struct sbtree_entry *sbtree_insert __ARGS((struct sbtree_entry **pnode,
            struct sbtree_entry *new_node,
            void const *key, sbtree_compare_t cmp_func));
#else /* ENABLE_INLINE */

    INLINE struct sbtree_entry*
sbtree_insert(pnode, new_node, key, cmp_func)
    struct sbtree_entry **pnode;
    struct sbtree_entry *new_node;
    void const      *key;
    sbtree_compare_t cmp_func;
DEFINE_SBTREE_INSERT_BODY(pnode, new_node, cmp_func(*pnode, key))

#endif /* ENABLE_INLINE */


/**
 * remove node from sbtree.
 *
 * \param pnode     tree node need to remove.
 * \param del_node  the node need to delete from the tree.
 *
 * \return the node deleted from the tree, or NULL when no found.
 */
#ifndef ENABLE_INLINE
struct sbtree_entry *sbtree_remove __ARGS((struct sbtree_entry **pnode,
            struct sbtree_entry const *del_node));
#else /* ENABLE_INLINE */

    INLINE sbtree_entry *
sbtree_remove(pnode, del_node)
    struct sbtree_entry **pnode;
    struct sbtree_entry *del_node;
{
    struct sbtree_entry *parent = del_node->parent;

    if (del_node->left == NULL || del_node->right == NULL)
    {
        struct sbtree_entry *child = del_node->left == NULL ?
            del_node->right : del_node->left;

        if (child != NULL)
            child->parent = parent;
        if (parent == NULL)
            *pnode = child;
        else
            *(parent->left == del_node ?
                    &parent->left : &parent->right) = child;
        return del_node;
    }

    struct sbtree_entry *succ = sbtree_get_succ(del_node);

    assert((succ->left == NULL || succ->right == NULL)
            && succ->parent != NULL);

    sbtree_remove(pnode, succ);
    succ->parent = del_node->parent;
    if (del_node->parent == NULL)
        *pnode = child;
    else
        *(parent->left == del_node ?
                &parent->left : &parent->right) = succ;
    succ->left = del_node->left;
    succ->left->parent = succ;
    succ->right = del_node->right;
    succ->right->parent = succ;

    return del_node;
}

#endif /* ENABLE_INLINE */


/**
 * defined a new lookup function that use cmp for compare.
 *
 * \param node the \b name of tree root pointer used to inserted.
 * \param cmp the compare \b expression used in the function body.
 *
 * see the implement of #sbtree_lookup for details usage.
 */
#define DEFINE_SBTREE_LOOKUP_BODY(node, cmp)                   \
{                                                              \
    int cmp_res;                                               \
                                                               \
    while (node != NULL)                                       \
    {                                                          \
        if ((cmp_res = (cmp)) == 0)                            \
            return node;                                       \
        node = cmp_res < 0 ? node->left : node->right;         \
    }                                                          \
                                                               \
    return NULL;                                               \
}


/**
 * find node from sbtree.
 *
 * \param node      root node of the tree used to find.
 * \param key       the key used in cmp_func.
 * \param cmp_func  the compare function used to compare nodes.
 *
 * \return the tree node found from tree, or NULL when no found.
 */
#ifndef ENABLE_INLINE
struct sbtree_entry *sbtree_lookup __ARGS((struct sbtree_entry *node,
        void const *key, sbtree_compare_t cmp_func));
#else /* ENABLE_INLINE */

    INLINE struct sbtree_entry *
sbtree_lookup(node, key, cmp_func)
    struct sbtree_entry *node;
    void const *key;
    sbtree_compare_t cmp_func;
DEFINE_SBTREE_LOOKUP_BODY(node, cmp_func(node, key))


#endif /* ENABLE_INLINE */


/**
 * defined a new lookup function that return the lower bound of the
 * key.
 *
 * \param node the \b name of tree root pointer used to inserted.
 * \param cmp the compare \b expression used in the function body.
 *
 * see the implement of #sbtree_lower_bound for details usage.
 */
#define DEFINE_SBTREE_LOWER_BOUND_BODY(node, cmp)               \
{                                                               \
    int cmp_res;                                                \
    struct sbtree_entry *prev = NULL;                           \
    struct sbtree_entry *prev_succ = NULL;                      \
                                                                \
    while (node != NULL)                                        \
    {                                                           \
        if ((cmp_res = cmp) == 0)                               \
            prev_succ = node;                                   \
                                                                \
        prev = node;                                            \
        node = cmp_res <= 0 ? node->left : node->right;         \
    }                                                           \
                                                                \
    return prev_succ == NULL ? prev : prev_succ;                \
}


/**
 * find the lower bound of the key.
 *
 * \param node      root node of the tree used to find.
 * \param key       the key used in cmp_func.
 * \param cmp_func  the compare function used to compare nodes, see
 *                  sbtree_insert() for details.
 * \return the upper bound of the key in the tree. see remarks for
 * details.
 *
 * \remark the lower bound is the maximum node that just small than
 * the key.
 */
#ifndef ENABLE_INLINE
struct sbtree_entry *sbtree_lower_bound __ARGS((struct sbtree_entry *node,
            void const *key, sbtree_compare_t cmp_func));
#else /* ENABLE_INLINE */

    INLINE struct sbtree_entry *
sbtree_lower_bound(node, key, cmp_func)
    struct sbtree_entry *node;
    void const *key;
    sbtree_compare_t cmp_func;
DEFINE_SBTREE_LOWER_BOUND_BODY(node, cmp_func(node, key))

#endif /* ENABLE_INLINE */


/**
 * defined a new lookup function that return the upper bound of the
 * key.
 *
 * \param node the \b name of tree root pointer used to inserted.
 * \param cmp the compare \b expression used in the function body.
 *
 * see the implement of sbtree_upper_bound() for details usage.
 */
#define DEFINE_SBTREE_UPPER_BOUND_BODY(node, cmp)               \
{                                                               \
    int cmp_res;                                                \
    struct sbtree_entry *prev = NULL;                           \
    struct sbtree_entry *prev_succ = NULL;                      \
                                                                \
    while (node != NULL)                                        \
    {                                                           \
        if ((cmp_res = cmp) == 0)                               \
            prev_succ = node;                                   \
                                                                \
        prev = node;                                            \
        node = cmp_res < 0 ? node->left : node->right;          \
    }                                                           \
                                                                \
    return prev_succ == NULL ? prev : prev_succ;                \
}


/**
 * find the upper bound of the key.
 *
 * \param node      root node of the tree used to find.
 * \param key       the key used in cmp_func.
 * \param cmp_func  the compare function used to compare nodes, see
 *                  sbtree_insert() for details.
 * \return the upper bound of the key in the tree. see remarks for
 *         details.
 *
 * \remark the upper bound is the minial node that just big than the
 *         key of can not find, or the latest node equal the key in
 *         the tree if found.
 */
#ifndef ENABLE_INLINE
struct sbtree_entry *sbtree_upper_bound __ARGS((struct sbtree_entry *node,
            void const *key, sbtree_compare_t cmp_func));
#else /* ENABLE_INLINE */

    INLINE struct sbtree_entry *
sbtree_upper_bound(node, key, cmp_func)
    struct sbtree_entry *node;
    void const *key;
    sbtree_compare_t cmp_func;
DEFINE_SBTREE_UPPER_BOUND_BODY(node, cmp_func(node, key))

#endif /* ENABLE_INLINE */


/**
 * get the predecessor of the given node, or NULL if no predecessor
 * found.
 * 
 * \param node the node used to get predecessor.
 * \return the predecessor of the given node, or NULL if no found.
 */
#ifndef ENABLE_INLINE
struct sbtree_entry *sbtree_get_pred
                __ARGS((struct sbtree_entry *node));
#else /* ENABLE_INLINE */

    INLINE struct sbtree_entry*
sbtree_get_pred(node)
    struct sbtree_entry *node;
{
    if (node->left != NULL)
    {
        node = node->left;
        while (node->right != NULL)
            node = node->right;
        return node;
    }

    while (node->parent != NULL
            && node != node->parent->right)
        node = node->parent;

    if (node->parent == NULL)
        return NULL;

    return node->parent;
}

#endif /* ENABLE_INLINE */


/**
 * get the successor of the given node, or NULL if no successor
 * found.
 * 
 * \param node the node used to get successor.
 * \return the successor of the given node, or NULL if no found.
 */
#ifndef ENABLE_INLINE
struct sbtree_entry *sbtree_get_succ
                __ARGS((struct sbtree_entry *node));
#else /* ENABLE_INLINE */

    INLINE struct sbtree_entry*
sbtree_get_succ(node)
    struct sbtree_entry *node;
{
    if (node->right != NULL)
    {
        node = node->right;
        while (node->left != NULL)
            node = node->left;
        return node;
    }

    while (node->parent != NULL
            && node != node->parent->left)
        node = node->parent;

    if (node->parent == NULL)
        return NULL;

    return node->parent;
}

#endif /* ENABLE_INLINE */


/**
 * get the number of nodes in a tree, include the root node.
 *
 * \param node the root node of the tree.
 * \return the numbers of the node.
 */
#ifndef ENABLE_INLINE
int sbtree_get_size __ARGS((struct sbtree_entry *node));
#else /* ENABLE_INLINE */

    INLINE int
sbtree_get_size(node)
    struct sbtree_entry *node;
{
    return node == NULL ? 0 : node->size;
}

#endif /* ENABLE_INLINE */


/**
 * find the nth node in the tree.
 *
 * \param node      root node of the tree used to find.
 * \param rank      the rank of node want to find
 *
 * \return the tree node found from tree, or NULL when no found.
 */
#ifndef ENABLE_INLINE
struct sbtree_entry *sbtree_select
    __ARGS((struct sbtree_entry *node, int rank));
#else /* ENABLE_INLINE */

    INLINE struct sbtree_entry *
sbtree_select(node, rank)
    struct sbtree_entry *node;
    int rank;
{
    int left_size;

    if (rank < 0 || rank >= sbtree_get_size(node))
        return NULL;

    if (rank == (left_size = sbtree_get_size(node->left)))
        return node;
    if (rank < left_size)
        return sbtree_select(node->left, rank);
    return sbtree_select(node->right, left_size - rank);
}

#endif /* ENABLE_INLINE */


/**
 * return the rank of a specfied node.
 *
 * \param node      the key used in cmp_func.
 *
 * \return the rank of found node, or -1 if no found.
 */
#ifndef ENABLE_INLINE
int sbtree_rank __ARGS((struct sbtree_entry *node));
#else /* ENABLE_INLINE */

    INCLIDE int
sbtree_rank(node)
    struct sbtree_entry *node;
{
    int rank = 0;

    if (node->left != NULL)
        rank += node->left;

    for (; node->parent != NULL; node = node->parent)
        if (node->parent->right == node)
            rank += sbtree_get_size(node->parent->left) + 1;
    
    return rank;
}

#endif /* ENABLE_INLINE */


#endif /* VIME_SBTREE_H */
