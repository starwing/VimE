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


/** the static initiallizer for the global leaf node of sbtree. */
#define SBTREE_NIL_INIT {0, &sbtree_nil, &sbtree_nil, &sbtree_nil}

/** the default static initiallizer for struct #sbtree_entry. */
#define SBTREE_INIT {1, &sbtree_nil, &sbtree_nil, &sbtree_nil}

/** the leaf node of sbtree. */
EXTERN(struct sbtree_entry sbtree_nil, = SBTREE_NIL_INIT);

/** get the container of the sbtree */
#define SBTREE_ENTRY(ptr, type, field) container_of(ptr, type, field)


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
 * initialize the sbtree_entry node.
 *
 * \param node the node need to initialize.
 */
#ifndef ENABLE_INLINE
void sbtree_init __ARGS((struct sbtree_entry *node));
#else /* ENABLE_INLINE */

    INLINE void
sbtree_init(node)
    struct sbtree_entry *node;
{
    node->size = 1;
    node->parent = &sbtree_nil;
    node->left = &sbtree_nil;
    node->right = &sbtree_nil;
}

#endif /* ENABLE_INLINE */


/**
 * get the minimum node of given tree.
 *
 * \param node the tree root node.
 * \return the minimum node in the tree.
 */
#ifndef ENABLE_INLINE
struct sbtree_entry *sbtree_get_min
                __ARGS((struct sbtree_entry *node));
#else /* ENABLE_INLINE */

    INLINE struct sbtree_entry*
sbtree_get_min(node)
    struct sbtree_entry *node;
{
    while (node->left != &sbtree_nil)
        node = node->left;
    return node;
}

#endif /* ENABLE_INLINE */


/**
 * get the maximum node of given tree.
 *
 * \param node the tree root node.
 * \return the maximum node in the tree.
 */
#ifndef ENABLE_INLINE
struct sbtree_entry *sbtree_get_max
                __ARGS((struct sbtree_entry *node));
#else /* ENABLE_INLINE */

    INLINE struct sbtree_entry*
sbtree_get_max(node)
    struct sbtree_entry *node;
{
    while (node->right != &sbtree_nil)
        node = node->right;
    return node;
}

#endif /* ENABLE_INLINE */

/**
 * get the predecessor of the given node, or &sbtree_nil if no predecessor
 * found.
 * 
 * \param node the node used to get predecessor.
 * \return the predecessor of the given node, or &sbtree_nil if no found.
 */
#ifndef ENABLE_INLINE
struct sbtree_entry *sbtree_get_pred
                __ARGS((struct sbtree_entry *node));
#else /* ENABLE_INLINE */

    INLINE struct sbtree_entry*
sbtree_get_pred(node)
    struct sbtree_entry *node;
{
    if (node->left != &sbtree_nil)
        return sbtree_get_max(node->left);

    while (node->parent != &sbtree_nil
            && node != node->parent->right)
        node = node->parent;

    if (node->parent == &sbtree_nil)
        return &sbtree_nil;

    return node->parent;
}

#endif /* ENABLE_INLINE */


/**
 * get the successor of the given node, or &sbtree_nil if no successor
 * found.
 * 
 * \param node the node used to get successor.
 * \return the successor of the given node, or &sbtree_nil if no found.
 */
#ifndef ENABLE_INLINE
struct sbtree_entry *sbtree_get_succ
                __ARGS((struct sbtree_entry *node));
#else /* ENABLE_INLINE */

    INLINE struct sbtree_entry*
sbtree_get_succ(node)
    struct sbtree_entry *node;
{
    if (node->right != &sbtree_nil)
        return sbtree_get_min(node->right);

    while (node->parent != &sbtree_nil
            && node != node->parent->left)
        node = node->parent;

    if (node->parent == &sbtree_nil)
        return &sbtree_nil;

    return node->parent;
}

#endif /* ENABLE_INLINE */


/** set the parent of sbtree node.  */
#define sbtree_set_parent(node, p) do {     \
    if ((node) != &sbtree_nil)              \
        (node)->parent = (p); } while (0)


