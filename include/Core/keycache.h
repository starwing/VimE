/*
 * VimE - the Vim Extensible
 */


#include <defs.h>
#include <Support/array.h>


/**
 * \file keycache.h
 *
 * the cache of key press for VimE.
 *
 * VimE support mappings, that means VimE can convert some input to
 * another. so first we need store all input, parse it and find
 * mappings, if there is a mapping in input, the input will be
 * modified into the mappinged text. e.g. if a mapping maps text `foo'
 * into `bar', then if you input abcfooabc, it will change to
 * abcbarabc.
 */


