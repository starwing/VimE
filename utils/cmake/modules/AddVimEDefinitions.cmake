# There is no clear way of keeping track of compiler command-line
# options chosen via `add_definitions', so we need our own method for
# using it.

macro(add_vime_definitions)
    set(VIME_DEFINITIONS "${VIME_DEFINITIONS} ${ARGN}")
    add_definitions(${ARGN})
endmacro(add_vime_definitions)

macro(remove_vime_definitions)
    foreach(d ${ARGN})
	list(REMOVE_ITEM VIME_DEFINITIONS ${d})
    endforeach(d)

    remove_definitions(${ARGN})
endmacro(remove_vime_definitions)