/** set the left branch of sbtree node.  */
#define sbtree_set_left(node, l) do {       \
    (node)->left = l;                       \
    sbtree_set_parent((node)->left, node); } while (0)


/** set the right branch of sbtree node.  */
#define sbtree_set_right(node, r) do {      \
    (node)->right = r;                      \
    sbtree_set_parent((node)->right, node); } while (0)


/** get the pointer to the parent field contains node. */
#define sbtree_get_parent_field(node)       \
    ((node)->parent->left == (node) ?       \
        &(node)->parent->left : &(node)->parent->right)

/**
 * fix the sbtree after insert new node.
 *
 * call it after insert items.
 *
 * \param pnode tree node need to maintained.
 * \param new_node the new node inserted into the tree, this node must
 *        in tree before call this function.
 */
#ifndef ENABLE_INLINE
void sbtree_insert_fixup __ARGS((struct sbtree_entry **pnode, 
            struct sbtree_entry *new_node));
#else /* ENABLE_INLINE */


/*
 * left rotate node.
 */
    INLINE void
sbtree_left_rotate(pnode)
    struct sbtree_entry **pnode;
{
    struct sbtree_entry *node = *pnode;
    struct sbtree_entry *right = node->right;

    sbtree_set_parent(right, node->parent);
    sbtree_set_right(node, right->left);
    sbtree_set_left(right, node);

    right->size = node->size;
    node->size = node->left->size + node->right->size + 1;
    *pnode = right;
}


/*
 * right rotate node.
 */
    INLINE void
sbtree_right_rotate(pnode)
    struct sbtree_entry **pnode;
{
    struct sbtree_entry *node = *pnode, *left = node->left;

    sbtree_set_parent(left, node->parent);
    sbtree_set_left(node, left->right);
    sbtree_set_right(left, node);

    left->size = node->size;
    node->size = node->left->size + node->right->size + 1;
    *pnode = left;
}


/*
 * keep the sbtree.
 */
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
    sbtree_maintain(pnode, TRUE);
    sbtree_maintain(pnode, FALSE);
}


    void
sbtree_insert_fixup(pnode, cur_node)
    struct sbtree_entry **pnode;
    struct sbtree_entry *cur_node;
{
    struct sbtree_entry *parent = cur_node->parent, *child;

    /* new node needn't maintain */
    child = cur_node;
    cur_node = parent;
    parent = cur_node->parent;

    if (parent == &sbtree_nil)
        return;

    /* parent is not root */
    while (parent->parent != &sbtree_nil)
    {
        sbtree_maintain(cur_node == parent->left ?
                    &parent->left : &parent->right,
                child == cur_node->left);
        child = cur_node;
        cur_node = parent;
        parent = parent->parent;
    }

    *pnode = parent;
    sbtree_maintain(pnode, cur_node == parent->left);
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
#define DEFINE_SBTREE_INSERT_BODY(pnode, new_node, cmp)         \
{                                                               \
    struct sbtree_entry **pchild, **old_pnode = pnode;          \
                                                                \
    if (*pnode == &sbtree_nil)                                  \
    {                                                           \
        new_node->parent = &sbtree_nil;                         \
        return *pnode = new_node;                               \
    }                                                           \
                                                                \
    for (;;)                                                    \
    {                                                           \
        (*pnode)->size += new_node->size;                       \
                                                                \
        pchild = (cmp) < 0 ?                                    \
            &(*pnode)->left : &(*pnode)->right;                 \
                                                                \
        if (*pchild == &sbtree_nil)                             \
            break;                                              \
                                                                \
        pnode = pchild;                                         \
    }                                                           \
                                                                \
    *pchild = new_node;                                         \
    sbtree_set_parent(new_node, *pnode);                        \
    sbtree_insert_fixup(old_pnode, new_node);                   \
    return new_node;                                            \
}


/**
 * insert node into #sbtree_entry.
 *
 * \param pnode     tree node need to insert.
 * \param new_node  new node will be inserted into node.
 * \param key       the key of the new node inserted.
 * \param cmp_func  the compare function used to compare nodes.
 * \return &sbtree_nil if there is a dupicate node in the sbtree. or new_node
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
 * \return the node deleted from the tree, or &sbtree_nil when no found.
 */
#ifndef ENABLE_INLINE
struct sbtree_entry *sbtree_remove __ARGS((struct sbtree_entry **pnode,
            struct sbtree_entry const *del_node));
#else /* ENABLE_INLINE */

