#include <stdio.h>
#include <Support/hashtab.h>

struct node
{
    struct hash_entry entry;
};

#define NODE_ENTRY(ptr) HASH_ENTRY((ptr), struct node, entry)
#define node_key(ptr) ((ptr)->entry.key)

#define N 1000
struct node array[N];
int main(void)
{
    struct fast_hashtable table = FAST_HASHTABLE_INIT;

    ht_init(HASHTABLE(&table));
    ht_drop(HASHTABLE(&table));
    ht_safe_init(HASHTABLE(&table));
    ht_drop(HASHTABLE(&table));
    printf("%p\n", &array[0].entry);
    node_key(&array[0]) = "abc";
    printf("%p\n",
        ht_lookup(HASHTABLE(&table), node_key(&array[0])));
    printf("%p\n",
        ht_insert(HASHTABLE(&table), &array[0].entry));
    printf("%p\n",
        ht_lookup(HASHTABLE(&table), node_key(&array[0])));
    ht_drop(HASHTABLE(&table));
    return 0;
}
/* cc: flags+='-I.. -I../../utils/nedmalloc' */
