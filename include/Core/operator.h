/*
 * VimE - the Vim Extensible
 */


#include <defs.h>


/**
 * \file operator.h
 *
 * operator routines for VimE.
 *
 * `operator' is a table that contain all actions. actions can be
 * emited by charactor command. e.g. when you press "l", a operator
 * named "forward_char" will be emited. and you will find cursor 
 * advanced a charactor on screen.
 *
 * action is a hook struction. if a command can find in operator
 * table, then the action will be called. so you can add, remove,
 * modify action list easily -- they are just #list.
 */


#ifndef VIME_OPERATOR_H
#define VIME_OPERATOR_H


/**
 * operator entry struction.
 */
struct op_entry
{
    char *name;
    int command;
    struct hook action;
    struct hash_entry cmd_node;
    struct hash_entry name_node;
};

/** default static constructor for op_entry. */
#define OP_ENTRY_INIT {NULL, 0, HOOK_INIT, HASH_ENTRY_INIT, HASH_ENTRY_INIT}

/** convert from command hash_entry to op_entry. */
#define OP_CMD_ENTRY(ptr) HASH_ENTRY((ptr), struct hash_entry, cmd_node)

/** convert from name hash_entry to op_entry. */
#define OP_NAME_ENTRY(ptr) HASH_ENTRY((ptr), struct hash_entry, name_node)


/**
 * operator table struction.
 */
struct op_table
{
    struct hashtable cmd_ht;    /**< command hashtable. */
    struct hashtable name_ht;   /**< op name hashtable. */

    /* hooks */
    struct hook default_hook;   /**< hooks for default behavior. */
    struct hook remain_hook;    /**< hooks for action remain from
                                     preious operator. */
};

/** default static constructor for op_entry. */
#define OP_INIT {HASHTABLE_INIT, HASHTABLE_INIT, HOOK_INIT, HOOK_INIT}


/**
 * alloc a new operator table.
 */
struct op_table *op_table_alloc(void);


/**
 * free a alloced operator table.
 */
void op_table_free(struct op_table *table);


/**
 * initialize a operator table.
 */
static op_table *op_table_init(struct op_table *table);


/**
 * drop a operator table.
 */
void op_table_drop(struct op_table *drop);


/**
 * exec a operaotr.
 */
int op_exec(struct op_table *table, int command);


#endif /* VIME_OPERATOR_H */
