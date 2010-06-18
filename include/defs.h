/*
 * VimE - the Vim Extensible
 */


#ifndef VIME_CONFIG_H
#include <config.h>
#endif

/* for assert. */
#if defined(HAVE_ASSERT_H)
#include <assert.h>
#endif

/* for NULL. */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

/* for offsetof() */
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

/**
 * \file defs.h
 *
 * this file used to make some defined used for VimE.
 */


#ifndef VIME_DEFS_H
#define VIME_DEFS_H


/*
 * System macros.
 */
#if !defined(WIN32) && !defined(UNIX)
#  if defined(VIME_ON_UNIX)
#    define UNIX
#  elif defined(VIME_ON_WIN32)
#    define WIN32
#  endif /* defined(VIME_ON_UNIX) */
#endif /* !defined(WIN32) && !defined(UNIX) */


/**
 * return the container from the pointer points to the field of
 * struction.
 *
 * \param ptr pointer points to a field of a struction.
 * \param type the type of that struction.
 * \param field the field in the struction pointed by pointer.
 * \return the pointer points to the struction.
 */
#ifndef container_of
#define container_of(ptr, type, field) \
    ((type*)((char*)(ptr) - offsetof(type, field)))
#endif /* container_of */


/**
 * the inline keyword, depends ENABLE_INLINE value.
 */
#ifndef INLINE
#  ifdef ENABLE_INLINE
#    define INLINE static inline
#  else /* ENABLE_INLINE */
#    define INLINE
#    ifndef VIME_ONLY_PROTOTYPE
#       define VIME_ONLY_PROTOTYPE
#    endif /* VIME_ONLY_PROTOTYPE */
#  endif /* ENABLE_INLINE */
#endif /* INLINE */


/**
 * the internal keyword, depends ENABLE_INLINE value.
 */
#ifndef INTERNAL
#  ifndef ENABLE_INLINE
#    define INTERNAL static inline
#  else /* ENABLE_INLINE */
#    define INTERNAL static
#  endif /* ENABLE_INLINE */
#endif /* INTERNAL */


/** the TRUE and FALSE value.  */
#ifndef TRUE
#define TRUE 1
#endif /* TRUE */
#ifndef FALSE
#define FALSE 0
#endif /* FALSE */


/** the return value of function */
#ifndef OK
#define OK 0
#endif /* OK */
#ifndef FAIL
#define FAIL 1
#endif /* FAIL */


/**
 * the extern macro.
 *
 * this macro is used to include static data in a header file.
 * normally, the data will expand to a extern declare. but if define
 * DEFINE_EXTERN_DATA, the data will expand to a define.
 */
#ifndef EXTERN
#  ifndef DEFINE_EXTERN_DATA
#    define EXTERN(l, r) extern l
#  else /* DEFINE_EXTERN_DATA */
#    define EXTERN(l, r) l r
#  endif /* DEFINE_EXTERN_DATA */
#endif /* EXTERN */


#endif /* VIME_DEFS_H */
