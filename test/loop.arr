test->("last evaluated is returned",
       expect->((loop x <- 0 until x = 10 ++x), 10))

test->("while loops"
       expect->((loop x <- 0 while x < 10 ++x), 10))

test->("loop as accumulator",
       expect->((loop (s <- 0, x <- 0) until x = 10 (s <- s + ++x)), 55))
