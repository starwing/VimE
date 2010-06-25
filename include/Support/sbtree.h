/*
 * VimE - the Vim Expand
 */


#include <defs.h>

/**
 * @file sbtree.h
 *
 * Size Balanced Tree routines.
 *
 * Size Balanced Tree is a kind of advance balanced search tree. used
 * for quickly find items with compareable keys.
 */


#ifndef VIME_SBTREE_H
#define VIME_SBTREE_H



/** the node struction of sbtree */
struct sbtree
{
    /** the size of the subtree of this node (contain self). */
    size_t size;

    /** the left child of current node. */
    struct sbtree *left;

    /** the right child of current node. */
    struct sbtree *right;
};


/** the default static initiallizer for struct sbtree. */
#define SBTREE_INIT {1, NULL, NULL}


/**
 * maintain sbtree. call it after insert/remove items.
 *
 * @param node tree node need to maintained.
 * @param left_gt_right whether the size of left child is bigger than
 *        the size of right child. this flags used to hint maintain
 *        routine.
 */
void sbtree_maintain __ARGS((struct sbtree *node, int left_gt_right));


/**
 * insert node into sbtree.
 *
 * @param node      tree node need to insert.
 * @param new_node  new node will be inserted into node.
 * @param cmp_func  the compare function used to compare nodes.
 */
void sbtree_insert __ARGS((struct sbtree *node, struct sbtree *new_node,
            int (*cmp_func)(struct sbtree const*, struct sbtree const*)));


/**
 * defined a new function that use cmp for compare.
 *
 * usage: DEFINE_SBTREE_INSERT(my_sbtree_insert,
 *     contain_of(new_node)->data < contain_of(node)->data)
 */
#define DEFINE_SBTREE_INSERT(name, cmp)         \
    void name(node, new_node)                   \
        struct sbtree *node;                    \
        struct sbtree *new_node;                \
    {                                           \
        node->size += new_node->size;           \
        if (cmp)                                \
        {                                       \
            if (node->left == NULL)             \
                node->left = new_node;          \
            else                                \
                name(node->left, new_node);     \
        }                                       \
        else                                    \
        {                                       \
            if (node->right == NULL)            \
                node->right = new_node;         \
            else                                \
                name(node->right, new_node);    \
        }                                       \
                                                \
        sbtree_maintain(node, !(cmp));          \
    }


/**
 * remove node from sbtree.
 *
 * @param node      tree node need to remove.
 * @param key       the key used in cmp_func.
 * @param cmp_func  the compare function used to compare nodes.
 *
 * @return the node deleted from the tree, or NULL when no found.
 */
struct sbtree *sbtree_remove(struct sbtree *node, void const *key,
            int (*cmp_func)(struct sbtree const*, struct sbtree const*));


/**
 * find node from sbtree.
 *
 * @param node      root node of the tree used to find.
 * @param key       the key used in cmp_func.
 * @param cmp_func  the compare function used to compare nodes.
 *
 * @return the tree node found from tree, or NULL when no found.
 */
struct sbtree *sbtree_lookup(struct sbtree *node, void const *key,
            int (*cmp_func)(struct sbtree const*, struct sbtree const*));


/**
 * find the nth node in the tree.
 *
 * @param node      root node of the tree used to find.
 * @param rank      the rank of node want to find
 *
 * @return the tree node found from tree, or NULL when no found.
 */
struct sbtree *sbtree_select(struct sbtree *node, int rank);


/**
 * return the rank of a specfied node.
 *
 * @param node      root node of the tree used to find.
 * @param key       the key used in cmp_func.
 * @param cmp_func  the compare function used to compare nodes.
 *
 * @return the rank of found node, or -1 if no found.
 */
int sbtree_rank(struct sbtree *node, void const *key,
            int (*cmp_func)(struct sbtree const*, struct sbtree const*));



#endif /* VIME_SBTREE_H */
