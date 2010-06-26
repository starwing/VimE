/*
 * VimE - the Vim Extensible
 */


/**
 * \file list.h
 *
 * the double linked list for VimE.
 *
 * we borrowed code from linux kernerl source. and make lots of
 * modified on it. so it will looks different with the kernerl source.
 * we make it more simple, and has the same interface with the other
 * data struction.
 */


#ifndef VIME_LIST_H
#define VIME_LIST_H

/**
 * Simple doubly linked list implementation.
 *
 * this struction is used to embed into the struction need linked list
 * servces. the next field must be the first field of this struction.
 * this list must be a circle linked list for the converice of other
 * routines.
 */
struct list_entry
{
    struct list_entry *next; /**< next pointer to another head */
    struct list_entry *prev; /**< prev pointer to another head */
};


/** the default constructor of #list_entry.  */
#define LIST_ENTRY_INIT {NULL, NULL}


/** initialize a single list entry variable.
 *
 * this macro only uses for init a single auto variable of
 * #list_entry. don't use it init the #list_entry that embeded into
 * other struction, use list_init() routines instead.
 */
#define LIST_ENTRY_VARINIT(name) {&(name), &(name)}

/**
 * define a list entry named name.
 *
 * \param name the name of the #list_entry variable.
 */
#define DEFINE_LIST_ENTRY(name) \
    struct list_head name = LIST_ENTRY_VARINIT(name)


/**
 * get the struct for this entry
 *
 * \param ptr   the &struct list_head pointer.
 * \param type  the type of the struct this is embedded in.
 * \param field the name of the list_struct within the struct.
 */
#define LIST_ENTRY(ptr, type, field) container_of(ptr, type, field)


#ifndef ENABLE_INLINE

/**
 * initialize a list to a empty linked list.
 *
 * \param list the list_entry of the list used to init.
 */
void list_init __ARGS((struct list_entry *list));


/**
 * add a new entry
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 *
 * \param node      list head to add it after
 * \param new_node  new entry to be added
 */
void list_append __ARGS((struct list_entry *node,
            struct list_entry *new_node));


/**
 * add a new entry
 *
 * \param node      list head to add it after
 * \param new_node  new entry to be added
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
void list_prepend __ARGS((struct list_entry *node,
            struct list_entry *new_node));

/**
 * deletes entry from list.
 *
 * \param entry the element to delete from the list.
 *
 * \remark list_empty() on entry does not return true after this, the
 *         entry is in an undefined state.
 */
void list_remove __ARGS((struct list_entry *entry));

/**
 * replace old entry by new one
 *
 * If \param node was empty, it will be overwritten.
 *
 * \param node      the element to be replaced
 * \param new_node  the new element to insert
 */
void list_replace __ARGS((struct list_entry *node,
                struct list_entry *new_node));


/**
 * tests whether \param list is the last entry in list \param head
 *
 * \param list  the entry to test
 * \param head  the head of the list
 */
int list_is_last __ARGS((struct list_entry const *list,
                struct list_entry const *head));


/**
 * tests whether a list is empty
 *
 * \param head the list to test.
 */
int list_empty __ARGS((struct const list_head *head));

#else  /* ENABLE_INLINE */

/**
 * initialize a list to a empty linked list.
 *
 * \param list the list_entry of the list used to init.
 */
INLINE void list_init (struct list_entry *list)
{
	list->next = list;
	list->prev = list;
}

/**
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 *
 * \param new_node 	the new entry to insert
 * \param prev 		the previous list entry
 * \param next 		the next list entry
 */
INLINE void list_insert(struct list_head *new_node,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new_node;
	new_node->next = next;
	new_node->prev = prev;
	prev->next = new_node;
}

/**
 * add a new entry
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 *
 * \param head      list head to add it after
 * \param new_node  new entry to be added
 */
