#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ENABLE_INLINE
#define DEFINE_EXTERN_DATA
#include "defs.h.in"
#include "sbtree.h"

struct test
{
    int key;
    struct sbtree_entry entry;
};

#define TEST_ENTRY(ptr) SBTREE_ENTRY(ptr, struct test, entry)

    int
test_get_height(struct sbtree_entry *node)
{
    if (node == &sbtree_nil)
        return 0;

    int l_height = test_get_height(node->left);
    int r_height = test_get_height(node->right);
    return (l_height > r_height ? l_height : r_height) + 1;
}


int cnt_l_rotate, cnt_r_rotate;


    INLINE void
test_left_rotate(struct sbtree_entry **pnode)
{
    struct sbtree_entry *node = *pnode;
    struct sbtree_entry *right = node->right;

    ++cnt_l_rotate;
    sbtree_set_parent(right, node->parent);
    sbtree_set_right(node, right->left);
    sbtree_set_left(right, node);

    right->size = node->size;
    node->size = node->left->size + node->right->size + 1;
    *pnode = right;
}


    INLINE void
test_right_rotate(struct sbtree_entry **pnode)
{
    struct sbtree_entry *node = *pnode, *left = node->left;

    ++cnt_r_rotate;
    sbtree_set_parent(left, node->parent);
    sbtree_set_left(node, left->right);
    sbtree_set_right(left, node);

    left->size = node->size;
    node->size = node->left->size + node->right->size + 1;
    *pnode = left;
}


    INLINE void
test_maintain1(struct sbtree_entry **pnode, int care_left)
{
    if (care_left)
    {
        if ((*pnode)->left->left->size > (*pnode)->right->size)
            test_right_rotate(pnode);
        else if ((*pnode)->left->right->size > (*pnode)->right->size)
        {
            test_left_rotate(&(*pnode)->left);
            test_right_rotate(pnode);
        }
        else return;
    }
    else
    {
        if ((*pnode)->right->right->size > (*pnode)->left->size)
            test_left_rotate(pnode);
        else if ((*pnode)->right->left->size > (*pnode)->left->size)
        {
            test_right_rotate(&(*pnode)->right);
            test_left_rotate(pnode);
        }
        else return;
    }

    test_maintain1(&(*pnode)->left, TRUE);
    test_maintain1(&(*pnode)->right, FALSE);
    test_maintain1(pnode, TRUE);
    test_maintain1(pnode, FALSE);
}


    void
test_insert_fixup1(struct sbtree_entry **pnode,
        struct sbtree_entry *cur_node)
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
    test_maintain1(pnode, cur_node == parent->left);
}


    struct sbtree_entry*
test_sbinsert1(
        struct sbtree_entry **pnode,
        struct sbtree_entry *new_node)
{
    struct sbtree_entry **old_pnode = pnode;
    struct sbtree_entry *node = *pnode;

    while (*pnode != &sbtree_nil)
    {
        node = *pnode;
        node->size += new_node->size;

        pnode = (
                TEST_ENTRY(node)->key - TEST_ENTRY(new_node)->key
                ) < 0 ?
            &node->right : &node->left;
    }

    sbtree_set_parent(new_node, node);
    *pnode = new_node;
    test_insert_fixup1(old_pnode, new_node);
    return new_node;
}


    INLINE void
test_maintain2(struct sbtree_entry **pnode, int care_left)
{
    if (care_left)
    {
        if ((*pnode)->left->left->size > (*pnode)->right->size)
            test_right_rotate(pnode);
        else if ((*pnode)->left->right->size > (*pnode)->right->size)
        {
            test_left_rotate(&(*pnode)->left);
            test_right_rotate(pnode);
        }
        else return;
    }
    else
    {
        if ((*pnode)->right->right->size > (*pnode)->left->size)
            test_left_rotate(pnode);
        else if ((*pnode)->right->left->size > (*pnode)->left->size)
        {
            test_right_rotate(&(*pnode)->right);
            test_left_rotate(pnode);
        }
        else return;
    }

    test_maintain1(&(*pnode)->left, TRUE);
    test_maintain1(&(*pnode)->right, FALSE);
    test_maintain1(pnode, TRUE);
    test_maintain1(pnode, FALSE);
}


    void
test_insert_fixup2(struct sbtree_entry **pnode,
        struct sbtree_entry *cur_node)
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
    test_maintain1(pnode, cur_node == parent->left);
}


    struct sbtree_entry*
test_sbinsert2(
        struct sbtree_entry **pnode,
        struct sbtree_entry *new_node)
{
    struct sbtree_entry **old_pnode = pnode;
    struct sbtree_entry *node = *pnode;

    while (*pnode != &sbtree_nil)
    {
        node = *pnode;
        node->size += new_node->size;

        pnode = (
                TEST_ENTRY(node)->key - TEST_ENTRY(new_node)->key
                ) < 0 ?
            &node->right : &node->left;
    }

    sbtree_set_parent(new_node, node);
    *pnode = new_node;
    test_insert_fixup2(old_pnode, new_node);
    return new_node;
}

#define N 1000000
struct test array[N];

#define M 2
double tt[M] = {};
int height[M][2] = {};
int rotate[M][2] = {};

void init(void)
{
    int i;
    for (i = 0; i < N; ++i)
        array[i].key = i;
}

void test(void)
{
    int i, idx;
    clock_t t;
    struct sbtree_entry *root;

    idx = 0;
    printf("test %d, N = %d\n", idx + 1, N);

    root = &sbtree_nil;
    for (i = 0; i < N; ++i)
        sbtree_init(&array[i].entry);

    cnt_l_rotate = cnt_r_rotate = 0;
    t = clock();
    for (i = 0; i < N; ++i)
        test_sbinsert1(&root, &array[i].entry);
    tt[idx] = (double)(clock() - t) / CLOCKS_PER_SEC; 
    height[idx][0] = test_get_height(root->left);
    height[idx][1] = test_get_height(root->right);
    rotate[idx][0] = cnt_l_rotate;
    rotate[idx][1] = cnt_r_rotate;
    
    idx = 1;
    printf("test %d, N = %d\n", idx + 1, N);

    root = &sbtree_nil;
    for (i = 0; i < N; ++i)
        sbtree_init(&array[i].entry);

    cnt_l_rotate = cnt_r_rotate = 0;
    t = clock();
    for (i = 0; i < N; ++i)
        test_sbinsert2(&root, &array[i].entry);
    tt[idx] = (double)(clock() - t) / CLOCKS_PER_SEC; 
    height[idx][0] = test_get_height(root->left);
    height[idx][1] = test_get_height(root->right);
    rotate[idx][0] = cnt_l_rotate;
    rotate[idx][1] = cnt_r_rotate;
}

void report(void)
{
    int i;

    printf(
            "===============================================================\n"
            "           time    l_height    r_height    l_rotate   r_rotate \n"
          );

    for (i = 0; i < M; ++i)
        printf(
            " test %-3d  %.2fms  %-6d      %-6d      %-5d       %-6d     \n",
            i + 1, tt[i], height[i][0], height[i][1], rotate[i][0], rotate[i][1]);

    printf(
            "===============================================================\n"
          );
}

int main(void)
{
    init();
    test();
    report();
    return 0;
}

/* cc: flags+='-std=c99' */

