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
 * this entry used to embed into the struction used the sbtree.
 */
struct sbtree_entry
{
    size_t size;            /**< the size of the subtree of this node
                                 (contain self). */

    struct sbtree_entry *parent;  /**< the parent node of current node. */
    struct sbtree_entry *left;    /**< the left child of current node. */
    struct sbtree_entry *right;   /**< the right child of current node. */
};


/** the default static initiallizer for struct #sbtree_entry. */
#define SBTREE_INIT {0, NULL, NULL}

/** initialize sbtree to size 1. */
#define SBTREE_INIT_1 {1, NULL, NULL}

/**
 * maintain #sbtree_entry.
 *
 * call it after insert items.
 *
 * \param node tree node need to maintained.
 * \param left_gt_right whether the size of left child's child is
 *        bigger than the size of right child. this flags used to hint
 *        maintain routine.
 *
 * \remark #sbtree_entry hash a size field, this field used to decide how to
 *         make the #sbtree_entry balance. so after insert items, we need
 *         call this function in *every* ancestor of the item
 *         inserted.
 */
#ifndef ENABLE_INLINE
void sbtree_maintain __ARGS((struct sbtree_entry *node, int left_gt_right));
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
sbtree_maintain(pnode, left_gt_right)
    struct sbtree_entry **pnode;
    int left_gt_right;
{
    if (left_gt_right)
    {
        /*
         * if we insert to left child of node (left_gt_right is true),
         * we must check whether node's left's child's child is
         * greater than node's right child.
         */
        if ((*pnode)->left->left->size > (*pnode)->right->size)
            sbtree_right_rotate(pnode);
        else if ((*pnode)->left->right->size > (*pnode)->right->size)
        {
            sbtree_left_rotate(&(*pnode)->left);
            sbtree_right_rotate(pnode);
        }
        else
            return;
    }
    else
    {
        /* or vice-versa */
        if ((*pnode)->right->right->size > (*pnode)->left->size)
            sbtree_left_rotate(pnode);
        else if ((*pnode)->right->left->size > (*pnode)->left->size)
        {
            sbtree_right_rotate(&(*pnode)->right);
            sbtree_left_rotate(pnode);
        }
        else
            return;
    }

    sbtree_maintain(&(*pnode)->left, TRUE);
    sbtree_maintain(&(*pnode)->right, FALSE);
    sbtree_maintain(pnode, TRUE);
    sbtree_maintain(pnode, FALSE);
}

#endif /* ENABLE_INLINE */


/**
 * defined a new insert function that use cmp for compare.
 *
 * \param name the \b name of the insert function.
 * \param pnode the \b name of tree root pointer used to inserted.
 * \param new_node the \b name of new node to insert to pnode.
 * \param cmp the compare \b expression used in the function body.
 *
 * see the implement of #sbtree_insert for details usage.
 */
#define DEFINE_SBTREE_INSERT_BODY(name, pnode, new_node, cmp)       \
    {                                                               \
        if (*pnode != NULL)                                         \
        {                                                           \
            int cmp_res = (cmp) < 0;                                \
                                                                    \
            (*pnode)->size += new_node->size;                       \
            name(cmp_res ?                                          \
                    &(*pnode)->left : &(*pnode)->right, new_node);  \
            sbtree_maintain(pnode, cmp_res);                        \
            return;                                                 \
        }                                                           \
                                                                    \
        /* otherwise, the node is NULL, so we set it to new_node */ \
        *pnode = new_node;                                          \
    }


/**
 * insert node into #sbtree_entry.
 *
 * \param pnode     tree node need to insert.
 * \param new_node  new node will be inserted into node.
 * \param key       the key of the new node inserted.
 * \param cmp_func  the compare function used to compare nodes.
 *
 * \remark this function don't allocate memory, so the insert must
 *         success. and this function don't return anything.
 */
#ifndef ENABLE_INLINE
void sbtree_insert __ARGS((struct sbtree_entry **pnode,
            struct sbtree_entry *new_node,
            void const *key, sbtree_compare_t cmp_func));
#else /* ENABLE_INLINE */

    INLINE void
sbtree_insert(pnode, new_node, key, cmp_func)
    struct sbtree_entry **pnode;
    struct sbtree_entry *new_node;
    void const      *key;
    sbtree_compare_t cmp_func;

        DEFINE_SBTREE_INSERT_BODY(sbtree_insert, pnode, new_node,
                cmp_func(*pnode, key))

#endif /* ENABLE_INLINE */


/**
 * remove node from sbtree.
 *
 * \param node      tree node need to remove.
 * \param key       the key used in cmp_func.
 * \param cmp_func  the compare function used to compare nodes.
 *
 * \return the node deleted from the tree, or NULL when no found.
 */
struct sbtree_entry *sbtree_remove __ARGS((struct sbtree_entry *node,
            void const *key, sbtree_compare_t cmp_func));


/**
 * defined a new remove function that use cmp for compare.
 *
 * usage: DEFINE_SBTREE_REMOVE(my_sbtree_remove,
 *     contain_of(new_node, my_type, tree)->data
 *     - contain_of(node, my_type, tree)->data)
 *
 * define this macro used to show how to insert a node by hand.
 */
/* TODO: write a correct remove function. */
#define DEFINE_SBTREE_REMOVE(name, cmp)         \
    static void name(node, new_node)            \
        struct sbtree_entry *node;              \
        struct sbtree_entry *new_node;          \
    {                                           \
        node->size -= new_node->size;           \
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
struct sbtree_entry *sbtree_lookup __ARGS((struct sbtree_entry *node,
        void const *key, sbtree_compare_t cmp_func));


/**
 * defined a new lookup function that use cmp for compare.
 *
 * usage: DEFINE_SBTREE_LOOKUP(my_sbtree_insert,
 *     contain_of(new_node, my_type, tree)->data
 *     - contain_of(node, my_type, tree)->data)
 */
#define DEFINE_SBTREE_LOOKUP(name, cmp)         \
    static void name(node, new_node)            \
        struct sbtree_entry *node;              \
        struct sbtree_entry *new_node;          \
    {                                           \
        int res = (cmp);                        \
        if (res == 0)                           \
            return node;                        \
        return name(res < 0 ?                   \
                node->left : node->right,       \
                new_node);                      \
    }


/**
 * find the nth node in the tree.
 *
 * \param node      root node of the tree used to find.
 * \param rank      the rank of node want to find
 *
 * \return the tree node found from tree, or NULL when no found.
 */
struct sbtree_entry *sbtree_select
    __ARGS((struct sbtree_entry *node, int rank));


/**
 * return the rank of a specfied node.
 *
 * \param node      root node of the tree used to find.
 * \param key       the key used in cmp_func.
 * \param cmp_func  the compare function used to compare nodes.
 *
 * \return the rank of found node, or -1 if no found.
 */
int sbtree_rank __ARGS((struct sbtree_entry *node,
            void const *key, sbtree_compare_t cmp_func));


#endif /* VIME_SBTREE_H */
