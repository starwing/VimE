set(VIME_USED_LIBS VimESystem)

add_vime_executable(hashtable
    Support/test_hashtab.c
    )

add_test(NAME hashtable
    COMMAND hashtable
    )
