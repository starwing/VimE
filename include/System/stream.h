/*
 * VimE - the Vim Extensible
 */


#include <defs.h>


/**
 * \file stream.h
 */


#ifndef VIME_STREAM_H
#define VIME_STREAM_H


/**
 * the stream operation struction.
 */
struct stream_ops
{
    int (*open)(char *name, flag_t flags, flag_t mode, stream stream_ops *self);
    int (*close)(stream stream_ops *self);
    int (*getc)(struct stream_ops *self);
    int (*putc)(int ch, struct stream_ops *self);
    ssize_t (*read)(void *buf, size_t buflen, stream stream_ops *self);
    ssize_t (*write)(void *buf, size_t buflen, stream stream_ops *self);
    ssize_t (*seek)(int whence, ssize_t offset, struct stream_ops *self);
};


#endif /* VIME_STREAM_H */
