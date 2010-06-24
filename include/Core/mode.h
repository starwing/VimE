/*
 * VimE - Vim Expand
 */


#include "defs.h"


/**
 * @file
 *
 * The modes routines of VimE.
 *
 * VimE is a Mode stacked editor. that means you can make recurse edit
 * with VimE. when VimE startup. the mode will be set to a default
 * normal mode. in that mode every keystroke will be treat as a
 * command.
 *
 * you can goto other modes. there are several standard modes to use.
 * the data of the standard modes is hard coded in lib/StaticData dir.
 *
 *  - normal mode: used to execute edit commands.
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



/** max entry count in mode table */
#define MODE_ENTRY_MAX 256

/**
 * the mode struction of VimE.
 */
struct vime_mode
{
    /** the name of mode */
    char *mode_name;

    /** the command entries of this mode */
    struct command_entry cmd_entries[MODE_ENTRY_MAX];

    /** the map entries of this mode */
    struct hashtable map_entries;
};



#endif /* VIME_MODE_H */





