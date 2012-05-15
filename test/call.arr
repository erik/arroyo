test->("anonymous functions",
       expect->((fn(x)x+1)->(1), 2))

test->("array indexing",
       expect->([0,1,2,3]->0, 0))

test->("passing arrays to functions",
       expect->((fn (a i) a->i)->([0], 0), 0))

-- test named functions
(
    add <- fn (x y) x + y
    fn add2 (x y) x + y

    test->("named functions",
           expect->(add->(1,2), 3))
    test->("named functions (alt declaration)",
           expect->(add2->(1,2), 3))
)

test->("functions returning functions",
       expect->((fn() fn(x) "= $x")->()->("abc"),
                "= abc"))

test->("splat arguments",
       expect->((fn(*x) x)->(1 2 3), [1 2 3]))

-- test macros

test->("anonymous macros",
       expect->((macro(x) "$x")->(not_nil), "not_nil"))

test->("macro evaluation",
       expect->((macro(x) x /= 2)->(1+1), true))


