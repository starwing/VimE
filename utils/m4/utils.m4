divert(-1)

this file:
    - used to generated VimE source code.
    - contain utility macro of m4.
    - used by VimE.m4
    - use [ and ] as quote char.
you should change quote charactor to [ and ] before include this file.

-- quote all arguments.
define([quote], [[$*]])

-- apply all arguments.
define([apply], [$*])

-- shift nth argument.
-- shift(n, args...) => shift(shift(...shift(args)))
define([shiftn],
    [ifelse(eval([$1 <= 0]), [1], [shift($@)],
            eval([$1 == 1]), [1], [shift(shift($@))],
    [$0(decr([$1]), shift(shift($@)))])])

-- mapping var to each of remain arguments.
-- map([i = %d, ], [%d], 1, 2, 3) => i = 1, i = 2, i = 3
define([map], [ifelse(eval([$# < 3]), [1], [],
                      eval([$# == 3]), [1],
    [pushdef([$2], [$3])[]$1[]popdef([$2])],
    [pushdef([$2], [$3])[]$1[]popdef([$2])$0([$1], [$2], shiftn(3, $@))])])

-- translate string to other case.
define([toupper], [translit([$*], [a-z], [A-Z])])
define([tolower], [translit([$*], [A-Z], [a-z])])

divert[]dnl
