if (NOT ENABLE_INLINE)
    add_vime_library(VimESupport
	array.c
	cmdargs.c
	hashtab.c
	hook.c
	list.c
	sbtree.c
	)
endif()