INLINE void list_append (struct list_entry *head,
            struct list_entry *new_node)
{
	list_insert(new_node, head, head->next);
}	
	


/**
 * add a new entry
 *
 * \param head      list head to add it after
 * \param new_node  new entry to be added
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
INLINE void list_prepend (struct list_entry *head,
            struct list_entry *new_node)
{
	list_insert(new_inode, head->prev, head);
}

/**
 * deletes entry from list.
 *
 * \param entry the element to delete from the list.
 */
INLINE list_remove (struct list_entry *entry)
{
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;

	entry->next = NULL;
	entry->prev = NULL;
}

/**
 * replace old entry by new one
 *
 * If \param node was empty, it will be overwritten.
 *
 * \param node      the element to be replaced
 * \param new_node  the new element to insert
 */
INLINE void list_replace (struct list_entry *node,
                struct list_entry *new_node)
{
	new_node->next = node->next;
	new_node->next->prev = new_node;
	new_inode->prev = node->prev;
	new_node->prev->next = new_node;
}


/**
 * tests whether \param list is the last entry in list \param head
 *
 * \param list  the entry to test
 * \param head  the head of the list
 */
INLINE int list_is_last (struct list_entry const *list,
                struct list_entry const *head)
{
	return list->next == head;
}



/**
 * tests whether a list is empty
 *
 * \param head the list to test.
 */
INLINE int list_empty (struct const list_head *head)
{
	return head->next == head;
}


#endif /* ENABLE_INLINE */


/**
 * get the first element from a list
 *
 * Note, that list is expected to be not empty.
 *
 * \param ptr:  the list head to take the element from.
 * \param type: the type of the struct this is embedded in.
 * \param member:   the name of the list_struct within the struct.
 */
#define LIST_FIRST_ENTRY(ptr, type, member) \
    lIST_ENTRY((ptr)->next, type, member)

/**
 * iterate over a list
 * \param pos   the &struct list_head to use as a loop cursor.
 * \param head  the head for your list.
 */
#define LIST_FOR_EACH(pos, head) \
    for (pos = (head)->next; LIST_PREFETCH(pos->next), pos != (head); \
            pos = pos->next)

/**
 * iterate over a list backwards
 *
 * \param pos   the &struct list_head to use as a loop cursor.
 * \param head  the head for your list.
 */
#define LIST_FOR_EACH_PREV(pos, head) \
    for (pos = (head)->prev; LIST_PREFETCH(pos->prev), pos != (head); \
            pos = pos->prev)

/**
 * iterate over a list safe against removal of list entry
 *
 * \param pos       the &struct list_head to use as a loop cursor.
 * \param n         another &struct list_head to use as temporary storage
 * \param head      the head for your list.
 */
#define LIST_FOR_EACH_SAFE(pos, n, head)                            \
    for (pos = (head)->next, n = pos->next; pos != (head);          \
        pos = n, n = pos->next)

/**
 * iterate over a list backwards safe against removal of list entry
 *
 * \param pos       the &struct list_head to use as a loop cursor.
 * \param n         another &struct list_head to use as temporary storage
 * \param head      the head for your list.
 */
#define LIST_FOR_EACH_PREV_SAFE(pos, n, head)                       \
    for (pos = (head)->prev, n = pos->prev;                         \
         LIST_PREFETCH(pos->prev), pos != (head);                   \
         pos = n, n = pos->prev)

/**
 * iterate over list of given type
 *
 * \param pos       the type * to use as a loop cursor.
 * \param head      the head for your list.
 * \param member    the name of the list_struct within the struct.
 */
#define LIST_FOR_EACH_ENTRY(pos, head, member)                      \
    for (pos = LIST_ENTRY((head)->next, typeof(*pos), member);      \
         LIST_PREFETCH(pos->member.next), &pos->member != (head);   \
         pos = LIST_ENTRY(pos->member.next, typeof(*pos), member))


#endif /* VIME_LIST_H */
