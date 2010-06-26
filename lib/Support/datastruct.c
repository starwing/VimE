/*
 * VimE - the Vim Extensible
 */

/*
 * this file used to include the data struction header file. if define
 * ENABLE_INLINE and define INLINE to empty, the header file will
 * replaces with definintion of functions.
 */

#ifndef ENABLE_INLINE
#define ENABLE_INLINE
#undef INLINE
#define INLINE

#include <Support/hashtab.h>
#include <Support/list.h>
#include <Support/sbtree.h>

#endif
