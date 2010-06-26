/*
 * VimE - the Vim Extensible
 */


#include <defs.h>
#include <Support/sbtree.h>


/**
 * \file mode.h
 *
 * The modes routines of VimE.
 *
 * VimE is a Mode stacked editor. that means you can make recurse edit
 * with VimE. when VimE startup. the mode will be set to a default
 * normal mode. in that mode every keystroke will be treat as a
 * operator.
 *
 * you can goto other modes. there are several standard modes to use.
 * the data of the standard modes is hard coded in lib/StaticData dir.
 *
 *  - normal mode: used to execute edit operators.
 *  - text object mode: used to select a range of text.
 *  - insert mode: used to insert text data.
 *  - replace mode: used to replace text under the cursor.
 *  - visual mode: used to select a range of text with highlight.
 *  - cmdline mode: used to insert cmdline.
 *  - dired mode: used to view directory.
 *  - message mode: used to view messages.
 */

#ifndef VIME_MODE_H
#define VIME_MODE_H


/**
 * the mappings keys struction.
 */
struct mapping_key
{
    struct sbtree_entry node; /**< the sbtree node. */
    char    *key;   /**< the lhs text of this mappings. */
    char    *value; /**< the text mappings to map. */
    int     flags;  /**< the flags of this mappings. */
};


/** the default constructor of mappings */
#define MAPPING_KEY_INIT {SBTREE_INIT, NULL, NULL, MAP_FLAGS_CLEAR}


/** no flags in mappings */
#define MAP_FLAGS_CLEAR     0x00000000

/** a silent mappings. */
#define MAP_FLAGS_SILENT    0x00000001


/** the operator process function argument structions. */
struct op_argument
{
    int     count;      /**< the count of operator. */
    int     register;   /**< the register of operator. */
    int     flags;      /**< the flags of operator. */

    char    *text;      /**< the argument string text. */
    void    *data;      /**< user argument passed to proce function */
};

/** the default constructor of #op_argument */
#define OPARG_INIT {0, 0, 0, NULL, NULL}


/** no flags in mappings */
#define OP_ARGUMENT_FLAGS_CLEAR   0x00000000


/** the operator process function type */
typedef void (*op_proc_t) __ARGS((struct op_argument *args));


/**
 * the operator key struction.
 */
struct operator_key
{
    struct sbtree_entry node; /**< the sbtree node */
    int         key;        /**< the key value of the operator */
    op_func_t   proc_func;  /**< the process function for operator */
    void        *args;      /**< user argument for process function */
};


/** the default constructor of #operator_key */
#define OPERATOR_KEY_INIT {SBTREE_INIT, 0, NULL, NULL}

/**
 * the mode struction of VimE.
 */
struct vime_mode
{
    struct list_entry node;     /**< the mode lists. */
    char    *mode_name;         /**< the name of mode. */

    struct operator_key op_array; /**< the operator entries of the mode. */
    struct mapping_key map_array; /**< the map entries of the mode. */
};

/** the default constructor of #vime_mode */
#define VIME_MODE_INIT {LIST_ENTRY_INIT, NULL, COMMAND_KEY_INIT, MAPPING_KEY_INIT}


#endif /* VIME_MODE_H */


/* vim: set ft=c fdc=2 cin et sta sts=4 sw=4 ts=8: */
