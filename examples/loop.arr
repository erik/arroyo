last <- (loop x <- 0 until x = 10 x <- x + 1)
print "loop to 10: $last"

-- sum integers 1 to 10
sum <- 0
loop x <- 0 until x = 10
     sum <- sum + ++x

print "sum 1 through 10 (=55): $sum"