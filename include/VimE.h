/*
 * VimE - Vim Expand
 * Maintainer: StarWing
 */


#include "System/allocator.h"
#include "Core/mode.h"
#include "Core/buffer.h"
#include "Core/option.h"
#include "UI/frame.h"


/**
 * \mainpage
 * \~english
 *
 * VimE is a new Text Editor that implements Vi-style operatings and
 * full flexible.\n
 * 
 *
 * VimE is the successor of Vim, a Text Editor, and it has several
 * features that different with Vim.
 *
 *   - flexible binary extension interface.
 *   - faster when process big file and file contain long line.
 *
 * \~chinese
 * VimE 是全新的文本编辑利器。它采用 Vi 风格的操作方式，但是提供了强大
 * 的可扩展性。
 * 
 *
 * VimE 是 Vim -- 一个著名文本编辑器的后继者。VimE 有很多 Vim 所不具备
 * 的特性。
 *
 *   - VimE 有强大的二进制扩展插件界面。
 *   - VimE 在处理大文件和包含长行的文件时比 Vim 快很多。
 */


/**
 * \file
 *
 * The global state function and structions of VimE.
 *
 * If you want use VimE, you must init it first. call vime_init
 * function with args will return a struction named struct vime_state.
 * then you can pass state struction into several functions to
 * maintain the main lopp of editor.
 *
 * All top-level functions in VimE need state struction.
 *
 * After use, you can simply use vime_drop to destroy the state
 * struction.
 */

#ifndef VIME_STATE_H
#define VIME_STATE_H



/**
 * The State struction of VimE.
 */
struct vime_state
{
    int     flags;      /**< the flags of state. */
    int     argc;       /**< the count of command line argument. */
    char    **argv;     /**< the command line argument. */

    /** the global option hashtable for VimE. */
    struct vime_option options;

    /** the top value of mode stack, used for recurse edit */
    struct vime_mode modes;

    /** the buffer list, a map from buffer name to buffer id */
    struct buffer_list buffers;

    /** the frame list of vime, used for UI. */
    struct frame_list frames;

    /** the global hooks of VimE. */
    struct hook init_hook;
    struct hook drop_hook;
    struct hook step_hook;
};


/** no flags in current state */
#define STATE_FLAGS_NULL 0x00000000 


/**
 * return a initialized vime_state block.
 *
 * \param argc
 * \param argv command line arguments from main function.
 * \return a allocatored initialized vime state block.
 */
struct vime_state *vime_init(int argc, char **argv);


/**
 * free the vime state block.
 *
 * \param state a vime state.
 */
void vime_drop(struct vime_state *state);


/**
 * step the main loops of Vime Editor.
 *
 * \param state a vime state.
 */
void vime_step(struct vime_state *state);


#endif /* VIME_STATE_H */