    INLINE struct sbtree_entry *
sbtree_remove(pnode, del_node)
    struct sbtree_entry **pnode;
    struct sbtree_entry *del_node;
{
    struct sbtree_entry *succ;

    if (del_node->left == &sbtree_nil
            || del_node->right == &sbtree_nil)
    {
        succ = del_node->left == &sbtree_nil ?
            del_node->right : del_node->left;

        sbtree_set_parent(succ, del_node->parent);
    }
    else
    {
        succ = sbtree_get_succ(del_node);

        sbtree_remove(pnode, succ);
        sbtree_set_parent(succ, del_node->parent);
        sbtree_set_left(succ, del_node->left);
        sbtree_set_right(succ, del_node->right);
    }

    if (del_node->parent == &sbtree_nil)
        *pnode = succ;
    else
        *sbtree_get_parent_field(del_node) = succ;

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
#define DEFINE_SBTREE_LOOKUP_BODY(node, cmp)                    \
{                                                               \
    int cmp_res;                                                \
                                                                \
    while (node != &sbtree_nil)                                 \
    {                                                           \
        if ((cmp_res = (cmp)) == 0)                             \
            return node;                                        \
        node = cmp_res < 0 ? node->left : node->right;          \
    }                                                           \
                                                                \
    return &sbtree_nil;                                         \
}


/**
 * find node from sbtree.
 *
 * \param node      root node of the tree used to find.
 * \param key       the key used in cmp_func.
 * \param cmp_func  the compare function used to compare nodes.
 *
 * \return the tree node found from tree, or &sbtree_nil when no found.
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
    struct sbtree_entry *prev = &sbtree_nil;                    \
    struct sbtree_entry *prev_succ = &sbtree_nil;               \
                                                                \
    while (node != &sbtree_nil)                                 \
    {                                                           \
        if ((cmp_res = cmp) == 0)                               \
            prev_succ = node;                                   \
                                                                \
        prev = node;                                            \
        node = cmp_res <= 0 ? node->left : node->right;         \
    }                                                           \
                                                                \
    return prev_succ == &sbtree_nil ? prev : prev_succ;         \
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
    struct sbtree_entry *prev = &sbtree_nil;                    \
    struct sbtree_entry *prev_succ = &sbtree_nil;               \
                                                                \
    while (node != &sbtree_nil)                                 \
    {                                                           \
        if ((cmp_res = cmp) == 0)                               \
            prev_succ = node;                                   \
                                                                \
        prev = node;                                            \
        node = cmp_res < 0 ? node->left : node->right;          \
    }                                                           \
                                                                \
    return prev_succ == &sbtree_nil ? prev : prev_succ;         \
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
 * find the nth node in the tree.
 *
 * \param node      root node of the tree used to find.
 * \param rank      the rank of node want to find
 *
 * \return the tree node found from tree, or &sbtree_nil when no found.
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

    if (rank < 0 || rank >= node->size)
        return &sbtree_nil;

    if (rank == (left_size = node->left->size))
        return node;
    if (rank < left_size)
        return sbtree_select(node->left, rank);
    return sbtree_select(node->right, rank - left_size - 1);
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

    INLINE int
sbtree_rank(node)
    struct sbtree_entry *node;
{
    int rank = 0;

    if (node->left != &sbtree_nil)
        rank += node->left->size;

    for (; node->parent != &sbtree_nil; node = node->parent)
        if (node->parent->right == node)
            rank += node->parent->left->size + 1;
    
    return rank;
}

#endif /* ENABLE_INLINE */


#endif /* VIME_SBTREE_H */
