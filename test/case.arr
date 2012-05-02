
test->("basic case use",
       expect->(case 2 of (0 => 0, 1 => 1, 2 => 2), 2))

test->("no matched case being nil",
       expect->(case 2 of ( 0 => "0" ), nil))

test->("default case",
       expect->(case 2 of ( default => 2 ), 2))



