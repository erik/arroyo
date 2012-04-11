choice <- 2

print (case choice of (
            1      "choice 1"
            2      "choice 2"
            3      "choice 3"
            default    "you chose $choice"))


condition <- (1+1) = 3

-- case can be used to implement if, and other such conditionals (will
-- be useful when macros are implemented)

print (case condition of (
           true    "true body here"
           false   "false body here"))



