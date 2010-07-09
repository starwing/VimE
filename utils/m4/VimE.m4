divert(-1)

this file used to generated new VimE source.
usage: 
    - first, write you code with m4, remember include this file first.
    - the quote char will change to [ and ].
    - use macro defined here to complete your code.
    - generate *.[ch] code, and drop it to VimE source tree.


-- change default quote char to [ and ].
changequote([,])

-- include utility macros.
divert(0)dnl
include(utils.m4)dnl
divert(-1)

-- define header of the header file.
-- usage:
-- HEADER(foobar, [
-- brief doc, this is the document of foobar.
-- 
-- docs...
-- ],
-- 
-- stdio.h
-- stdlib.h
-- stdarg.h,
-- 
-- [dnl
-- /* body of function~~~ */])


define([HEADER], [dnl
/*
 * VimE - the Vim Extensible
 */


ifelse([$3], [], [], [dnl
map([[#]include <header>
], [header],
patsubst([$3], [
], [, ]))dnl


])dnl
dnl
dnl
ifelse([$2], [], [], [dnl
/*
 * \file $1.h
map([ * doc
], [doc], patsubst([$2], [
], [,]))dnl
 */


])dnl
dnl
dnl
[#]ifdef VIME_[]toupper($1)_H
[#]define VIME_[]toupper($1)_H


$4

[#]endif /* VIME_[]toupper($1)_H */
/* vim: set ft=c fdm=syntax fdc=2 sw=4 ts=8 sts=4 nu et sta: */])


define([STRUCT], [dnl
/**
/* $2
 */
struct $1
{
};
])

divert(0)dnl
