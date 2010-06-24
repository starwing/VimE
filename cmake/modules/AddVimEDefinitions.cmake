# There is no clear way of keeping track of compiler command-line
# options chosen via `add_definitions', so we need our own method for
# using it.

# Beware that there is no implementation of remove_vime_definitions.

macro(add_vime_definitions)
    set(VIME_DEFINITIONS "${VIME_DEFINITIONS} ${ARGN}")
    add_definitions(${ARGN})
endmacro(add_vime_definitions)

