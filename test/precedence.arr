test->("quoting precedence",
       expect->(#1 + 2, 3))

test->("comparison precedence",
       expect->(1 + 2 >= 3, true))

test->("binary precedence",
       expect->((1 or nil) = 1 and 2 /= 3, true))

test->("mixed binary/comparison precedence",
       expect->(1 < 2 and 2 > 1, true))

test->("and / or precedence",
       expect->(true and false or 2=2, true))

test->("chained operator precedence",
       expect->(1 + 2 * 3 / 4 > 5 * 6 + 4 - 3 * 2 + 1, false))