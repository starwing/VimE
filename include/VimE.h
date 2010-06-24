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
 * @file
 *
 * @section DESCRIPTION
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
    int state_flags;
    int argc;
    char **argv;

    /* the allocator for all core structions */
    struct allocator alloc;

    /* the mode stack, used for recurse edit */
    struct mode_stack modes;

    /* the buffer list, a map from buffer name to buffer id */
    struct buffer_list buffers;

    /* the frame list of vime */
    struct frame_list frames;

    /* option table */
    struct option_table options;
};


#define STATE_FLAGS_CLEAR 0x00000000 


/**
 * return a initialized vime_state block.
 *
 * @param argc
 * @param argv command line arguments from main function.
 * @return a allocatored initialized vime state block.
 */
struct vime_state *vime_init __ARGS((int argc, char **argv));


/**
 * free the vime state block.
 *
 * @param state a vime state.
 */
void vime_drop __ARGS((struct vime_state *state));


/**
 * step the main loops of Vime Editor.
 *
 * @param state a vime state.
 */
void vime_step __ARGS((struct vime_state *state));



#endif /* VIME_STATE_H */
